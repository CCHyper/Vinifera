/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended SessionClass.
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
#include "sessionext_functions.h"
#include "session.h"
#include "multimiss.h"
#include "ccini.h"
#include "addon.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  
 * 
 *  @author: CCHyper
 */
void Session_Read_Scenario_Descriptions(SessionClass *session)
{
    if (!session) {
        return;
    }

    /**
     *  Clear the scenario description lists.
     */
    session->Scenarios.Clear();

    INIClass ini;
    CCFileClass file;

    /**
     *  Fetch the main multiplayer scenario packet data.
     */
    if (CCFileClass("MISSIONS.PKT").Is_Available()) {

        file.Set_Name("MISSIONS.PKT");
        ini.Clear();

        ini.Load(file);
        int count = ini.Entry_Count("MultiMaps");
        
        DEV_DEBUG_INFO("Found %d missions in \"MISSIONS.PKT\"\n", count);

        for (int index = 0; index < count; index++) {
            char const *fname = ini.Get_Entry("MultiMaps", index);
            char buffer[128];
            ini.Get_String("MultiMaps", fname, "", buffer, sizeof(buffer));

            session->Scenarios.Add(new MultiMission(fname, buffer));
        }
    }

    ini.Clear();
    file.Close();

    /**
     *  Fetch the addon multiplayer scenario packet data.
     */
    char fname_buffer[16];
    for (int addon = AddonType(2); addon > ADDON_NONE; --addon) {

        if (!Addon_407150(addon)) {
            continue;
        }

        std::snprintf(fname_buffer, sizeof(fname_buffer), "MULTI%02d.PKT", addon);

        if (CCFileClass(fname_buffer).Is_Available()) {
            
            file.Set_Name(fname_buffer);
            ini.Clear();

            ini.Load(file);
            int count = ini.Entry_Count("MultiMaps");
        
            DEV_DEBUG_INFO("Found %d missions in \"%s\"\n", fname_buffer);

            for (int index = 0; index < count; index++) {
                char const *fname = ini.Get_Entry("MultiMaps", index);
                char buffer[128];
                ini.Get_String("MultiMaps", fname, "", buffer, sizeof(buffer));

                session->Scenarios.Add(new MultiMission(fname, buffer));
            }
        }
    }

    ini.Clear();
    file.Close();

    /**
     *  Fetch any scenario packet lists and apply them first.
     */
    WIN32_FIND_DATA block;
    HANDLE handle = FindFirstFile("*.PKT", &block);
    while (handle != INVALID_HANDLE_VALUE) {
        if ((block.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY)) == 0) {
            char const * name = &block.cAlternateFileName[0];
            if (*name == '\0') name = &block.cFileName[0];
            
            DEV_DEBUG_INFO("Found alternate PKT file \"%s\".\n", name);

            file.Set_Name(name);
            ini.Clear();

            ini.Load(file);

            int count = ini.Entry_Count("Missions");
            for (int index = 0; index < count; index++) {
                char const * fname = ini.Get_Entry("Missions", index);
                char buffer[128];
                ini.Get_String("Missions", fname, "", buffer, sizeof(buffer));

                session->Scenarios.Add(new MultiMission(fname, buffer));
            }
        }

        if (FindNextFile(handle, &block) == 0) break;
    }

    ini.Clear();
    file.Close();

    /**
     *  Scan the current directory for any loose .MPR files and build the
     *  appropriate entries into the scenario list list.
     */
    char const * file_name;
    char name_buffer[128];
    char digest_buffer[32];

    handle = FindFirstFile("*.MPR", &block);
    while (handle != INVALID_HANDLE_VALUE) {
        if ((block.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY)) == 0) {
            file_name = &block.cAlternateFileName[0];
            if (*file_name == '\0') file_name = &block.cFileName[0];

            DEV_DEBUG_INFO("Found MPR \"%s\"\n", file_name);

            file.Set_Name(file_name);

            ini.Load(file);

            ini.Get_String("Basic", "Name", "No Name", name_buffer, sizeof (name_buffer));
            ini.Get_String("Digest", "1", "No Digest", digest_buffer, sizeof (digest_buffer));
            session->Scenarios.Add(new MultiMission(file_name, name_buffer, digest_buffer,ini.Get_Bool("Basic", "Official", false)));
        }

        if (FindNextFile(handle, &block) == 0) break;
    }
}
