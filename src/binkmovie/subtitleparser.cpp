#include "subtitleparser.h"
#include "subtitle.h"
#include "straw.h"
#include "readline.h"
#include "strtrim.h"
#include "tibsun_defines.h"
#include "debughandler.h"
#include <wchar.h>
#include <stdlib.h>

// Subtitle control file parsing tokens
#define BEGINMOVIE_TOKEN "BeginMovie"
#define ENDMOVIE_TOKEN   "EndMovie"
#define TIMEBIAS_TOKEN   "TimeBias"
#define TIME_TOKEN       "Time"
#define DURATION_TOKEN   "Duration"
#define POSITION_TOKEN   "Position"
#define COLOR_TOKEN      "Color"
#define TEXT_TOKEN       "Text"

unsigned long Decode_Time_String(Wstring & string);
void Parse_Time(Wstring & string, SubTitleClass * subTitle);
void Parse_Duration(Wstring & string, SubTitleClass * subTitle);
void Parse_Position(Wstring & string, SubTitleClass * subTitle);
void Parse_Color(Wstring & string, SubTitleClass * subTitle);
void Parse_Text(Wstring & string, SubTitleClass * subTitle);

SubTitleParserClass::TokenHook SubTitleParserClass::TokenHooks[] =
{
    {TIME_TOKEN, Parse_Time},
    {DURATION_TOKEN, Parse_Duration},
    {POSITION_TOKEN, Parse_Position},
    {COLOR_TOKEN, Parse_Color},
    {TEXT_TOKEN, Parse_Text},
    {nullptr, nullptr}
};


SubTitleParserClass::SubTitleParserClass(Straw & input) :
    Input(input),
    LineNumber(0)
{
}


SubTitleParserClass::~SubTitleParserClass()
{
}


DynamicVectorClass<SubTitleClass *> * SubTitleParserClass::Get_Sub_Titles(const char * moviename)
{
    DynamicVectorClass<SubTitleClass *> * subTitleCollection = nullptr;
    
    // Find the movie marker
    if (Find_Movie_Entry(moviename) == true) {

        // Allocate container to hold subtitles
        subTitleCollection = new DynamicVectorClass<SubTitleClass*>;
        assert(subTitleCollection != nullptr);

        if (subTitleCollection != nullptr) {
            for (;;) {
                // Retrieve a line from the control file
                Wstring & string = Get_Next_Line();

                if ((string != nullptr) && (wcslen(string) > 0)) {

                    // Check for subtitle entry markers
                    if ((string[0] == '<') && (string[wcslen(string) - 1] == '>')) {
                        // Trim off markers
                        string++;
                        string[wcslen(string) - 1] = 0;
                        strtrim(string);

                        // Ignore empty caption
                        if (wcslen(string) == 0) {
                            continue;
                        }

                        // Create a new SubTitleClass
                        SubTitleClass* subTitle = new SubTitleClass();
                        assert(subTitle != nullptr);

                        if (subTitle == nullptr) {
                            DEBUG_ERROR(("***** Failed to create SubTitleClass!\n"));
                            break;
                        }

                        if (Parse_Sub_Title(string, subTitle) == true)    {
                            subTitleCollection->Add(subTitle);
                        }
                        else {
                            delete subTitle;
                        }

                        continue;
                    }

                    // Terminate if end movie token encountered.
                    if (wcsnicmp(string, ENDMOVIE_TOKEN, wcslen(ENDMOVIE_TOKEN)) == 0) {
                        break;
                    }
                }
            }

            if (subTitleCollection->Count() == 0) {
                delete subTitleCollection;
                subTitleCollection = nullptr;
            }
        }
    }

    return subTitleCollection;
}


bool SubTitleParserClass::Find_Movie_Entry(const char* moviename)
{
    // Convert the moviename into Unicode
    assert(moviename != nullptr);
    wchar_t wideName[32];
    mbstowcs(wideName, moviename, 32);

    do {
        // Retrieve line of text
        Wstring & string = Get_Next_Line();

        // Terminate if no string read.
        if (string == nullptr) {
            break;
        }

        // Look for begin movie token
        if (wcsnicmp(string, BEGINMOVIE_TOKEN, wcslen(BEGINMOVIE_TOKEN)) == 0) {

            // Get moviename following the token
            Wstring & ptr = strchr(string, ' ');

            // Check for matching moviename
            if (ptr != nullptr) {
                strtrim(ptr);

                if (strcmpi(ptr, wideName) == 0) {
                    DEBUG_INFO(("Found movie entry %s\n", moviename));
                    return true;
                }
            }
        }
    } while (true);

    return false;
}


bool SubTitleParserClass::Parse_Sub_Title(Wstring & string, SubTitleClass* subTitle)
{
    // Parameter check
    assert(string != nullptr);
    assert(subTitle != nullptr);

    for (;;) {

        // Find token separator
        Wstring & separator = strchr(string, '=');

        if (separator == nullptr) {
            DEBUG_INFO("Error on line %d: syntax error\n", Get_Line_Number());
            return false;
        }

        // nullptr terminate token part
        *separator++ = '\0';

        // Tokens are to the left of the separator
        Wstring & token = string;
        strtrim(token);

        // Parameters are to the right of the separator
        Wstring & param = separator;
        strtrim(param);

        // Quoted parameters are treated as literals (ignore contents)
        if (param[0] == '"') {

            // Skip leading quote
            param++;

            // Use next quote to mark end of parameter
            separator = strchr(param, '"');

            if (separator == nullptr) {
                DEBUG_INFO("Error on line %d: mismatched quotes\n", Get_Line_Number());
                return false;
            }

            // nullptr terminate parameter
            *separator++ = 0;

            // Skip any comma following a literal string since we used the trailing
            // quote to terminate the tokens parameters
            strtrim(separator);
            
            if (*separator == ',') {
                separator++;
            }

            // Advance string past quoted parameter
            string = separator;
        }
        else {
            // Look for separator to next token
            separator = wcspbrk(param, ", ");

            if (separator != nullptr) {
                *separator++ = 0;
                string = separator;
            }
            else {
                string = "";
            }
        }

        // Error on empty tokens
        if (wcslen(token) == 0) {
            DEBUG_INFO(("Error on line %d: missing token\n", Get_Line_Number()));
            return false;
        }

        // Parse current token
        Parse_Token(token, param, subTitle);

        // Prepare for next token
        strtrim(string);

        if (wcslen(string) == 0) {
            break;
        }
    }

    return true;
}


