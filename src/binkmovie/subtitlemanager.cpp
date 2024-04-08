
#include "subtitlemanager.h"
#include "subtitleparser.h"
#include "subtitle.h"
#include "filestraw.h"
#include "ccfile.h"


// Instantiate a subtitle manager for the specified movie.
SubTitleManagerClass * SubTitleManagerClass::Create(const Wstring &filename, const Wstring &subtitlefilename, FontClass * font)
{
    if (filename.Is_Empty() || !filename.Get_Length()) {
        return nullptr;
    }

    if (subtitlefilename.Is_Empty() && !font) {
        return nullptr;
    }

    // Create subtitle manager for the vqa
    SubTitleManagerClass * instance = new SubTitleManagerClass();
    assert(instance != nullptr);

    if (instance != nullptr) {
        instance->Set_Font(font);

        // Retrieve moviename
        char fname[_MAX_FNAME];
        _splitpath(filename, nullptr, nullptr, fname, nullptr);
        bool loaded = instance->Load_Sub_Titles(fname, subtitlefilename);

        if (loaded == false) {
            delete instance;
            return nullptr;
        }
    }

    return instance;
}


SubTitleManagerClass::SubTitleManagerClass() :
    SubTitles(nullptr),
    SubTitleIndex(0),
    ActiveSubTitle(nullptr)
{
}


SubTitleManagerClass::~SubTitleManagerClass()
{
    // Release subtitle entries
    if (SubTitles != nullptr) {
        for (int index = 0; index < SubTitles->Count(); index++) {
            delete (*SubTitles)[index];
        }

        delete SubTitles;
    }
}


void SubTitleManagerClass::Set_Font(FontClass * font)
{
    if (font) {
        Renderer.Set_Font(font);
    }
}


bool SubTitleManagerClass::Load_Sub_Titles(const Wstring &moviename, const Wstring &subtitlefilename)
{
    if (moviename.Is_Empty() || moviename.Get_Length()) {
        return false;
    }

    if ((subtitlefilename == nullptr) || subtitlefilename.Get_Length()) {
        return false;
    }

    CCFileClass file(subtitlefilename);

    if (!file.Is_Available()) {
        return false;
    }

     FileStraw input(file);
     SubTitleParserClass parser(input);
     
     SubTitles = parser.Get_Sub_Titles(moviename);

     if (SubTitles == nullptr) {
         return false;
    }

    // TODO: Make sure entries are sorted by time.

    return true;
}


// Handle subtitle processing. This must be called each frame advance.
bool SubTitleManagerClass::Process(unsigned long movieTime)
{
    if (SubTitles == nullptr) {
        return false;
    }

    bool update = false;

    for (;;) {
        // Terminate if there aren't more subtitles
        if (SubTitleIndex >= SubTitles->Count()) {
            break;
        }

        // Get the next subtitle
        SubTitleClass* subtitle = (*SubTitles)[SubTitleIndex];
        assert(subtitle != nullptr);

        // Check the display time against the current movie time. If it is time
        // to display the subtitle then send a subtitle event to the client.
        unsigned long displayTime = subtitle->Get_Display_Time();

        // If its not time then we are done.
        if (displayTime > movieTime) {
            break;

        } else {
            // Make this subtitle the active one
            ActiveSubTitle = subtitle;

            // Advance to the next subtitle entry
            SubTitleIndex++;

            Draw_Sub_Title(subtitle);
            update = true;

//            WWDEBUG_SAY(("SubTitle: %04d @ %u\n", SubTitleIndex, movieTime));
        }
    }

    // If the active subtitles duration has expired then remove it as being active.
    if (ActiveSubTitle != nullptr) {
        SubTitleClass* subtitle = ActiveSubTitle;
        unsigned long expireTime = subtitle->Get_Display_Time() + subtitle->Get_Display_Duration();

        if (movieTime >= expireTime) {
            ActiveSubTitle = nullptr;

            // Erase subtitle
            Renderer.Reset();
            update = true;
        }
    }

    return update;
}


void SubTitleManagerClass::Reset(void)
{
    SubTitleIndex = 0;
    ActiveSubTitle = nullptr;
}


void SubTitleManagerClass::Draw_Sub_Title(const SubTitleClass* subtitle)
{
    assert(subtitle != nullptr);

    Renderer.Reset();

    Wstring & string = subtitle->Get_Caption();

    int w,h,bits;
    bool windowed;
    WW3D::Get_Device_Resolution(w,h,bits,windowed);
    Vector2 extents=Renderer.Get_Text_Extents( string );

    // Assume left justification
    int xPos = 0;
    int yPos = subtitle->Get_Line_Position() * (h/16);
    int xSize=extents[0];

    SubTitleClass::AlignmentEnum align = subtitle->Get_Alignment();

    if (align == SubTitleClass::Center)    {
        xPos = ((w - xSize) / 2);
    }
    else if (align == SubTitleClass::Right) {
        xPos = (w - xSize);
    }

    Renderer.Set_Location(Vector2(xPos,yPos));
    Renderer.Build_Sentence(string);

    // Set font color
    unsigned long rgbColor = subtitle->Get_RGB_Color()|0xff000000;

    Renderer.Draw_Sentence(rgbColor);
}


void SubTitleManagerClass::Render()
{
    Renderer.Render();
}
