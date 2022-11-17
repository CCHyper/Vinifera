/*******************************************************************************
/*               O P E N  S O U R C E -- P R O J E C T T S Y R                **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          INI.CPP
 *
 *  @author        Joe L. Bostic (see notes below)
 *
 *  @contributors  CCHyper (Extensions and rewrites), tomsons26.
 *
 *  @brief         INI database handler class.
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/
#include "newini.h"
#include "ffactory.h"
#include "filestraw.h"
#include "filepipe.h"
#include "cstraw.h"
#include "readline.h"
#include "strtrim.h"
#include <debughandler.h>
#include <asserthandler.h>


#ifndef NDEBUG
/**
 *  Enable debug logging of the ini load and save process.
 */
//#define INI_DEBUG 1
#endif


bool ViniferaINIClass::KeepFormatting = false;
bool ViniferaINIClass::KeepEmptySections = true;
bool ViniferaINIClass::KeepBlankEntries = true;

#ifndef NDEBUG
bool ViniferaINIClass::DumpDatabaseToDebuggerOutput = false;
#endif


bool ViniferaINIClass::Load(const char *filename, bool load_comments)
{
    FileAutoPtr file(TheFileFactory, filename);
    return ViniferaINIClass::Load(*file, load_comments);
}


/***********************************************************************************************
 * ViniferaINIClass::Load -- Load INI data from the file specified.                                    *
 *                                                                                             *
 *    Use this routine to load the INI class with the data from the specified file.            *
 *                                                                                             *
 * INPUT:   file  -- Reference to the file that will be used to fill up this INI manager.      *
 *                                                                                             *
 * OUTPUT:  bool; Was the file loaded successfully?                                            *
 *                                                                                             *
 * WARNINGS:   This routine allocates memory.                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool ViniferaINIClass::Load(FileClass & file, bool load_comments)
{
    FileStraw straw(file);
    return ViniferaINIClass::Load(straw, load_comments);
}


/***********************************************************************************************
 * ViniferaINIClass::Load -- Load the INI data from the data stream (straw).                           *
 *                                                                                             *
 *    This will fetch data from the straw and build an INI database from it.                   *
 *                                                                                             *
 * INPUT:   straw -- The straw that the data will be provided from.                            *
 *                                                                                             *
 * OUTPUT:  bool; Was the database loaded ok?                                                  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/10/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool ViniferaINIClass::Load(Straw & ffile, bool load_comments)
{
    char section[64];
    bool end_of_file = false; // Did we reach the end of the file?
    char buffer[INI_MAX_LINE_LENGTH];
    bool merge = false;

    //INIComment *commptr = nullptr;
    //INIComment *commptr_2 = nullptr;
    //INIComment *commptr_3 = nullptr;

#ifdef INI_DEBUG
    WWTIMEIT("INIClass::Load");
#endif

    if (Section_Count() > 0) {
#ifdef INI_DEBUG
            DEBUG_INFO("INIClass::Load() - Database already has %d sections, merging.\n", Section_Count());
#endif
        merge = true;
        load_comments = false;
    }

    CacheStraw file;
    file.Get_From(ffile);

    /*
    ** Prescan until the first section is found.
    */
    while (!end_of_file) {
        Read_Line(file, buffer, sizeof(buffer), end_of_file);
        if (end_of_file) {
            if (load_comments) {

                //DEBUG_INFO("INIClass::Load() - Comm 1 \"%s\".\n", commptr->Value);

                LineComments = commptr;
                return true;
            }
            if (commptr) {
                INIComment *next;
                while (LineComments) {
                    next = LineComments->Next;
                    delete LineComments;
                    LineComments = next;
                }
            }
            return false;
        }

        if (buffer[0] == '[' && std::strchr(buffer, ']') != nullptr) break;

        /**
         *  #BUGFIX: Added check for ';' to make sure the line contains valid a comment.
        */
        if (load_comments && std::strchr(buffer, ';') != nullptr) {
            INIComment *tmpcom = new INIComment;
            if (tmpcom) {
                if (commptr_2) {
                    commptr_2->Next = tmpcom;
                    commptr_2 = tmpcom;
                } else {
                    commptr = tmpcom;
                    commptr_2 = tmpcom;
                    commptr_3 = tmpcom;
                }
                tmpcom->Value = strdup(buffer);

                //DEBUG_INFO("INIClass::Load() - Comm 2 \"%s\".\n", tmpcom->Value);

            } else {
                load_comments = false;
            }
        }
    }

    /**
     *  Merge with the existing database.
     */
    if (merge) {

        /*
        ** Process a section. The buffer is prefilled with the section name line.
        */
        while (!end_of_file) {

            buffer[0] = ' ';
            char *ptr = std::strchr(buffer, ']');
            if (ptr) *ptr = '\0';
            strtrim(buffer);
            std::strcpy(section, buffer);

            /*
            ** Process a section. The buffer is prefilled with the section name line.
            */
            while (!end_of_file) {

                /*
                ** If this line is the start of another section, then bail out
                ** of the entry loop and let the outer section loop take
                ** care of it.
                */
                int len = Read_Line(file, buffer, sizeof(buffer), end_of_file);
                if (buffer[0] == '[' && std::strchr(buffer, ']') != nullptr) break;

                /*
                ** Determine if this line is a comment or blank line. Throw it out if it is, we have already stored it.
                */
                Strip_Comments(buffer);
                //if (len == 0 || buffer[0] == ';' || buffer[0] == '=') continue;
#ifdef INI_DEBUG
                DEBUG_INFO("INIClass::Load(merge) - \"%s\".\n", buffer);
#endif

                if (len == 0) {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load(merge) - Skipping blank line.\n");
#endif
                    continue;
                }

                if (buffer[0] == '=') {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load(merge) - Skipping invalid line \"%s.\".\n", buffer);
#endif
                    continue;
                }

                if (buffer[0] == ';') {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load(merge) - Skipping line comment \"%s.\".\n", buffer);
#endif
                    continue;
                }

                /**
                 *  The line isn't an obvious comment. Make sure that there is the "=" character
                 *  at an appropriate spot.
                 */
                char *divider = std::strchr(buffer, '=');
                if (!divider) continue;

                /**
                 *  Split the line into entry and value sections. Be sure to catch the
                 *  "=foobar" and "foobar=" cases. These lines are ignored.
                 */
                *divider++ = '\0';

                strtrim(buffer);
                //if (!std::strlen(buffer)) continue;

                if (buffer[0] == '\0') {
                    continue;
                }

                strtrim(divider);
                //if (!std::strlen(divider)) continue;

                if (buffer == nullptr || buffer[0] == '\0') {
                    continue;
                }

                if (divider == nullptr || divider[0] == '\0') {

                    if (!KeepBlankEntries) {
                        //DEBUG_INFO("INIClass::Load(merge) - Skipping blank entry. Section \"%s\", Entry \"%s\".\n", section, buffer);
                        continue;
                    }

                    //DEBUG_INFO("INIClass::Load(merge) - Keeping blank entry. Section \"%s\", Entry \"%s\".\n", section, buffer);

                    /**
                     *  Ensure value is valid and 'blank' (otherwise it breaks entry checks).
                     */
                    divider = " ";
                }

                /**
                 *  Put the entry combination to the existing index.
                 */
                if (!Put_String(section, buffer, divider)) {
                    if (Vinifera_DeveloperMode) {
                        DEBUG_WARNING("INIClass::Load(merge) - Merge failed! Section:\"%s\" Entry:\"%s\" Value\"%s\"!\n", section, buffer, divider);
                    }
                    return false;
                }

            }

        }

    } else {

        /*
        ** Process a section. The buffer is prefilled with the section name line.
        */
        while (!end_of_file) {

            buffer[0] = ' ';
            char *ptr = std::strchr(buffer, ']');
            if (ptr) *ptr = '\0';
            strtrim(buffer);
            INISection * secptr = new INISection(strdup(buffer));
            if (secptr == nullptr) {
                DEBUG_ERROR("INIClass::Load() - Failed to create section '%s'!", buffer);
                Clear();
                return false;
            }

            /*
            ** Read in the entries of this section.
            */
            while (!end_of_file) {

                /*
                ** If this line is the start of another section, then bail out
                ** of the entry loop and let the outer section loop take
                ** care of it.
                */
                int len = Read_Line(file, buffer, sizeof(buffer), end_of_file);
                if (buffer[0] == '[' && std::strchr(buffer, ']') != nullptr) break;

                /**
                 *  If this line contains a comment, store it in the list.
                 * 
                 *  #BUGFIX: Added check for ';' to make sure the line contains valid a comment.
                 */
                int pre_indent_cursor = 0;
                int post_indent_cursor = 0;
                int comment_cursor = 0;
                char *comment_str = nullptr;

                INIComment *commentptr = nullptr;
                if (load_comments) {

                    char *comm = std::strchr(buffer, ';');
                    if (comm != nullptr) {
                        commentptr = new INIComment;
                        if (commentptr == nullptr) {
                            load_comments = false;
                        } else {
                            if (commptr_3 != nullptr) {
                                commptr_3->Next = commentptr;
                            } else {
                                commptr_3 = commentptr;
                            }
                            commentptr->Value = strdup(buffer);
#ifdef INI_DEBUG
                            if (Vinifera_DeveloperMode) {
                                DEBUG_INFO("INIClass::Load() - Found comment on line \"%s\", extracting.\n", comm);
                            }
#endif
                        }

                        comment_str = Extract_Line_Comment(buffer, &pre_indent_cursor, &post_indent_cursor, &comment_cursor);
                        if (comment_str != nullptr) {
                            comment_str = strdup(comment_str);
                        }
                    }
                }

                /*
                ** Determine if this line is a comment or blank line. Throw it out if it is, we have already stored it.
                */
                Strip_Comments(buffer);
                //if (len == 0 || buffer[0] == ';' || buffer[0] == '=') continue;

#ifdef INI_DEBUG
                DEBUG_INFO("INIClass::Load() - \"%s\".\n", buffer);
#endif

                if (len == 0) {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load() - Skipping blank line.\n");
#endif
                    continue;
                }

                if (buffer[0] == '=') {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load() - Skipping invalid line \"%s.\".\n", buffer);
#endif
                    continue;
                }

                if (buffer[0] == ';') {
#ifdef INI_DEBUG
                    DEBUG_INFO("INIClass::Load() - Skipping line comment \"%s.\".\n", buffer);
#endif
                    continue;
                }

                /**
                 *  The line isn't an obvious comment. Make sure that there is the "=" character
                 *  at an appropriate spot.
                 */
                char *divider = std::strchr(buffer, '=');
                if (!divider) continue;

                /**
                 *  Split the line into entry and value sections. Be sure to catch the
                 *  "=foobar" and "foobar=" cases. These lines are ignored.
                 */
                *divider++ = '\0';
                strtrim(buffer);
                //if (!std::strlen(buffer)) continue;

                if (buffer[0] == '\0') {
                    continue;
                }

                strtrim(divider);
                //if (!std::strlen(divider)) continue;

                if (divider == nullptr || divider[0] == '\0') {

                    if (!KeepBlankEntries) {
                        if (Vinifera_DeveloperMode) {
                            DEBUG_INFO("INIClass::Load() - Skipping blank entry value. Section \"%s\", Entry \"%s\".\n", secptr->Section, buffer);
                        }
                        continue;
                    }

                    if (Vinifera_DeveloperMode) {
                        DEBUG_INFO("INIClass::Load() - Keeping blank entry value. Section \"%s\", Entry \"%s\".\n", secptr->Section, buffer);
                    }

                    /**
                     *  Ensure value is valid and 'blank' (otherwise it breaks entry checks).
                     */
                    divider = " ";
                }

                INIEntry * entryptr = new INIEntry(strdup(buffer), strdup(divider), commentptr, strdup(comment_str), pre_indent_cursor, post_indent_cursor, comment_cursor);
                if (entryptr == nullptr) {

                  DEBUG_WARNING("INIClass::Load() - Failed to create entry \"%s\" with value \"%s\".\n", buffer, divider);

                    std::free(comment_str);

                    if (commentptr) {
                        INIComment *comptr;
                        do {
                            comptr = commentptr->Next;
                            delete commentptr;
                            commentptr = comptr;
                        } while (comptr);
                        commentptr = nullptr;
                    }

                    delete secptr;
                    Clear();
                    return false;
                }

            #ifdef INI_DEBUG
                if (comment_str) {
                  //DEBUG_INFO("INIClass::Load() - Added entry \"%s\" with comment \"%s\".\n", buffer, comment_str);
                } else {
                  //DEBUG_INFO("INIClass::Load() - Added entry \"%s\".\n", buffer);
                }
            #endif

                int id = entryptr->Index_ID();

                /**
                 *  Is this entry already present under this section?
                 */
                if (secptr->EntryIndex.Is_Present(id)) {

                    /**
                     *  Release builds don't need to worry about duplicate clash messages.
                     */
                    if (Vinifera_DeveloperMode) {
                        DEBUG_WARNING("INIClass::Load() - Duplicate entry detected!.\n");
                        Duplicate_CRC_Error("INIClass::Load()", secptr->Section, entryptr->Entry, id);
                    }

                    //std::free(comment_str);
                    //return false;
                }

                secptr->EntryIndex.Add_Index(id, entryptr);
                secptr->EntryList.Add_Tail(entryptr);

                commptr_3 = nullptr;

                // Free the comment string for the next loop.
                std::free(comment_str);
            }

            /*
            ** All the entries for this section have been parsed. If this section is blank, then
            ** don't bother storing it unless we have been specifically asked to.
            */
            if (secptr->EntryList.Is_Empty() && !KeepEmptySections) {
                delete secptr;
            } else {

                if (Vinifera_DeveloperMode) {
                    if (secptr->EntryList.Is_Empty() && KeepEmptySections) {
                      DEBUG_INFO("INIClass::Load() - Keeping empty section '%s'.\n", secptr->Section);
                    }
                }

                SectionIndex.Add_Index(secptr->Index_ID(), secptr);
                SectionList.Add_Tail(secptr);
            }
        }

    }

    if (load_comments) {

        commptr_3 = nullptr;

        delete LineComments;
        LineComments = nullptr;

        while (!end_of_file) {

            int count = Read_Line(file, buffer, sizeof(buffer), end_of_file);
            if (count <= 0) {
                continue;
            }

            // Check we don't have section.
            if (buffer[0] == '[' && std::strchr(buffer, ']') != nullptr) {
                continue;
            }

            // Make sure the line starts with or contains the comment char.
            if (buffer[0] != ';' && std::strchr(buffer, ';') != nullptr) {
                continue;
            }

            if (load_comments) {
                INIComment *commentptr = new INIComment;
                if (commentptr == nullptr) {
                    load_comments = false;
                } else {
                    if (commptr_3 != nullptr) {
                        commptr_3->Next = commentptr;
                    } else {
                        commptr_3 = commentptr;
                    }
                    commentptr->Value = strdup(buffer);
                }
            }

            if (load_comments) {
                LineComments = commptr_3;
            }

        }

    }

    /**
     *  Dump the database to the debugger output.
     */