void SubTitleParserClass::Parse_Token(Wstring & token, Wstring & param, SubTitleClass * subTitle)
{
    // Parameter check
    assert(token != nullptr);
    assert(subTitle != nullptr);

    if (token != nullptr) {
        int index = 0;

        while (TokenHooks[index].Token != nullptr) {
            TokenHook& hook = TokenHooks[index];

            if (strcmpi(hook.Token, token) == 0) {
                assert(subTitle != nullptr);
                hook.Handler(param, subTitle);
                return;
            }

            index++;
        }
    }
}


// Retrieve the next line of text from the control file.
Wstring SubTitleParserClass::Get_Next_Line(void)
{
    bool eof = false;

    while (eof == false) {

        // Read in a line of text
        Read_Line(Input, Buffer, LINE_MAX, eof);
        LineNumber++;

        // Remove whitespace
        Wstring string = strtrim(Buffer);

        // Skip comments and blank lines
        if (string.Get_Length() > 0 && string[0] != ';') {
            return string;
        }
    }

    return nullptr;
}


// Convert a time string in the format hh:mm:ss:tt into 1/60 second ticks.
unsigned long Decode_Time_String(Wstring & string)
{
    assert(string != nullptr);

    char buffer[12];
    strncpy(buffer, string.Peek_Buffer(), 12);
    buffer[11] = '\0';

    char * ptr = &buffer[0];

    // Isolate hours part
    char * separator = strchr(ptr, ':');
    assert(separator != nullptr);
    *separator++ = 0;
    unsigned long hours = strtol(ptr, nullptr, 10);

    // Isolate minutes part
    ptr = separator;
    separator = strchr(ptr, ':');
    assert(separator != nullptr);
    *separator++ = 0;
    unsigned long minutes = strtol(ptr, nullptr, 10);

    // Isolate seconds part
    ptr = separator;
    separator = strchr(ptr, ':');
    assert(separator != nullptr);
    *separator++ = 0;
    unsigned long seconds = strtol(ptr, nullptr, 10);

    // Isolate hundredth part (1/100th of a second)
    ptr = separator;
    unsigned long hundredth = strtol(ptr, nullptr, 10);

    unsigned long time = (hours * TICKS_PER_HOUR);
    time += (minutes * TICKS_PER_MINUTE);
    time += (seconds * TICKS_PER_SECOND);
    time += ((hundredth * TICKS_PER_SECOND) / 100);

    return time;
}


void Parse_Time(Wstring & param, SubTitleClass * subTitle)
{
    assert(param != nullptr);
    assert(subTitle != nullptr);
    unsigned long time = Decode_Time_String(param);
    subTitle->Set_Display_Time(time);
}


void Parse_Duration(Wstring & param, SubTitleClass * subTitle)
{
    assert(param != nullptr);
    assert(subTitle != nullptr);
    unsigned long time = Decode_Time_String(param);

    if (time > 0) {
        subTitle->Set_Display_Duration(time);
    }
}


void Parse_Position(Wstring & param, SubTitleClass * subTitle)
{
    static struct
    {
        const Wstring & Name;
        SubTitleClass::AlignmentEnum Align;
        } _alignLookup[] = {
            { "Left", SubTitleClass::Left },
            { "Right", SubTitleClass::Right },
            { "Center", SubTitleClass::Center },
            { nullptr, SubTitleClass::Center }
    };

    assert(subTitle != nullptr);
    assert(param != nullptr);

    char * ptr = param.Peek_Buffer();

    // Line position
    char * separator = strchr(ptr, ':');

    if (separator != nullptr) {
        *separator++ = '\0';
        int linePos = strtol(ptr, nullptr, 0);
        subTitle->Set_Line_Position(linePos);
        ptr = separator;
    }

    // Justification
    SubTitleClass::AlignmentEnum align = SubTitleClass::Center;
    int index = 0;

    while (_alignLookup[index].Name != nullptr) {
        if (_alignLookup[index].Name == ptr) {
            align = _alignLookup[index].Align;
            break;
        }

        index++;
    }

    subTitle->Set_Alignment(align);
}


void Parse_Color(Wstring & param, SubTitleClass* subTitle)
{
    assert(param != nullptr);
    assert(subTitle != nullptr);

    char * ptr = param.Peek_Buffer();

    char * separator = strchr(ptr, ':');
    *separator++ = '\0';
    unsigned char red = (unsigned char)strtol(ptr, nullptr, 10);
    
    ptr = separator;
    separator = strchr(ptr, ':');
    *separator++ = '\0';
    unsigned char green = (unsigned char)strtol(ptr, nullptr, 10);

    ptr = separator;
    unsigned char blue = (unsigned char)strtol(ptr, nullptr, 10);

    subTitle->Set_RGB_Color(red, green, blue);
}


void Parse_Text(Wstring & param, SubTitleClass * subTitle)
{
    assert(param != nullptr);
    assert(subTitle != nullptr);

    subTitle->Set_Caption(param);
}
