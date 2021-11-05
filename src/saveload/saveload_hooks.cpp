/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SAVELOAD_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for related to save/load.
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
#include "ext_hooks.h"
#include "tibsun_globals.h"
#include "theme.h"
#include "saveload.h"
#include "loadoptions.h"
#include "language.h"
#include "newswizzle_hooks.h"
#include "vinifera_util.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "fatal.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-269
 * 
 *  Adds a "Load Game" button to the dialog shown on mission lose.
 * 
 *  @author: CCHyper
 */
static bool _Save_Games_Available()
{
    return LoadOptionsClass().Read_Save_Files();
}

static bool _Do_Load_Dialog()
{
    return LoadOptionsClass().Load_Dialog();
}

DECLARE_PATCH(_Do_Lose_Create_Lose_WWMessageBox)
{
    static int ret;

    /**
     *  Show the message box.
     */
retry_dialog:
    ret = Vinifera_Do_WWMessageBox(Text_String(TXT_TO_REPLAY), Text_String(TXT_YES), Text_String(TXT_NO), "Load Game");
    switch (ret) {
        default:
        case 0: // User pressed "Yes"
            JMP(0x005DCE1A);

        case 1: // User pressed "No"
            JMP(0x005DCE56);

        case 2: // User pressed "Load Game"
        {
//#if defined(RELEASE) || !defined(NDEBUG)
            /**
             *  If no save games are available, notify the user and return back
             *  and reissue the main dialog.
             */
            if (!_Save_Games_Available()) {
                Vinifera_Do_WWMessageBox("No saved games available.", Text_String(TXT_OK));
                goto retry_dialog;
            }

            /**
             *  Show the load game dialog.
             */
            ret = _Do_Load_Dialog();
            if (ret) {
                Theme.Stop();
                JMP(0x005DCE48);
            }
//#else
//            /**
//             *  We disable loading in non-release builds.
//             */
//            Vinifera_Do_WWMessageBox("Saving and Loading is disabled for non-release builds.", Text_String(TXT_OK));
//#endif

            /**
             *  Reissue the dialog if the user pressed cancel on the load dialog.
             */
            goto retry_dialog;
        }
    };
}


#if 0//#ifndef RELEASE
/**
 *  Disables the Load, Save and Delete buttons in the options menu.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SaveLoad_Disable_Buttons)
{
    GET_REGISTER_STATIC(HWND, hDlg, ebp);

    EnableWindow(GetDlgItem(hDlg, 1310), FALSE); // Load button
    EnableWindow(GetDlgItem(hDlg, 1311), FALSE); // Save button
    EnableWindow(GetDlgItem(hDlg, 1312), FALSE); // Delete button

    JMP(0x004B6DF5);
}

/**
 *  Disables the Load button on the Firestorm main menu.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_NewMenuClass_Process_Disable_Load_Button_Firestorm)
{
    JMP(0x0057FFAC);
}

/**
 *  Disables the Load button on the Tiberian Sun main menu.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_NewMenuClass_Process_Disable_Load_Button_TiberianSun)
{
    JMP(0x00580075);
}
#endif


void SaveLoad_Hooks()
{
    Patch_Jump(0x005DCDFD, &_Do_Lose_Create_Lose_WWMessageBox);

#if 0//#ifndef RELEASE
    if (!Vinifera_DeveloperMode) {
        /**
         *  Disable loading and saving in non-release builds.
         */
        Patch_Jump(0x004B6D96, &_SaveLoad_Disable_Buttons);
        Patch_Jump(0x0057FF8B, &_NewMenuClass_Process_Disable_Load_Button_Firestorm);
        Patch_Jump(0x0058004D, &_NewMenuClass_Process_Disable_Load_Button_TiberianSun);
    }
#endif

    /**
     *  Replaces ALL calls to Swizzler functions in the original game with call to our debug wrappers.
     */
    NewSwizzle_Hooks();
}
