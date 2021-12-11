/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUBTITLEPARSER.CPP
 *
 *  @author        CCHyper, tomsons26
 *
 *  @brief         
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "subtitleparser.h"
#include "subtitle.h"
#include "straw.h"
#include "readline.h"
#include "strtrim.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Structures for the parsing arrays.
 */
typedef struct SubTitleEntryStruct
{
    const char *EntryName;
    void (*ParserFunc)(char *, SubTitle *);
} SubTitleEntryStruct;

typedef struct SubTitleAlignmentStruct
{
    const char *AlignmentName;
    const SubTitleAlignType Alignment;
} SubTitleAlignmentStruct;

typedef struct SubTitleColorStruct
{
    const char *ColorName;
    const ColorType Color;
} SubTitleColorStruct;


/**
 *  Available text colors.
 */
static SubTitleColorStruct SubTitleColorNames[] = {
    { "None", COLOR_TBLACK },
    { "Purple", COLOR_PURPLE },
    { "Cyan", COLOR_CYAN },
    { "Green", COLOR_GREEN },
    { "LightGreen", COLOR_LTGREEN },
    { "Yellow", COLOR_YELLOW },
    { "Pink", COLOR_PINK },
    { "Brown", COLOR_BROWN },
    { "Red", COLOR_RED },
    { "LightCyan", COLOR_LTCYAN },
    { "LightBlue", COLOR_LTBLUE },
    { "Blue", COLOR_BLUE },
    { "Black", COLOR_BLACK },
    { "Grey", COLOR_GREY },
    { "LightGrey", COLOR_LTGREY },
    { "White", COLOR_WHITE },
};


