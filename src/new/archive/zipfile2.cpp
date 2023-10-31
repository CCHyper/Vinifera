/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ZIPFILE.CPP
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
#include "zipfile2.h"
//#include "search.h"
#include "crc32.h"
#include "cdfile.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <algorithm>


#if 0

#include "ZipFile.h"
#include "ZipArchive.h"
#include "streams/memstream.h"
#include "methods/Bzip2Method.h"

#include <sstream>


#include "base64.h"




static void Split_Filename(const Wstring & qualified_name, Wstring & out_basename, Wstring & out_path)
{
    std::string path = qualified_name.Peek_Buffer();

    // Replace "\" with "/" first.
    std::replace(path.begin(), path.end(), '\\', '/');

    // split based on final "/".
    size_t i = path.find_last_of('/');

    if (i == std::string::npos) {
        out_path.Release_Buffer();
        out_basename = qualified_name;
    } else {
        out_basename = path.substr(i + 1, path.size() - i - 1).c_str();
        out_path = path.substr(0, i + 1).c_str();
    }
}





ZipFileClass2::ZipFileClass2(Wstring filename) :
    ArchiveFileClass(filename),
    ZipFile(nullptr),
    ZipFileBuffer(),
    ZipFileIndex()
{
    CDFileClass file(filename.Peek_Buffer());
    if (!file.Is_Available()) {
        DEBUG_WARNING("Zip::Zip - Failed to find \"%s\"!\n", filename.Peek_Buffer());
        return;
    }

    ZipFileBuffer = Buffer(file.Size());

    ZipArchive::Ptr archive = ZipFile::Open(filename.Peek_Buffer());
    if (archive && archive->GetEntriesCount() > 0) {
        DEBUG_INFO("[+] Filename: %s\n", filename.Peek_Buffer());
        DEBUG_INFO("[+] Listing archive (comment: '%s')\n", archive->GetComment().length() > 0 ? archive->GetComment().c_str() : "none");
        DEBUG_INFO("[+] Entries count: %lu\n", archive->GetEntriesCount());

        ZipFile = archive;

        Build_Index();
    }
}

ZipFileClass2::~ZipFileClass2()
{
    Unload();
}

bool ZipFileClass2::Load()
{
    if (ZipFile) {
        return true;
    }

    ZipArchive::Ptr archive = ZipFile::Open(Filename.Peek_Buffer());
    if (!archive) {
        return false;
    }

    size_t nentries = archive->GetEntriesCount();

    DEBUG_INFO("[+] Filename: %s\n", Filename.Peek_Buffer());
    DEBUG_INFO("[+] Listing archive (comment: '%s')\n", archive->GetComment().length() > 0 ? archive->GetComment().c_str() : "none");
    DEBUG_INFO("[+] Entries count: %lu\n", nentries);

    ZipFile = archive;

    return true;

#if 0
    ZipArchive::Ptr archive = ZipFile::Open(Filename.Peek_Buffer());
    size_t entries = archive->GetEntriesCount();

    DEBUG_INFO("[+] Filename: %s\n", Filename.Peek_Buffer());
    DEBUG_INFO("[+] Listing archive (comment: '%s')\n", archive->GetComment().length() > 0 ? archive->GetComment().c_str() : "none");
    DEBUG_INFO("[+] Entries count: %lu\n", entries);

    for (size_t i = 0; i < entries; ++i)
    {
        {
            ZipArchiveEntry::Ptr entry = archive->GetEntry(int(i));
            if (!entry) {
                continue;
            }

            ASSERT_FATAL(entry->GetName().length() > 0);
            ASSERT_FATAL(entry->GetFullName().length() > 0);

            DEBUG_INFO("[%d] -- %s\n", i, entry->GetName().c_str());
            if (entry->GetName() != entry->GetFullName()) {
                DEBUG_INFO("[%d] -- %s\n", i, entry->GetFullName().c_str());
            }
            DEBUG_INFO("[%d]   >> is dir: '%s'\n", i, entry->IsDirectory() ? "yes" : "no");
            DEBUG_INFO("[%d]   >> uncompressed size: %lu\n", i, entry->GetSize());
            DEBUG_INFO("[%d]   >> compressed size: %lu\n", i, entry->GetCompressedSize());
            DEBUG_INFO("[%d]   >> password protected: '%s'\n", i, entry->IsPasswordProtected() ? "yes" : "no");
            DEBUG_INFO("[%d]   >> compression method: '%s'\n", i, entry->GetCompressionMethod() == DeflateMethod::CompressionMethod ? "DEFLATE" : "stored");
            DEBUG_INFO("[%d]   >> comment: '%s'\n", i, entry->GetComment().length() > 0 ? entry->GetComment().c_str() : "none");
            DEBUG_INFO("[%d]   >> crc32: 0x%08X\n", i, entry->GetCrc32());

            time_t time = entry->GetLastWriteTime();
            DEBUG_INFO("[%d]   >> datetime: %s", i, ctime(&time));
        }

        {
            ZipArchiveEntry::Ptr entry = archive->GetEntry("wwlogo.vqa");
            if (!entry) {
                continue;
            }

            if (entry->IsPasswordProtected()) {
                entry->SetPassword("wrongpass");
            }
        }
    }
#endif

    return true;
}

