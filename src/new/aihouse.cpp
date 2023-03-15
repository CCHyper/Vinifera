/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIHOUSE.CPP
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
#include "aihouse.h"
#include "house.h"
#include "debughandler.h"


HouseClass *House_This = nullptr;


HRESULT AIHouse::QueryInterface(const CLSID &riid, void **ppvObject)
{
    /**
     *  Always set out parameter to NULL, validating it first.
     */
    if (ppvObject == nullptr) {
        return E_POINTER;
    }
    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown)) {
        *ppvObject = reinterpret_cast<IUnknown *>(this);
    } 

    if (riid == __uuidof(IAIHouse)) {
        *ppvObject = reinterpret_cast<IAIHouse *>(this);
    } 

    if (riid == __uuidof(IDescription)) {
        *ppvObject = static_cast<IDescription *>(this);
    }

    if (*ppvObject == nullptr) {
        return E_NOINTERFACE;
    }

    /**
     *  Increment the reference count and return the pointer.
     */
    reinterpret_cast<IUnknown *>(*ppvObject)->AddRef();

    return S_OK;
}


ULONG AIHouse::AddRef()
{
    return S_OK;
}


ULONG AIHouse::Release()
{
    return S_OK;
}


// Retrieves the class identifier (CLSID) of the object.
/*HRESULT AIHouse::GetClassID(CLSID *lpClassID)
{
    DEBUG_INFO("Enter");

    if (lpClassID != nullptr) {
        *lpClassID = __uuidof(this);
        return S_OK;
    }
    return E_POINTER;
}*/



HRESULT AIHouse::AI(long *framedelay)
{
    int frame_delay = 0;
    if (framedelay != nullptr) {
        frame_delay = *framedelay;
    }

    return S_OK;
}



HRESULT AIMeade::QueryInterface(const CLSID &riid, void **ppvObject)
{
    return S_OK;
}


ULONG AIMeade::AddRef()
{
    return S_OK;
}


ULONG AIMeade::Release()
{
    return S_OK;
}


#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "unittype.h"
#include "wwmouse.h"
#include "tactical.h"
#include "super.h"
HRESULT AIMeade::AI(long *framedelay)
{
    //DEBUG_INFO("AIMeade::AI(enter)\n");

    int frame_delay = 0;
    if (framedelay != nullptr) {
        frame_delay = *framedelay;
    }

    //if (House == nullptr) {
    //    return S_OK;
    //}

    if (House_This == PlayerPtr) {

        // give credits every x frames.
        if ((Frame % 1000) == 0) {
            PlayerPtr->Credits += 1000;
            DEBUG_INFO("Frame %d, gave player %d credits\n", Frame, 1000);
        }

        // give credits every x frames.
        if (Frame >= 500) {
            PlayerPtr->SuperWeapon[SPECIAL_DROP_PODS]->Forced_Charge(true);
            PlayerPtr->SuperWeapon[SPECIAL_DROP_PODS]->Enable(true, true, false);
            DEBUG_INFO("Frame %d, gave player %d credits\n", Frame, 1000);
        }

        // grant a the player an mcv if it lost its construction yard
        //if (PlayerPtr->ConstructionYards.Count() <= 0) {
        //
        //    Coordinate mouse_coord(-1, -1, -1);
        //
        //    /**
        //     *  Fetch the mouse position.
        //     */
        //    Point2D mouse = WWMouse->Get_Mouse_XY();
        //
        //    /**
        //     *  If mouse position is valid, convert to world coordinates and return.
        //     */
        //    if (mouse.Is_Valid()) {
        //        mouse_coord = Cell_Coord(TacticalMap->Click_Cell_Calc(mouse));
        //    }
        //
        //    const UnitTypeClass *uptr = UnitTypeClass::As_Pointer("MCV");
        //    uptr->Create_And_Place(Coord_Cell(mouse_coord), PlayerPtr);
        //}
    }

    //DEBUG_INFO(
    //    "Name: %S, ID: %d\n"
    //    "IsHuman: %d\n"
    //    "framedelay: %d\n"
    //    "AvilMoney: %d, AvilStorage: %d\n"
    //    "PowerOutput: %d, PowerDrain: %d\n",
    //    House->Name(), House->ID_Number(),
    //    House_This->IsHuman,
    //    frame_delay,
    //    House->Available_Money(), House->Available_Storage(),
    //    House->Power_Output(), House->Power_Drain()
    //);

    ///**
    // *  x
    // */
    //HouseClass *lpPS = nullptr;
    //HRESULT hr = House->QueryInterface(__uuidof(HouseClass), (LPVOID*)&lpPS);
    //if (FAILED(hr)) {
    //    DEBUG_ERROR("IHouse does not support HouseClass!\n");
    //    return E_FAIL;
    //}

    //DEBUG_INFO("AIMeade::AI(exit)\n");

    return S_OK;
}


