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

#include "wstring.h"
#include "vector.h"
#include "tclassfactory.h"
#include "swizzle.h"
#include "vinifera_saveload.h"

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
static DynamicVectorClass<AircraftClassExtension *> AircraftExtensions;
static DynamicVectorClass<AircraftTypeClassExtension *> AircraftTypeExtensions;
static DynamicVectorClass<AnimClassExtension *> AnimExtensions;
static DynamicVectorClass<AnimTypeClassExtension *> AnimTypeExtensions;
static DynamicVectorClass<BuildingClassExtension *> BuildingExtensions;
static DynamicVectorClass<BuildingTypeClassExtension *> BuildingTypeExtensions;
static DynamicVectorClass<BulletTypeClassExtension *> BulletTypeExtensions;
static DynamicVectorClass<CampaignClassExtension *> CampaignExtensions;
static DynamicVectorClass<SideClassExtension *> SideExtensions;
static DynamicVectorClass<HouseClassExtension *> HouseExtensions;
static DynamicVectorClass<HouseTypeClassExtension *> HouseTypeExtensions;
static DynamicVectorClass<InfantryClassExtension *> InfantryExtensions;
static DynamicVectorClass<InfantryTypeClassExtension *> InfantryTypeExtensions;
static DynamicVectorClass<IsometricTileTypeClassExtension *> IsometricTileTypeExtensions;
static DynamicVectorClass<OverlayTypeClassExtension *> OverlayTypeExtensions;
static DynamicVectorClass<ParticleSystemTypeClassExtension *> ParticleSystemTypeExtensions;
static DynamicVectorClass<ParticleTypeClassExtension *> ParticleTypeExtensions;
static DynamicVectorClass<SmudgeTypeClassExtension *> SmudgeTypeExtensions;
static DynamicVectorClass<SuperClassExtension *> SuperExtensions;
static DynamicVectorClass<SuperWeaponTypeClassExtension *> SuperWeaponTypeExtensions;
static DynamicVectorClass<TerrainClassExtension *> TerrainExtensions;
static DynamicVectorClass<TerrainTypeClassExtension *> TerrainTypeExtensions;
static DynamicVectorClass<TiberiumClassExtension *> TiberiumExtensions;
static DynamicVectorClass<UnitClassExtension *> UnitExtensions;
static DynamicVectorClass<UnitTypeClassExtension *> UnitTypeExtensions;
static DynamicVectorClass<VoxelAnimTypeClassExtension *> VoxelAnimTypeExtensions;
static DynamicVectorClass<WarheadTypeClassExtension *> WarheadTypeExtensions;
static DynamicVectorClass<WaveClassExtension *> WaveExtensions;
static DynamicVectorClass<WeaponTypeClassExtension *> WeaponTypeExtensions;


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
bool Is_Extension_Support_Enabled(ExtensionRTTIType rtti)
{
#ifndef NDEBUG
    switch (rtti) {
        //case EXT_RTTI_AIRCRAFT:
        //case EXT_RTTI_AIRCRAFTTYPE:
        //case EXT_RTTI_ANIM:
        //case EXT_RTTI_ANIMTYPE:
        case EXT_RTTI_BUILDING:
        case EXT_RTTI_BUILDINGTYPE:
        //case EXT_RTTI_BULLETTYPE:
        //case EXT_RTTI_CAMPAIGN:
        //case EXT_RTTI_SIDE:
        //case EXT_RTTI_HOUSE:
        //case EXT_RTTI_HOUSETYPE:
        //case EXT_RTTI_INFANTRY:
        //case EXT_RTTI_INFANTRYTYPE:
        //case EXT_RTTI_ISOTILETYPE:
        //case EXT_RTTI_OVERLAYTYPE:
        //case EXT_RTTI_PARTICLESYSTEMTYPE:
        //case EXT_RTTI_PARTICLETYPE:
        //case EXT_RTTI_SMUDGETYPE:
        //case EXT_RTTI_SUPERWEAPON:
        //case EXT_RTTI_SUPERWEAPONTYPE:
        //case EXT_RTTI_TERRAIN:
        //case EXT_RTTI_TERRAINTYPE:
        //case EXT_RTTI_TIBERIUM:
        //case EXT_RTTI_UNIT:
        //case EXT_RTTI_UNITTYPE:
        //case EXT_RTTI_VOXELANIMTYPE:
        //case EXT_RTTI_WARHEADTYPE:
        //case EXT_RTTI_WAVE:
        //case EXT_RTTI_WEAPONTYPE:
            return true;

        default:
            return false;
    };
#else
    return true;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static bool Supports_Extension_For(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    switch (abstract->What_Am_I()) {
        case RTTI_AIRCRAFT:
        case RTTI_AIRCRAFTTYPE:
        case RTTI_ANIM:
        case RTTI_ANIMTYPE:
        case RTTI_BUILDING:
        case RTTI_BUILDINGTYPE:
        case RTTI_BULLETTYPE:
        case RTTI_CAMPAIGN:
        case RTTI_SIDE:
        case RTTI_HOUSE:
        case RTTI_HOUSETYPE:
        case RTTI_INFANTRY:
        case RTTI_INFANTRYTYPE:
        case RTTI_ISOTILETYPE:
        case RTTI_OVERLAYTYPE:
        case RTTI_PARTICLESYSTEMTYPE:
        case RTTI_PARTICLETYPE:
        case RTTI_SMUDGETYPE:
        case RTTI_SUPERWEAPON:
        case RTTI_SUPERWEAPONTYPE:
        case RTTI_TERRAIN:
        case RTTI_TERRAINTYPE:
        case RTTI_TIBERIUM:
        case RTTI_UNIT:
        case RTTI_UNITTYPE:
        case RTTI_VOXELANIMTYPE:
        case RTTI_WARHEADTYPE:
        case RTTI_WAVE:
        case RTTI_WEAPONTYPE:
            return true;

        default:
            return false;
    };
}


/**
 *  Is_Valid_Extension_Pointer
 * 
 *  This is not guanteed to work, but it should capture the majority of possible bad pointers.
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
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static EXT_CLASS * Extension_Find_Or_Make(const BASE_CLASS *abstract_ptr, DynamicVectorClass<EXT_CLASS *> &list, bool &created, bool allow_make)
{
    //EXT_DEBUG_INFO("Extension_Find_Or_Make... %s %s %s\n", Get_Abstract_Name(abstract_ptr), Get_TypeID_Name<BASE_CLASS>().c_str(), Get_TypeID_Name<EXT_CLASS>().c_str());

    created = false;
    const BASE_CLASS *abs_ptr = reinterpret_cast<const BASE_CLASS *>(abstract_ptr);
    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Get_Extension_Pointer(abs_ptr));

    int found_index = list.ID(ext_ptr);
    if (found_index != -1) {
        ext_ptr = list[found_index];
        EXT_DEBUG_INFO("\"%s\" extension found for \"%s\".\n", Get_TypeID_Name<BASE_CLASS>().c_str(), Get_Abstract_Name(abs_ptr));
        return ext_ptr;
    }

    if (allow_make) {
        ext_ptr = new EXT_CLASS(reinterpret_cast<const BASE_CLASS *>(abs_ptr));
        bool added = list.Add(ext_ptr);
        ASSERT(added);
        if (added) {
            EXT_DEBUG_INFO("\"%s\" extension created for \"%s\".\n", Get_TypeID_Name<BASE_CLASS>().c_str(), Get_Abstract_Name(abs_ptr));
            created = true;
            return ext_ptr;
        }
        return ext_ptr;
    }

    EXT_DEBUG_WARNING("Failed to find or make \"%s\" extension for \"%s\"!\n", Get_TypeID_Name<BASE_CLASS>().c_str(), Get_Abstract_Name(abs_ptr));

    return nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Destroy(const BASE_CLASS *abstract_ptr, DynamicVectorClass<EXT_CLASS *> &list)
{
    //EXT_DEBUG_INFO("Extension_Destroy... %s %s %s\n", Get_Abstract_Name(abstract_ptr), Get_TypeID_Name<BASE_CLASS>().c_str(), Get_TypeID_Name<EXT_CLASS>().c_str());
    bool removed = false;

    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Get_Extension_Pointer(abstract_ptr));
    if (ext_ptr) {

        removed = list.Delete(ext_ptr);

#ifndef NDEBUG
        ASSERT_FATAL_PRINT(removed, "Failed to destroy \"%s\" extension!\n", Get_TypeID_Name<BASE_CLASS>().c_str());
#endif

        if (!removed) {
            return false;
        }

        EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Get_TypeID_Name<BASE_CLASS>().c_str());

        /**
         *  x
         */
        Clear_Extension_Pointer(abstract_ptr);
    }

    return removed;
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
AbstractClassExtension *Find_Or_Make_Extension_Internal(const AbstractClass *abstract, bool allow_make)
{
    ASSERT(abstract != nullptr);

    bool created = false;
    AbstractClassExtension *extptr = nullptr;

    switch (const_cast<AbstractClass *>(abstract)->What_Am_I()) {

        case RTTI_AIRCRAFT:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const AircraftClass *>(abstract), AircraftExtensions, created, allow_make);
            break;

        case RTTI_AIRCRAFTTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const AircraftTypeClass *>(abstract), AircraftTypeExtensions, created, allow_make);
            break;

        case RTTI_ANIM:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const AnimClass *>(abstract), AnimExtensions, created, allow_make);
            break;

        case RTTI_ANIMTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const AnimTypeClass *>(abstract), AnimTypeExtensions, created, allow_make);
            break;

        case RTTI_BUILDING:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const BuildingClass *>(abstract), BuildingExtensions, created, allow_make);
            break;

        case RTTI_BUILDINGTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const BuildingTypeClass *>(abstract), BuildingTypeExtensions, created, allow_make);
            break;

        case RTTI_BULLETTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const BulletTypeClass *>(abstract), BulletTypeExtensions, created, allow_make);
            break;

        case RTTI_CAMPAIGN:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const CampaignClass *>(abstract), CampaignExtensions, created, allow_make);
            break;

        case RTTI_SIDE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const SideClass *>(abstract), SideExtensions, created, allow_make);
            break;

        case RTTI_HOUSE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const HouseClass *>(abstract), HouseExtensions, created, allow_make);
            break;

        case RTTI_HOUSETYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const HouseTypeClass *>(abstract), HouseTypeExtensions, created, allow_make);
            break;

        case RTTI_INFANTRY:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const InfantryClass *>(abstract), InfantryExtensions, created, allow_make);
            break;

        case RTTI_INFANTRYTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const InfantryTypeClass *>(abstract), InfantryTypeExtensions, created, allow_make);
            break;

        case RTTI_ISOTILETYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const IsometricTileTypeClass *>(abstract), IsometricTileTypeExtensions, created, allow_make);
            break;

        case RTTI_OVERLAYTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const OverlayTypeClass *>(abstract), OverlayTypeExtensions, created, allow_make);
            break;

        case RTTI_PARTICLESYSTEMTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const ParticleSystemTypeClass *>(abstract), ParticleSystemTypeExtensions, created, allow_make);
            break;

        case RTTI_PARTICLETYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const ParticleTypeClass *>(abstract), ParticleTypeExtensions, created, allow_make);
            break;

        case RTTI_SMUDGETYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const SmudgeTypeClass *>(abstract), SmudgeTypeExtensions, created, allow_make);
            break;

        case RTTI_SUPERWEAPON:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const SuperClass *>(abstract), SuperExtensions, created, allow_make);
            break;

        case RTTI_SUPERWEAPONTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const SuperWeaponTypeClass *>(abstract), SuperWeaponTypeExtensions, created, allow_make);
            break;

        case RTTI_TERRAIN:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const TerrainClass *>(abstract), TerrainExtensions, created, allow_make);
            break;

        case RTTI_TERRAINTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const TerrainTypeClass *>(abstract), TerrainTypeExtensions, created, allow_make);
            break;

        case RTTI_TIBERIUM:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const TiberiumClass *>(abstract), TiberiumExtensions, created, allow_make);
            break;

        case RTTI_UNIT:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const UnitClass *>(abstract), UnitExtensions, created, allow_make);
            break;

        case RTTI_UNITTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const UnitTypeClass *>(abstract), UnitTypeExtensions, created, allow_make);
            break;

        case RTTI_VOXELANIMTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const VoxelAnimTypeClass *>(abstract), VoxelAnimTypeExtensions, created, allow_make);
            break;

        case RTTI_WARHEADTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const WarheadTypeClass *>(abstract), WarheadTypeExtensions, created, allow_make);
            break;

        case RTTI_WAVE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const WaveClass *>(abstract), WaveExtensions, created, allow_make);
            break;

        case RTTI_WEAPONTYPE:
            extptr = Extension_Find_Or_Make(reinterpret_cast<const WeaponTypeClass *>(abstract), WeaponTypeExtensions, created, allow_make);
            break;

        default:
            DEBUG_ERROR("Find_Or_Make_Extension: No extension support for \"%s\" implemented!\n", Name_From_RTTI((RTTIType)abstract->What_Am_I()));
            break;

    };

    /**
     *  
     */
    if (created && extptr != nullptr) {
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
            removed = Extension_Destroy(reinterpret_cast<const AircraftClass *>(abstract), AircraftExtensions);
            break;

        case RTTI_AIRCRAFTTYPE:
            removed = Extension_Destroy(reinterpret_cast<const AircraftTypeClass *>(abstract), AircraftTypeExtensions);
            break;

        case RTTI_ANIM:
            removed = Extension_Destroy(reinterpret_cast<const AnimClass *>(abstract), AnimExtensions);
            break;

        case RTTI_ANIMTYPE:
            removed = Extension_Destroy(reinterpret_cast<const AnimTypeClass *>(abstract), AnimTypeExtensions);
            break;

        case RTTI_BUILDING:
            removed = Extension_Destroy(reinterpret_cast<const BuildingClass *>(abstract), BuildingExtensions);
            break;

        case RTTI_BUILDINGTYPE:
            removed = Extension_Destroy(reinterpret_cast<const BuildingTypeClass *>(abstract), BuildingTypeExtensions);
            break;

        case RTTI_BULLETTYPE:
            removed = Extension_Destroy(reinterpret_cast<const BulletTypeClass *>(abstract), BulletTypeExtensions);
            break;

        case RTTI_CAMPAIGN:
            removed = Extension_Destroy(reinterpret_cast<const CampaignClass *>(abstract), CampaignExtensions);
            break;

        case RTTI_SIDE:
            removed = Extension_Destroy(reinterpret_cast<const SideClass *>(abstract), SideExtensions);
            break;

        case RTTI_HOUSE:
            removed = Extension_Destroy(reinterpret_cast<const HouseClass *>(abstract), HouseExtensions);
            break;

        case RTTI_HOUSETYPE:
            removed = Extension_Destroy(reinterpret_cast<const HouseTypeClass *>(abstract), HouseTypeExtensions);
            break;

        case RTTI_INFANTRY:
            removed = Extension_Destroy(reinterpret_cast<const InfantryClass *>(abstract), InfantryExtensions);
            break;

        case RTTI_INFANTRYTYPE:
            removed = Extension_Destroy(reinterpret_cast<const InfantryTypeClass *>(abstract), InfantryTypeExtensions);
            break;

        case RTTI_ISOTILETYPE:
            removed = Extension_Destroy(reinterpret_cast<const IsometricTileTypeClass *>(abstract), IsometricTileTypeExtensions);
            break;

        case RTTI_OVERLAYTYPE:
            removed = Extension_Destroy(reinterpret_cast<const OverlayTypeClass *>(abstract), OverlayTypeExtensions);
            break;

        case RTTI_PARTICLESYSTEMTYPE:
            removed = Extension_Destroy(reinterpret_cast<const ParticleSystemTypeClass *>(abstract), ParticleSystemTypeExtensions);
            break;

        case RTTI_PARTICLETYPE:
            removed = Extension_Destroy(reinterpret_cast<const ParticleTypeClass *>(abstract), ParticleTypeExtensions);
            break;

        case RTTI_SMUDGETYPE:
            removed = Extension_Destroy(reinterpret_cast<const SmudgeTypeClass *>(abstract), SmudgeTypeExtensions);
            break;

        case RTTI_SUPERWEAPON:
            removed = Extension_Destroy(reinterpret_cast<const SuperClass *>(abstract), SuperExtensions);
            break;

        case RTTI_SUPERWEAPONTYPE:
            removed = Extension_Destroy(reinterpret_cast<const SuperWeaponTypeClass *>(abstract), SuperWeaponTypeExtensions);
            break;

        case RTTI_TERRAIN:
            removed = Extension_Destroy(reinterpret_cast<const TerrainClass *>(abstract), TerrainExtensions);
            break;

        case RTTI_TERRAINTYPE:
            removed = Extension_Destroy(reinterpret_cast<const TerrainTypeClass *>(abstract), TerrainTypeExtensions);
            break;

        case RTTI_TIBERIUM:
            removed = Extension_Destroy(reinterpret_cast<const TiberiumClass *>(abstract), TiberiumExtensions);
            break;

        case RTTI_UNIT:
            removed = Extension_Destroy(reinterpret_cast<const UnitClass *>(abstract), UnitExtensions);
            break;

        case RTTI_UNITTYPE:
            removed = Extension_Destroy(reinterpret_cast<const UnitTypeClass *>(abstract), UnitTypeExtensions);
            break;

        case RTTI_VOXELANIMTYPE:
            removed = Extension_Destroy(reinterpret_cast<const VoxelAnimTypeClass *>(abstract), VoxelAnimTypeExtensions);
            break;

        case RTTI_WARHEADTYPE:
            removed = Extension_Destroy(reinterpret_cast<const WarheadTypeClass *>(abstract), WarheadTypeExtensions);
            break;

        case RTTI_WAVE:
            removed = Extension_Destroy(reinterpret_cast<const WaveClass *>(abstract), WaveExtensions);
            break;

        case RTTI_WEAPONTYPE:
            removed = Extension_Destroy(reinterpret_cast<const WeaponTypeClass *>(abstract), WeaponTypeExtensions);
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
    
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNIT) && !Extension_Save<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFT) && !Extension_Save<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFTTYPE) && !Extension_Save<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIM) && !Extension_Save<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIMTYPE) && !Extension_Save<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDING) && !Extension_Save<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDINGTYPE) && !Extension_Save<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BULLETTYPE) && !Extension_Save<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_CAMPAIGN) && !Extension_Save<CampaignClass, CampaignClassExtension>(pStm, CampaignExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSE) && !Extension_Save<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSETYPE) && !Extension_Save<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRY) && !Extension_Save<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRYTYPE) && !Extension_Save<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ISOTILETYPE) && !Extension_Save<IsometricTileTypeClass, IsometricTileTypeClassExtension>(pStm, IsometricTileTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_OVERLAY) && !Extension_Save<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLESYSTEMTYPE) && !Extension_Save<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLETYPE) && !Extension_Save<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SIDE) && !Extension_Save<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SMUDGETYPE) && !Extension_Save<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPONTYPE) && !Extension_Save<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAIN) && !Extension_Save<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAINTYPE) && !Extension_Save<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNITTYPE) && !Extension_Save<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_VOXELANIMTYPE) && !Extension_Save<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WAVE) && !Extension_Save<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WEAPONTYPE) && !Extension_Save<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WARHEADTYPE) && !Extension_Save<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPON) && !Extension_Save<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TIBERIUM) && !Extension_Save<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }

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

    if (Is_Extension_Support_Enabled(EXT_RTTI_UNIT) && !Extension_Load<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFT) && !Extension_Load<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFTTYPE) && !Extension_Load<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIM) && !Extension_Load<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIMTYPE) && !Extension_Load<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDING) && !Extension_Load<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDINGTYPE) && !Extension_Load<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BULLETTYPE) && !Extension_Load<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_CAMPAIGN) && !Extension_Load<CampaignClass, CampaignClassExtension>(pStm, CampaignExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSE) && !Extension_Load<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSETYPE) && !Extension_Load<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRY) && !Extension_Load<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRYTYPE) && !Extension_Load<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ISOTILETYPE) && !Extension_Load<IsometricTileTypeClass, IsometricTileTypeClassExtension>(pStm, IsometricTileTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_OVERLAY) && !Extension_Load<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLESYSTEMTYPE) && !Extension_Load<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLETYPE) && !Extension_Load<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SIDE) && !Extension_Load<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SMUDGETYPE) && !Extension_Load<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPONTYPE) && !Extension_Load<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAIN) && !Extension_Load<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAINTYPE) && !Extension_Load<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNITTYPE) && !Extension_Load<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_VOXELANIMTYPE) && !Extension_Load<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WAVE) && !Extension_Load<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WEAPONTYPE) && !Extension_Load<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WARHEADTYPE) && !Extension_Load<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPON) && !Extension_Load<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TIBERIUM) && !Extension_Load<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }

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

    if (Is_Extension_Support_Enabled(EXT_RTTI_UNIT) && !Extension_Request_Pointer_Remap<UnitClass, UnitClassExtension>(Units)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFT) && !Extension_Request_Pointer_Remap<AircraftClass, AircraftClassExtension>(Aircrafts)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFTTYPE) && !Extension_Request_Pointer_Remap<AircraftTypeClass, AircraftTypeClassExtension>(AircraftTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIM) && !Extension_Request_Pointer_Remap<AnimClass, AnimClassExtension>(Anims)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIMTYPE) && !Extension_Request_Pointer_Remap<AnimTypeClass, AnimTypeClassExtension>(AnimTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDING) && !Extension_Request_Pointer_Remap<BuildingClass, BuildingClassExtension>(Buildings)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDINGTYPE) && !Extension_Request_Pointer_Remap<BuildingTypeClass, BuildingTypeClassExtension>(BuildingTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BULLETTYPE) && !Extension_Request_Pointer_Remap<BulletTypeClass, BulletTypeClassExtension>(BulletTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_CAMPAIGN) && !Extension_Request_Pointer_Remap<CampaignClass, CampaignClassExtension>(Campaigns)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSE) && !Extension_Request_Pointer_Remap<HouseClass, HouseClassExtension>(Houses)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSETYPE) && !Extension_Request_Pointer_Remap<HouseTypeClass, HouseTypeClassExtension>(HouseTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRY) && !Extension_Request_Pointer_Remap<InfantryClass, InfantryClassExtension>(Infantry)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRYTYPE) && !Extension_Request_Pointer_Remap<InfantryTypeClass, InfantryTypeClassExtension>(InfantryTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ISOTILETYPE) && !Extension_Request_Pointer_Remap<IsometricTileTypeClass, IsometricTileTypeClassExtension>(IsoTileTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_OVERLAYTYPE) && !Extension_Request_Pointer_Remap<OverlayTypeClass, OverlayTypeClassExtension>(OverlayTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLESYSTEMTYPE) && !Extension_Request_Pointer_Remap<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(ParticleSystemTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLETYPE) && !Extension_Request_Pointer_Remap<ParticleTypeClass, ParticleTypeClassExtension>(ParticleTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SIDE) && !Extension_Request_Pointer_Remap<SideClass, SideClassExtension>(Sides)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SMUDGE) && !Extension_Request_Pointer_Remap<SmudgeTypeClass, SmudgeTypeClassExtension>(SmudgeTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPONTYPE) && !Extension_Request_Pointer_Remap<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(SuperWeaponTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAIN) && !Extension_Request_Pointer_Remap<TerrainClass, TerrainClassExtension>(Terrains)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAINTYPE) && !Extension_Request_Pointer_Remap<TerrainTypeClass, TerrainTypeClassExtension>(TerrainTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNITTYPE) && !Extension_Request_Pointer_Remap<UnitTypeClass, UnitTypeClassExtension>(UnitTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_VOXELANIM) && !Extension_Request_Pointer_Remap<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(VoxelAnimTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WAVE) && !Extension_Request_Pointer_Remap<WaveClass, WaveClassExtension>(Waves)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WEAPONTYPE) && !Extension_Request_Pointer_Remap<WeaponTypeClass, WeaponTypeClassExtension>(WeaponTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WARHEADTYPE) && !Extension_Request_Pointer_Remap<WarheadTypeClass, WarheadTypeClassExtension>(WarheadTypes)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPON) && !Extension_Request_Pointer_Remap<SuperClass, SuperClassExtension>(Supers)) { return false; }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TIBERIUM) && !Extension_Request_Pointer_Remap<TiberiumClass, TiberiumClassExtension>(Tiberiums)) { return false; }

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
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNIT)) { REGISTER_CLASS(UnitClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFT)) { REGISTER_CLASS(AircraftClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_AIRCRAFTTYPE)) { REGISTER_CLASS(AircraftTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIM)) { REGISTER_CLASS(AnimClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ANIMTYPE)) { REGISTER_CLASS(AnimTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDING)) { REGISTER_CLASS(BuildingClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BUILDINGTYPE)) { REGISTER_CLASS(BuildingTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_BULLETTYPE)) { REGISTER_CLASS(BulletTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_CAMPAIGN)) { REGISTER_CLASS(CampaignClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSE)) { REGISTER_CLASS(HouseClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_HOUSETYPE)) { REGISTER_CLASS(HouseTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRY)) { REGISTER_CLASS(InfantryClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_INFANTRYTYPE)) { REGISTER_CLASS(InfantryTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_ISOTILETYPE)) { REGISTER_CLASS(IsometricTileTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_OVERLAYTYPE)) { REGISTER_CLASS(OverlayTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLESYSTEMTYPE)) { REGISTER_CLASS(ParticleSystemTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_PARTICLETYPE)) { REGISTER_CLASS(ParticleTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SIDE)) { REGISTER_CLASS(SideClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SMUDGE)) { REGISTER_CLASS(SmudgeTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPONTYPE)) { REGISTER_CLASS(SuperWeaponTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAIN)) { REGISTER_CLASS(TerrainClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TERRAINTYPE)) { REGISTER_CLASS(TerrainTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_UNITTYPE)) { REGISTER_CLASS(UnitTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_VOXELANIM)) { REGISTER_CLASS(VoxelAnimTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WAVE)) { REGISTER_CLASS(WaveClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WEAPONTYPE)) { REGISTER_CLASS(WeaponTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_WARHEADTYPE)) { REGISTER_CLASS(WarheadTypeClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_SUPERWEAPON)) { REGISTER_CLASS(SuperClassExtension) }
    if (Is_Extension_Support_Enabled(EXT_RTTI_TIBERIUM)) { REGISTER_CLASS(TiberiumClassExtension) }

    return true;
}