bool ZipFileClass2::Unload()
{
    return true;
}

bool ZipFileClass2::Open(Wstring &filename, bool ready_only)
{
    return true;
}

bool ZipFileClass2::Exists(Wstring &filename) const
{
    return false;
}

DynamicVectorClass<Wstring> ZipFileClass2::Filename_List(bool recursive, bool dirs) const
{
    return DynamicVectorClass<Wstring>();
}

const ZipFileClass2::FileInfoStruct * ZipFileClass2::Find_File_Info(Wstring &filename, bool read_only)
{
    for (int index = 0; index < FileList.Count(); ++index) {

        unsigned long target_crc = CRC32_String(filename.Peek_Buffer());
        if (FileList.Is_Present(target_crc)) {
            const FileInfoStruct *fileinfo = &FileList[target_crc];
            return fileinfo;
        }

    }

    return nullptr;
}

const void * ZipFileClass2::Retrieve_Memory(Wstring &filename)
{
    Wstring tmp = filename;
    tmp.To_Upper();
    unsigned int target_crc = CRC32_String(tmp.Peek_Buffer());

    const ZipEntryStruct * entry = &ZipFileIndex[target_crc];

    if (!ZipFileIndex.Is_Present(target_crc)) {
#ifndef NDEBUG
        DEBUG_WARNING("Zip::Retrieve - Failed to locate \"%s\" in the ZipFileIndex!\n", filename.Peek_Buffer());
#endif
        return nullptr;
    }

#ifndef NDEBUG
    DEV_DEBUG_INFO("Zip::Retrieve - Located \"%s\" in the ZipFileIndex!\n", filename.Peek_Buffer());
    DEV_DEBUG_INFO("Zip::Retrieve - Info - Size: %d\n", entry->UncompEntrySize);
#endif

    std::string line;
    std::getline(*entry->DataStream, line);

    DEBUG_INFO("[+] Content of a file: '%s'\n", line.c_str());

    return entry->DataStream->;
}





