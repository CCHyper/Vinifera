/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXTENSION_GLOBALS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "extension.h"
#include "tibsun_functions.h"
#include "vinifera_saveload.h"
#include "wstring.h"
#include "vector.h"
#include "tclassfactory.h"
#include "swizzle.h"
#include "tracker.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "abstract.h"

#include "aircraft.h"
#include "aircrafttype.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "buildingtype.h"
#include "bullettype.h"
#include "campaign.h"
#include "side.h"
#include "house.h"
#include "housetype.h"
#include "infantry.h"
#include "infantrytype.h"
#include "isotiletype.h"
#include "objecttype.h"
#include "overlaytype.h"
#include "particlesystype.h"
#include "particletype.h"
#include "smudgetype.h"
#include "super.h"
#include "supertype.h"
#include "terrain.h"
#include "terraintype.h"
#include "tiberium.h"
#include "unit.h"
#include "unittype.h"
#include "voxelanimtype.h"
#include "warheadtype.h"
#include "wave.h"
#include "weapontype.h"

#include "aircraftext.h"
#include "aircrafttypeext.h"
#include "animext.h"
#include "animtypeext.h"
#include "buildingext.h"
#include "buildingtypeext.h"
#include "bullettypeext.h"
#include "campaignext.h"
#include "sideext.h"
#include "houseext.h"
#include "housetypeext.h"
#include "infantryext.h"
#include "infantrytypeext.h"
#include "isotiletypeext.h"
#include "objecttypeext.h"
#include "overlaytypeext.h"
#include "particlesystypeext.h"
#include "particletypeext.h"
#include "smudgetypeext.h"
#include "superext.h"
#include "supertypeext.h"
#include "terrainext.h"
#include "terraintypeext.h"
#include "tiberiumext.h"
#include "unitext.h"
#include "unittypeext.h"
#include "voxelanimtypeext.h"
#include "warheadtypeext.h"
#include "waveext.h"
#include "weapontypeext.h"

#include "rulesext.h"
#include "scenarioext.h"
#include "sessionext.h"
#include "tacticalext.h"

#include <iostream>
#include <typeinfo>


/**
 *  Wrapper for "typeid(T).name()", removes the "class" prefix on the string.
 */
template<typename T>
static std::string Get_TypeID_Name(void *dummy = nullptr)
{
    std::string str = typeid(T).name();
    str.erase(0, 6);
    return str;
}


/**
 *  x
 */
#define ABSTRACT_EXTENSION_POINTER_CAST_MACRO(ptr)    (*(uintptr_t *)(((unsigned char *)ptr) + 0x10))

/**
 *  x
 */
#define ABSTRACT_EXTENSION_POINTER_REMAP_MACRO(ptr)    (uintptr_t **)(((unsigned char *)ptr) + 0x10);


/**
 *  Set_Extension_Pointer
 * 
 *  @author: CCHyper
 */
static void Set_Extension_Pointer(const AbstractClass *abstract, const AbstractClassExtension *abstract_extension)
{
    ASSERT(abstract != nullptr);
    ASSERT(abstract_extension != nullptr);

    ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract) = (uintptr_t)abstract_extension;
}


/**
 *  Get_Extension_Pointer
 * 
 *  @author: CCHyper
 */
static AbstractClassExtension *Get_Extension_Pointer(const AbstractClass *abstract)
{
    uintptr_t abstract_extension_address = ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract);
    AbstractClassExtension *abstract_extension = (AbstractClassExtension *)abstract_extension_address;
    return abstract_extension;
}


/**
 *  Clear_Extension_Pointer
 * 
 *  @author: CCHyper
 */
