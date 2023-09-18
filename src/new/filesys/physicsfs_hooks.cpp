/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PHYSICSFS_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for PhysicsFS interface.
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

#if defined(VINIFERA_USE_PHYSICSFS)

#include "physicsfs_hooks.h"
#include "physicsfs.h"
#include "ccfile.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
static class CCFileClassExt final : public CCFileClass
{
    public:
        bool _Is_Available(bool forced = false);
        bool _Is_Open() const;
        bool _Open1(const char * filename, FileAccessType rights = FILE_ACCESS_READ);
        bool _Open2(FileAccessType rights = FILE_ACCESS_READ);
        long _Read(void * buffer, int length);
        off_t _Seek(off_t offset, FileSeekType whence = FILE_SEEK_CURRENT);
        off_t _Size();
        long _Write(const void * buffer, int length);
        void _Close();
        LONG _Get_Date_Time() const;
        bool _Set_Date_Time(LONG date_time);
        void _Error(FileErrorType error, bool can_retry = false, const char * filename = nullptr);
};


bool CCFileClassExt::_Is_Available(bool forced)
{
    /*
    **  A file that is open is presumed available.
    */
    if (Is_Open()) return(true);

    /*
    **  A file that is part of a mixfile is also presumed available.
    */
#if 0
    if (MFCD::Offset(File_Name())) {
        return(true);
    }
#else
    if (PhysicsFS_IsAvailable(File_Name())) {
        return(true);
    }
#endif

    /*
    **  Otherwise a manual check of the file system is required to
    **  determine if the file is actually available.
    */
    return(CDFileClass::Is_Available());
}


bool CCFileClassExt::_Is_Open() const
{
    /*
    **  If the file is part of a cached file, then return that it is opened. A closed file
    **  doesn't have a valid pointer.
    */
    if (Is_Resident()) return(true);

    /*
    **  Otherwise, go to a lower level to determine if the file is open.
    */
    return(CDFileClass::Is_Open());
}


bool CCFileClassExt::_Open1(const char * filename, FileAccessType rights)
{
    Set_Name(filename);
    return Open(rights);
}


bool CCFileClassExt::_Open2(FileAccessType rights)
{
    /*
    **  Always close the file if it was open.
    */
    Close();

    /*
    **  Perform a preliminary check to see if the specified file
    **  exists on the disk. If it does, then open this file regardless
    **  of whether it also exists in RAM. This is slower, but allows
    **  upgrade files to work.
    */
    if ((rights & FILE_ACCESS_WRITE) || CDFileClass::Is_Available()) {
        return(CDFileClass::Open(rights));
    }

    /*
    **  Check to see if file is part of a mixfile and that mixfile is currently loaded
    **  into RAM.
    */
#if 0
    MFCD* mixfile = NULL;
    void* pointer = NULL;
    long length = 0;
    long start = 0;
    if (MFCD::Offset(File_Name(), &pointer, &mixfile, &start, &length)) {

        assert(mixfile != NULL);

        /*
        **  If the mixfile is located on disk, then fake out the file system to read from
        **  the mixfile, but think it is reading from a solitary file.
        */
        if (pointer == NULL && mixfile != NULL) {

            /*
            **  This is a legitimate open to the file. All access to the file through this
            **  file object will be appropriately adjusted for mixfile support however. Also
            **  note that the filename attached to this object is NOT the same as the file
            **  attached to the file handle.
            */
            char* dupfile = strdup(File_Name());
            Open(mixfile->Filename, READ);
            Searching(false);                // Disable multi-drive search.
            Set_Name(dupfile);
            Searching(true);
            free(dupfile);
            Bias(0);
            Bias(start, length);
            Seek(0, SEEK_SET);
        } else {
            new (&Data) ::Buffer(pointer, length);
            Position = 0;
        }

    }
#else
    if (PhysicsFS_IsAvailable(File_Name())) {
        if (true) {
        } else {
            //new (&Data) ::Buffer(pointer, length);
            //Position = 0;
        }
    }
#endif
    else {

        /*
        **  The file cannot be found in any mixfile, so it must reside as
        ** an individual file on the disk. Or else it is just plain missing.
        */
        return(CDFileClass::Open(rights));
    }
    return(true);
}


