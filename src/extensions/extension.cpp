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


/**
 *  x
 */
#define ABSTRACT_EXTENSION_POINTER_CAST_MACRO(ptr)    (*(uintptr_t *)(((unsigned char *)ptr) + 0x10))

/**
 *  x
 */
#define ABSTRACT_EXTENSION_POINTER_REMAP_MACRO(ptr)    (uintptr_t **)(((unsigned char *)ptr) + 0x10);


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void Extension_Set_Abstract_Pointer(const AbstractClass *abstract, const AbstractClassExtension *abstract_extension)
{
    ASSERT(abstract != nullptr);
    ASSERT(abstract_extension != nullptr);

    ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract) = (uintptr_t)abstract_extension;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static AbstractClassExtension *Extension_Get_Abstract_Pointer(const AbstractClass *abstract)
{
    uintptr_t abstract_extension_address = ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract);
    AbstractClassExtension *abstract_extension = (AbstractClassExtension *)abstract_extension_address;
    return abstract_extension;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void Extension_Clear_Abstract_Pointer(const AbstractClass *abstract)
{
    ABSTRACT_EXTENSION_POINTER_CAST_MACRO(abstract) = (uintptr_t)0x00000000;
}


/**
 *  Check if the extension pointer is a valid memory address.
 * 
 *  #WARNING: This is not guanteed to work, but it should capture the majority of possible bad pointers.
 * 
 *  0x00870000 -> End of GAME.EXE .data segment virtual address.
 *  0x20000000 -> Arbitrary address VINIFERA.DLL 'should' never get to.
 */
static bool Extension_Is_Valid_Pointer(const AbstractClassExtension *abstract_extension)
{
    return ((uintptr_t)abstract_extension) >= 0x00870000 || ((uintptr_t)abstract_extension) < 0x20000000;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static const char *Extension_Get_Abstract_Name(const AbstractClass *abstract)
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
    //EXT_DEBUG_INFO("Extension_Make... %s %s %s\n", Extension_Get_Abstract_Name(abstract_ptr), Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());

    const BASE_CLASS *abs_ptr = reinterpret_cast<const BASE_CLASS *>(abstract_ptr);
    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Extension_Get_Abstract_Pointer(abs_ptr));
    
    /**
     *  x
     */
    ext_ptr = new EXT_CLASS(reinterpret_cast<const BASE_CLASS *>(abs_ptr));
    ASSERT(ext_ptr != nullptr);
    if (!ext_ptr) {

        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Extension_Make: Failed to make \"%s\" extension!\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

        EXT_DEBUG_WARNING(buffer);

        ShowCursor(TRUE);
        MessageBoxA(MainWindow, buffer, "Vinifera", MB_OK|MB_ICONEXCLAMATION);

        //Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create WeaponTypeClassExtension instance!\n");

        return nullptr;
    }

    EXT_DEBUG_INFO("Created \"%s\" extension.\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

    /**
     *  Assign the extension class instance to the abstract class.
     */
    Extension_Set_Abstract_Pointer(abstract_ptr, ext_ptr);

    return ext_ptr;
}


/**
 *  Destroys the attached extension instance for the abstract class.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Destroy(const BASE_CLASS *abstract_ptr)
{
    //EXT_DEBUG_INFO("Extension_Destroy... %s %s %s\n", Extension_Get_Abstract_Name(abstract_ptr), Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
    bool removed = false;

    EXT_CLASS *ext_ptr = reinterpret_cast<EXT_CLASS *>(Extension_Get_Abstract_Pointer(abstract_ptr));
    if (!ext_ptr) {
        EXT_DEBUG_WARNING("Extension_Destroy: \"%s\" extension pointer is null!\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());
        return false;
    }
    
    /**
     *  Destroy the attached extension class instance.
     */
    delete ext_ptr;

    EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

    /**
     *  Clear the extension pointer for the abstract class.
     */
    Extension_Clear_Abstract_Pointer(abstract_ptr);

    return true;
}


/**
 *  List version.
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
        DEBUG_INFO("List for \"%s\" has a count of zero, skipping save.\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return true;
    }

    DEBUG_INFO("Saving \"%s\" extensions (Count: %d)\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), list.Count());

    for (int index = 0; index < count; ++index) {

        EXT_CLASS *ptr = list[index];

        /**
         *  Tell the extension class to persist itself into the data stream.
         */
        IPersistStream *lpPS = nullptr;
        hr = ptr->QueryInterface(__uuidof(IPersistStream), (LPVOID *)&lpPS);
        if (FAILED(hr)) {
            DEBUG_ERROR("Extension \"%s\" does not support IPersistStream!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
            return false;
        }

        hr = OleSaveToStream(lpPS, pStm);
        if (FAILED(hr)) {
            DEBUG_ERROR("OleSaveToStream failed for extension \"%s\" (Index: %d)!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str(), index);
            return false;
        }

        hr = lpPS->Release();
        if (FAILED(hr)) {
            DEBUG_ERROR("Failed to release extension \"%s\" stream!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
            return false;
        }

        EXT_CLASS * ext_ptr = reinterpret_cast<EXT_CLASS *>(lpPS);

        if (ext_ptr->What_Am_I() != EXT_RTTI_WAVE) {
            EXT_DEBUG_INFO("  -> %s\n", ext_ptr->Name());
        }
    }

    return true;
}


/**
 *  Singleton abstract version.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Save(IStream *pStm, EXT_CLASS *ext)
{
    DEBUG_INFO("Saving \"%s\" extension\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

    /**
     *  Tell the extension class to persist itself into the data stream.
     */
    IPersistStream *lpPS = nullptr;
    HRESULT hr = ext->QueryInterface(__uuidof(IPersistStream), (LPVOID *)&lpPS);
    if (FAILED(hr)) {
        DEBUG_ERROR("Extension \"%s\" does not support IPersistStream!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return false;
    }
    
    hr = OleSaveToStream(lpPS, pStm);
    if (FAILED(hr)) {
        DEBUG_ERROR("OleSaveToStream failed for extension \"%s\"!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return false;
    }
    
    hr = lpPS->Release();
    if (FAILED(hr)) {
        DEBUG_ERROR("Failed to release extension \"%s\" stream!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return false;
    }
    
    EXT_CLASS * ext_ptr = reinterpret_cast<EXT_CLASS *>(lpPS);
    EXT_DEBUG_INFO("  -> %s\n", ext_ptr->Name());

    return true;
}


/**
 *  List version.
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
        DEBUG_INFO("List for \"%s\" has a count of zero, skipping load.\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return true;
    }
    DEBUG_INFO("Loading \"%s\" extensions (Count: %d)\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), count);
    
    for (int index = 0; index < count; ++index) {
        
        IUnknown *spUnk = nullptr;
        hr = OleLoadFromStream(pStm, __uuidof(IUnknown), (LPVOID *)&spUnk);
        if (FAILED(hr)) {
            DEBUG_ERROR("OleLoadFromStream failed for extension \"%s\" (Index: %d)!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str(), index);
            return false;
        }

    }

    return true;
}


/**
 *  Singleton abstract version.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Load(IStream *pStm, EXT_CLASS *ext)
{
    DEBUG_INFO("Loading \"%s\" extension.\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());
        
    IUnknown *spUnk = nullptr;
    HRESULT hr = OleLoadFromStream(pStm, __uuidof(IUnknown), (LPVOID *)&spUnk);
    if (FAILED(hr)) {
        DEBUG_ERROR("OleLoadFromStream failed for extension \"%s\"!\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
        return false;
    }

    return true;
}


/**
 *  List version.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Request_Pointer_Remap(const DynamicVectorClass<BASE_CLASS *> &list)
{
    if (!list.Count()) {
        DEBUG_INFO("Requested remap of \"%s\" extension pointers, but the list is empty!\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());
        return true;
    }

    DEBUG_INFO("Requesting remap of \"%s\" extension pointers (Count %d)...\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), list.Count());

    for (int index = 0; index < list.Count(); ++index) {
        if (index == 0) {
            EXT_DEBUG_INFO("\n");
        }

        const BASE_CLASS *object = list[index];
        if (object) {

            if (!Extension_Get_Abstract_Pointer(object)) {
                DEV_DEBUG_ERROR("Extension_Request_Pointer_Remap: \"%s\" extension pointer (index %d) for is null!\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str(), index);
                continue; //return false;
            }

            /**
             *  
             */
            uintptr_t **ext_ptr_addr = ABSTRACT_EXTENSION_POINTER_REMAP_MACRO(object);
            VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(*ext_ptr_addr, "AbstractClass::ExtPtr");

            EXT_DEBUG_INFO("  Requested remap of index %d extension pointer complete.\n", index);
        }
    }

    return true;
}


