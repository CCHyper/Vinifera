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
#include <zip.h>


class ZipFileClass : public ArchiveFileClass, public Node<ZipFileClass *>
{
    public:
        ZipFileClass(Wstring filename);
        virtual ~ZipFileClass();

        virtual bool Load() override;
        virtual bool Unload() override;

        virtual bool Open(Wstring &filename, bool ready_only = true) override;

        virtual bool Exists(Wstring &filename) const override;

        virtual DynamicVectorClass<Wstring> Filename_List(bool recursive = true, bool dirs = false) const override;

        virtual const FileInfoStruct * Find_File_Info(Wstring &filename, bool read_only = true) override;

    private:
        /**
         *  The zip archive handle.
         */
        zip_t *ZipFileHandle;

        /**
         *  x
         */
        Buffer ZipFileBuffer;
};