#ifdef INI_DEBUG
    Dump_Database();
#endif

    return true;
}


int ViniferaINIClass::Save(const char *filename, bool save_comments) const
{
    FileAutoPtr file(TheWritingFileFactory, filename);
    return ViniferaINIClass::Save(*file, save_comments);
}


/***********************************************************************************************
 * ViniferaINIClass::Save -- Save the ini data to the file specified.                                  *
 *                                                                                             *
 *    Use this routine to save the ini data to the file specified. All existing data in the    *
 *    file, if it was present, is replaced.                                                    *
 *                                                                                             *
 * INPUT:   file  -- Reference to the file to write the INI data to.                           *
 *                                                                                             *
 * OUTPUT:  bool; Was the data written to the file?                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
int ViniferaINIClass::Save(FileClass & file, bool save_comments) const
{
    FilePipe pipe(file);
    return ViniferaINIClass::Save(pipe, save_comments);
}


/***********************************************************************************************
 * ViniferaINIClass::Save -- Saves the INI data to a pipe stream.                                      *
 *                                                                                             *
 *    This routine will output the data of the INI file to a pipe stream.                      *
 *                                                                                             *
 * INPUT:   pipe  -- Reference to the pipe stream to pump the INI image to.                    *
 *                                                                                             *
 * OUTPUT:  Returns with the number of bytes output to the pipe.                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
int ViniferaINIClass::Save(Pipe & pipe, bool save_comments) const
{
    static const char _newline[] = "\r\n";

    /**
     *  Total amount of bytes written to the stream.
     */
    int total = 0;

    /**
     *  Buffer of spaces to use for indentation.
     */
    char spacebuff[INI_MAX_LINE_LENGTH];
    std::memset(&spacebuff, ' ', sizeof(spacebuff));

    for (const INISection *sectionptr = SectionList.First(); sectionptr != nullptr; sectionptr = sectionptr->Next()) {

        /**
         *  If this is not a valid section node, break out of the loop.
         */
        if (!sectionptr->Is_Valid()) {
#ifdef INI_DEBUG
            DEBUG_WARNING("INIClass::Save() - Last section encountered!\n");
#endif
            break;
        }

        /**
         *  If we have no more comments to write, just write a new line char.
         */
        if (total > 0) {
            total += pipe.Put(_newline, std::strlen(_newline));

        } else if (save_comments) {
            if (total > 0 && sectionptr->Comments == nullptr) {
                total += pipe.Put(_newline, std::strlen(_newline));
            }
        }

        /**
         *  Write comments above sections.
         */
        if (save_comments) {
            for (const INIComment *i = sectionptr->Comments; i != nullptr; i = i->Next) {
                if (i->Value != nullptr) {
                    total += pipe.Put(i->Value, std::strlen(i->Value));
                    total += pipe.Put(_newline, std::strlen(_newline));
                }
            }
        }

        /**
         *  Output the section identifier.
         */
        total += pipe.Put("[", 1);
        total += pipe.Put(sectionptr->Section, std::strlen(sectionptr->Section));
        total += pipe.Put("]", 1);
        total += pipe.Put(_newline, std::strlen(_newline));

        /*
        ** Output all the entries and values in this section.
        */
        for (const INIEntry *entryptr = sectionptr->EntryList.First(); entryptr != nullptr; entryptr = entryptr->Next()) {

            /**
             *  If this is not a valid entry node, break out of the loop.
             */
            if (!entryptr->Is_Valid()) {
#ifdef INI_DEBUG
                DEBUG_WARNING("INIClass::Save() - Last entry encountered!\n");
#endif
                break;
            }

            /**
             *  Write comments above entries.
             */
            if (save_comments) {
                for (const INIComment *i = entryptr->Comments; i != nullptr; i = i->Next) {
                    if (i->Value != nullptr) {
                        total += pipe.Put(i->Value, std::strlen(i->Value));
                        total += pipe.Put(_newline, std::strlen(_newline));
                    }
                }
            }

            int pos = 0;

            int name_len = std::strlen(entryptr->Entry);
            int value_len = std::strlen(entryptr->Value);

            /**
             *  Write the entry name.
             */
            total += pipe.Put(entryptr->Entry, name_len);

            if (KeepFormatting) {

                /**
                 *  Write the indentation before the equals char.
                 */
                int preindent_len = std::min((entryptr->PreIndentCursor - name_len), int(INI_MAX_LINE_LENGTH));
                if (preindent_len > 0) {
                    total += pipe.Put(spacebuff, preindent_len);
                    pos += preindent_len;
                }
            }

            /**
             *  Next, write the equals char.
             */
            total += pipe.Put("=", 1);

            if (KeepFormatting) {

                /**
                 *  Write the indentation after the equals char.
                 */
                int postindent_len = std::min((entryptr->PostIndentCursor - pos - name_len - 1), int(INI_MAX_LINE_LENGTH));
                if (postindent_len > 0) {
                    total += pipe.Put(spacebuff, postindent_len);
                    pos += postindent_len;
                }
            }

            /**
             *  Then write the entry value.
             */
            total += pipe.Put(entryptr->Value, value_len);

            /**
             *  If this entry has a comment attached, write this out too.
             */
            if (save_comments) {
                if (entryptr->CommentString != nullptr) {

                    if (KeepFormatting) {

                        /**
                         *  Write the indentation for the comment itself.
                         */
                        int commentindent_len = std::min((entryptr->CommentCursor - pos - value_len - name_len - 1), int(INI_MAX_LINE_LENGTH));
                        if (commentindent_len > 0) {
                            total += pipe.Put(spacebuff, commentindent_len);
                            pos += commentindent_len;
                        }
                    }

                    /**
                     *  Write the comment char.
                     */
                    total += pipe.Put(";", 1);

                    /**
                     *  Write the comment.
                     */
                    total += pipe.Put(entryptr->CommentString, std::strlen(entryptr->CommentString));

                }

            }

            /**
             *  End of entry, write a carriage return.
             */
            total += pipe.Put(_newline, std::strlen(_newline));
        }

        /**
         *  After the last entry in this section, output an extra
         *  blank line for readability purposes.
         */
        if (sectionptr->Next() == nullptr) {
            total += pipe.Put(_newline, std::strlen(_newline));
        }
    }

    /**
     *  Write out any remaining line comments.
     */
    if (save_comments) {
        for (const INIComment *i = LineComments; i != nullptr; i = i->Next) {
            if (i->Value != nullptr) {
                total += pipe.Put(i->Value, std::strlen(i->Value));
                total += pipe.Put(_newline, std::strlen(_newline));
            }
        }
    }

    /**
     *  End the data stream.
     */
    total += pipe.End();