/**
 *  Singleton abstract version.
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
static bool Extension_Request_Pointer_Remap(BASE_CLASS *abstract)
{
    DEBUG_INFO("Requesting remap of \"%s\" extension pointer...\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

    if (!Extension_Get_Abstract_Pointer(abstract)) {
        EXT_DEBUG_ERROR("Extension_Request_Pointer_Remap: \"%s\" extension pointer for is null!\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());
        return false;
    }

    /**
     *  
     */
    uintptr_t **ext_ptr_addr = ABSTRACT_EXTENSION_POINTER_REMAP_MACRO(abstract);
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(*ext_ptr_addr, "AbstractClass::ExtPtr");

    EXT_DEBUG_INFO("  Requested remap of extension pointer complete.\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
static void Extension_Detach_This_From_All(DynamicVectorClass<EXT_CLASS *> &list, TARGET target, bool all)
{
    for (int index = 0; index < list.Count(); ++index) {
        list[index]->Detach(target, all);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
static void Extension_Print_Sync_Data(DynamicVectorClass<EXT_CLASS *> &list, FILE *fp, EventClass *ev)
{
#if 0
    for (int house = 0; house <= Houses.Count(); ++house) {
        HouseClass *housep = Houses[house];
        if (housep) {
            GameCRC = 0;
            std::fprintf(fp, "------------- %s %s ------------\n", housep->Class->Name(), Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
            for (int index = 0; index < list.Count(); ++index) {
                EXT_CLASS *ptr = list[index];
                if (ptr->Owner() == house) {
                    Add_CRC(&GameCRC, (int)ptr->Coord + (int)ptr->PrimaryFacing);
                    std::fprintf(fp, "COORD:%x   Facing:%d   Mission:%d   Type:%d   Tgt:%x\n",
                                ptr->This()->Coord, (int)ptr->This()->PrimaryFacing, ptr->This()->Get_Mission(),
                                ptr->This()->Class->Type, ptr->This()->As_Target());
                }
            }
            EXT_DEBUG_INFO("%s %s:%x\n", housep->Class->Name(), Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str(), GameCRC);
        }
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
static void Extension_Print_CRCs(DynamicVectorClass<EXT_CLASS *> &list, FILE *fp)
{
    WWCRCEngine *crc = new WWCRCEngine;

    std::fprintf(fp, "\n\n********* %s CRCs ********\n\n", Extension::Private::Get_TypeID_Name<EXT_CLASS>().c_str());
    std::fprintf(fp, "Index     CRC\n");
    std::fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    for (int index = 0; index < list.Count(); ++index) {
        EXT_CLASS *ptr = list[index];
        ptr->Compute_CRC(*crc);
        std::fprintf(fp, "%05d      %08x\n", index, crc->CRC_Value());
        EXT_DEBUG_INFO("%05d      %08x\n", index, crc->CRC_Value());
    }

    delete crc;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AbstractClassExtension *Extension::Private::Make_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    AbstractClassExtension *extptr = nullptr;

    switch (const_cast<AbstractClass *>(abstract)->What_Am_I()) {
        case RTTI_UNIT: { extptr = Extension_Make<UnitClass, UnitClassExtension>(reinterpret_cast<const UnitClass *>(abstract)); break; }
        case RTTI_AIRCRAFT: { extptr = Extension_Make<AircraftClass, AircraftClassExtension>(reinterpret_cast<const AircraftClass *>(abstract)); break; }
        case RTTI_AIRCRAFTTYPE: { extptr = Extension_Make<AircraftTypeClass, AircraftTypeClassExtension>(reinterpret_cast<const AircraftTypeClass *>(abstract)); break; }
        case RTTI_ANIM: { extptr = Extension_Make<AnimClass, AnimClassExtension>(reinterpret_cast<const AnimClass *>(abstract)); break; }
        case RTTI_ANIMTYPE: { extptr = Extension_Make<AnimTypeClass, AnimTypeClassExtension>(reinterpret_cast<const AnimTypeClass *>(abstract)); break; }
        case RTTI_BUILDING: { extptr = Extension_Make<BuildingClass, BuildingClassExtension>(reinterpret_cast<const BuildingClass *>(abstract)); break; }
        case RTTI_BUILDINGTYPE: { extptr = Extension_Make<BuildingTypeClass, BuildingTypeClassExtension>(reinterpret_cast<const BuildingTypeClass *>(abstract));break; }
        //case RTTI_BULLET: { }                   // <- Not yet implemented
        case RTTI_BULLETTYPE: { extptr = Extension_Make<BulletTypeClass, BulletTypeClassExtension>(reinterpret_cast<const BulletTypeClass *>(abstract));break; }
        case RTTI_CAMPAIGN: { extptr = Extension_Make<CampaignClass, CampaignClassExtension>(reinterpret_cast<const CampaignClass *>(abstract)); break; }
        //case RTTI_CELL: { }                     // <- Not yet implemented
        //case RTTI_FACTORY: { }                  // <- Not yet implemented
        case RTTI_HOUSE: { extptr = Extension_Make<HouseClass, HouseClassExtension>(reinterpret_cast<const HouseClass *>(abstract)); break; }
        case RTTI_HOUSETYPE: { extptr = Extension_Make<HouseTypeClass, HouseTypeClassExtension>(reinterpret_cast<const HouseTypeClass *>(abstract)); break; }
        case RTTI_INFANTRY: {  extptr = Extension_Make<InfantryClass, InfantryClassExtension>(reinterpret_cast<const InfantryClass *>(abstract)); break; }
        case RTTI_INFANTRYTYPE: {  extptr = Extension_Make<InfantryTypeClass, InfantryTypeClassExtension>(reinterpret_cast<const InfantryTypeClass *>(abstract)); break; }
        //case RTTI_ISOTILE: { }                  // <- Not yet implemented
        case RTTI_ISOTILETYPE: { extptr = Extension_Make<IsometricTileTypeClass, IsometricTileTypeClassExtension>(reinterpret_cast<const IsometricTileTypeClass *>(abstract)); break; }
        //case RTTI_LIGHT: { }                    // <- Not yet implemented
        //case RTTI_OVERLAY: { }                  // <- Not yet implemented
        case RTTI_OVERLAYTYPE: { extptr = Extension_Make<OverlayTypeClass, OverlayTypeClassExtension>(reinterpret_cast<const OverlayTypeClass *>(abstract)); break; }
        //case RTTI_PARTICLE: { }                 // <- Not yet implemented
        case RTTI_PARTICLETYPE: { extptr = Extension_Make<ParticleTypeClass, ParticleTypeClassExtension>(reinterpret_cast<const ParticleTypeClass *>(abstract)); break; }
        //case RTTI_PARTICLESYSTEM: { }           // <- Not yet implemented
        case RTTI_PARTICLESYSTEMTYPE: { extptr = Extension_Make<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(reinterpret_cast<const ParticleSystemTypeClass *>(abstract)); break; }
        //case RTTI_SCRIPT: { }                   // <- Not yet implemented
        //case RTTI_SCRIPTTYPE: { }               // <- Not yet implemented
        case RTTI_SIDE: { extptr = Extension_Make<SideClass, SideClassExtension>(reinterpret_cast<const SideClass *>(abstract)); break; }
        //case RTTI_SMUDGE: { }                   // <- Not yet implemented
        case RTTI_SMUDGETYPE: { extptr = Extension_Make<SmudgeTypeClass, SmudgeTypeClassExtension>(reinterpret_cast<const SmudgeTypeClass *>(abstract)); break; }
        //case RTTI_SPECIAL: { }                  // <- Special case, does not actually "exist".
        case RTTI_SUPERWEAPONTYPE: { extptr = Extension_Make<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(reinterpret_cast<const SuperWeaponTypeClass *>(abstract)); break; }
        //case RTTI_TASKFORCE: { }                // <- Not yet implemented
        //case RTTI_TEAM: { }                     // <- Not yet implemented
        //case RTTI_TEAMTYPE: { }                 // <- Not yet implemented
        case RTTI_TERRAIN: { extptr = Extension_Make<TerrainClass, TerrainClassExtension>(reinterpret_cast<const TerrainClass *>(abstract)); break; }
        case RTTI_TERRAINTYPE: { extptr = Extension_Make<TerrainTypeClass, TerrainTypeClassExtension>(reinterpret_cast<const TerrainTypeClass *>(abstract)); break; }
        //case RTTI_TRIGGER: { }                  // <- Not yet implemented
        //case RTTI_TRIGGERTYPE: { }              // <- Not yet implemented
        case RTTI_UNITTYPE: { extptr = Extension_Make<UnitTypeClass, UnitTypeClassExtension>(reinterpret_cast<const UnitTypeClass *>(abstract)); break; }
        //case RTTI_VOXELANIM: { }                // <- Not yet implemented
        case RTTI_VOXELANIMTYPE: { extptr = Extension_Make<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(reinterpret_cast<const VoxelAnimTypeClass *>(abstract)); break; }
        case RTTI_WAVE: { extptr = Extension_Make<WaveClass, WaveClassExtension>(reinterpret_cast<const WaveClass *>(abstract)); break; }
        //case RTTI_TAG: { }                      // <- Not yet implemented
        //case RTTI_TAGTYPE: { }                  // <- Not yet implemented
        case RTTI_TIBERIUM: { extptr = Extension_Make<TiberiumClass, TiberiumClassExtension>(reinterpret_cast<const TiberiumClass *>(abstract)); break; }
        //case RTTI_ACTION: { }                   // <- Not yet implemented
        //case RTTI_EVENT: { }                    // <- Not yet implemented
        case RTTI_WEAPONTYPE: { extptr = Extension_Make<WeaponTypeClass, WeaponTypeClassExtension>(reinterpret_cast<const WeaponTypeClass *>(abstract)); break; }
        case RTTI_WARHEADTYPE: { extptr = Extension_Make<WarheadTypeClass, WarheadTypeClassExtension>(reinterpret_cast<const WarheadTypeClass *>(abstract)); break; }
        //case RTTI_WAYPOINT: { }                 // <- Not yet implemented
        //case RTTI_ABSTRACT: { }                 // <- Special case, does not actually "exist".
        //case RTTI_TUBE: { }                     // <- Not yet implemented
        //case RTTI_LIGHTSOURCE: { }              // <- Not yet implemented
        //case RTTI_EMPULSE: { }                  // <- Not yet implemented
        case RTTI_TACTICALMAP: { extptr = Extension_Make<Tactical, TacticalExtension>(reinterpret_cast<const Tactical *>(abstract)); break; }
        case RTTI_SUPERWEAPON: { extptr = Extension_Make<SuperClass, SuperClassExtension>(reinterpret_cast<const SuperClass *>(abstract)); break; }
        //case RTTI_AITRIGGER: { }                // <- Not yet implemented
        //case RTTI_AITRIGGERTYPE: { }            // <- Not yet implemented
        //case RTTI_NEURON: { }                   // <- Not yet implemented
        //case RTTI_FOGGEDOBJECT: { }             // <- Not yet implemented
        //case RTTI_ALPHASHAPE: { }               // <- Not yet implemented
        //case RTTI_VEINHOLEMONSTER: { }          // <- Not yet implemented
        default: { DEBUG_ERROR("Extension::Make: No extension support for \"%s\" implemented!\n", Name_From_RTTI((RTTIType)abstract->What_Am_I())); break; }
    };

    return extptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Extension::Private::Destroy_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);
    
    bool removed = false;

    switch (abstract->What_Am_I()) {
        case RTTI_UNIT: { removed = Extension_Destroy<UnitClass, UnitClassExtension>(reinterpret_cast<const UnitClass *>(abstract)); break; }
        case RTTI_AIRCRAFT: { removed = Extension_Destroy<AircraftClass, AircraftClassExtension>(reinterpret_cast<const AircraftClass *>(abstract)); break; }
        case RTTI_AIRCRAFTTYPE: { removed = Extension_Destroy<AircraftTypeClass, AircraftTypeClassExtension>(reinterpret_cast<const AircraftTypeClass *>(abstract)); break; }
        case RTTI_ANIM: { removed = Extension_Destroy<AnimClass, AnimClassExtension>(reinterpret_cast<const AnimClass *>(abstract)); break; }
        case RTTI_ANIMTYPE: { removed = Extension_Destroy<AnimTypeClass, AnimTypeClassExtension>(reinterpret_cast<const AnimTypeClass *>(abstract)); break; }
        case RTTI_BUILDING: { removed = Extension_Destroy<BuildingClass, BuildingClassExtension>(reinterpret_cast<const BuildingClass *>(abstract)); break; }
        case RTTI_BUILDINGTYPE: { removed = Extension_Destroy<BuildingTypeClass, BuildingTypeClassExtension>(reinterpret_cast<const BuildingTypeClass *>(abstract));break; }
        //case RTTI_BULLET: { }                   // <- Not yet implemented
        case RTTI_BULLETTYPE: { removed = Extension_Destroy<BulletTypeClass, BulletTypeClassExtension>(reinterpret_cast<const BulletTypeClass *>(abstract));break; }
        case RTTI_CAMPAIGN: { removed = Extension_Destroy<CampaignClass, CampaignClassExtension>(reinterpret_cast<const CampaignClass *>(abstract)); break; }
        //case RTTI_CELL: { }                     // <- Not yet implemented
        //case RTTI_FACTORY: { }                  // <- Not yet implemented
        case RTTI_HOUSE: { removed = Extension_Destroy<HouseClass, HouseClassExtension>(reinterpret_cast<const HouseClass *>(abstract)); break; }
        case RTTI_HOUSETYPE: { removed = Extension_Destroy<HouseTypeClass, HouseTypeClassExtension>(reinterpret_cast<const HouseTypeClass *>(abstract)); break; }
        case RTTI_INFANTRY: {  removed = Extension_Destroy<InfantryClass, InfantryClassExtension>(reinterpret_cast<const InfantryClass *>(abstract)); break; }
        case RTTI_INFANTRYTYPE: {  removed = Extension_Destroy<InfantryTypeClass, InfantryTypeClassExtension>(reinterpret_cast<const InfantryTypeClass *>(abstract)); break; }
        //case RTTI_ISOTILE: { }                  // <- Not yet implemented
        case RTTI_ISOTILETYPE: { removed = Extension_Destroy<IsometricTileTypeClass, IsometricTileTypeClassExtension>(reinterpret_cast<const IsometricTileTypeClass *>(abstract)); break; }
        //case RTTI_LIGHT: { }                    // <- Not yet implemented
        //case RTTI_OVERLAY: { }                  // <- Not yet implemented
        case RTTI_OVERLAYTYPE: { removed = Extension_Destroy<OverlayTypeClass, OverlayTypeClassExtension>(reinterpret_cast<const OverlayTypeClass *>(abstract)); break; }
        //case RTTI_PARTICLE: { }                 // <- Not yet implemented
        case RTTI_PARTICLETYPE: { removed = Extension_Destroy<ParticleTypeClass, ParticleTypeClassExtension>(reinterpret_cast<const ParticleTypeClass *>(abstract)); break; }
        //case RTTI_PARTICLESYSTEM: { }           // <- Not yet implemented
        case RTTI_PARTICLESYSTEMTYPE: { removed = Extension_Destroy<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(reinterpret_cast<const ParticleSystemTypeClass *>(abstract)); break; }
        //case RTTI_SCRIPT: { }                   // <- Not yet implemented
        //case RTTI_SCRIPTTYPE: { }               // <- Not yet implemented
        case RTTI_SIDE: { removed = Extension_Destroy<SideClass, SideClassExtension>(reinterpret_cast<const SideClass *>(abstract)); break; }
        //case RTTI_SMUDGE: { }                   // <- Not yet implemented
        case RTTI_SMUDGETYPE: { removed = Extension_Destroy<SmudgeTypeClass, SmudgeTypeClassExtension>(reinterpret_cast<const SmudgeTypeClass *>(abstract)); break; }
        //case RTTI_SPECIAL: { }                  // <- Special case, does not actually "exist".
        case RTTI_SUPERWEAPONTYPE: { removed = Extension_Destroy<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(reinterpret_cast<const SuperWeaponTypeClass *>(abstract)); break; }
        //case RTTI_TASKFORCE: { }                // <- Not yet implemented
        //case RTTI_TEAM: { }                     // <- Not yet implemented
        //case RTTI_TEAMTYPE: { }                 // <- Not yet implemented
        case RTTI_TERRAIN: { removed = Extension_Destroy<TerrainClass, TerrainClassExtension>(reinterpret_cast<const TerrainClass *>(abstract)); break; }
        case RTTI_TERRAINTYPE: { removed = Extension_Destroy<TerrainTypeClass, TerrainTypeClassExtension>(reinterpret_cast<const TerrainTypeClass *>(abstract)); break; }
        //case RTTI_TRIGGER: { }                  // <- Not yet implemented
        //case RTTI_TRIGGERTYPE: { }              // <- Not yet implemented
        case RTTI_UNITTYPE: { removed = Extension_Destroy<UnitTypeClass, UnitTypeClassExtension>(reinterpret_cast<const UnitTypeClass *>(abstract)); break; }
        //case RTTI_VOXELANIM: { }                // <- Not yet implemented
        case RTTI_VOXELANIMTYPE: { removed = Extension_Destroy<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(reinterpret_cast<const VoxelAnimTypeClass *>(abstract)); break; }
        case RTTI_WAVE: { removed = Extension_Destroy<WaveClass, WaveClassExtension>(reinterpret_cast<const WaveClass *>(abstract)); break; }
        //case RTTI_TAG: { }                      // <- Not yet implemented
        //case RTTI_TAGTYPE: { }                  // <- Not yet implemented
        case RTTI_TIBERIUM: { removed = Extension_Destroy<TiberiumClass, TiberiumClassExtension>(reinterpret_cast<const TiberiumClass *>(abstract)); break; }
        //case RTTI_ACTION: { }                   // <- Not yet implemented
        //case RTTI_EVENT: { }                    // <- Not yet implemented
        case RTTI_WEAPONTYPE: { removed = Extension_Destroy<WeaponTypeClass, WeaponTypeClassExtension>(reinterpret_cast<const WeaponTypeClass *>(abstract)); break; }
        case RTTI_WARHEADTYPE: { removed = Extension_Destroy<WarheadTypeClass, WarheadTypeClassExtension>(reinterpret_cast<const WarheadTypeClass *>(abstract)); break; }
        //case RTTI_WAYPOINT: { }                 // <- Not yet implemented
        //case RTTI_ABSTRACT: { }                 // <- Special case, does not actually "exist".
        //case RTTI_TUBE: { }                     // <- Not yet implemented
        //case RTTI_LIGHTSOURCE: { }              // <- Not yet implemented
        //case RTTI_EMPULSE: { }                  // <- Not yet implemented
        case RTTI_TACTICALMAP: { removed = Extension_Destroy<Tactical, TacticalExtension>(reinterpret_cast<const Tactical *>(abstract)); break; }
        case RTTI_SUPERWEAPON: { removed = Extension_Destroy<SuperClass, SuperClassExtension>(reinterpret_cast<const SuperClass *>(abstract)); break; }
        //case RTTI_AITRIGGER: { }                // <- Not yet implemented
        //case RTTI_AITRIGGERTYPE: { }            // <- Not yet implemented
        //case RTTI_NEURON: { }                   // <- Not yet implemented
        //case RTTI_FOGGEDOBJECT: { }             // <- Not yet implemented
        //case RTTI_ALPHASHAPE: { }               // <- Not yet implemented
        //case RTTI_VEINHOLEMONSTER: { }          // <- Not yet implemented
        default: { DEBUG_ERROR("Extension::Destroy: No extension support for \"%s\" implemented!\n", Name_From_RTTI((RTTIType)abstract->What_Am_I())); break; }
    };

    ASSERT(removed);

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AbstractClassExtension *Extension::Private::Fetch_Internal(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    AbstractClassExtension *ext_ptr = Extension_Get_Abstract_Pointer(abstract);

    if (!ext_ptr) {
        DEBUG_ERROR("Extension::Fetch: Extension for \"%s\" is null!\n", Extension_Get_Abstract_Name(abstract));
        return nullptr;
    }

    /**
     *  Check for a malformed extension pointer.
     */
    if (!Extension_Is_Valid_Pointer(ext_ptr)) {
        DEBUG_ERROR("Extension::Fetch: Corrupt extension pointer for \"%s\"!\n", Extension_Get_Abstract_Name(abstract));
        return nullptr;
    }

    /**
     *  Its still possible the pointer could be invalid, so perform a final check.
     */
    if (ext_ptr->What_Am_I() <= EXT_RTTI_NONE || ext_ptr->What_Am_I() >= EXT_RTTI_COUNT) {
        DEBUG_ERROR("Extension::Fetch: Invalid extension rtti type for \"%s\"!\n", Extension_Get_Abstract_Name(abstract));
        return nullptr;
    }

    //EXT_DEBUG_INFO("Extension::Fetch: Abstract \"%s\", got extension \"%s\".\n", Extension_Get_Abstract_Name(abstract), ext_ptr->Name());

    return ext_ptr;
}


/**
 *  Do we support the class indentified by the RTTIType with an extension class?
 * 
 *  @author: CCHyper
 */
bool Extension::Is_Supported(RTTIType rtti)
{
    switch (rtti) {
        case RTTI_UNIT:
        case RTTI_AIRCRAFT:
        case RTTI_AIRCRAFTTYPE:
        case RTTI_ANIM:
        case RTTI_ANIMTYPE:
        case RTTI_BUILDING:
        case RTTI_BUILDINGTYPE:
        //case RTTI_BULLET:                     // <- Not yet implemented
        case RTTI_BULLETTYPE:
        case RTTI_CAMPAIGN:                     // Supported, but Campaign's are not saved to file.
        //case RTTI_CELL:                       // <- Not yet implemented
        //case RTTI_FACTORY:                    // <- Not yet implemented
        case RTTI_HOUSE:
        case RTTI_HOUSETYPE:
        case RTTI_INFANTRY:
        case RTTI_INFANTRYTYPE:
        //case RTTI_ISOTILE:                    // <- Not yet implemented
        case RTTI_ISOTILETYPE:                  // Supported, but IsoTileTypes's are not saved to file.
        //case RTTI_LIGHT:                      // <- Not yet implemented
        //case RTTI_OVERLAY:                    // <- Not yet implemented
        case RTTI_OVERLAYTYPE:
        //case RTTI_PARTICLE:                   // <- Not yet implemented
        case RTTI_PARTICLETYPE:
        //case RTTI_PARTICLESYSTEM:             // <- Not yet implemented
        case RTTI_PARTICLESYSTEMTYPE:
        //case RTTI_SCRIPT:                     // <- Not yet implemented
        //case RTTI_SCRIPTTYPE:                 // <- Not yet implemented
        case RTTI_SIDE:
        //case RTTI_SMUDGE:                     // <- Not yet implemented
        case RTTI_SMUDGETYPE:
        case RTTI_SUPERWEAPONTYPE:
        //case RTTI_TASKFORCE:                  // <- Not yet implemented
        //case RTTI_TEAM:                       // <- Not yet implemented
        //case RTTI_TEAMTYPE:                   // <- Not yet implemented
        case RTTI_TERRAIN:
        case RTTI_TERRAINTYPE:
        //case RTTI_TRIGGER:                    // <- Not yet implemented
        //case RTTI_TRIGGERTYPE:                // <- Not yet implemented
        case RTTI_UNITTYPE:
        //case RTTI_VOXELANIM:                  // <- Not yet implemented
        case RTTI_VOXELANIMTYPE:
        case RTTI_WAVE:
        //case RTTI_TAG:                        // <- Not yet implemented
        //case RTTI_TAGTYPE:                    // <- Not yet implemented
        //case RTTI_TIBERIUM:
        //case RTTI_ACTION:                     // <- Not yet implemented
        //case RTTI_EVENT:                      // <- Not yet implemented
        case RTTI_WEAPONTYPE:
        case RTTI_WARHEADTYPE:
        //case RTTI_WAYPOINT:                   // <- Not yet implemented
        //case RTTI_TUBE:                       // <- Not yet implemented
        //case RTTI_LIGHTSOURCE:                // <- Not yet implemented
        //case RTTI_EMPULSE:                    // <- Not yet implemented
        case RTTI_TACTICALMAP:
        case RTTI_SUPERWEAPON:
        //case RTTI_AITRIGGER:                  // <- Not yet implemented
        //case RTTI_AITRIGGERTYPE:              // <- Not yet implemented
        //case RTTI_NEURON:                     // <- Not yet implemented
        //case RTTI_FOGGEDOBJECT:               // <- Not yet implemented
        //case RTTI_ALPHASHAPE:                 // <- Not yet implemented
        //case RTTI_VEINHOLEMONSTER:            // <- Not yet implemented
            return true;

        case RTTI_SPECIAL:                      // Special case for sidebar cameos.
        case RTTI_ABSTRACT:                     // Special case for identifying classes with no What_Am_I() implemented.
            return false;

        case RTTI_NONE:
        default:
            return false;
    };
}


/**
 *  Do we support the class indentified by the RTTIType with an extension class?
 * 
 *  @author: CCHyper
 */
bool Extension::Is_Supported(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return Extension::Is_Supported(RTTIType(abstract->What_Am_I()));
}


/**
 *  Save all the extension class data to the stream.
 * 
 *  @author: CCHyper
 */
bool Extension::Save(IStream *pStm)
{
    ASSERT(pStm != nullptr);

    if (!pStm) {
        return false;
    }

    DEBUG_INFO("Extension::Save(enter)\n");
    
    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT) && !Extension_Save<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFT) && !Extension_Save<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE) && !Extension_Save<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIM) && !Extension_Save<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIMTYPE) && !Extension_Save<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDING) && !Extension_Save<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE) && !Extension_Save<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE) && !Extension_Save<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { }                       // <- Do not save!
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE) && !Extension_Save<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_HOUSETYPE) && !Extension_Save<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRY) && !Extension_Save<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE) && !Extension_Save<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { }                    // <- Do not save!
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE) && !Extension_Save<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE) && !Extension_Save<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE) && !Extension_Save<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE) && !Extension_Save<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE) && !Extension_Save<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Do not save!
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE) && !Extension_Save<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN) && !Extension_Save<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE) && !Extension_Save<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE) && !Extension_Save<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE) && !Extension_Save<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WAVE) && !Extension_Save<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM) && !Extension_Save<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE) && !Extension_Save<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE) && !Extension_Save<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Do not save!
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP) && !Extension_Save<Tactical, TacticalExtension>(pStm, TacticalMapExtension)) { return false; }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON) && !Extension_Save<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    if (FAILED(RuleExtension->Save(pStm, true))) { return false; }
    DEBUG_INFO("Saved \"%s\" extension\n", RuleExtension->Name());

    if (FAILED(ScenExtension->Save(pStm, true))) { return false; }
    DEBUG_INFO("Saved \"%s\" extension\n", ScenExtension->Name());

    if (FAILED(SessionExtension->Save(pStm, true))) { return false; }
    DEBUG_INFO("Saved \"%s\" extension\n", SessionExtension->Name());

    DEBUG_INFO("Extension::Save(exit)\n");

    return true;
}