static unsigned Read_Time_Format(char *string)
{
    char buffer[12];

    std::strncpy(buffer, string, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';

    /**
     *  
     */
    char *pc1 = std::strchr(buffer, ':');
    if (!pc1) return 0;
    *pc1++ = '\0';
    int h = std::strtoul(buffer, nullptr, 10);

    char *pc2 = std::strchr(pc1, ':');
    if (!pc2) return 0;
    *pc2++ = '\0';
    int m = std::strtoul(pc2, nullptr, 10);

    char *pc3 = std::strchr(pc2, ':');
    if (!pc3) return 0;
    *pc3++ = '\0';
    int s = std::strtoul(pc3, nullptr, 10);

    int ms = std::strtoul(pc3, nullptr, 10);

    /**
     *  Calculate and pack the time value into a unsigned int.
     */
    return (60 * ms / 100 + 60 * (s + 60 * (m + 60 * h)));
}

static void Read_Time(char *string, SubTitle *subtitle)
{
    ASSERT(string != nullptr);
    ASSERT(subtitle != nullptr);

    subtitle->Time = Read_Time_Format(string);
}

static void Read_Duration(char *string, SubTitle *subtitle)
{
    ASSERT(string != nullptr);
    ASSERT(subtitle != nullptr);

    int duration = Read_Time_Format(string);
    if (duration) {
        subtitle->Time = duration;
    }
}

static void Read_Position(char *string, SubTitle *subtitle)
{
    ASSERT(string != nullptr);
    ASSERT(subtitle != nullptr);

    static SubTitleAlignmentStruct SubTitlePositions[] = {
        { "Left", ALIGN_LEFT },
        { "Right", ALIGN_RIGHT },
        { "Center", ALIGN_CENTER },
        { '\0', ALIGN_CENTER }
    };

    if (string) {

        char *cmd = string;

        /**
         *  Default values. 
         */
        subtitle->YAdjust = 15;
        subtitle->XAlignment = ALIGN_CENTER;

        /**
         *  Search for the separate character. The expected format here
         *  is "YAdjust:XAlignment", if the YAdjust is not present then
         *  we assume the next string will be the XAlignment value.
         */
        char *pc = std::strchr(string, ':');
        if (pc) {

            /**
             *  Convert the string number to a integer.
             */
            subtitle->YAdjust = std::strtol(string, nullptr, 10);

            /**
             *  #BUGFIX:
             *  Clamp the value to the max of 15 otherwise numbers higher
             *  than this go off the screen.
             */
            subtitle->YAdjust = std::clamp(subtitle->YAdjust, 0, 15);

            cmd = ++pc;
        }

        for (int i = 0; i < ARRAY_SIZE(SubTitlePositions); ++i) {
            if (!strcasecmp(SubTitlePositions[i].AlignmentName, cmd)) {
                subtitle->XAlignment = SubTitlePositions[i].Alignment;
                return;
            }
        }
    }
}

static void Read_Color(char *string, SubTitle *subtitle)
{
    ASSERT(string != nullptr);
    ASSERT(subtitle != nullptr);

    for (int index = 0; index < ARRAY_SIZE(SubTitleColorNames); ++index) {

        /**
         *  Text can not be transparent!
         */
        if (!strcasecmp(SubTitleColorNames[index].ColorName, SubTitleColorNames[COLOR_TBLACK].ColorName)) {
            continue;
        }

        if (!strcasecmp(SubTitleColorNames[index].ColorName, string)) {
            subtitle->Set_Color(SubTitleColorNames[index].Color);            
            break;
        }
    }
}

static void Read_Back_Color(char *string, SubTitle *subtitle)
{
    for (int index = 0; index < ARRAY_SIZE(SubTitleColorNames); ++index) {
        if (!strcasecmp(SubTitleColorNames[index].ColorName, string)) {
            subtitle->Set_Color(SubTitleColorNames[index].Color);            
            break;
        }
    }
}

static void Read_Text(char *string, SubTitle *subtitle)
{
    ASSERT(string != nullptr);
    ASSERT(subtitle != nullptr);

    subtitle->Set_Text(string);
}


/**
 *  Execute the parser for this entry. 
 * 
 *  @author: CCHyper
 */
static void Parse_Entry(char *entry, char *value, SubTitle *subtitle)
{
    static SubTitleEntryStruct SubTitleEntrys[] = {
        { "Time", Read_Time },
        { "Duration", Read_Duration },
        { "Position", Read_Position },
        { "Color", Read_Color },
        { "Background", Read_Back_Color },
        { "Text", Read_Text },
    };

    if (!entry) {
        DEBUG_WARNING("Invalid buffer in Parse_Entry!\n");
        return;
    }

    for (int i = 0; i < ARRAY_SIZE(SubTitleEntrys); ++i) {
        if (!strcasecmp(SubTitleEntrys[i].EntryName, entry)) {
            SubTitleEntrys[i].ParserFunc(value, subtitle);
            return;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
SubTitleParser::SubTitleParser(Straw &straw) :
    FileHandler(nullptr),
    LineBuffer(),
    CurrentLine(0)
{
    FileHandler = &straw;
    ASSERT(FileHandler != nullptr);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
SubTitleParser::~SubTitleParser()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
DynamicVectorClass<SubTitle *> * SubTitleParser::Parse(const char *moviename)
{
    if (!Find_Section(moviename)) {
        DEV_DEBUG_WARNING("SubTitle: Failed to find definition for '%s'.\n", moviename);
        return nullptr;
    }

    DynamicVectorClass<SubTitle *> *subvec = new DynamicVectorClass<SubTitle *>();
    if (!subvec) {
        return nullptr;
    }

    while (true) {

        char *v0 = nullptr;
        do  {

            char *linebuff = nullptr;
            while (true) {

                linebuff = Read_Line();

                /**
                 *  Walk over any white space.
                 */
                char *linebuffptr = linebuff;
                while (isspace(*linebuffptr)) {
                    ++linebuffptr;
                    if (*linebuffptr == '<') {
                        break;
                    }
                }

                if (/**linebuff == '<' && */linebuff[std::strlen(linebuff)-1] == '>') {
                    break;
                }

                if (!strncasecmp(linebuff, "EndMovie", std::strlen("EndMovie"))) {
                    break;
                }
            }

            //v0 = (linebuff+1);

            // skip indentation white space.
            v0 = std::strstr(linebuff, "<");

            // Skip line begin "<"
            v0 += 1;

            // Remove line end ">"
            v0[std::strlen(v0)-1] = '\0';
            strtrim(v0);

        } while (!std::strlen(v0));

        SubTitle *subtitle = new SubTitle();
        if (!subtitle) {
            DEBUG_ERROR("SubTitleParser::Parse() - Failed to create SubTitle!\n");
            break;
        }

        if (Parse_Line(v0, subtitle)) {
            subvec->Add(subtitle);
        } else {
            delete subtitle;
        }
    }

    if (!subvec->Count()) {
        delete subvec;
        return nullptr;
    }

    return subvec;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool SubTitleParser::Find_Section(const char *moviename)
{
    /**
     *  Read a line stream into the buffer.
     */
    char *linebuff = Read_Line();

    if (!linebuff) {
        return false;
    }

    /**
     *  Check if this is is a valid entry section.
     */
    if (strncasecmp(linebuff, "BeginMovie", std::strlen("BeginMovie"))) {
        return false;
    }

    /**
     *  Search the string for the space separator. The expected format of a subtitle
     *  entry is "BeginMovie MOVIENAME", so if a space was found in the line buffer
     *  then the next string should be the movie entry name. The pointer returned 
     *  from strchr() if sucessfully found will be to the start of the "MOVIENAME"
     *  in the example mentioned.
     */
    char *entryname = std::strchr(linebuff, ' ');
    if (!entryname) {
        return false;
    }

    /**
     *  Now remove any whitespace from the buffer.
     */
    strtrim(entryname);

    if (!strcasecmp(entryname, moviename)) {
        DEBUG_INFO("SubTitleParser::Find_Section() - Found movie entry \"%s\"\n", moviename);
        return true;
    }

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool SubTitleParser::Parse_Line(const char *line, SubTitle *subtitle)
{
    char *linebuff = (char *)line;

    if (!linebuff) {
        DEBUG_ERROR("SubTitleParser::Parse_Line() - Error on line %d: Line buffer is null!\n", CurrentLine);
        return false;
    }

    char *token = std::strchr(linebuff, '=');
    if (!token) {
        DEBUG_ERROR("SubTitleParser::Parse_Line() - Error on line %d: Invalid line!\n", CurrentLine);
        return false;
    }

    while (token) {

        //if (!token) {
        //    DEBUG_ERROR("SubTitleParser::Parse_Line() - Error on line %d: Syntax error!\n", CurrentLine);
        //    return false;
        //}

        /**
         *  Remove the token char and trim and whitespace. This will
         *  leave us with the entry name hopefully.
         */
        *token = '\0';
        char *entry = linebuff;
        strtrim(entry);

        /**
         *  Fetch start of the value an trim any additional whitespace.
         */
        char *value = ++token;
        strtrim(value);

        /**
         *  Is this entry a text entry (as its the only one in quotations)?
         */
        if (*value == '"') {

            // skip over the quotation char to get the string value.
            ++value;

            // fetch value and find the close quote.
            char *tmp = std::strchr(value, '"');

            if (!tmp) {
                DEBUG_ERROR("SubTitleParser::Parse_Line() - Error on line %d: Mismatched quotes!\n", CurrentLine);
                return false;
            }

            // Found it, now remove the closing quote
            *tmp = '\0';

            // fetch the next entry/value pointer.

            //char *tmp = tmptok+1;

            strtrim(value);

            if (*value == ',') {
                ++value;
            }

            linebuff = value;

        } else {

            // fetch the next entry/value pointer.

            char *tmp = std::strpbrk(value+1, ", ");

            if (!tmp) {
                linebuff = '\0';

            } else {
                // remove the separator.
                *tmp = '\0';

                // skip over the removed separator.
                linebuff = tmp+1;
            }

        }

        if (*entry == '\0' || !std::strlen(entry)) {
            DEBUG_ERROR("SubTitleParser::Parse_Line() - Error on line %d: Missing token!\n", CurrentLine);
            return false;
        }

        // parse the entry/value pair.
        Parse_Entry(entry, value, subtitle);

        // remove any existing whitespace from the buffer.
        strtrim(linebuff);

        DEBUG_DBG_OUTPUT(linebuff);
        DEBUG_DBG_OUTPUT("\n");

        if (!linebuff) {
            DEBUG_ERROR("SubTitleParser::Parse_Line() - Line %d buffer is null!\n", CurrentLine);
            return false;
        }

        /**
         *  We have parsed the line, if the line length is empty then we have
         *  reached the end of the line and we can exit.
         */
        if (*linebuff == '\0' || !std::strlen(linebuff)) {
            DEBUG_INFO("SubTitleParser::Parse_Line() - End of line %d reached.\n", CurrentLine);
            break;
        }

        /**
         *  Fetch the token pointer to the next entry.
         */
        token = std::strchr(linebuff, '=');
        DEBUG_DBG_OUTPUT(token);
        DEBUG_DBG_OUTPUT("\n");

    }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
char *SubTitleParser::Read_Line()
{
    bool done = false;
    char *buffptr = LineBuffer;

    while (true) {

        *buffptr = '\0';

        int total = ::Read_Line(*FileHandler, buffptr, sizeof(LineBuffer), done);
        if (!total) {
            return nullptr;
        }

        ++CurrentLine;

        char *str = strtrim(buffptr);

        if (std::strlen(str)) {
            if (*str != ';') {
                return str;
            }
        }

        if (done) {
            return nullptr;
        }
    }
}
