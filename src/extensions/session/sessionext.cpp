/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended SessionClass class.
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
#include "sessionext.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "session.h"
#include "multimiss.h"
#include "noinit.h"
#include "options.h"
#include "ccini.h"
#include "rawfile.h"
#include "voc.h"
#include "rules.h"
#include "cd.h"
#include "asserthandler.h"
#include "debughandler.h"


SessionClassExtension *SessionExtension = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(SessionClass *this_ptr) :
    Extension(this_ptr)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));

   /**
     *  Initialises the default game options.
     */
    ExtOptions.IsAutoDeployMCV = false;
    ExtOptions.IsPrePlacedConYards = false;
    ExtOptions.IsBuildOffAlly = true;
    
    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::~SessionClassExtension()
{
    //EXT_DEBUG_TRACE("SessionClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int SessionClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Read_MultiPlayer_Settings()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Read_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension::Read_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Saves the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Write_MultiPlayer_Settings()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Write_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension::Write_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Reads in scenario descriptions.
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Read_Scenario_Descriptions()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Read_Scenario_Descriptions - 0x%08X\n", (uintptr_t)(ThisPtr));

    /**
     *  Scan the current directory for any loose .MMX files and build the
     *  appropriate entries into the scenario list.
     */
    char name_buffer[128];
    char digest_buffer[32];
    char pkt_buffer[128];
    CCFileClass file;
    INIClass ini;

    file.Close();
    ini.Clear();

    int temp = CD::RequiredCD;
    CD::Set_Required_CD(DISK_LOCAL);

    WIN32_FIND_DATA block;
    HANDLE handle = FindFirstFile("*.MMX", &block);
    while (handle != INVALID_HANDLE_VALUE) {
        if ((block.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY)) == 0) {
            
#ifndef NDEBUG
            //DEBUG_INFO("Found file \"%s\"\n", block.cAlternateFileName);
            //DEBUG_INFO("Found file \"%s\"\n", block.cFileName);
#endif

            /**
             *  #BUGFIX:
             *  cAlternateFileName stores the filename in 8.3 format, so we need
             *  to use cFileName to get the true, full filename.
             */
            //const char *file_name = &block.cAlternateFileName[0];
            //if (*file_name == '\0') file_name = &block.cFileName[0];

            char *file_name = block.cFileName;
            if (file_name[0] != '\0') {

                /**
                 *  
                 */
                if (strcmpi(file_name, "MISSIONS.MMX") != 0) {
            
                    strupr(file_name);
                    DEBUG_INFO("Found MMX \"%s\"\n", file_name);

                    MFCC *mix = new MFCC(file_name, &FastKey);
                    if (mix) {

                        ViniferaScenarioMixes.Add(mix);

                        std::strncpy(pkt_buffer, file_name, std::strlen(file_name)-4);
                        pkt_buffer[std::strlen(file_name)-4] = '\0';
                        strcat(pkt_buffer, ".PKT");

                        file.Set_Name(pkt_buffer);
                        if (file.Is_Available()) {

                            ini.Load(file);

			                int count = ini.Entry_Count("MultiMaps");
			                for (int index = 0; index < count; ++index) {
				                const char *fname = ini.Get_Entry("MultiMaps", index);
				                char buffer[128];
				                if (ini.Get_String("MultiMaps", fname, "", buffer, sizeof(buffer)) > 0) {

#if 0 // No need for digest information, these map packs are "unofficial".
                                    ini.Get_String("Basic", "Name", "No Name", name_buffer, sizeof(name_buffer));
                                    ini.Get_String("Digest", "1", "No Digest", digest_buffer, sizeof(digest_buffer));
                                    MultiMission *multimiss = new MultiMission(buffer, name_buffer, digest_buffer, ini.Get_Bool("Basic", "Official", false)));
#else
                                    MultiMission *multimiss = new MultiMission(ini, buffer);
#endif
                                    if (multimiss) {
                                        ThisPtr->Scenarios.Add(multimiss);
                                    }

                                }

                            }

                        } else {
                            DEBUG_WARNING("Can't see .PKT inside \"%s\"!\n", file_name);
                        }

                    } else {
                        DEBUG_WARNING("Failed to load \"%s\"!\n", file_name);
                    }
                }
            }
        }
            
        ini.Clear();
        file.Close();

        if (FindNextFile(handle, &block) == 0) break;
    }

    CD::Set_Required_CD(temp);
}
