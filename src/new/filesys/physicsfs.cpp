/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PHYSFSFILE.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Interface for loading files from zip files.
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

#include "physicsfs.h"
#include "debughandler.h"


// https://gist.github.com/Ybalrid/fb3453c997b4925d25dc9a1bf4f952b6


// https://github.com/OpenApoc/OpenApoc/blob/2803d7882e9b56de0bdfcaa234b513b444c154e9/framework/fs/physfs_archiver_cue.cpp#L1237


// From test_physfs.c.
static void modTimeToStr(PHYSFS_sint64 modtime, char* modstr, size_t strsize)
{
    const char* str = "unknown modtime";
    if (modtime != -1) {
        time_t t = (time_t)modtime;
        str = ctime(&t);
    }
    strncpy(modstr, str, strsize);
    modstr[strsize - 1] = '\0';
    strsize = strlen(modstr);
    while ((modstr[strsize - 1] == '\n') || (modstr[strsize - 1] == '\r')) {
        modstr[--strsize] = '\0';
    }
}




static void * physfs_malloc(PHYSFS_uint64 size)
{
    return malloc(size);
}

static void * physfs_realloc(void * ptr, PHYSFS_uint64 size)
{
    return realloc(ptr, size);
}

static void physfs_free(void * ptr)
{
    return free(ptr);
}





static void physfs_Parse_Mix_File(Wstring fileName)
{

}

static void * physfs_MixOpenArchive(PHYSFS_Io *io, const char * filename, int forWriting, int * claimed)
{
    return nullptr;
}

static PHYSFS_EnumerateCallbackResult physfs_MixEnumerateFiles(void * opaque, const char * dirname,
    PHYSFS_EnumerateCallback cb,
    const char * origdir, void * callbackdata)
{
    return PHYSFS_ENUM_ERROR;
}

static PHYSFS_Io * physfs_MixOpenRead(void * opaque, const char * fnm)
{
    return nullptr;
}

static PHYSFS_Io * physfs_MixOpenWrite(void * opaque, const char * filename)
{
    PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
    return nullptr;
}

static PHYSFS_Io * physfs_MixOpenAppend(void * opaque, const char * filename)
{
    PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
    return nullptr;
}

static int physfs_MixRemove(void * opaque, const char * filename)
{
    PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
    return 0;
}

static int physfs_MixMkdir(void * opaque, const char * filename)
{
    PHYSFS_setErrorCode(PHYSFS_ERR_READ_ONLY);
    return 0;
}

static int physfs_MixStat(void * opaque, const char * fn, PHYSFS_Stat * stat)
{
    return 0;
}

static void physfs_MixCloseArchive(void * opaque)
{
}

static PHYSFS_Archiver * physfs_Create_Mix_Archiver()
{
    static PHYSFS_Archiver cueArchiver = { 0,
                                         {
                                             "MIX", "Westwood Mix Archive (v2)",
                                             "CCHyper",
                                             "https://github.com/cchyper",
                                             0 // supportsSymlinks
                                         },
                                         physfs_MixOpenArchive,
                                         physfs_MixEnumerateFiles,
                                         physfs_MixOpenRead,
                                         physfs_MixOpenWrite,
                                         physfs_MixOpenAppend,
                                         physfs_MixRemove,
                                         physfs_MixMkdir,
                                         physfs_MixStat,
                                         physfs_MixCloseArchive };
    return &cueArchiver;
}

static PHYSFS_Archiver * physfs_Get_Mix_Archiver()
{
    return physfs_Create_Mix_Archiver();
}