long CCFileClassExt::_Read(void * buffer, int length)
{
    bool opened = false;

    /*
    **  If the file isn't currently open, then open it.
    */
    if (!Is_Open()) {
        if (Open()) {
            opened = true;
        }
    }

    /*
    **  If the file is part of a loaded mixfile, then a mere copy is
    **  all that is required for the read.
    */
    if (Is_Resident()) {
        long    maximum = Data.Get_Size() - Position;

        length = maximum < length ? maximum : length;
        //        length = MIN(maximum, length);
        if (length) {
            memmove(buffer, (char *)Data + Position, length);
            //            Mem_Copy((char *)Pointer + Position, buffer, length);
            Position += length;
        }
        if (opened) Close();
        return(length);
    }

    long s = CDFileClass::Read(buffer, length);

    /*
    **  If the file was opened by this routine, then close it at this time.
    */
    if (opened) Close();

    /*
    **  Return with the number of bytes read.
    */
    return(s);
}


off_t CCFileClassExt::_Seek(off_t offset, FileSeekType whence)
{
    /*
    **  When the file is resident, a mere adjustment of the virtual file position is
    **  all that is required of a seek.
    */
    if (Is_Resident()) {
        switch (whence) {
            case SEEK_END:
                Position = Data.Get_Size();
                break;

            case SEEK_SET:
                Position = 0;
                break;

            case SEEK_CUR:
            default:
                break;
        }
        Position += offset;
        Position = Position < 0 ? 0 : Position;
        Position = Position > Data.Get_Size() ? Data.Get_Size() : Position;
        //        Position = Bound(Position+offset, 0L, Length);
        return(Position);
    }
    return(CDFileClass::Seek(offset, whence));
}


off_t CCFileClassExt::_Size()
{
    /*
    **  If the file is resident, the the size is already known. Just return the size in this
    **  case.
    */
    if (Is_Resident()) return(Data.Get_Size());

    /*
    **  If the file is not available as a stand alone file, then search for it in the
    **  mixfiles in order to get its size.
    */
    if (!CDFileClass::Is_Available()) {
        long length = 0;
#if 0
        MFCD::Offset(File_Name(), NULL, NULL, NULL, &length);
#else
        length = PhysicsFS_Size(File_Name());
#endif
        return(length);
    }

    return(CDFileClass::Size());
}


long CCFileClassExt::_Write(const void * buffer, int length)
{
    /*
    **  If this is part of a mixfile, then writing is not allowed. Error out with a fatal
    **  message.
    */
    if (Is_Resident()) {
        Error(FILE_ERROR_ACCES, false, File_Name());
    }

    return(CDFileClass::Write(buffer, length));
}


void CCFileClassExt::_Close()
{
    new(&Data) ::Buffer;
    Position = 0;                // Starts at beginning offset.
    CDFileClass::Close();
}


LONG CCFileClassExt::_Get_Date_Time() const
{
    unsigned long datetime;
#if 0
    MFCD* mixfile;

    datetime = CDFileClass::Get_Date_Time();

    if (!datetime) {
        if (MFCD::Offset(File_Name(), NULL, &mixfile, NULL, NULL)) {
            //
            // check for nested MIX files
            //
            return(CCFileClass(mixfile->Filename).Get_Date_Time());
        }
        // else return 0 indicating no file
    }
#endif
    return(datetime);
}


bool CCFileClassExt::_Set_Date_Time(LONG date_time)
{
    bool status;
#if 0
    MFCD* mixfile;

    status = CDFileClass::Set_Date_Time(datetime);

    if (!status) {
        if (MFCD::Offset(File_Name(), NULL, &mixfile, NULL, NULL)) {
            //
            // check for nested MIX files
            //
            return(CCFileClass(mixfile->Filename).Set_Date_Time(datetime));
        }
        // else return 0 indicating no file
    }
#endif
    return(status);
}