/**
 *  Load all the extension class data from the stream.
 * 
 *  @author: CCHyper
 */
bool Extension::Load(IStream *pStm)
{
    ASSERT(pStm != nullptr);

    if (!pStm) {
        return false;
    }

    DEBUG_INFO("Extension::Load(enter)\n");

    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT) && !Extension_Load<UnitClass, UnitClassExtension>(pStm, UnitExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFT) && !Extension_Load<AircraftClass, AircraftClassExtension>(pStm, AircraftExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE) && !Extension_Load<AircraftTypeClass, AircraftTypeClassExtension>(pStm, AircraftTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIM) && !Extension_Load<AnimClass, AnimClassExtension>(pStm, AnimExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIMTYPE) && !Extension_Load<AnimTypeClass, AnimTypeClassExtension>(pStm, AnimTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDING) && !Extension_Load<BuildingClass, BuildingClassExtension>(pStm, BuildingExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE) && !Extension_Load<BuildingTypeClass, BuildingTypeClassExtension>(pStm, BuildingTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE) && !Extension_Load<BulletTypeClass, BulletTypeClassExtension>(pStm, BulletTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { }                       // <- Do not save!
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE) && !Extension_Load<HouseClass, HouseClassExtension>(pStm, HouseExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_HOUSETYPE) && !Extension_Load<HouseTypeClass, HouseTypeClassExtension>(pStm, HouseTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRY) && !Extension_Load<InfantryClass, InfantryClassExtension>(pStm, InfantryExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE) && !Extension_Load<InfantryTypeClass, InfantryTypeClassExtension>(pStm, InfantryTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { }                    // <- Do not save!
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE) && !Extension_Load<OverlayTypeClass, OverlayTypeClassExtension>(pStm, OverlayTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE) && !Extension_Load<ParticleTypeClass, ParticleTypeClassExtension>(pStm, ParticleTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE) && !Extension_Load<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(pStm, ParticleSystemTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE) && !Extension_Load<SideClass, SideClassExtension>(pStm, SideExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE) && !Extension_Load<SmudgeTypeClass, SmudgeTypeClassExtension>(pStm, SmudgeTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Do not save!
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE) && !Extension_Load<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(pStm, SuperWeaponTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN) && !Extension_Load<TerrainClass, TerrainClassExtension>(pStm, TerrainExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE) && !Extension_Load<TerrainTypeClass, TerrainTypeClassExtension>(pStm, TerrainTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE) && !Extension_Load<UnitTypeClass, UnitTypeClassExtension>(pStm, UnitTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE) && !Extension_Load<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(pStm, VoxelAnimTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WAVE) && !Extension_Load<WaveClass, WaveClassExtension>(pStm, WaveExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM) && !Extension_Load<TiberiumClass, TiberiumClassExtension>(pStm, TiberiumExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE) && !Extension_Load<WeaponTypeClass, WeaponTypeClassExtension>(pStm, WeaponTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE) && !Extension_Load<WarheadTypeClass, WarheadTypeClassExtension>(pStm, WarheadTypeExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Do not save!
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP) && !Extension_Load<Tactical, TacticalExtension>(pStm, TacticalMapExtension)) { return false; }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON) && !Extension_Load<SuperClass, SuperClassExtension>(pStm, SuperExtensions)) { return false; }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    if (FAILED(RuleExtension->Load(pStm))) { return false; }
    DEBUG_INFO("Loaded \"%s\" extension.\n", RuleExtension->Name());
    RuleExtension->Assign_This(Rule);

    if (FAILED(ScenExtension->Load(pStm))) { return false; }
    DEBUG_INFO("Loaded \"%s\" extension.\n", ScenExtension->Name());
    ScenExtension->Assign_This(Scen);

    if (FAILED(SessionExtension->Load(pStm))) { return false; }
    DEBUG_INFO("Loaded \"%s\" extension.\n", SessionExtension->Name());
    SessionExtension->Assign_This(&Session);

    DEBUG_INFO("Extension::Load(exit)\n");

    /**
     *  x
     */
    if (!Extension::Request_Pointer_Remap()) { return false; }

    return true;
}