bool PhysicsFS_Init()
{
    DEBUG_INFO("PhysicsFS_Init()\n");

    // output_versions

    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    DEBUG_INFO("  physfs version %d.%d.%d.\n"
        "  Compiled against PhysicsFS version %d.%d.%d,\n",
        (int)compiled.major, (int)compiled.minor, (int)compiled.patch,
        (int)linked.major, (int)linked.minor, (int)linked.patch);


#if 0
    PHYSFS_Allocator allocator;
    //allocator.Init = nullptr;
    //allocator.Deinit = nullptr;
    allocator.Malloc = nullptr;
    allocator.Realloc = nullptr;
    allocator.Free = nullptr;

    if (PHYSFS_setAllocator(&allocator))
        DEBUG_INFO("PHYSFS_init -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_init -> Failure. reason: %s.\n", PHYSFS_getLastError());
#endif



    // cmd_init

    if (!PHYSFS_init(NULL)) 
        DEBUG_ERROR("PHYSFS_init -> Failure. reason: %s.\n", PHYSFS_getLastError());




    // cmd_getbasedir

    DEBUG_INFO("Base dir is [%s].\n", PHYSFS_getBaseDir());




    // PHYSFS_permitSymbolicLinks(true);


    // FIXME: Is this the right thing to do that?
    DEBUG_INFO("PhysicsFS: Registering Westwood MIX archiver...\n");
    PHYSFS_registerArchiver(physfs_Get_Mix_Archiver());


    // output_archivers
    const PHYSFS_ArchiveInfo** rc = PHYSFS_supportedArchiveTypes();
    const PHYSFS_ArchiveInfo** i;

    DEBUG_INFO("Supported archive types:\n");
    if (*rc == NULL) {
        DEBUG_INFO(" * Apparently, NONE!\n");
    } else {
        for (i = rc; *i != NULL; i++) {
            DEBUG_INFO(" * %s: %s\n    Written by %s.\n    %s\n",
                (*i)->extension, (*i)->description,
                (*i)->author, (*i)->url);
            DEBUG_INFO("    %s symbolic links.\n",
                (*i)->supportsSymlinks ? "Supports" : "Does not support");
        }
    }

    return true;
}


bool PhysicsFS_Shutdown()
{
    DEBUG_INFO("PhysicsFS_Shutdown()\n");

    // cmd_deinit

    if (!PHYSFS_deinit())
        DEBUG_ERROR("PHYSFS_deinit -> Failure. reason: %s.\n", PHYSFS_getLastError());

    return true;
}


Wstring PhysicsFS_AddArchive2(Wstring archive, Wstring path, Wstring mount_point, bool append)
{
    bool mounted_with_path = false;

    if (path.Is_Not_Empty()) {
        Wstring tmp = path + PHYSFS_getDirSeparator() + archive;
        if (PHYSFS_mount(tmp.Peek_Buffer(), mount_point.Peek_Buffer(), (append == true ? 1 : 0)) != 0) {
            DEBUG_INFO("PhysicsFS(Archive) -> Mounted \"%s\".\n", tmp.Peek_Buffer());
            mounted_with_path = true;
            return tmp;
        }
    }
    if (!mounted_with_path) {
        if (PHYSFS_mount(archive.Peek_Buffer(), mount_point.Peek_Buffer(), (append == true ? 1 : 0)) != 0) {
            DEBUG_INFO("PhysicsFS(Archive) -> Mounted \"%s\".\n", archive.Peek_Buffer());
        }
    }

    return archive;
}


bool PhysicsFS_RemoveArchive(Wstring archive)
{
    bool mounted = PHYSFS_unmount(archive.Peek_Buffer()) != 0;
    if (mounted) {
        DEBUG_INFO("PhysicsFS(Archive) -> Removed \"%s\".\n", archive.Peek_Buffer());
    }
    return mounted;
}


bool PhysicsFS_AddPath(Wstring path, bool append)
{
    bool mounted = PHYSFS_mount(path.Peek_Buffer(), "/", (append == true ? 1 : 0)) != 0;
    if (mounted) {
        DEBUG_INFO("PhysicsFS(Path) -> Mounted \"%s\".\n", path.Peek_Buffer());
    }
    return mounted;
}


bool PhysicsFS_RemovePath(Wstring path)
{
    bool mounted = PHYSFS_unmount(path.Peek_Buffer()) != 0;
    if (mounted) {
        DEBUG_INFO("PhysicsFS(Path) -> Removed \"%s\".\n", path.Peek_Buffer());
    }
    return mounted;
}


bool PhysicsFS_IsMounted(Wstring filename)
{
    return PHYSFS_getMountPoint(filename.Peek_Buffer()) != nullptr;
}


bool PhysicsFS_IsFile(Wstring filename)
{
    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);
    return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}


bool PhysicsFS_IsDirectory(Wstring filename)
{
    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);
    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}


int PhysicsFS_Size(Wstring filename)
{
    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);
    return stat.filesize;
}


Wstring PhysicsFS_ModTime(Wstring filename)
{
    char timestr[64];

    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);

    modTimeToStr(stat.modtime, timestr, sizeof(timestr));
    Wstring modtime = timestr;

    return modtime;
}