void CCFileClassExt::_Error(FileErrorType error, bool can_retry, const char * filename)
{
#if 0
    if (!Force_CD_Available(RequiredCD)) {
        Prog_End("CCFileClass::Error CD not found", true);
        if (!RunningAsDLL) {    //PG
            Emergency_Exit(EXIT_FAILURE);
        }
    }
#endif
}


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 *
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
static class MixFileClassExt final : public MixFileClass
{
    public:
        static bool _Offset(const char * filename, void ** realptr = nullptr, MixFileClass ** mixfile = nullptr, long * offset = nullptr, long * size = nullptr);
        static const void * _Retrieve(const char * filename);
};

bool MixFileClassExt::_Offset(const char * filename, void ** realptr, MixFileClass ** mixfile, long * offset, long * size)
{
#if 0
    MixFileClass* ptr;

    if (filename == NULL) {
        assert(filename != NULL);//BG
        return(false);
    }

    /*
    **  Create the key block that will be used to binary search for the file.
    */
    // Can't call strupr on a const string. ST - 5/20/2019
    //long crc = Calculate_CRC(strupr((char *)filename), strlen(filename));
    char filename_upper[_MAX_PATH];
    strcpy(filename_upper, filename);
    strupr(filename_upper);
    long crc = Calculate_CRC(strupr(filename_upper), strlen(filename_upper));
    SubBlock key;
    key.CRC = crc;

    /*
    **  Sweep through all registered mixfiles, trying to find the file in question.
    */
    ptr = List.First();
    while (ptr->Is_Valid()) {
        SubBlock* block;

        /*
        **  Binary search for the file in this mixfile. If it is found, then extract the
        **  appropriate information and store it in the locations provided and then return.
        */
        block = (SubBlock*)bsearch(&key, ptr->HeaderBuffer, ptr->Count, sizeof(SubBlock), compfunc);
        if (block != NULL) {
            if (mixfile != NULL) *mixfile = ptr;
            if (size != NULL) *size = block->Size;
            if (realptr != NULL) *realptr = NULL;
            if (offset != NULL) *offset = block->Offset;
            if (realptr != NULL && ptr->Data != NULL) {
                *realptr = (char*)ptr->Data + block->Offset;
            }
            if (ptr->Data == NULL && offset != NULL) {
                *offset += ptr->DataStart;
            }
            return(true);
        }

        /*
        **  Advance to next mixfile.
        */
        ptr = ptr->Next();
    }

    /*
    **  All the mixfiles have been examined but no match was found. Return with the non success flag.
    */
    assert(1);//BG
#endif
    return(false);
}

const void * MixFileClassExt::_Retrieve(const char * filename)
{
    void * ptr = 0;
    Offset(filename, &ptr);
    return(ptr);
}


#endif

/**
 *  Main function for patching the hooks.
 */
void PhysicsFS_Hooks()
{
#if defined(VINIFERA_USE_PHYSICSFS)
    //DEFINE_IMPLEMENTATION_CONSTRUCTOR(CCFileClass::CCFileClass(), 0x004497F0);
    //DEFINE_IMPLEMENTATION_CONSTRUCTOR(CCFileClass::CCFileClass(const char *), 0x004497B0);
    //DEFINE_IMPLEMENTATION_DESTRUCTOR(CCFileClass::~CCFileClass(), 0x004E8970);

    Patch_Jump(0x004499C0, &CCFileClassExt::_Is_Available);
    Patch_Jump(0x00449A00, &CCFileClassExt::_Is_Open);
    Patch_Jump(0x00407270, &CCFileClassExt::_Open1);
    Patch_Jump(0x00449A40, &CCFileClassExt::_Open2);
    Patch_Jump(0x00449880, &CCFileClassExt::_Read);
    Patch_Jump(0x00449910, &CCFileClassExt::_Seek);
    Patch_Jump(0x00449970, &CCFileClassExt::_Size);
    Patch_Jump(0x00449850, &CCFileClassExt::_Error);
    Patch_Jump(0x00449A10, &CCFileClassExt::_Close);
    Patch_Jump(0x00449B70, &CCFileClassExt::_Get_Date_Time);
    Patch_Jump(0x00449C20, &CCFileClassExt::_Set_Date_Time);
    Patch_Jump(0x00449820, &CCFileClassExt::_Error);

    Patch_Jump(0x00559DE0, &MixFileClassExt::_Retrieve);
    Patch_Jump(0x0055A1C0, &MixFileClassExt::_Offset);
#endif
}
