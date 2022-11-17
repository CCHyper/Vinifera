/*******************************************************************************
/*               O P E N  S O U R C E -- P R O J E C T T S Y R                **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          INI.H
 *
 *  @author        Joe L. Bostic (see notes below)
 *
 *  @contributors  CCHyper (Extensions and rewrites)
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
#pragma once

#include "always.h"
#include "ini.h"


/*******************************************************************************
 *  @class   ViniferaINIClass
 *
 *  @brief   This is an INI database handler class. It handles a database with a disk
 *           format identical to the INI files commonly used by Windows.
 */
/**
 *  x
 */
class ViniferaINIClass : public INIClass
{
    public:
        bool Load(const char *filename, bool load_comments = false);
        bool Load(FileClass &file, bool load_comments = false);
        bool Load(Straw &straw, bool load_comments = false);
        int Save(const char *filename, bool save_comments = false) const;
        int Save(FileClass &file, bool save_comments = false) const;
        int Save(Pipe &straw, bool save_comments = false) const;

#ifndef NDEBUG
        void Dump_Database() const;
#endif

        static void Keep_Formatting(bool keep) { KeepFormatting = keep; }
        static void Keep_Empty_Sections(bool keep) { KeepEmptySections = keep; }
        static void Keep_Blank_Entries(bool keep) { KeepBlankEntries = keep; }
        static void Output_To_Debugger(bool output) { DumpDatabaseToDebuggerOutput = output; }

    protected:
        /**
         *  Utility routines to help find the appropriate section and entry objects.
         */
        const INISection * Find_Section(const char *section) const;
        const INIEntry * Find_Entry(const char *section, const char *entry) const;

        void Initialize();
        void Shutdown();

        static void Strip_Comments(char *buffer);
        static char *Extract_Line_Comment(char *buffer, int *pre_indent_cursor, int *entry_indent, int *comment_cursor);

        static bool Line_Contains_Section(const char *buffer);
        static bool Line_Contains_Entry(const char *buffer);

        static void Duplicate_CRC_Error(const char *function_name, const char *section, const char *entry, uint32_t crc);
        static int CRC(const char *string);

    private:
        /**
         *  Keep the ini formatting when loading an ini [only applies when resaving]?
         */
        static bool KeepFormatting;

        /**
         *  Keep sections with no entries?
         */
        static bool KeepEmptySections;

        /**
         *  Keep blank entries [stores them internally as " "]?
         */
        static bool KeepBlankEntries;

#ifndef NDEBUG
        /**
         *  Dump the contents of the database to the debugger output window (if one is attached)?
         *  For example, set this before a Load call, and reset afterwards.
         */
        static bool DumpDatabaseToDebuggerOutput;
#endif
};
