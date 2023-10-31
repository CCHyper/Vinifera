/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ARCHIVEFILE.H
 *
 *  @author        CCHyper
 *
 *  @brief         Interface for loading files from archives.
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
#pragma once

#include "always.h"
#include "wstring.h"
#include "vector.h"
#include "search.h"


class ArchiveFileClass
{
    protected:
        struct FileInfoStruct
        {
            /// The archive in which the file has been found (for info when performing
            /// multi-Archive searches, note you should still open through ResourceGroupManager)
            const ArchiveFileClass * Archive;

            unsigned long FilenameCRC32;    // 
            Wstring Filename;               // The file's fully qualified name.
            Wstring Path;                   // Path name; separated by '/' and ending with '/'.
            Wstring Basename;               // Base filename.

            size_t CompressedSize;          // Compressed size.
            size_t UncompressedSize;        // Uncompressed size.

            bool operator == (const FileInfoStruct &info) const { return FilenameCRC32 == info.FilenameCRC32; }
            bool operator != (const FileInfoStruct &info) const { return FilenameCRC32 != info.FilenameCRC32; }

            const FileInfoStruct & operator = (const FileInfoStruct & info)
            {
                if (this != &info) {
                    Archive = info.Archive;
                    FilenameCRC32 = info.FilenameCRC32;
                    Filename = info.Filename;
                    Path = info.Path;
                    Basename = info.Basename;
                    CompressedSize = info.CompressedSize;
                    UncompressedSize = info.UncompressedSize;
                }
                return *this;
            }
        };

    public:
        ArchiveFileClass(Wstring filename) : Filename(filename), IsReadOnly(false), FileList() {}
        virtual ~ArchiveFileClass() { FileList.Clear(); }

        virtual Wstring Get_Filename() const { return Filename; }

        virtual bool Is_Read_Only() const { return IsReadOnly; }

        virtual bool Load(bool in_thread = false) = 0;
        virtual bool Unload() = 0;

        virtual bool Open(Wstring &filename, bool ready_only = true) = 0;

        virtual bool Exists(Wstring &filename) const = 0;

        virtual DynamicVectorClass<Wstring> Filename_List(bool recursive = true, bool dirs = false) const = 0;

        virtual const FileInfoStruct * Find_File_Info(Wstring &filename, bool read_only = true) = 0;

    protected:
        Wstring Filename;
        bool IsReadOnly;
        IndexClass<unsigned long, FileInfoStruct> FileList;
};