HRESULT AIJackson::QueryInterface(const CLSID &riid, void **ppvObject)
{
    return S_OK;
}


ULONG AIJackson::AddRef()
{
    return S_OK;
}


ULONG AIJackson::Release()
{
    return S_OK;
}


HRESULT AIJackson::AI(long *framedelay)
{
    DEBUG_INFO("AIJackson::AI(enter)\n");

    int frame_delay = 0;
    if (framedelay != nullptr) {
        frame_delay = *framedelay;
    }

    if (House == nullptr) {
        return S_OK;
    }

    DEBUG_INFO(
        "Name: %S, ID: %d\n"
        "framedelay: %d\n"
        "AvilMoney: %d, AvilStorage: %d\n"
        "PowerOutput: %d, PowerDrain: %d\n",
        House->Name(), House->ID_Number(),
        frame_delay,
        House->Available_Money(), House->Available_Storage(),
        House->Power_Output(), House->Power_Drain()
    );

    DEBUG_INFO("AIJackson::AI(exit)\n");

    return S_OK;
}


HRESULT AIGrant::QueryInterface(const CLSID &riid, void **ppvObject)
{
    return S_OK;
}


ULONG AIGrant::AddRef()
{
    return S_OK;
}


ULONG AIGrant::Release()
{
    return S_OK;
}


HRESULT AIGrant::AI(long *framedelay)
{
    DEBUG_INFO("AIJackson::AI(enter)\n");

    int frame_delay = 0;
    if (framedelay != nullptr) {
        frame_delay = *framedelay;
    }

    if (House == nullptr) {
        return S_OK;
    }

    DEBUG_INFO(
        "Name: %S, ID: %d\n"
        "framedelay: %d\n"
        "AvilMoney: %d, AvilStorage: %d\n"
        "PowerOutput: %d, PowerDrain: %d\n",
        House->Name(), House->ID_Number(),
        frame_delay,
        House->Available_Money(), House->Available_Storage(),
        House->Power_Output(), House->Power_Drain()
    );

    DEBUG_INFO("AIJackson::AI(exit)\n");

    return S_OK;
}


HRESULT AIHooker::QueryInterface(const CLSID &riid, void **ppvObject)
{
    return S_OK;
}


ULONG AIHooker::AddRef()
{
    return S_OK;
}


ULONG AIHooker::Release()
{
    return S_OK;
}


LONG AIHooker::AI(long *framedelay)
{
    DEBUG_INFO("AIHooker::AI(enter)\n");

    int frame_delay = 0;
    if (framedelay != nullptr) {
        frame_delay = *framedelay;
    }

    if (House == nullptr) {
        return S_OK;
    }

    DEBUG_INFO(
        "Name: %S, ID: %d\n"
        "framedelay: %d\n"
        "AvilMoney: %d, AvilStorage: %d\n"
        "PowerOutput: %d, PowerDrain: %d\n",
        House->Name(), House->ID_Number(),
        frame_delay,
        House->Available_Money(), House->Available_Storage(),
        House->Power_Output(), House->Power_Drain()
    );

    DEBUG_INFO("AIHooker::AI(exit)\n");

    return S_OK;
}