#ifdef INI_DEBUG
  DEBUG_INFO("INIClass::Save() - Finished\n");
#endif

    /**
     *  Dump the database to the debugger output.
     */
#ifndef NDEBUG
    Dump_Database();
#endif

    return total;
}


#ifdef INI_DEBUG
void ViniferaINIClass::Dump_Database() const
{
    if (!DumpDatabaseToDebuggerOutput) {
        return;
    }

    char buff[1024];

    DEBUG_INFO("\nAbout to dump ini database...\n\n");

    DEBUG_INFO("SectionIndex.Count: %d.\n", SectionIndex.Count());

    for (const INISection *sectionptr = SectionList.First(); sectionptr != nullptr && sectionptr->Is_Valid(); sectionptr = sectionptr->Next()) {

        DEBUG_INFO(" Section: \"%s\"  ID:0x%08X  EntryIndex.Count: %d.\n", sectionptr->Section, sectionptr->Index_ID(), sectionptr->EntryIndex.Count());

        for (INIComment *i = sectionptr->Comments; i != nullptr; i = i->Next) {
            if (i->Value) {
                DEBUG_INFO(" Comment: \"%s\".", i->Value);
            }
        }

        int index = 0;
        for (const INIEntry *entryptr = sectionptr->EntryList.First(); entryptr != nullptr && entryptr->Is_Valid(); entryptr = entryptr->Next()) {

            DEBUG_INFO("  Entry(%d): \"%s\" \tID: 0x%08X \tValue: \"%s\" \tComment: \"%s\".\n", index++, entryptr->Entry, entryptr->Index_ID(), entryptr->Value, entryptr->CommentString != nullptr ? entryptr->CommentString : "");

            for (INIComment *i = entryptr->Comments; i != nullptr; i = i->Next) {
                if (i->Value) {
                    DEBUG_INFO("    Comment: \"%s\".", i->Value);
                }
            }
        }
    }

    for (INIComment *i = LineComments; i != nullptr; i = i->Next) {
        if (i->Value) {
            DEBUG_INFO("LineComments: \"%s\".", i->Value);
        }
    }

    DEBUG_INFO("\nDump of ini database finished.\n\n");
}
#endif