/**
 *  Request pointer remap on all extension pointers.
 * 
 *  @author: CCHyper
 */
bool Extension::Request_Pointer_Remap()
{
    DEBUG_INFO("Extension::Request_Pointer_Remap(enter)\n");

    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT) && !Extension_Request_Pointer_Remap<UnitClass, UnitClassExtension>(Units)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFT) && !Extension_Request_Pointer_Remap<AircraftClass, AircraftClassExtension>(Aircrafts)) { return false; }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE) && !Extension_Request_Pointer_Remap<AircraftTypeClass, AircraftTypeClassExtension>(AircraftTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIM) && !Extension_Request_Pointer_Remap<AnimClass, AnimClassExtension>(Anims)) { return false; }
    if (Extension::Is_Supported(RTTI_ANIMTYPE) && !Extension_Request_Pointer_Remap<AnimTypeClass, AnimTypeClassExtension>(AnimTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDING) && !Extension_Request_Pointer_Remap<BuildingClass, BuildingClassExtension>(Buildings)) { return false; }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE) && !Extension_Request_Pointer_Remap<BuildingTypeClass, BuildingTypeClassExtension>(BuildingTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE) && !Extension_Request_Pointer_Remap<BulletTypeClass, BulletTypeClassExtension>(BulletTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { }                       // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE) && !Extension_Request_Pointer_Remap<HouseClass, HouseClassExtension>(Houses)) { return false; }
    if (Extension::Is_Supported(RTTI_HOUSETYPE) && !Extension_Request_Pointer_Remap<HouseTypeClass, HouseTypeClassExtension>(HouseTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRY) && !Extension_Request_Pointer_Remap<InfantryClass, InfantryClassExtension>(Infantry)) { return false; }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE) && !Extension_Request_Pointer_Remap<InfantryTypeClass, InfantryTypeClassExtension>(InfantryTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { }                    // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE) && !Extension_Request_Pointer_Remap<OverlayTypeClass, OverlayTypeClassExtension>(OverlayTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE) && !Extension_Request_Pointer_Remap<ParticleTypeClass, ParticleTypeClassExtension>(ParticleTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE) && !Extension_Request_Pointer_Remap<ParticleSystemTypeClass, ParticleSystemTypeClassExtension>(ParticleSystemTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE) && !Extension_Request_Pointer_Remap<SideClass, SideClassExtension>(Sides)) { return false; }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE) && !Extension_Request_Pointer_Remap<SmudgeTypeClass, SmudgeTypeClassExtension>(SmudgeTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE) && !Extension_Request_Pointer_Remap<SuperWeaponTypeClass, SuperWeaponTypeClassExtension>(SuperWeaponTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN) && !Extension_Request_Pointer_Remap<TerrainClass, TerrainClassExtension>(Terrains)) { return false; }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE) && !Extension_Request_Pointer_Remap<TerrainTypeClass, TerrainTypeClassExtension>(TerrainTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE) && !Extension_Request_Pointer_Remap<UnitTypeClass, UnitTypeClassExtension>(UnitTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE) && !Extension_Request_Pointer_Remap<VoxelAnimTypeClass, VoxelAnimTypeClassExtension>(VoxelAnimTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_WAVE) && !Extension_Request_Pointer_Remap<WaveClass, WaveClassExtension>(Waves)) { return false; }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM) && !Extension_Request_Pointer_Remap<TiberiumClass, TiberiumClassExtension>(Tiberiums)) { return false; }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE) && !Extension_Request_Pointer_Remap<WeaponTypeClass, WeaponTypeClassExtension>(WeaponTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE) && !Extension_Request_Pointer_Remap<WarheadTypeClass, WarheadTypeClassExtension>(WarheadTypes)) { return false; }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP) && !Extension_Request_Pointer_Remap<Tactical, TacticalExtension>(TacticalMap)) { return false; }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON) && !Extension_Request_Pointer_Remap<SuperClass, SuperClassExtension>(Supers)) { return false; }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Extension::Register_Class_Factories()
{
    DEBUG_INFO("Extension::Register_Class_Factories(enter)\n");

    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT)) { REGISTER_CLASS(UnitClassExtension); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { REGISTER_CLASS(AircraftClassExtension); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { REGISTER_CLASS(AircraftTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_ANIM)) { REGISTER_CLASS(AnimClassExtension); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { REGISTER_CLASS(AnimTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { REGISTER_CLASS(BuildingClassExtension); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { REGISTER_CLASS(BuildingTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { REGISTER_CLASS(BulletTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { REGISTER_CLASS(CampaignClassExtension); }
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { REGISTER_CLASS(HouseClassExtension); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { REGISTER_CLASS(HouseTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { REGISTER_CLASS(InfantryClassExtension); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { REGISTER_CLASS(InfantryTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { REGISTER_CLASS(IsometricTileTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { REGISTER_CLASS(OverlayTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { REGISTER_CLASS(ParticleTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { REGISTER_CLASS(ParticleSystemTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { REGISTER_CLASS(SideClassExtension); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { REGISTER_CLASS(SmudgeTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { REGISTER_CLASS(SuperWeaponTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { REGISTER_CLASS(TerrainClassExtension); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { REGISTER_CLASS(TerrainTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { REGISTER_CLASS(UnitTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { REGISTER_CLASS(VoxelAnimTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WAVE)) { REGISTER_CLASS(WaveClassExtension); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { REGISTER_CLASS(TiberiumClassExtension); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { REGISTER_CLASS(WeaponTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { REGISTER_CLASS(WarheadTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { REGISTER_CLASS(TacticalExtension); }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { REGISTER_CLASS(SuperClassExtension); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented
    
    DEBUG_INFO("Extension::Register_Class_Factories(exit)\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void Extension::Free_Heaps()
{
    DEBUG_INFO("Extension::Free_Heaps(enter)\n");

    ++ScenarioInit;

    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT)) { UnitExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { AircraftExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { AircraftTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_ANIM)) { AnimExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { AnimTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { BuildingExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { BuildingTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { BulletTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { /* CampaignExtensions.Clear(); */ } // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { HouseExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { HouseTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { InfantryExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { InfantryTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { /* IsometricTileTypeExtensions.Clear(); */ } // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { OverlayTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { ParticleTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { ParticleSystemTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { SideExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { SmudgeTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { SuperWeaponTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { TerrainExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { TerrainTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { UnitTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { VoxelAnimTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_WAVE)) { WaveExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { TiberiumExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { WeaponTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { WarheadTypeExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { }                    // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { SuperExtensions.Clear(); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    --ScenarioInit;

    DEBUG_INFO("Extension::Free_Heaps(exit)\n");
}



/**
 *  Prints a data file for finding Sync Bugs.
 * 
 *  @author: CCHyper
 */
void Extension::Print_CRCs(FILE *fp, EventClass *ev)
{
    DEBUG_INFO("Extension::Print_CRCs(enter)\n");

    if (Extension::Is_Supported(RTTI_INFANTRY)) { Extension_Print_Sync_Data(InfantryExtensions, fp, ev); }
    if (Extension::Is_Supported(RTTI_UNIT)) { Extension_Print_Sync_Data(UnitExtensions, fp, ev); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { Extension_Print_Sync_Data(BuildingExtensions, fp, ev); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { Extension_Print_Sync_Data(AircraftExtensions, fp, ev); }

    if (Extension::Is_Supported(RTTI_UNIT)) { Extension_Print_CRCs(UnitExtensions, fp); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { Extension_Print_CRCs(AircraftExtensions, fp); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { Extension_Print_CRCs(AircraftTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_ANIM)) { Extension_Print_CRCs(AnimExtensions, fp); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { Extension_Print_CRCs(AnimTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { Extension_Print_CRCs(BuildingExtensions, fp); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { Extension_Print_CRCs(BuildingTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { Extension_Print_CRCs(BulletTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { }                       // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { Extension_Print_CRCs(HouseExtensions, fp); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { Extension_Print_CRCs(HouseTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { Extension_Print_CRCs(InfantryExtensions, fp); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { Extension_Print_CRCs(InfantryTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { Extension_Print_CRCs(IsometricTileTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { Extension_Print_CRCs(OverlayTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { Extension_Print_CRCs(ParticleTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { Extension_Print_CRCs(ParticleSystemTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { Extension_Print_CRCs(SideExtensions, fp); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { Extension_Print_CRCs(SmudgeTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { Extension_Print_CRCs(SuperWeaponTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { Extension_Print_CRCs(TerrainExtensions, fp); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { Extension_Print_CRCs(TerrainTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { Extension_Print_CRCs(UnitTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { Extension_Print_CRCs(VoxelAnimTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_WAVE)) { Extension_Print_CRCs(WaveExtensions, fp); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { Extension_Print_CRCs(TiberiumExtensions, fp); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { Extension_Print_CRCs(WeaponTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { Extension_Print_CRCs(WarheadTypeExtensions, fp); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { }                    // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { Extension_Print_CRCs(SuperExtensions, fp); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    DEBUG_INFO("Extension::Print_CRCs(exit)\n");
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void Extension::Detach_This_From_All(TARGET target, bool all)
{
    //DEBUG_INFO("Extension::Detach_This_From_All(enter)\n");

    /**
     *  #NOTE: The order of these calls must match the relevent RTTIType order!
     */
    if (Extension::Is_Supported(RTTI_UNIT)) { Extension_Detach_This_From_All(UnitExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { Extension_Detach_This_From_All(AircraftExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { Extension_Detach_This_From_All(AircraftTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_ANIM)) { Extension_Detach_This_From_All(AnimExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { Extension_Detach_This_From_All(AnimTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { Extension_Detach_This_From_All(BuildingExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { Extension_Detach_This_From_All(BuildingTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { Extension_Detach_This_From_All(BulletTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { }                       // Does not need to be processed.
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { Extension_Detach_This_From_All(HouseExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { Extension_Detach_This_From_All(HouseTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { Extension_Detach_This_From_All(InfantryExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { Extension_Detach_This_From_All(InfantryTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { Extension_Detach_This_From_All(IsometricTileTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { Extension_Detach_This_From_All(OverlayTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { Extension_Detach_This_From_All(ParticleTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { Extension_Detach_This_From_All(ParticleSystemTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { Extension_Detach_This_From_All(SideExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { Extension_Detach_This_From_All(SmudgeTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { Extension_Detach_This_From_All(SuperWeaponTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { Extension_Detach_This_From_All(TerrainExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { Extension_Detach_This_From_All(TerrainTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { Extension_Detach_This_From_All(UnitTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { Extension_Detach_This_From_All(VoxelAnimTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_WAVE)) { Extension_Detach_This_From_All(WaveExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { Extension_Detach_This_From_All(TiberiumExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { Extension_Detach_This_From_All(WeaponTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { Extension_Detach_This_From_All(WarheadTypeExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { TacticalMapExtension->Detach(target, all); }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { Extension_Detach_This_From_All(SuperExtensions, target, all); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    /*if (Extension::Singleton::Is_Supported())*/ { RuleExtension->Detach(target, all); }
    /*if (Extension::Singleton::Is_Supported())*/ { ScenExtension->Detach(target, all); }
    /*if (Extension::Singleton::Is_Supported())*/ { SessionExtension->Detach(target, all); }

    //DEBUG_INFO("Extension::Detach_This_From_All(exit)\n");
}


/**
 *  Calculate the save game version.
 * 
 *  @author: CCHyper
 */
unsigned Extension::Get_Save_Version_Number()
{
    unsigned version = 10000;

    /**
     *  Game classes.
     */
    if (Extension::Is_Supported(RTTI_UNIT)) { version += sizeof(UnitClassExtension); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { version += sizeof(AircraftClassExtension); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { version += sizeof(AircraftTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_ANIM)) { version += sizeof(AnimClassExtension); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { version += sizeof(AnimTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { version += sizeof(BuildingClassExtension); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { version += sizeof(BuildingTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_BULLET)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { version += sizeof(BulletTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { version += sizeof(CampaignClassExtension); }
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { version += sizeof(HouseClassExtension); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { version += sizeof(HouseTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { version += sizeof(InfantryClassExtension); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { version += sizeof(InfantryTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { version += sizeof(IsometricTileTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { version += sizeof(OverlayTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { version += sizeof(ParticleTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { }                 // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { version += sizeof(ParticleSystemTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { version += sizeof(SideClassExtension); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { version += sizeof(SmudgeTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { version += sizeof(SuperWeaponTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { version += sizeof(TerrainClassExtension); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { version += sizeof(TerrainTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { version += sizeof(UnitTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { version += sizeof(VoxelAnimTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WAVE)) { version += sizeof(WaveClassExtension); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { version += sizeof(TiberiumClassExtension); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { version += sizeof(WeaponTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { version += sizeof(WarheadTypeClassExtension); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Special case, does not actually "exist".
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { version += sizeof(TacticalExtension); }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { version += sizeof(SuperClassExtension); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    /**
    *  Global classes.
    */
    version += sizeof(RulesClassExtension);
    version += sizeof(ScenarioClassExtension);
    version += sizeof(SessionClassExtension);

    return version;
}