static void Clear_Extension_Pointer(const AbstractClass *abstract)
{
    ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract) = (uintptr_t)0x00000000;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Is_Extension_Support_Enabled(RTTIType rtti)
{
    switch (rtti) {
        case RTTI_AIRCRAFT:
        case RTTI_AIRCRAFTTYPE:
        //case RTTI_ANIM:
        //case RTTI_ANIMTYPE:
        //case RTTI_BUILDING:
        case RTTI_BUILDINGTYPE:
        //case RTTI_BULLETTYPE:
        //case RTTI_CAMPAIGN:                   // <- crashes
        case RTTI_SIDE:
        case RTTI_HOUSE:
        case RTTI_HOUSETYPE:
        case RTTI_INFANTRY:
        case RTTI_INFANTRYTYPE:
        //case RTTI_ISOTILETYPE:
        //case RTTI_OVERLAYTYPE:
        //case RTTI_PARTICLESYSTEMTYPE:
        //case RTTI_PARTICLETYPE:
        //case RTTI_SMUDGETYPE:
        //case RTTI_SUPERWEAPON:
        //case RTTI_SUPERWEAPONTYPE:
        //case RTTI_TERRAIN:
        //case RTTI_TERRAINTYPE:
        //case RTTI_TIBERIUM:
        case RTTI_UNIT:
        case RTTI_UNITTYPE:
        //case RTTI_VOXELANIMTYPE:
        //case RTTI_WARHEADTYPE:
        //case RTTI_WAVE:
        //case RTTI_WEAPONTYPE:
            return true;

        default:
            return false;
    };
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Is_Extension_Support_Enabled(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return Is_Extension_Support_Enabled(RTTIType(abstract->What_Am_I()));
}


/**
 *  Check if the extension pointer is a valid memory address.
 * 
 *  #WARNING: This is not guanteed to work, but it should capture the majority of possible bad pointers.
 * 
 *  0x00870000 -> End of GAME.EXE .data segment virtual address.
 *  0x20000000 -> Arbitrary address VINIFERA.DLL 'should' never get to.
 */
static bool Is_Valid_Extension_Pointer(const AbstractClassExtension *abstract_extension)
{
    return ((uintptr_t)abstract_extension) >= 0x00870000 || ((uintptr_t)abstract_extension) < 0x20000000;
}


/**
 *  Get_Abstract_Name
 * 
 *  @author: CCHyper
 */
static const char *Get_Abstract_Name(const AbstractClass *abstract)
{
#if 0
    if (abstract) {
        if (Is_Object(abstract)) {
            return reinterpret_cast<const ObjectClass *>(abstract)->Name();
        }
        if (Is_TypeClass(abstract)) {
            return reinterpret_cast<const ObjectTypeClass *>(abstract)->Name();
        }
        switch (abstract->What_Am_I()) {
            case RTTI_HOUSE:
            {
                /**
                 *  In most cases Class is null, so we use IniName as the default.
                 */
                const char *name = reinterpret_cast<const HouseClass *>(abstract)->IniName;
        
                HouseTypeClass *htptr = reinterpret_cast<const HouseClass *>(abstract)->Class;
                if (htptr) {
                    name = htptr->Name(); // IHouse interface's Name() returns BSTR (wide), so call the Class one instead.
                }
                return name;
            }
            case RTTI_SUPERWEAPON:
                return reinterpret_cast<const SuperClass *>(abstract)->Name();
            default:
                break;
        };
    }
#endif
    return "<unknown>";
}


/**
 *  Creates and attach an instance of the extension class associated with the abstract class.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static EXT_CLASS * Extension_Make(const BASE_CLASS *abstract_ptr)
{
    //EXT_DEBUG_INFO("Extension_Make... %s %s %s\n", Get_Abstract_Name(abstract_ptr), Get_TypeID_Name<BASE_CLASS>().c_str(), Get_TypeID_Name<EXT_CLASS>().c_str());

    const BASE_CLASS *abs_ptr = reinterpret_cast<const BASE_CLASS *>(abstract_ptr);
    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Get_Extension_Pointer(abs_ptr));
    
    /**
     *  x
     */
    ext_ptr = new EXT_CLASS(reinterpret_cast<const BASE_CLASS *>(abs_ptr));
    ASSERT(ext_ptr != nullptr);
    if (ext_ptr) {
        EXT_DEBUG_INFO("\"%s\" extension created.\n", Get_TypeID_Name<BASE_CLASS>().c_str());
        return ext_ptr;
    }

    EXT_DEBUG_WARNING("Failed to make \"%s\" extension!\n", Get_TypeID_Name<BASE_CLASS>().c_str());

    return nullptr;
}