Wstring PhysicsFS_CreateTime(Wstring filename)
{
    char timestr[64];

    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);

    modTimeToStr(stat.createtime, timestr, sizeof(timestr));
    Wstring createtime = timestr;

    return createtime;
}


Wstring PhysicsFS_AccessTime(Wstring filename)
{
    char timestr[64];

    PHYSFS_Stat stat;
    bool success = PHYSFS_stat(filename.Peek_Buffer(), &stat);

    modTimeToStr(stat.accesstime, timestr, sizeof(timestr));
    Wstring createtime = timestr;

    return createtime;
}


bool PhysicsFS_IsAvailable(Wstring filename)
{
    return PHYSFS_exists(filename.Peek_Buffer()) != 0;
}


bool PhysicsFS_Read(Wstring filename, void* buffer, int buffer_length)
{
    return false;
}


bool PhysicsFS_Delete(Wstring filename)
{
    return PHYSFS_delete(filename.Peek_Buffer()) != 0;
}


void PhysicsFS_LogSearchPaths()
{
    // cmd_getsearchpath
    DEBUG_INFO("PhysicsFS SearchPaths:\n");

    char ** rc = PHYSFS_getSearchPath();
    char ** search_paths = PHYSFS_getSearchPath();
    int index = 0;
    for (char ** i = search_paths; *i != NULL; i++) {
        DEBUG_INFO("  %d: \"%s\"\n", index++, *i);
    }
    PHYSFS_freeList(rc);

    DEBUG_INFO("\n");
}




typedef struct FileStruct
{
    Wstring Filename;
    Wstring Mount;
    void * BufferPtr;
    unsigned int BufferLength;
} FileStruct;

#include "search.h"
static IndexClass<Wstring, FileStruct> FileIndex;



#include "vector.h"
typedef DynamicVectorClass<Wstring> EnumeratedFileList;

static EnumeratedFileList Enumerate_Mount(Wstring mount)
{
    EnumeratedFileList file_list;

    char ** files = PHYSFS_enumerateFiles(mount.Peek_Buffer());

    char * currentFile;
    for (unsigned i = 0; currentFile = files[i], currentFile != nullptr; i++) {
        if (!PHYSFS_isDirectory(currentFile)) {
            file_list.Add(currentFile);
        }
    }

    return file_list;
}






bool Load_File_Into_Memory(const Wstring mount, const Wstring filename)
{
    PHYSFS_file * data_file = PHYSFS_openRead(filename.Peek_Buffer());
    if (!data_file) {
        DEBUG_ERROR("PHYSFS: Error while opening file %s: %s.\n", filename.Peek_Buffer(), PHYSFS_getLastError());
        return false;
    }

    int file_length = PHYSFS_fileLength(data_file);
    if (!file_length) {
        return false;
    }

    unsigned char * buffer_ptr = new unsigned char [file_length];
    if (!buffer_ptr) {
        return false;
    }

    PHYSFS_sint64 file_read = PHYSFS_read(data_file, buffer_ptr, 1, file_length);
    if (file_read != file_length) {
        DEBUG_ERROR("PHYSFS: Error while reading from file %s: %s.\n", filename.Peek_Buffer(), PHYSFS_getLastError());
        return false;
    }

    DEBUG_INFO("PHYSFS: Adding file \"%s\":\"%s\" to the file index.\n", mount.Peek_Buffer(), filename.Peek_Buffer());

    /**
     *  x
     */
    {
        FileStruct fileinfo;
        fileinfo.Filename = filename;
        fileinfo.Mount = mount;
        fileinfo.BufferPtr = buffer_ptr;
        fileinfo.BufferLength = file_length;

        FileIndex.Add_Index(filename, fileinfo);
    }

    if (!PHYSFS_close(data_file)) {
        DEBUG_ERROR("PHYSFS: Error while closing file %s: %s.\n", filename.Peek_Buffer(), PHYSFS_getLastError());
        return false;
    }

    return true;
}



