/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CCFILEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended CCFileClass.
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
#include "ccfileext_hooks.h"
#include "ccfile.h"
#include "cd.h"
#include "wstring.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class CCFileClassFake final : public CCFileClass
{
    public:
        bool _Is_Available(bool forced = true);
        void _Error(FileErrorType error, bool can_retry = false, const char *filename = nullptr);
};


/**
 *  Checks for existence of file on disk or in mixfile.
 * 
 *  The original CDFileClass did not have this function implemented, so
 *  availability checks with CCFileClass do not check the search paths.
 * 
 *  @author: CCHyper
 */
bool CCFileClassFake::_Is_Available(bool forced)
{
    //DEV_DEBUG_INFO("Checking if %s is available.\n", Filename);

    /**
     *  Backup of the current filename.
     */
    Wstring fname = Filename;

    /**
     *	A file that is open is presumed available.
     */
    if (CCFileClass::Is_Open()) {
        return true;
    }

    /**
     *	First do a manual check if it is available with the current filename.
     */
    if (BufferIOFileClass::Is_Available()) {
        return true;
    }

    /**
     *  Attempt to find the file first. Check the current directory. If not found there, then
     *  search all the path specifications available. If it still can't be found, then just
     *  fall into the normal raw file filename setting system.
     */
    char path[_MAX_PATH];
    SearchDriveType * srch = CDFileClass::First;

    while (srch) {

        /**
         *  Build a pathname to search for.
         */
        std::snprintf(path, sizeof(path), "%s%s", srch->Path, fname.Peek_Buffer());

        //DEV_DEBUG_INFO("Path is %s.\n", path);

        /**
         *  Check to see if the file could be found. The low level Is_Available logic will
         *  prompt if necessary when the CD-ROM drive has been removed. In all other cases,
         *  it will return false and the search process will continue.
         */
        BufferIOFileClass::Set_Name(path);
        //DEV_DEBUG_INFO("Checking if %s is available.\n", fname.Peek_Buffer());
        if (BufferIOFileClass::Is_Available()) {
            return true;
        }

        /**
         *  It wasn't found, so try the next path entry.
         */
        srch = (SearchDriveType *)srch->Next;
    }

    return false;
}


/**
 *  Handles displaying a file error message. 
 * 
 *  @author: 10/17/1994 JLB - Red Alert source code.
 *           CCHyper - Adjustments for Tiberian Sun, minor bug fix.
 */
void CCFileClassFake::_Error(FileErrorType error, bool can_retry, const char *filename)
{
    /**
     *  File system is failled as local, no need to check if required cd is available.
     */
    if (CD::IsFilesLocal) {
        CDFileClass::Error(error, can_retry, filename);

    } else {

        /**
         *  If the file was not found, its possible we have the wrong disk inserted
         *  so prompt the user to insert the correct disk.
         */
        if (!CD().Is_Available(CD::RequiredCD)) {

            DEV_DEBUG_ERROR("File - Error, CD '%d' not found!", CD::RequiredCD);

            /**
             *  If still not available, now let the low level file interface report the error.
             */
            CDFileClass::Error(error, can_retry, filename);
        }

    }
}



/**
 *  Main function for patching the hooks.
 */
void CCFileClassExtension_Hooks()
{
    /**
     *  These patches replace specific calls to CCFileClass::Is Available.
     */
    Patch_Call(0x004E092C, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E09EE, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E11B3, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E11F9, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E140E, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E155A, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005C64AF, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005C6547, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005C661A, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005D1B3B, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005DDC98, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005DDD1D, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005DDE66, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005EF50A, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005EF1E8, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005EE8CA, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005E4655, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005ACE26, &CCFileClassFake::_Is_Available);
    Patch_Call(0x005ACCEB, &CCFileClassFake::_Is_Available);
    Patch_Call(0x00505020, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004E0DEB, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004964E8, &CCFileClassFake::_Is_Available);
    Patch_Call(0x004B70DB, &CCFileClassFake::_Is_Available);

    Patch_Jump(0x00449820, &CCFileClassFake::_Error);
}
