/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ZIPFILE.H
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
#pragma once

#include "always.h"
#include "archivefile.h"
#include "buff.h"
#include "listnode.h"
#include "wstring.h"

#if 0

#include "ZipFile.h"
#include "ZipArchive.h"


class ZipFileClass2 : public ArchiveFileClass, public Node<ZipFileClass2 *>
{
    public:
        ZipFileClass2(Wstring filename);
        virtual ~ZipFileClass2();

        virtual bool Load() override;
        virtual bool Unload() override;

        virtual bool Open(Wstring &filename, bool ready_only = true) override;

        virtual bool Exists(Wstring &filename) const override;

        virtual DynamicVectorClass<Wstring> Filename_List(bool recursive = true, bool dirs = false) const override;

        virtual const FileInfoStruct * Find_File_Info(Wstring &filename, bool read_only = true) override;

    private:
        virtual const void * Retrieve_Memory(Wstring &filename);

        bool Build_Index();

    private:
        /**
         *  x
         */
        ZipArchive::Ptr ZipFile;

        /**
         *  x
         */
        Buffer ZipFileBuffer;

        /**
         *  An index list of all files within a zip file and their entry info.
         */
        struct ZipEntryStruct
        {
            ZipArchiveEntry::Ptr EntryPtr;      // 
            ZipFileClass2 * Container;          // The zip file in which this file is within.
            unsigned long CRC;                  // 
            std::ifstream * DataStream;

            //Wstring Filename;                   // Filename for quick retrival.
            //bool IsCompressed;                  // 
            //unsigned long long UncompEntrySize; // 
            //unsigned long long CompEntrySize;   // 
            //unsigned long EntrySize;            // 
            //unsigned char * EntryData;          // Pointer to the data loaded in memory.
        };

        IndexClass<unsigned int, ZipEntryStruct> ZipFileIndex;
};

#endif