bool ZipFileClass2::Build_Index()
{
    if (!ZipFile) {
        return false;
    }

    size_t entries = ZipFile->GetEntriesCount();
    for (size_t i = 0; i < entries; ++i) {

        ZipArchiveEntry::Ptr entry = ZipFile->GetEntry(int(i));
        if (!entry) {
            continue;
        }

        ASSERT_FATAL(entry->GetName().length() > 0);
        ASSERT_FATAL(entry->GetFullName().length() > 0);

#ifndef NDEBUG
        DEV_DEBUG_INFO("[%d] -- %s\n", i, entry->GetName().c_str());
        if (entry->GetName() != entry->GetFullName()) {
            DEBUG_INFO("[%d] -- %s\n", i, entry->GetFullName().c_str());
        }
        DEV_DEBUG_INFO("[%d]   >> is dir: '%s'\n", i, entry->IsDirectory() ? "yes" : "no");
        DEV_DEBUG_INFO("[%d]   >> uncompressed size: %lu\n", i, entry->GetSize());
        DEV_DEBUG_INFO("[%d]   >> compressed size: %lu\n", i, entry->GetCompressedSize());
        DEV_DEBUG_INFO("[%d]   >> password protected: '%s'\n", i, entry->IsPasswordProtected() ? "yes" : "no");
        DEV_DEBUG_INFO("[%d]   >> compression method: '%s'\n", i, entry->GetCompressionMethod() == DeflateMethod::CompressionMethod ? "DEFLATE" : "stored");
        DEV_DEBUG_INFO("[%d]   >> comment: '%s'\n", i, entry->GetComment().length() > 0 ? entry->GetComment().c_str() : "none");
        DEV_DEBUG_INFO("[%d]   >> crc32: 0x%08X\n", i, entry->GetCrc32());

        time_t time = entry->GetLastWriteTime();
        DEV_DEBUG_INFO("[%d]   >> datetime: %s", i, ctime(&time));
#endif

        {
            ZipEntryStruct zip_entry;
            zip_entry.EntryPtr = entry;
            zip_entry.Container = this;

            {
                Wstring tmp = entry->GetName().c_str();
                tmp.To_Upper();
                unsigned int entry_crc = CRC32_String(tmp.Peek_Buffer());
                zip_entry.CRC = entry_crc;
            }

            zip_entry.DataStream = entry->GetDecompressionStream();
            ASSERT_FATAL(zip_entry.DataStream != nullptr);

            ZipFileIndex.Add_Index(zip_entry.CRC, zip_entry);
        }
    }

    DEV_DEBUG_INFO("Zip::Build_Index - Count = %d.\n", ZipFileIndex.Count());

    return true;
}

























































#if 0

/**
 *  This is the pointer to the first zipfile in the list of zipfile registered
 *  with the zipfile system.
 */
static List<ZipFileClass2 *> ZipFileList;




/**
 *  An index list of all files within a zip file and their entry info.
 */
struct ZipIndexEntry
{
    Wstring Filename;                   // Filename for quick retrival.
    ZipFileClass2 *Container;            // The zip file in which this file is within.
    bool IsCompressed;                  // 
    unsigned long long UncompEntrySize; // 
    unsigned long long CompEntrySize;   // 
    unsigned long EntrySize;            // 
    unsigned char * EntryData;          // Pointer to the data loaded in memory.
};
static IndexClass<unsigned int, ZipIndexEntry> FileIndex;


/**
 *  x
 */
struct ZipBufferStruct {
    unsigned char * BufferPtr;
    size_t BufferSize;
};
static ZipBufferStruct ZipMemoryBuffer;


/**
 *  Extraction callback for the zip library.
 * 
 *  This callback will grow a single global memory block for each file
 *  that is extracted, so it can be freed on application shutdown.
 * 
 *  @author: CCHyper
 */
