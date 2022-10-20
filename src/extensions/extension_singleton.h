/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXTENSION_SINGLETON.H
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
#pragma once

#include "always.h"
#include "tibsun_defines.h"
#include "vinifera_defines.h"
#include "vinifera_saveload.h"
#include "wstring.h"
#include "swizzle.h"
#include "noinit.h"
#include "asserthandler.h"

#include <objidl.h> // for IStream.
#include <typeinfo>


class WWCRCEngine;


/**
 *  x
 */
template<class T>
class ExtensionSingleton
{
    public:
        STDMETHOD(Load)(IStream *pStm);
        STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);

    public:
        ExtensionSingleton(const T *this_ptr = nullptr);
        ExtensionSingleton(const NoInitClass &noinit);
        virtual ~ExtensionSingleton();

        /**
         *  Return the raw size of class data for save/load purposes.
         *  
         *  @note: This must be overridden by the extended class!
         */
        virtual int Size_Of() const = 0;

        /**
         *  Removes the specified target from any targeting and reference trackers.
         *  
         *  @note: This must be overridden by the extended class!
         */
        virtual void Detach(TARGET target, bool all = true) = 0;

        /**
         *  Compute a unique crc value for this instance.
         *  
         *  @note: This must be overridden by the extended class!
         */
        virtual void Compute_CRC(WWCRCEngine &crc) const = 0;

        /**
         *  Access to the class instance we extend.
         */
        virtual T *This() const { return const_cast<T *>(ThisPtr); }
        virtual const T *This_Const() const { return ThisPtr; }

        /**
         *  x
         */
        virtual void Assign_This(const T *this_ptr) { ASSERT(this_ptr != nullptr); ThisPtr = this_ptr; }

        /**
         *  x
         */
        virtual const char *Name() const = 0;

        /**
         *  x
         */
        virtual const char *Full_Name() const = 0;

    private:
        /**
         *  Pointer to the class we are extending. This provides us with a way of
         *  quickly referencing the base class without doing a look-up each time.
         */
        const T *ThisPtr;

    private:
        ExtensionSingleton(const ExtensionSingleton &) = delete;
        void operator = (const ExtensionSingleton &) = delete;

    public:
};


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
ExtensionSingleton<T>::ExtensionSingleton(const T *this_ptr) :
    ThisPtr(this_ptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("ExtensionSingleton<%s>::ExtensionSingleton - 0x%08X\n", typeid(T).name(), (uintptr_t)(ThisPtr));
    //ASSERT(ThisPtr != nullptr);      // NULL ThisPtr is valid when performing a Load state operation.
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
ExtensionSingleton<T>::ExtensionSingleton(const NoInitClass &noinit)
{
    //EXT_DEBUG_TRACE("ExtensionSingleton<%s>::ExtensionSingleton(NoInitClass) - 0x%08X\n", typeid(T).name(), (uintptr_t)(ThisPtr));
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
ExtensionSingleton<T>::~ExtensionSingleton()
{
    //EXT_DEBUG_TRACE("ExtensionSingleton<%s>::~ExtensionSingleton - 0x%08X\n", typeid(T).name(), (uintptr_t)(ThisPtr));

    ThisPtr = nullptr;
}


/**
 *  Loads the object from the stream and requests a new pointer to
 *  the class we extended post-load.
 * 
 *  As singleton is static data, we do not need to request
 *  pointer remap of "ThisPtr" after loading has finished.
 * 
 *  @author: CCHyper, tomsons26
 */
template<class T>
HRESULT ExtensionSingleton<T>::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("ExtensionSingleton<%s>::Load - 0x%08X\n", typeid(T).name(), (uintptr_t)(ThisPtr));

    if (!pStm) {
        return E_POINTER;
    }

    HRESULT hr;

#if 0
    /**
     *  Load the unique id for this class.
     */
    LONG id = 0;
    hr = pStm->Read(&id, sizeof(LONG), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    //DEV_DEBUG_INFO("Read id = 0x%08X.\n", id);

    Wstring class_name = Name();
    Wstring this_name = Wstring("this") + ":" + class_name;
    Wstring thisptr_name = Wstring("ThisPtr") + ":" + class_name;

    /**
     *  x
     */
    VINIFERA_SWIZZLE_REGISTER_POINTER(id, this, this_name.Peek_Buffer());

    //DEV_DEBUG_INFO("Registering pointer id = 0x%08X, this = 0x%08X.\n", id, (uintptr_t)(this));
#endif

    /**
     *  Read this classes binary blob data directly into this instance.
     */
    hr = pStm->Read(this, Size_Of(), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    //DEV_DEBUG_INFO("Read Size_Of = %d.\n", sizeof(*this));
    
#if 0
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(ThisPtr, thisptr_name.Peek_Buffer());

    //DEV_DEBUG_INFO("Requested remap of ThisPtr.\n");
#endif

    return hr;
}


/**
 *  Saves the object to the stream.
 * 
 *  @author: CCHyper, tomsons26
 */
template<class T>
HRESULT ExtensionSingleton<T>::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("ExtensionSingleton<%s>::Save - 0x%08X\n", typeid(T).name(), (uintptr_t)(ThisPtr));

    if (!pStm) {
        return E_POINTER;
    }

    HRESULT hr;

#if 0
    Wstring class_name = Name();
    Wstring this_name = Wstring("this") + ":" + class_name;

    /**
     *  x
     */
    LONG id;
    VINIFERA_SWIZZLE_FETCH_SWIZZLE_ID(this, id, this_name.Peek_Buffer());

    //DEV_DEBUG_INFO("Writing id = 0x%08X.\n", id);

    hr = pStm->Write(&id, sizeof(id), nullptr);
    if (FAILED(hr)) {
        return hr;
    }
#endif

    //DEV_DEBUG_INFO("Writing Size_Of = %d.\n", Size_Of());
    
    /**
     *  Write this class instance as a binary blob.
     */
    hr = pStm->Write(this, Size_Of(), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}