/**
 *  Destroys the attached extension instance for the abstract class.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Destroy(const BASE_CLASS *abstract_ptr)
{
    //EXT_DEBUG_INFO("Extension_Destroy... %s %s %s\n", Get_Abstract_Name(abstract_ptr), Get_TypeID_Name<BASE_CLASS>().c_str(), Get_TypeID_Name<EXT_CLASS>().c_str());
    bool removed = false;

    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Get_Extension_Pointer(abstract_ptr));
    if (ext_ptr) {
    
        /**
         *  Destroy the attached extension class instance.
         */
        delete ext_ptr;

        EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Get_TypeID_Name<BASE_CLASS>().c_str());

        /**
         *  NULL the extension pointer for the abstract instances.
         */
        Clear_Extension_Pointer(abstract_ptr);

        return true;
    }

    EXT_DEBUG_WARNING("\"%s\" extension pointer is null!\n", Get_TypeID_Name<BASE_CLASS>().c_str());

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Save(IStream *pStm, const DynamicVectorClass<EXT_CLASS *> &list)
{
    int count = list.Count();
    HRESULT hr = pStm->Write(&count, sizeof(count), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    if (list.Count() <= 0) {
        DEBUG_INFO("Extension_Save: List for \"%s\" has a count of zero, skipping save.\n", Get_TypeID_Name<EXT_CLASS>().c_str());
        return true;
    }

    DEBUG_INFO("Saving \"%s\" extensions (Count: %d)\n", Get_TypeID_Name<EXT_CLASS>().c_str(), list.Count());

    for (int index = 0; index < count; ++index) {

        EXT_CLASS *ptr = list[index];

        /**
         *  Tell the extension class to persist itself into the data stream.
         */
        IPersistStream *lpPS = nullptr;
        hr = ptr->QueryInterface(__uuidof(IPersistStream), (LPVOID *)&lpPS);
        if (FAILED(hr)) {
            DEBUG_ERROR("Extension \"%s\" does not support IPersistStream!\n", Get_TypeID_Name<EXT_CLASS>().c_str());
            return false;
        }

        hr = OleSaveToStream(lpPS, pStm);
        if (FAILED(hr)) {
            DEBUG_ERROR("OleSaveToStream failed for extension \"%s\" (Index: %d)!\n", Get_TypeID_Name<EXT_CLASS>().c_str(), index);
            return false;
        }

        hr = lpPS->Release();
        if (FAILED(hr)) {
            DEBUG_ERROR("Failed to release extension \"%s\" stream!\n", Get_TypeID_Name<EXT_CLASS>().c_str());
            return false;
        }

        EXT_DEBUG_INFO("  -> %s\n", reinterpret_cast<EXT_CLASS *>(lpPS)->Name());
    }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Load(IStream *pStm, DynamicVectorClass<EXT_CLASS *> &list)
{
    int count = 0;
    HRESULT hr = pStm->Read(&count, sizeof(count), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    if (count <= 0) {
        DEBUG_INFO("Extension_Save: Block for \"%s\" has a count of zero, skipping load.\n", Get_TypeID_Name<EXT_CLASS>().c_str());
        return true;
    }
    DEBUG_INFO("Loading \"%s\" extensions (Count: %d)\n", Get_TypeID_Name<BASE_CLASS>().c_str(), count);
    
    for (int index = 0; index < count; ++index) {
        
        IUnknown *spUnk = nullptr;
        hr = OleLoadFromStream(pStm, __uuidof(IUnknown), (LPVOID *)&spUnk);
        if (FAILED(hr)) {
            DEBUG_ERROR("OleLoadFromStream failed for extension \"%s\" (Index: %d)!\n", Get_TypeID_Name<EXT_CLASS>().c_str(), index);
            return false;
        }

    }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Request_Pointer_Remap(const DynamicVectorClass<BASE_CLASS *> &list)
{
    DEBUG_INFO("Requesting remap of \"%s\" extension pointers (Count %d)... ", Get_TypeID_Name<BASE_CLASS>().c_str(), list.Count());

    if (!list.Count()) {
        DEBUG_INFO("(List empty)\n");
        return true;
    }

    for (int index = 0; index < list.Count(); ++index) {
        if (index == 0) {
            DEBUG_INFO("\n");
        }

        const BASE_CLASS *object = list[index];
        if (object) {

            if (!Get_Extension_Pointer(object)) {
                DEV_DEBUG_ERROR("\"%s\" extension pointer for is null!\n", Get_TypeID_Name<BASE_CLASS>().c_str());
                return false;
            }

            /**
             *  
             */
            uintptr_t **ext_ptr_addr = ABSTRACT_EXTENSION_POINTER_REMAP_MACRO(object);
            VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(*ext_ptr_addr, "AbstractClass::ExtPtr");

            DEV_DEBUG_INFO("  Requested remap of index %d extension pointer.\n", index);
        }
    }

    DEBUG_INFO("DONE!\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AbstractClassExtension *Make_Extension_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    AbstractClassExtension *extptr = nullptr;

    switch (const_cast<AbstractClass *>(abstract)->What_Am_I()) {

        case RTTI_AIRCRAFT:
            extptr = Extension_Make<AircraftClass, AircraftClassExtension>(reinterpret_cast<const AircraftClass *>(abstract));
            break;

        case RTTI_AIRCRAFTTYPE:
            extptr = Extension_Make<AircraftTypeClass, AircraftTypeClassExtension>(reinterpret_cast<const AircraftTypeClass *>(abstract));
            break;

        case RTTI_ANIM:
            extptr = Extension_Make<AnimClass, AnimClassExtension>(reinterpret_cast<const AnimClass *>(abstract));
            break;

        case RTTI_ANIMTYPE:
            extptr = Extension_Make<AnimTypeClass, AnimTypeClassExtension>(reinterpret_cast<const AnimTypeClass *>(abstract));
            break;

        case RTTI_BUILDING:
            extptr = Extension_Make<BuildingClass, BuildingClassExtension>(reinterpret_cast<const BuildingClass *>(abstract));
            break;

        case RTTI_BUILDINGTYPE:
            extptr = Extension_Make<BuildingTypeClass, BuildingTypeClassExtension>(reinterpret_cast<const BuildingTypeClass *>(abstract));
            break;

        case RTTI_BULLETTYPE:
            extptr = Extension_Make<BulletTypeClass, BulletTypeClassExtension>(reinterpret_cast<const BulletTypeClass *>(abstract));
            break;

        case RTTI_CAMPAIGN:
            extptr = Extension_Make<CampaignClass, CampaignClassExtension>(reinterpret_cast<const CampaignClass *>(abstract));
            break;

        case RTTI_SIDE:
            extptr = Extension_Make<SideClass, SideClassExtension>(reinterpret_cast<const SideClass *>(abstract));
            break;

        case RTTI_HOUSE:
            extptr = Extension_Make<HouseClass, HouseClassExtension>(reinterpret_cast<const HouseClass *>(abstract));
            break;

        case RTTI_HOUSETYPE:
            extptr = Extension_Make<HouseTypeClass, HouseTypeClassExtension>(reinterpret_cast<const HouseTypeClass *>(abstract));
            break;

        case RTTI_INFANTRY:
            extptr = Extension_Make<InfantryClass, InfantryClassExtension>(reinterpret_cast<const InfantryClass *>(abstract));
            break;

        case RTTI_INFANTRYTYPE:
            extptr = Extension_Make<InfantryTypeClass, InfantryTypeClassExtension>(reinterpret_cast<const InfantryTypeClass *>(abstract));
            break;

        case RTTI_ISOTILETYPE:
            extptr = Extension_Make<IsometricTileTypeClass, IsometricTileTypeClassExtension>(reinterpret_cast<const IsometricTileTypeClass *>(abstract));
            break;

        case RTTI_OVERLAYTYPE:
            extptr = Extension_Make<OverlayTypeClass, OverlayTypeClassExtension>(reinterpret_cast<const OverlayTypeClass *>(abstract));
            break;

        case RTTI_PARTICLESYSTEMTYPE:
            extptr = Extension_Make<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(reinterpret_cast<const ParticleSystemTypeClass *>(abstract));
            break;

        case RTTI_PARTICLETYPE:
            extptr = Extension_Make<ParticleTypeClass, ParticleTypeClassExtension>(reinterpret_cast<const ParticleTypeClass *>(abstract));
            break;

        case RTTI_SMUDGETYPE:
            extptr = Extension_Make<SmudgeTypeClass, SmudgeTypeClassExtension>(reinterpret_cast<const SmudgeTypeClass *>(abstract));
            break;

        case RTTI_SUPERWEAPON:
            extptr = Extension_Make<SuperClass, SuperClassExtension>(reinterpret_cast<const SuperClass *>(abstract));
            break;

        case RTTI_SUPERWEAPONTYPE:
            extptr = Extension_Make<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(reinterpret_cast<const SuperWeaponTypeClass *>(abstract));
            break;

        case RTTI_TERRAIN:
            extptr = Extension_Make<TerrainClass, TerrainClassExtension>(reinterpret_cast<const TerrainClass *>(abstract));
            break;

        case RTTI_TERRAINTYPE:
            extptr = Extension_Make<TerrainTypeClass, TerrainTypeClassExtension>(reinterpret_cast<const TerrainTypeClass *>(abstract));
            break;

        case RTTI_TIBERIUM:
            extptr = Extension_Make<TiberiumClass, TiberiumClassExtension>(reinterpret_cast<const TiberiumClass *>(abstract));
            break;

        case RTTI_UNIT:
            extptr = Extension_Make<UnitClass, UnitClassExtension>(reinterpret_cast<const UnitClass *>(abstract));
            break;

        case RTTI_UNITTYPE:
            extptr = Extension_Make<UnitTypeClass, UnitTypeClassExtension>(reinterpret_cast<const UnitTypeClass *>(abstract));
            break;

        case RTTI_VOXELANIMTYPE:
            extptr = Extension_Make<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(reinterpret_cast<const VoxelAnimTypeClass *>(abstract));
            break;

        case RTTI_WARHEADTYPE:
            extptr = Extension_Make<WarheadTypeClass, WarheadTypeClassExtension>(reinterpret_cast<const WarheadTypeClass *>(abstract));
            break;

        case RTTI_WAVE:
            extptr = Extension_Make<WaveClass, WaveClassExtension>(reinterpret_cast<const WaveClass *>(abstract));
            break;

        case RTTI_WEAPONTYPE:
            extptr = Extension_Make<WeaponTypeClass, WeaponTypeClassExtension>(reinterpret_cast<const WeaponTypeClass *>(abstract));
            break;

        default:
            DEBUG_ERROR("Make_Extension: No extension support for \"%s\" implemented!\n", Name_From_RTTI((RTTIType)abstract->What_Am_I()));
            break;

    };

    /**
     *  
     */
    if (extptr != nullptr) {
        Set_Extension_Pointer(abstract, extptr);
    }

    return extptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AbstractClassExtension *Fetch_Extension_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    AbstractClassExtension *ext_ptr = Get_Extension_Pointer(abstract);

    if (!ext_ptr) {
        DEBUG_ERROR("Fetch_Extension: Extension for \"%s\" is null!\n", Get_Abstract_Name(abstract));
        return nullptr;
    }

    /**
     *  Check for a malformed extension pointer.
     */
    if (!Is_Valid_Extension_Pointer(ext_ptr)) {
        DEBUG_ERROR("Fetch_Extension: Corrupt extension pointer for \"%s\"!\n", Get_Abstract_Name(abstract));
        return nullptr;
    }

    /**
     *  Its still possible the pointer could be invalid, so perform a final check.
     */
    if (ext_ptr->What_Am_I() <= EXT_RTTI_NONE || ext_ptr->What_Am_I() >= EXT_RTTI_COUNT) {
        DEBUG_ERROR("Fetch_Extension: Invalid extension rtti type for \"%s\"!\n", Get_Abstract_Name(abstract));
        return nullptr;
    }

    //EXT_DEBUG_INFO("Fetch_Extension: Abstract \"%s\", got extension \"%s\".\n", Get_Abstract_Name(abstract), ext_ptr->Name());

    return ext_ptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Destroy_Extension_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);
    
    bool removed = false;

    switch (abstract->What_Am_I()) {

        case RTTI_AIRCRAFT:
            removed = Extension_Destroy<AircraftClass, AircraftClassExtension>(reinterpret_cast<const AircraftClass *>(abstract));
            break;

        case RTTI_AIRCRAFTTYPE:
            removed = Extension_Destroy<AircraftTypeClass, AircraftTypeClassExtension>(reinterpret_cast<const AircraftTypeClass *>(abstract));
            break;

        case RTTI_ANIM:
            removed = Extension_Destroy<AnimClass, AnimClassExtension>(reinterpret_cast<const AnimClass *>(abstract));
            break;

        case RTTI_ANIMTYPE:
            removed = Extension_Destroy<AnimTypeClass, AnimTypeClassExtension>(reinterpret_cast<const AnimTypeClass *>(abstract));
            break;

        case RTTI_BUILDING:
            removed = Extension_Destroy<BuildingClass, BuildingClassExtension>(reinterpret_cast<const BuildingClass *>(abstract));
            break;

        case RTTI_BUILDINGTYPE:
            removed = Extension_Destroy<BuildingTypeClass, BuildingTypeClassExtension>(reinterpret_cast<const BuildingTypeClass *>(abstract));
            break;

        case RTTI_BULLETTYPE:
            removed = Extension_Destroy<BulletTypeClass, BulletTypeClassExtension>(reinterpret_cast<const BulletTypeClass *>(abstract));
            break;

        case RTTI_CAMPAIGN:
            removed = Extension_Destroy<CampaignClass, CampaignClassExtension>(reinterpret_cast<const CampaignClass *>(abstract));
            break;

        case RTTI_SIDE:
            removed = Extension_Destroy<SideClass, SideClassExtension>(reinterpret_cast<const SideClass *>(abstract));
            break;

        case RTTI_HOUSE:
            removed = Extension_Destroy<HouseClass, HouseClassExtension>(reinterpret_cast<const HouseClass *>(abstract));
            break;

        case RTTI_HOUSETYPE:
            removed = Extension_Destroy<HouseTypeClass, HouseTypeClassExtension>(reinterpret_cast<const HouseTypeClass *>(abstract));
            break;

        case RTTI_INFANTRY:
            removed = Extension_Destroy<InfantryClass, InfantryClassExtension>(reinterpret_cast<const InfantryClass *>(abstract));
            break;

        case RTTI_INFANTRYTYPE:
            removed = Extension_Destroy<InfantryTypeClass, InfantryTypeClassExtension>(reinterpret_cast<const InfantryTypeClass *>(abstract));
            break;

        case RTTI_ISOTILETYPE:
            removed = Extension_Destroy<IsometricTileTypeClass, IsometricTileTypeClassExtension>(reinterpret_cast<const IsometricTileTypeClass *>(abstract));
            break;

        case RTTI_OVERLAYTYPE:
            removed = Extension_Destroy<OverlayTypeClass, OverlayTypeClassExtension>(reinterpret_cast<const OverlayTypeClass *>(abstract));
            break;

        case RTTI_PARTICLESYSTEMTYPE:
            removed = Extension_Destroy<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(reinterpret_cast<const ParticleSystemTypeClass *>(abstract));
            break;

        case RTTI_PARTICLETYPE:
            removed = Extension_Destroy<ParticleTypeClass, ParticleTypeClassExtension>(reinterpret_cast<const ParticleTypeClass *>(abstract));
            break;

        case RTTI_SMUDGETYPE:
            removed = Extension_Destroy<SmudgeTypeClass, SmudgeTypeClassExtension>(reinterpret_cast<const SmudgeTypeClass *>(abstract));
            break;

        case RTTI_SUPERWEAPON:
            removed = Extension_Destroy<SuperClass, SuperClassExtension>(reinterpret_cast<const SuperClass *>(abstract));
            break;

        case RTTI_SUPERWEAPONTYPE:
            removed = Extension_Destroy<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(reinterpret_cast<const SuperWeaponTypeClass *>(abstract));
            break;

        case RTTI_TERRAIN:
            removed = Extension_Destroy<TerrainClass, TerrainClassExtension>(reinterpret_cast<const TerrainClass *>(abstract));
            break;

        case RTTI_TERRAINTYPE:
            removed = Extension_Destroy<TerrainTypeClass, TerrainTypeClassExtension>(reinterpret_cast<const TerrainTypeClass *>(abstract));
            break;

        case RTTI_TIBERIUM:
            removed = Extension_Destroy<TiberiumClass, TiberiumClassExtension>(reinterpret_cast<const TiberiumClass *>(abstract));
            break;

        case RTTI_UNIT:
            removed = Extension_Destroy<UnitClass, UnitClassExtension>(reinterpret_cast<const UnitClass *>(abstract));
            break;

        case RTTI_UNITTYPE:
            removed = Extension_Destroy<UnitTypeClass, UnitTypeClassExtension>(reinterpret_cast<const UnitTypeClass *>(abstract));
            break;

        case RTTI_VOXELANIMTYPE:
            removed = Extension_Destroy<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(reinterpret_cast<const VoxelAnimTypeClass *>(abstract));
            break;

        case RTTI_WARHEADTYPE:
            removed = Extension_Destroy<WarheadTypeClass, WarheadTypeClassExtension>(reinterpret_cast<const WarheadTypeClass *>(abstract));
            break;

        case RTTI_WAVE:
            removed = Extension_Destroy<WaveClass, WaveClassExtension>(reinterpret_cast<const WaveClass *>(abstract));
            break;

        case RTTI_WEAPONTYPE:
            removed = Extension_Destroy<WeaponTypeClass, WeaponTypeClassExtension>(reinterpret_cast<const WeaponTypeClass *>(abstract));
            break;

        default:
            DEBUG_ERROR("Destroy_Extension: No extension support for \"%s\" implemented!\n", Name_From_RTTI((RTTIType)abstract->What_Am_I()));
            break;

    };

    /**
     *  Clear the pointer in the abstract class.
     */
    Clear_Extension_Pointer(abstract);

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Save_Extensions(IStream *pStm)
{
    ASSERT(pStm != nullptr);

    if (!pStm) {
        return false;
    }

    /**
     *  Save all the extension class data to the stream.
     *
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    DEBUG_INFO("Save_Extensions(enter)\n");
    
    if (Is_Extension_Support_Enabled(RTTI_UNIT) && !Extension_Save<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT) && !Extension_Save<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE) && !Extension_Save<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIM) && !Extension_Save<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE) && !Extension_Save<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING) && !Extension_Save<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE) && !Extension_Save<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE) && !Extension_Save<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN) && !Extension_Save<CampaignClass, CampaignClassExtension>(pStm, CampaignExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE) && !Extension_Save<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE) && !Extension_Save<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY) && !Extension_Save<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE) && !Extension_Save<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE) && !Extension_Save<IsometricTileTypeClass, IsometricTileTypeClassExtension>(pStm, IsometricTileTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAY) && !Extension_Save<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE) && !Extension_Save<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE) && !Extension_Save<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SIDE) && !Extension_Save<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE) && !Extension_Save<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE) && !Extension_Save<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN) && !Extension_Save<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE) && !Extension_Save<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE) && !Extension_Save<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE) && !Extension_Save<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WAVE) && !Extension_Save<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE) && !Extension_Save<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE) && !Extension_Save<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON) && !Extension_Save<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM) && !Extension_Save<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }

    DEBUG_INFO("Save_Extensions(exit)\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Load_Extensions(IStream *pStm)
{
    ASSERT(pStm != nullptr);

    if (!pStm) {
        return false;
    }

    /**
     *  Load all the extension class data from the stream.
     *
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    DEBUG_INFO("Load_Extensions(enter)\n");

    if (Is_Extension_Support_Enabled(RTTI_UNIT) && !Extension_Load<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT) && !Extension_Load<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE) && !Extension_Load<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIM) && !Extension_Load<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE) && !Extension_Load<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING) && !Extension_Load<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE) && !Extension_Load<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE) && !Extension_Load<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN) && !Extension_Load<CampaignClass, CampaignClassExtension>(pStm, CampaignExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE) && !Extension_Load<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE) && !Extension_Load<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY) && !Extension_Load<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE) && !Extension_Load<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE) && !Extension_Load<IsometricTileTypeClass, IsometricTileTypeClassExtension>(pStm, IsometricTileTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE) && !Extension_Load<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE) && !Extension_Load<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE) && !Extension_Load<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SIDE) && !Extension_Load<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE) && !Extension_Load<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE) && !Extension_Load<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN) && !Extension_Load<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE) && !Extension_Load<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE) && !Extension_Load<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE) && !Extension_Load<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WAVE) && !Extension_Load<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE) && !Extension_Load<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE) && !Extension_Load<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON) && !Extension_Load<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM) && !Extension_Load<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }

    DEBUG_INFO("Load_Extensions(exit)\n");

    /**
     *  x
     */
    Request_Extension_Pointer_Remap();

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Request_Extension_Pointer_Remap()
{
    /**
     *  Request pointer remap on all extension pointers.
     *
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    DEBUG_INFO("Requesting remap for extension pointers...\n");

    if (Is_Extension_Support_Enabled(RTTI_UNIT) && !Extension_Request_Pointer_Remap<UnitClass, UnitClassExtension>(Units)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT) && !Extension_Request_Pointer_Remap<AircraftClass, AircraftClassExtension>(Aircrafts)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE) && !Extension_Request_Pointer_Remap<AircraftTypeClass, AircraftTypeClassExtension>(AircraftTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIM) && !Extension_Request_Pointer_Remap<AnimClass, AnimClassExtension>(Anims)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE) && !Extension_Request_Pointer_Remap<AnimTypeClass, AnimTypeClassExtension>(AnimTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING) && !Extension_Request_Pointer_Remap<BuildingClass, BuildingClassExtension>(Buildings)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE) && !Extension_Request_Pointer_Remap<BuildingTypeClass, BuildingTypeClassExtension>(BuildingTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE) && !Extension_Request_Pointer_Remap<BulletTypeClass, BulletTypeClassExtension>(BulletTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN) && !Extension_Request_Pointer_Remap<CampaignClass, CampaignClassExtension>(Campaigns)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE) && !Extension_Request_Pointer_Remap<HouseClass, HouseClassExtension>(Houses)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE) && !Extension_Request_Pointer_Remap<HouseTypeClass, HouseTypeClassExtension>(HouseTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY) && !Extension_Request_Pointer_Remap<InfantryClass, InfantryClassExtension>(Infantry)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE) && !Extension_Request_Pointer_Remap<InfantryTypeClass, InfantryTypeClassExtension>(InfantryTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE) && !Extension_Request_Pointer_Remap<IsometricTileTypeClass, IsometricTileTypeClassExtension>(IsoTileTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE) && !Extension_Request_Pointer_Remap<OverlayTypeClass, OverlayTypeClassExtension>(OverlayTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE) && !Extension_Request_Pointer_Remap<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(ParticleSystemTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE) && !Extension_Request_Pointer_Remap<ParticleTypeClass, ParticleTypeClassExtension>(ParticleTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SIDE) && !Extension_Request_Pointer_Remap<SideClass, SideClassExtension>(Sides)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE) && !Extension_Request_Pointer_Remap<SmudgeTypeClass, SmudgeTypeClassExtension>(SmudgeTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE) && !Extension_Request_Pointer_Remap<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(SuperWeaponTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN) && !Extension_Request_Pointer_Remap<TerrainClass, TerrainClassExtension>(Terrains)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE) && !Extension_Request_Pointer_Remap<TerrainTypeClass, TerrainTypeClassExtension>(TerrainTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE) && !Extension_Request_Pointer_Remap<UnitTypeClass, UnitTypeClassExtension>(UnitTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE) && !Extension_Request_Pointer_Remap<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(VoxelAnimTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WAVE) && !Extension_Request_Pointer_Remap<WaveClass, WaveClassExtension>(Waves)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE) && !Extension_Request_Pointer_Remap<WeaponTypeClass, WeaponTypeClassExtension>(WeaponTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE) && !Extension_Request_Pointer_Remap<WarheadTypeClass, WarheadTypeClassExtension>(WarheadTypes)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON) && !Extension_Request_Pointer_Remap<SuperClass, SuperClassExtension>(Supers)) { return false; }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM) && !Extension_Request_Pointer_Remap<TiberiumClass, TiberiumClassExtension>(Tiberiums)) { return false; }

    DEBUG_INFO("Requests of extension pointers complete.\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Register_Extension_Class_Factories()
{
    /**
     *  x
     *
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Is_Extension_Support_Enabled(RTTI_UNIT)) { REGISTER_CLASS(UnitClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT)) { REGISTER_CLASS(AircraftClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE)) { REGISTER_CLASS(AircraftTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_ANIM)) { REGISTER_CLASS(AnimClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE)) { REGISTER_CLASS(AnimTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING)) { REGISTER_CLASS(BuildingClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE)) { REGISTER_CLASS(BuildingTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE)) { REGISTER_CLASS(BulletTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN)) { REGISTER_CLASS(CampaignClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE)) { REGISTER_CLASS(HouseClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE)) { REGISTER_CLASS(HouseTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY)) { REGISTER_CLASS(InfantryClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE)) { REGISTER_CLASS(InfantryTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE)) { REGISTER_CLASS(IsometricTileTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE)) { REGISTER_CLASS(OverlayTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE)) { REGISTER_CLASS(ParticleSystemTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE)) { REGISTER_CLASS(ParticleTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_SIDE)) { REGISTER_CLASS(SideClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE)) { REGISTER_CLASS(SmudgeTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE)) { REGISTER_CLASS(SuperWeaponTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN)) { REGISTER_CLASS(TerrainClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE)) { REGISTER_CLASS(TerrainTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE)) { REGISTER_CLASS(UnitTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE)) { REGISTER_CLASS(VoxelAnimTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_WAVE)) { REGISTER_CLASS(WaveClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE)) { REGISTER_CLASS(WeaponTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE)) { REGISTER_CLASS(WarheadTypeClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON)) { REGISTER_CLASS(SuperClassExtension) }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM)) { REGISTER_CLASS(TiberiumClassExtension) }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void Clear_Extension_Lists()
{
    DEBUG_INFO("Clear_Extension_Lists(enter)\n");

    ++ScenarioInit;

    if (Is_Extension_Support_Enabled(RTTI_UNIT)) { UnitExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT)) { AircraftExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE)) { AircraftTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_ANIM)) { AnimExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE)) { AnimTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING)) { BuildingExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE)) { BuildingTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE)) { BulletTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN)) { CampaignExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE)) { HouseExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE)) { HouseTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY)) { InfantryExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE)) { InfantryTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE)) { IsometricTileTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE)) { OverlayTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE)) { ParticleSystemTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE)) { ParticleTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_SIDE)) { SideExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE)) { SmudgeTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE)) { SuperWeaponTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN)) { TerrainExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE)) { TerrainTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE)) { UnitTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE)) { VoxelAnimTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_WAVE)) { WaveExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE)) { WeaponTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE)) { WarheadTypeExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON)) { SuperExtensions.Clear(); }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM)) { TiberiumExtensions.Clear(); }

    --ScenarioInit;

    DEBUG_INFO("Clear_Extension_Lists(exit)\n");
}


/**
 *  Calculate the save game version.
 * 
 *  @author: CCHyper
 */
unsigned Get_Extension_Save_Version_Number()
{
    unsigned version = 10000;

    /**
     *  Game classes.
     */
    if (Is_Extension_Support_Enabled(RTTI_UNIT)) { version += sizeof(UnitClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT)) { version += sizeof(AircraftClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE)) { version += sizeof(AircraftTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_ANIM)) { version += sizeof(AnimClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE)) { version += sizeof(AnimTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING)) { version += sizeof(BuildingClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE)) { version += sizeof(BuildingTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_BULLET)) { EXT_RTTI_BULLET; }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE)) { version += sizeof(BulletTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN)) { version += sizeof(CampaignClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_CELL)) { EXT_RTTI_CELL; }
    if (Is_Extension_Support_Enabled(RTTI_FACTORY)) { EXT_RTTI_FACTORY; }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE)) { version += sizeof(HouseClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE)) { version += sizeof(HouseTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY)) { version += sizeof(InfantryClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE)) { version += sizeof(InfantryTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILE)) { EXT_RTTI_ISOTILE; }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE)) { version += sizeof(IsometricTileTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_LIGHT)) { EXT_RTTI_LIGHT; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAY)) { EXT_RTTI_OVERLAY; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE)) { version += sizeof(OverlayTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLE)) { EXT_RTTI_PARTICLE; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE)) { version += sizeof(ParticleTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEM)) { EXT_RTTI_PARTICLESYSTEM; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE)) { version += sizeof(ParticleSystemTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_SCRIPT)) { EXT_RTTI_SCRIPT; }
    if (Is_Extension_Support_Enabled(RTTI_SCRIPTTYPE)) { EXT_RTTI_SCRIPTTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_SIDE)) { version += sizeof(SideClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGE)) { EXT_RTTI_SMUDGE; }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE)) { version += sizeof(SmudgeTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE)) { version += sizeof(SuperWeaponTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_TASKFORCE)) { EXT_RTTI_TASKFORCE; }
    if (Is_Extension_Support_Enabled(RTTI_TEAM)) { EXT_RTTI_TEAM; }
    if (Is_Extension_Support_Enabled(RTTI_TEAMTYPE)) { EXT_RTTI_TEAMTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN)) { version += sizeof(TerrainClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE)) { version += sizeof(TerrainTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_TRIGGER)) { EXT_RTTI_TRIGGER; }
    if (Is_Extension_Support_Enabled(RTTI_TRIGGERTYPE)) { EXT_RTTI_TRIGGERTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE)) { version += sizeof(UnitTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIM)) { EXT_RTTI_VOXELANIM; }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE)) { version += sizeof(VoxelAnimTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_WAVE)) { version += sizeof(WaveClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_TAG)) { EXT_RTTI_TAG; }
    if (Is_Extension_Support_Enabled(RTTI_TAGTYPE)) { EXT_RTTI_TAGTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM)) { version += sizeof(TiberiumClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_ACTION)) { EXT_RTTI_ACTION; }
    if (Is_Extension_Support_Enabled(RTTI_EVENT)) { EXT_RTTI_EVENT; }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE)) { version += sizeof(WeaponTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE)) { version += sizeof(WarheadTypeClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_WAYPOINT)) { EXT_RTTI_WAYPOINT; }
    if (Is_Extension_Support_Enabled(RTTI_TUBE)) { EXT_RTTI_TUBE; }
    if (Is_Extension_Support_Enabled(RTTI_LIGHTSOURCE)) { EXT_RTTI_LIGHTSOURCE; }
    if (Is_Extension_Support_Enabled(RTTI_EMPULSE)) { EXT_RTTI_EMPULSE; }
    if (Is_Extension_Support_Enabled(RTTI_TACTICALMAP)) { EXT_RTTI_TACTICALMAP; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON)) { version += sizeof(SuperClassExtension); }
    if (Is_Extension_Support_Enabled(RTTI_AITRIGGER)) { EXT_RTTI_AITRIGGER; }
    if (Is_Extension_Support_Enabled(RTTI_AITRIGGERTYPE)) { EXT_RTTI_AITRIGGERTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_NEURON)) { EXT_RTTI_NEURON; }
    if (Is_Extension_Support_Enabled(RTTI_FOGGEDOBJECT)) { EXT_RTTI_FOGGEDOBJECT; }
    if (Is_Extension_Support_Enabled(RTTI_ALPHASHAPE)) { EXT_RTTI_ALPHASHAPE; }
    if (Is_Extension_Support_Enabled(RTTI_VEINHOLEMONSTER)) { EXT_RTTI_VEINHOLEMONSTER; }

    /**
    *  Global classes.
    */
    version += sizeof(RulesClassExtension);
    version += sizeof(ScenarioClassExtension);
    version += sizeof(SessionClassExtension);

    // TOOD EXT_RTTI_TACTICALMAP
    version += sizeof(TacticalExtension);

    return version;
}
