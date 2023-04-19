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

#if defined(VINIFERA_USE_VFS)

#include "filesys.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <CVirtualFileSystem.h>
#include <CNativeFileSystem.h>
#include <CMemoryFileSystem.h>
#include <CZipFileSystem.h>


#define INITIALIZE_NATIVE_FILE_SYSTEM(path, alias) \
{ \
    vfspp::IFileSystemPtr fs(new vfspp::CNativeFileSystem(path)); \
    fs->Initialize(); \
    vfspp::vfs_get_global()->AddFileSystem(alias, fs); \
}

#define INITIALIZE_ZIP_FILE_SYSTEM(zip, base, alias) \
{ \
    vfspp::IFileSystemPtr fs(new vfspp::CZipFileSystem(zip, base)); \
    fs->Initialize(); \
    vfspp::vfs_get_global()->AddFileSystem(alias, fs); \
} 


void VFS_Test()
{
    // Root file system.
    INITIALIZE_NATIVE_FILE_SYSTEM("./", "/");

    // Custom file systems.
    INITIALIZE_NATIVE_FILE_SYSTEM("./ini", "/ini");
    INITIALIZE_NATIVE_FILE_SYSTEM("./movie", "/movie");
    INITIALIZE_NATIVE_FILE_SYSTEM("./sounds", "/sounds");
    INITIALIZE_NATIVE_FILE_SYSTEM("./music", "/music");

    // Zip archive file system.
    INITIALIZE_NATIVE_FILE_SYSTEM("./zip", "/zip");

    // Game archive file systems.
    INITIALIZE_ZIP_FILE_SYSTEM("./tibsun.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./expand01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./movies01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./movies02.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./movies03.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./scores.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./scores01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./sounds.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./sounds01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./maps01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./maps02.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./maps03.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./sidecd01.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./sidecd02.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./gmenu.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./wdt.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./wdtvox.zip", "/", "/zip");
    INITIALIZE_ZIP_FILE_SYSTEM("./patch.zip", "/", "/zip");

    vfspp::CVirtualFileSystemPtr vfs = vfspp::vfs_get_global();

    vfspp::IFilePtr file = vfs->OpenFile(vfspp::CFileInfo("wwlogo.vqa"), vfspp::IFile::In);
    if (file && file->IsOpened())
    {
        char data[256];
        file->Read(reinterpret_cast<uint8_t*>(data), 256);

        DEBUG_INFO("%s\n", data);
    }



}

#endif