void PhysicsFS_Test()
{
    PhysicsFS_AddPath("ini");
    PhysicsFS_AddPath("movies");
    PhysicsFS_AddPath("sounds");
    PhysicsFS_AddPath("music");

    PhysicsFS_AddPath("zip");
    PhysicsFS_AddPath("7z");

    PhysicsFS_AddArchive2("tibsun.zip", "zip");
    PhysicsFS_AddArchive2("expand01.zip", "zip");

    PhysicsFS_AddArchive2("movies01.zip", "zip");
    PhysicsFS_AddArchive2("movies02.zip", "zip");
    PhysicsFS_AddArchive2("movies03.zip", "zip");

    PhysicsFS_AddArchive2("scores.zip", "zip");
    PhysicsFS_AddArchive2("scores01.zip", "zip");

    PhysicsFS_AddArchive2("sounds.zip", "zip");
    PhysicsFS_AddArchive2("sounds01.zip", "zip");

    PhysicsFS_AddArchive2("maps01.zip", "zip");
    PhysicsFS_AddArchive2("maps02.zip", "zip");
    PhysicsFS_AddArchive2("maps03.zip", "zip");

    PhysicsFS_AddArchive2("sidecd01.zip", "zip");
    PhysicsFS_AddArchive2("sidecd02.zip", "zip");

    PhysicsFS_AddArchive2("gmenu.zip", "zip");

    PhysicsFS_AddArchive2("wdt.zip", "zip");
    PhysicsFS_AddArchive2("wdtvox.zip", "zip");

    Wstring mount_path = PhysicsFS_AddArchive2("patch.zip", "zip");
    if () {

    }
    EnumeratedFileList file_list = Enumerate_Mount(mount_path);

    for (int index = 0; index < file_list.Count(); ++index) {
        Load_File_Into_Memory(mount_path, file_list[index]);
    }




    PhysicsFS_LogSearchPaths();













#if 0


    // cmd_getsearchpath
    {
        char** rc = PHYSFS_getSearchPath();

        if (rc == NULL)
            DEBUG_ERROR("PHYSFS_getSearchPath -> Failure. reason: %s.\n", PHYSFS_getLastError());
        else
        {
            int dir_count;
            char** i;
            for (i = rc, dir_count = 0; *i != NULL; i++, dir_count++)
                DEBUG_INFO("%s\n", *i);

            DEBUG_INFO("\n total (%d) search paths.\n", dir_count);
            PHYSFS_freeList(rc);
        } /* else */

        DEBUG_INFO("\n");
    }




    char** rc = PHYSFS_enumerateFiles("vqa");
    char** i;
    for (i = rc; *i != NULL; i++)
        DEBUG_INFO(" * We've got [%s].\n", *i);
    PHYSFS_freeList(rc);




    if (PHYSFS_exists("wwlogo.vqa"))
        DEBUG_INFO("PHYSFS_exists -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_exists -> Failure. reason: %s.\n", PHYSFS_getLastError());


    if (PHYSFS_exists("fmod.vqa"))
        DEBUG_INFO("PHYSFS_exists -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_exists -> Failure. reason: %s.\n", PHYSFS_getLastError());





    char modstr[64];
    modTimeToStr(PHYSFS_getLastModTime("wwlogo.vqa"), modstr, sizeof(modstr));
    DEBUG_INFO("PHYSFS_getLastModTime -> %s)\n", modstr);





    PHYSFS_file* myfile = PHYSFS_openRead("vinifera.vqa");
    if (myfile)
        DEBUG_INFO("PHYSFS_openRead -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_openRead -> Failure. reason: %s.\n", PHYSFS_getLastError());



    PHYSFS_sint64 file_size = PHYSFS_fileLength(myfile);
    if (file_size)
        DEBUG_INFO("PHYSFS_fileLength -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_fileLength -> Failure. reason: %s.\n", PHYSFS_getLastError());





    char* myBuf = new char[PHYSFS_fileLength(myfile)];
    int length_read = PHYSFS_read(myfile, myBuf, 1, file_size);




    DEBUG_INFO("modtime -> %s)\n", PhysicsFS_ModTime("wwlogo.vqa").Peek_Buffer());
    DEBUG_INFO("createtime -> %s)\n", PhysicsFS_CreateTime("wwlogo.vqa").Peek_Buffer());
    DEBUG_INFO("accesstime -> %s)\n", PhysicsFS_AccessTime("wwlogo.vqa").Peek_Buffer()); // not changed on read()


    if (PHYSFS_close(myfile))
        DEBUG_INFO("PHYSFS_close -> Successful.\n");
    else
        DEBUG_ERROR("PHYSFS_close -> Failure. reason: %s.\n", PHYSFS_getLastError());




    PhysicsFS_Shutdown();

#endif
}

#endif