static size_t zip_on_extract_callback(void * arg, unsigned long long offset, const void * data, size_t size)
{
    ZipBufferStruct * buf = (ZipBufferStruct *)arg;

    /**
     *  Round up input size to nearest multiple of 4 for alignment.
     */
    //size_t roundup_size = ((buf->BufferSize + size) + (4 - 1)) & (~(4 - 1));

    /**
     *  Reallocate the memory buffer to accomodate the file the zip library extacted.
     */
    buf->BufferPtr = (unsigned char *)realloc(buf->BufferPtr, buf->BufferSize + size + 1);
    assert(buf->BufferPtr != nullptr);

    /**
     *  Copy the extracted entry file data into the buffer.
     */
    memcpy(&(buf->BufferPtr[buf->BufferSize]), data, size);

    buf->BufferSize += size;
    buf->BufferPtr[buf->BufferSize] = '\0';

    return size;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
ZipFileClass2::ZipFileClass2(Wstring filename) :
    Handle(nullptr),
    IsOpen(false),
    IsZip64(false),
    Filename(),
    CompLevel(-1)
{
    if (filename.Is_Not_Empty()) {
        Filename = filename;

    } else {
        DEBUG_ERROR("Invalid filename in ZipFile constructor!\n");
    }

    /*
    **	Attach to list of zipfile.
    */
    ZipFileList.Add_Tail(this);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
ZipFileClass2::~ZipFileClass2()
{
    Close();

    /*
    **	Unlink this zipfile object from the chain.
    */
    Unlink();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Open(FileAccessType rights, int compression_level)
{
    if (Handle) {
        Close();
    }

    char mode = '\0';
    switch (rights)
    {
        default:
        case FILE_ACCESS_READ:
            mode = 'r';
            break;
        case FILE_ACCESS_WRITE:
            mode = 'w';
            break;
        case FILE_ACCESS_APPEND:
            mode = 'a';
            break;
    }

    zip_t *handle = zip_open(Filename.Peek_Buffer(), compression_level, mode);
    if (!handle) {
        return false;
    }

    Handle = handle;
    CompLevel = compression_level;
    IsOpen = true;
    IsZip64 = (zip_is64(handle) == 1);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Close()
{
    if (Handle) {
        zip_close(Handle);
        Handle = nullptr;
    }

    //CompLevel = -1;
    IsOpen = false;
    //IsZip64 = false;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int ZipFileClass2::Size() const
{
    bool opened = false;

    if (!IsOpen) {
        if (!const_cast<ZipFileClass2 *>(this)->Open()) {
            return false;
        }
        opened = true;
    }

    // TODO, needs new zip function.

    if (opened) {
        const_cast<ZipFileClass2 *>(this)->Close();
    }

    return 0;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int ZipFileClass2::Total_Entries() const
{
    bool opened = false;

    if (!IsOpen) {
        if (!const_cast<ZipFileClass2 *>(this)->Open()) {
            return false;
        }
        opened = true;
    }

    int total_entries = zip_entries_total(Handle);

    if (opened) {
        const_cast<ZipFileClass2 *>(this)->Close();
    }

    return total_entries;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
unsigned ZipFileClass2::Entry_Size(Wstring filename) const
{
    int zip_error = 0;
    bool opened = false;

    if (!IsOpen) {
        if (!const_cast<ZipFileClass2 *>(this)->Open()) {
            return false;
        }
        opened = true;
    }

    zip_error = zip_entry_open(Handle, filename.Peek_Buffer());
    if (zip_error < 0) {
        DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
        return -1;
    }

    unsigned long long size = zip_entry_size(Handle);

    zip_error = zip_entry_close(Handle);
    if (zip_error < 0) {
        DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
        return -1;
    }

    if (opened) {
        const_cast<ZipFileClass2 *>(this)->Close();
    }

    return size;
}


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Load(ZipFileClass2 * zipfile)
{
    ASSERT_FATAL_PRINT(zipfile != nullptr, "Invalid zip file handle!");

    if (!zipfile) {
        DEBUG_ERROR("Zip::Cache - Invalid zip file handle!\n");
        return false;
    }

    DEBUG_INFO("Zip::Cache - Caching zip file \"%s\"...\n", zipfile->Filename.Peek_Buffer());

    int zip_error = 0;
    bool opened = false;

    /**
     *  x
     */
    if (!zipfile->Is_Open()) {
        opened = zipfile->Open();
    }

    /**
     *  x
     */
    for (int index = 0; index < zipfile->Total_Entries(); ++index) {

        /**
         *  x
         */
        zip_t * handle = zipfile->Handle;
        ASSERT(handle != nullptr);

#if 1
        zip_t * zip2 = zip_stream_open(nullptr, 0, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
#endif

        /**
         *  x
         */
        zip_error = zip_entry_openbyindex(handle, index);
        if (zip_error < 0) {
            DEBUG_WARNING("Zip::Cache - Failed to open entry with index '%d'!\n", index);
            DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
            continue;
        }

        ssize_t entry_index = zip_entry_index(handle);
        Wstring entry_name = zip_entry_name(handle);

        /**
         *  x
         */
        entry_name.To_Upper();

        /**
         *  x
         */
        bool entry_isdir = zip_entry_isdir(handle);
        if (entry_isdir) {
            DEBUG_WARNING("Zip::Cache - \"%s\" is a directory, skipping...\n", entry_name.Peek_Buffer());
            //DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
            continue;
        }

        /**
         *  x
         */
        unsigned long long entry_uncompsize = zip_entry_uncomp_size(handle);
        unsigned long long entry_compsize = zip_entry_comp_size(handle);

        /**
         *  x
         */
        if (entry_uncompsize == 0) {
            DEBUG_WARNING("Zip::Cache - \"%s\" file size is zero! Skipping...\n", entry_name.Peek_Buffer());
            continue;
        }

        //unsigned int entry_crc = zip_entry_crc32(handle);
        unsigned int entry_crc = CRC32_String(entry_name.Peek_Buffer());

        //ASSERT_FATAL_PRINT(entry_uncompsize > 0, "entry_uncompsize is invalid for '%s'!", entry_name.Peek_Buffer());
        //ASSERT_FATAL_PRINT(entry_compsize > 0, "entry_compsize is invalid for '%s'!", entry_name.Peek_Buffer());

        DEBUG_INFO("Zip::Cache - Caching file \"%s\" (index '%d') within \"%s\"...\n", entry_name.Peek_Buffer(), entry_index, zipfile->Filename.Peek_Buffer());

        ZipBufferStruct buff;
        ZeroMemory(&buff, sizeof(ZipBufferStruct));

#if 0
        /**
         *  x
         */
        zip_error = zip_entry_extract(handle, zip_on_extract_callback, &buff);
        if (zip_error < 0) {
            DEBUG_WARNING("Zip::Cache - Failed to read \"%s\" into buffer!\n", entry_name.Peek_Buffer());
            DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
            continue;
        }
#else
        /**
         *  x
         */
        int entry_readsize = zip_entry_read(zip2, (void **)&buff.BufferPtr, &buff.BufferSize);
        if (entry_readsize < 0) {
            DEBUG_WARNING("Zip::Cache - Failed to read \"%s\" into buffer!\n", entry_name.Peek_Buffer());
            continue;
        }
#endif

        ASSERT_FATAL_PRINT(buff.BufferPtr != nullptr, "");
        ASSERT_FATAL_PRINT(buff.BufferSize > 0, "");

        /**
         *  x
         */
        ZipIndexEntry entry;
        entry.Filename = entry_name;
        entry.Container = zipfile;
        entry.IsCompressed = entry_uncompsize > entry_compsize;
        entry.UncompEntrySize = entry_uncompsize;
        entry.CompEntrySize = entry_compsize;
        entry.EntrySize = buff.BufferSize;
        entry.EntryData = buff.BufferPtr;

        FileIndex.Add_Index(entry_crc, entry);

        /**
         *  We have everything we need from the entry, it can now be closed.
         */
        zip_entry_close(handle);

#if 1
        zip_stream_close(zip2);
#endif
    }

    /**
     *  x
     */
    if (opened) {
        zipfile->Close();
    }

    /**
     *  Sort the table (by filename).
     */
    FileIndex.Sort_Table();

    return true;
}
#endif


/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Load(Wstring filename)
{
    DEBUG_INFO("Zip::Cache - Loading zip file \"%s\"...\n", zipfile->Filename.Peek_Buffer());

    int zip_error = 0;

    RawFileClass file(filename.Peek_Buffer());
    Buffer buff(file.Size());

    zip_t * zip_handle = zip_stream_open((char *)buff.Get_Buffer(), buff.Get_Size(), 0, 'r');
    if (!zip_handle) {
        return false;
    }

    int total_entries = zip_entries_total(zip_handle);
    if (!total_entries) {
        return false;
    }

    /**
     *  x
     */
    for (int index = 0; index < total_entries; ++index) {

        /**
         *  x
         */
        zip_error = zip_entry_openbyindex(zip_handle, index);
        if (zip_error < 0) {
            DEBUG_WARNING("Zip::Cache - Failed to open entry with index '%d'!\n", index);
            DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
            continue;
        }

        ssize_t entry_index = zip_entry_index(zip_handle);
        Wstring entry_name = zip_entry_name(zip_handle);
        unsigned long long entry_uncompsize = zip_entry_uncomp_size(zip_handle);
        unsigned long long entry_compsize = zip_entry_comp_size(zip_handle);

        /**
         *  x
         */
        entry_name.To_Upper();

        /**
         *  x
         */
        bool entry_isdir = zip_entry_isdir(zip_handle);
        if (entry_isdir) {
            //DEBUG_WARNING("Zip::Cache - \"%s\" is a directory, skipping...\n", entry_name.Peek_Buffer());
            ////DEBUG_WARNING("zip error - %s!\n", zip_strerror(zip_error));
            //continue;

            //info.filename = info.filename.substr(0, info.filename.length() - 1);
            //StringUtil::splitFilename(info.filename, info.basename, info.path);
            // Set compressed size to -1 for folders; anyway nobody will check
            // the compressed size of a folder, and if he does, its useless anyway
            entry_compsize = -1;
            continue;
        }

        /**
         *  x
         */
        if (entry_uncompsize == 0) {
            DEBUG_WARNING("Zip::Cache - \"%s\" file size is zero! Skipping...\n", entry_name.Peek_Buffer());
            continue;
        }

        //unsigned int entry_crc = zip_entry_crc32(handle);
        unsigned int entry_crc = CRC32_String(entry_name.Peek_Buffer());

        //ASSERT_FATAL_PRINT(entry_uncompsize > 0, "entry_uncompsize is invalid for '%s'!", entry_name.Peek_Buffer());
        //ASSERT_FATAL_PRINT(entry_compsize > 0, "entry_compsize is invalid for '%s'!", entry_name.Peek_Buffer());

        DEBUG_INFO("Zip::Cache - Caching file \"%s\" (index '%d') within \"%s\"...\n", entry_name.Peek_Buffer(), entry_index, zipfile->Filename.Peek_Buffer());

        ASSERT_FATAL_PRINT(buff.BufferPtr != nullptr, "");
        ASSERT_FATAL_PRINT(buff.BufferSize > 0, "");

        /**
         *  x
         */
        ZipIndexEntry entry;
        entry.Filename = entry_name;
        entry.Container = zipfile;
        entry.IsCompressed = entry_uncompsize > entry_compsize;
        entry.UncompEntrySize = entry_uncompsize;
        entry.CompEntrySize = entry_compsize;
        entry.EntrySize = buff.BufferSize;
        entry.EntryData = buff.BufferPtr;

        FileIndex.Add_Index(entry_crc, entry);

        /**
         *  We have everything we need from the entry, it can now be closed.
         */
        zip_entry_close(handle);

#if 1
        zip_stream_close(zip2);
#endif
    }

    /**
     *  x
     */
    if (opened) {
        zipfile->Close();
    }

    /**
     *  Sort the table (by filename).
     */
    FileIndex.Sort_Table();

    return true;
}


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Free_Cache()
{
    /**
     *  x
     */
    for (int index = 0; index < FileIndex.Count(); ++index) {

        ZipIndexEntry &entry = FileIndex.Fetch_By_Position(index);

        entry.Filename.Release_Buffer();

        ASSERT_FATAL(entry.EntryData != nullptr);

        delete entry.EntryData;
        entry.EntryData = nullptr;
    }

    /**
     *  x
     */
    FileIndex.Clear();
    ASSERT_FATAL(FileIndex.Count() == 0);

    return true;
}
#endif


/**
 *  x
 *
 *  @author: CCHyper
 */
bool ZipFileClass2::Free_Memory()
{
#if 1
    /**
     *  x
     */
    for (int index = 0; index < FileIndex.Count(); ++index) {

        ZipIndexEntry & entry = FileIndex.Fetch_By_Position(index);

        entry.Filename.Release_Buffer();
        free(entry.EntryData);
        entry.EntryData = nullptr;
    }
#endif

    /**
     *  x
     */
    DEBUG_INFO("Zip::Free_Memory - About to clear FileIndex (Count == '%d')...\n", FileIndex.Count());
    FileIndex.Clear();
    ASSERT_FATAL(FileIndex.Count() == 0);

    DEBUG_INFO("Zip::Free_Memory - About to free file entry buffer (Size == '%d')...\n", ZipMemoryBuffer.BufferSize);
    //vinifera_free(ZipMemoryBuffer.BufferPtr);
    free(ZipMemoryBuffer.BufferPtr);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
const void * ZipFileClass2::Retrieve(Wstring filename, Wstring within)
{
    filename.To_Upper();
    unsigned int target_crc = CRC32_String(filename.Peek_Buffer());

    ZipFileClass2 *ptr = ZipFileList.First();
    while (ptr->Is_Valid()) {

        if (FileIndex.Is_Present(target_crc)) {

            const ZipIndexEntry *entry = &FileIndex[target_crc];

            if (FileIndex[target_crc].Container == ptr) {

#ifndef NDEBUG
                DEBUG_INFO("Zip::Retrieve - Located \"%s\" in the FileIndex!\n", filename.Peek_Buffer());
                DEBUG_INFO("Zip::Retrieve - Info - Size: %d\n", entry->UncompEntrySize);
#endif

                return entry->EntryData;

            }

            if (within.Is_Not_Empty() && entry->Container != nullptr && entry->Container->Filename != within) {
                DEBUG_WARNING("Zip::Retrieve - Failed to locate \"%s\" within \"%s\" in the FileIndex!\n", filename.Peek_Buffer(), entry->Container->Filename.Peek_Buffer());
                return nullptr;
            }

        }

        /**
         *  Advance to next zipfile.
         */
        ptr = ptr->Next();
    }

    /*
    **	All the zipfiles have been examined but no match was found.
    */
    return false;






    const ZipIndexEntry *entry = &FileIndex[target_crc];

    if (!FileIndex.Is_Present(target_crc)) {
#ifndef NDEBUG
        DEBUG_WARNING("Zip::Retrieve - Failed to locate \"%s\" in the FileIndex!\n", filename.Peek_Buffer());
#endif
        return nullptr;
    }

    if (within.Is_Not_Empty() && entry->Container != nullptr && entry->Container->Filename != within) {
        DEBUG_WARNING("Zip::Retrieve - Failed to locate \"%s\" within \"%s\" in the FileIndex!\n", filename.Peek_Buffer(), entry->Container->Filename.Peek_Buffer());
        return nullptr;
    }

#ifndef NDEBUG
    DEBUG_INFO("Zip::Retrieve - Located \"%s\" in the FileIndex!\n", filename.Peek_Buffer());
    DEBUG_INFO("Zip::Retrieve - Info - Size: %d\n", entry->UncompEntrySize);
#endif

    return entry->EntryData;
}


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
void ZipFileClass2::List_All_Entries(Wstring within)
{
    struct zip_t* zip = zip_open("foo.zip", 0, 'r');
    int i, n = zip_entries_total(zip);
    for (i = 0; i < n; ++i) {
        zip_entry_openbyindex(zip, i);
        {
            const char* name = zip_entry_name(zip);
            int isdir = zip_entry_isdir(zip);
            unsigned long long size = zip_entry_size(zip);
            unsigned int crc32 = zip_entry_crc32(zip);
        }
        zip_entry_close(zip);
    }
    zip_close(zip);
}
#endif


#endif




#endif
