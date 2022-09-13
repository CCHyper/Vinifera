/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RULESEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended RulesClass class.
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
#include "rulesext.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "ccini.h"
#include "rules.h"
#include "tiberium.h"
#include "weapontype.h"
#include "wwcrc.h"
#include "noinit.h"
#include "swizzle.h"
#include "vinifera_saveload.h"
#include "asserthandler.h"
#include "debughandler.h"


RulesClassExtension *RulesExtension = nullptr;
RulesClassExtension::UIControlsStruct RulesClassExtension::UIControls;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
RulesClassExtension::RulesClassExtension(const RulesClass *this_ptr) :
    This(const_cast<RulesClass *>(this_ptr)),
    IsMPAutoDeployMCV(false),
    IsMPPrePlacedConYards(false),
    IsBuildOffAlly(true),
    IsShowSuperWeaponTimers(true)
{
    //EXT_DEBUG_TRACE("RulesClassExtension constructor - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
RulesClassExtension::RulesClassExtension(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
RulesClassExtension::~RulesClassExtension()
{
    //EXT_DEBUG_TRACE("RulesClassExtension destructor - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Initializes an object from the stream where it was saved previously.
 * 
 *  As RulesClassExtension is static data, we do not need to request
 *  pointer remap of "ThisPtr" after loading has finished.
 *  
 *  @author: CCHyper
 */
HRESULT RulesClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Load - 0x%08X\n", (uintptr_t)(This()));

    if (!pStm) {
        return E_POINTER;
    }

    /**
     *  Load the unique id for this class.
     */
    ULONG id = 0;
    HRESULT hr = pStm->Read(&id, sizeof(ULONG), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    new (this) RulesClassExtension(NoInitClass());

    /**
     *  x
     */
    VINIFERA_SWIZZLE_REGISTER_POINTER(id, this, "this");

    /**
     *  Read this classes instance binary blob data.
     */
    hr = pStm->Read(this, Size_Of(), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT RulesClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Save - 0x%08X\n", (uintptr_t)(This()));

    if (!pStm) {
        return E_POINTER;
    }

    /**
     *  x
     */
    ULONG id = (ULONG)this;
    HRESULT hr = pStm->Write(&id, sizeof(id), nullptr);
    if (FAILED(hr)) {
        return hr;
    }
    
    /**
     *  Write this class instance as a binary blob.
     */
    hr = pStm->Write(this, Size_Of(), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int RulesClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void RulesClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void RulesClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));

    crc(IsMPAutoDeployMCV);
    crc(IsMPPrePlacedConYards);
    crc(IsBuildOffAlly);
    crc(IsShowSuperWeaponTimers);
}


/**
 *  Fetch the bulk of the rule data from the control file.
 *  
 *  @author: CCHyper
 */
void RulesClassExtension::Process(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Process - 0x%08X\n", (uintptr_t)(This()));

    /**
     *  This function replaces the original rules process, so we need to duplicate
     *  the its behaviour here first.
     */

    This->Colors(ini);
    This->Houses(ini);
    This->Sides(ini);
    This->Overlays(ini);

    /**
     *  #issue-117
     * 
     *  Add reading of Weapons list from RULES.INI. This needs to be done before
     *  all weapon
     * 
     *  @author: CCHyper
     */
    Weapons(ini);

    This->SuperWeapons(ini);
    This->Warheads(ini);
    This->Smudges(ini);
    This->Terrains(ini);
    This->Buildings(ini);
    This->Vehicles(ini);
    This->Aircraft(ini);
    This->Infantry(ini);
    This->Animations(ini);
    This->VoxelAnims(ini);
    This->Particles(ini);
    This->ParticleSystems(ini);
    This->JumpjetControls(ini);
    This->MPlayer(ini);
    This->AI(ini);
    This->Powerups(ini);
    This->Land_Types(ini);
    This->IQ(ini);
    This->General(ini);
    This->Objects(ini);
    This->Difficulty(ini);
    This->CrateRules(ini);
    This->CombatDamage(ini);
    This->AudioVisual(ini);
    This->SpecialWeapons(ini);
    TiberiumClass::Process(ini);

    /**
     *  Process the rules extension.
     * 
     *  #NOTE: These must be performed last!
     */
    General(ini);
    MPlayer(ini);
    AudioVisual(ini);

    /**
     *  Run some checks to ensure certain values are as expected.
     */
    Check();
}


/**
 *  Process and initialise rule data from the control file.
 *  
 *  @author: CCHyper
 */
void RulesClassExtension::Initialize(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Process the general main game rules.
 *  
 *  @author: CCHyper
 */
bool RulesClassExtension::General(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::General - 0x%08X\n", (uintptr_t)(This()));

    static char const * const GENERAL = "General";

    if (!ini.Is_Present(GENERAL)) {
        return false;
    }

    return true;
}


/**
 *  Process the audio/visual game settings.
 *  
 *  @author: CCHyper
 */
bool RulesClassExtension::AudioVisual(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::General - 0x%08X\n", (uintptr_t)(This()));

    static char const * const AUDIOVISUAL = "AudioVisual";

    if (!ini.Is_Present(AUDIOVISUAL)) {
        return false;
    }

    IsShowSuperWeaponTimers = ini.Get_Bool(AUDIOVISUAL, "ShowSuperWeaponTimers", IsShowSuperWeaponTimers);

    return true;
}


/**
 *  Process the general main game rules.
 *  
 *  @author: CCHyper
 */
bool RulesClassExtension::MPlayer(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::MPlayer - 0x%08X\n", (uintptr_t)(This()));

    static char const * const MPLAYER = "MultiplayerDefaults";

    if (!ini.Is_Present(MPLAYER)) {
        return false;
    }

    IsMPAutoDeployMCV = ini.Get_Bool(MPLAYER, "AutoDeployMCV", IsMPAutoDeployMCV);
    IsMPPrePlacedConYards = ini.Get_Bool(MPLAYER, "PrePlacedConYards", IsMPPrePlacedConYards);
    IsBuildOffAlly = ini.Get_Bool(MPLAYER, "BuildOffAlly", IsBuildOffAlly);

    return true;
}


/**
 *  Fetch all the weapon characteristic values.
 * 
 *  @author: CCHyper
 */
bool RulesClassExtension::Weapons(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("RulesClassExtension::Weapons - 0x%08X\n", (uintptr_t)(This()));

    static const char * const WEAPONS = "Weapons";

    char buf[128];
    const WeaponTypeClass *weapontype;

    int counter = ini.Entry_Count(WEAPONS);
    for (int index = 0; index < counter; ++index) {
        const char *entry = ini.Get_Entry(WEAPONS, index);

        /**
         *  Get a weapon entry.
         */
        if (ini.Get_String(WEAPONS, entry, buf, sizeof(buf))) {

            /**
             *  Find or create a weapon of the name specified.
             */
            weapontype = WeaponTypeClass::Find_Or_Make(buf);
            if (weapontype) {
                DEV_DEBUG_INFO("Rules: Found WeaponType \"%s\".\n", buf);
            } else {
                DEV_DEBUG_WARNING("Rules: Error processing WeaponType \"%s\"!\n", buf);
            }

        }

    }

    return counter > 0;
}


/**
 *  Process the UI controls and overrides.
 * 
 *  This function is here for simplicity, and is loaded from its own INI, not Rules.
 *  
 *  @author: CCHyper
 */
bool RulesClassExtension::Read_UI_INI()
{
    static char const * const GENERAL = "General";
    static char const * const INGAME = "Ingame";

    CCFileClass file("UI.INI");
    CCINIClass ini(file);

    //if (!ini.Is_Present(GENERAL)) {
    //    return false;
    //}

    UIControls.UnitHealthBarDrawPos = ini.Get_Point(INGAME, "UnitHealthBarPos", UIControls.UnitHealthBarDrawPos);
    UIControls.InfantryHealthBarDrawPos = ini.Get_Point(INGAME, "InfantryHealthBarPos", UIControls.InfantryHealthBarDrawPos);

    UIControls.IsTextLabelOutline = ini.Get_Bool(INGAME, "TextLabelOutline", UIControls.IsTextLabelOutline);
    UIControls.TextLabelBackgroundTransparency = ini.Get_Int_Clamp(INGAME, "TextLabelBackgroundTransparency", 0, 100, UIControls.TextLabelBackgroundTransparency);

    return true;
}


/**
 *  Initialises the UI controls defaults.
 *  
 *  @author: CCHyper
 */
bool RulesClassExtension::Init_UI_Controls()
{
    /**
     *  #issue-541
     * 
     *  The health bar graphics "Y" position on selection boxes is off by 1 pixel.
     * 
     *  @author: CCHyper
     */
    UIControls.UnitHealthBarDrawPos.X = -25;
    UIControls.UnitHealthBarDrawPos.Y = -16; // was -15

    UIControls.InfantryHealthBarDrawPos.X = -24;
    UIControls.InfantryHealthBarDrawPos.Y = -5;

    UIControls.IsTextLabelOutline = true;
    UIControls.TextLabelBackgroundTransparency = 50;

    return false;
}


/**
 *  Performs checks on rules data to ensure values are as expected.
 *  
 *  @author: CCHyper
 */
void RulesClassExtension::Check()
{
    ASSERT_PRINT(This->CreditTicks.Count() == 2, "CreditTicks must contain 2 valid entries!");
}