/***********************************************************************************************
 * ViniferaINIClass::Find_Section -- Find the specified section within the INI data.                   *
 *                                                                                             *
 *    This routine will scan through the INI data looking for the section specified. If the    *
 *    section could be found, then a pointer to the section control data is returned.          *
 *                                                                                             *
 * INPUT:   section  -- The name of the section to search for. Don't enclose the name in       *
 *                      brackets. Case is NOT sensitive in the search.                         *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the INI section control structure if the section was     *
 *          found. Otherwise, nullptr is returned.                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *   11/02/1996 JLB : Uses index manager.                                                      *
 *=============================================================================================*/
const ViniferaINIClass::INISection * ViniferaINIClass::Find_Section(const char *section) const
{
    if (section != nullptr) {
        uint32_t crc = CRC(section);
        if (SectionIndex.Is_Present(crc)) {

            /**
             *  #BUGFIX:
             *  Changed due to Fetch_Index removed from IndexClass because of a bug
             *  with the return value being optimised out in non-debug builds.
             * 
             *  Credit to tomsons26 for catching this.
             */
            return SectionIndex[crc]; // SectionIndex.Fetch_Index(crc);
        }
    }
    return nullptr;
}


/***********************************************************************************************
 * ViniferaINIClass::Find_Entry -- Find specified entry within section.                                *
 *                                                                                             *
 *    This support routine will find the specified entry in the specified section. If found,   *
 *    a pointer to the entry control structure will be returned.                               *
 *                                                                                             *
 * INPUT:   section  -- Pointer to the section name to search under.                           *
 *                                                                                             *
 *          entry    -- Pointer to the entry name to search for.                               *
 *                                                                                             *
 * OUTPUT:  If the entry was found, then a pointer to the entry control structure will be      *
 *          returned. Otherwise, nullptr will be returned.                                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/02/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
const ViniferaINIClass::INIEntry * ViniferaINIClass::Find_Entry(const char *section, const char *entry) const
{
    const INISection * secptr = nullptr;

    if (!secptr) {
        secptr = Find_Section(section);
    }

    if (secptr != nullptr) {
        return secptr->Find_Entry(entry);
    }

    return nullptr;
}
