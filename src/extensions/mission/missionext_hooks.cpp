/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MISSIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended MissionClass.
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
#include "missionext_hooks.h"
#include "missionext.h"
#include "mission.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  This patch extends the MissionType switch in MissionClass::AI.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MissionClass_AI_Extend_Switch_Patch)
{
    GET_REGISTER_STATIC(MissionClass *, this_ptr, edi);
    static MissionType mission;

    mission = this_ptr->Mission;

#ifndef NDEBUG
    /**
     *  Helper info for debugging when adding new script missions.
     */
    //DEV_DEBUG_INFO("MissionClass::AI()\n");
#endif

    /**
     *  Skip null team script missions.
     */
    if (this_ptr->Mission == MISSION_NONE) {
        goto mission_statue;
    }

    /**
     *  Handle the original MissionType's.
     */
    if (this_ptr->Mission < MISSION_COUNT) {
#ifndef NDEBUG
        //if (this_ptr->Is_Players_Army()) {
        //    DEV_DEBUG_INFO("Executing mission \"%s\" for \"%s\".\n", MissionClassExtension::Mission_Name(this_ptr->Mission), this_ptr->Name());
        //}
#endif
        goto mission_switch;
    }

    /**
     *  New MissionType switch.
     */
    if (this_ptr->Mission < NEW_MISSION_COUNT) {
#ifndef NDEBUG
        if (this_ptr->Is_Players_Army()) {
            DEV_DEBUG_INFO("Executing mission \"%s\" for \"%s\".\n", MissionClassExtension::Mission_Name(this_ptr->Mission), this_ptr->Name());
        }
#endif
    }

    /**
     *  Execute the new trigger action.
     */
    MissionClassExtension::Switch(this_ptr);

    /**
     *  The default case, return doing nothing.
     */
mission_statue:
    JMP(0x005593BB);

    /**
     *  The switch case for the original MissionType's
     */
mission_switch:
    _asm { mov eax, mission }
    JMP_REG(ecx, 0x00559062);
}


/**
 *  Main function for patching the hooks.
 */
void MissionClassExtension_Hooks()
{
    Patch_Jump(0x00559056, &_MissionClass_AI_Extend_Switch_Patch);

    Patch_Jump(0x00559760, &MissionClassExtension::Mission_From_Name);
    Patch_Jump(0x005597A0, &MissionClassExtension::Mission_Name);

    /**
     *  Patch in the new mission name array.
     */
    Patch_Dword(0x0055959D+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x005595C2+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x005595F0+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x00559619+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x00559642+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x0055966B+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x00559694+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x005596BD+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x005596E6+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x00559713+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x0055976B+3, (uintptr_t)&MissionClassExtension::Missions);
    Patch_Dword(0x005597A5+3, (uintptr_t)&MissionClassExtension::Missions);
}
