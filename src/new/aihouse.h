/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIHOUSE.H
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
#include "iaihouse.h"
#include "idescription.h"
#include <unknwn.h>


/**
 *  Each of the AI personalities are named after
 *  real Generals from the America Civil War;
 *  
 *  "General Meade"
 *  "General James S. Jackson" or "John K. Jackson"
 *  "General Grant"
 *  "General Hooker"
 */


/**
 *  Brain dead general - No behavior defined.
 */
class __declspec(uuid("F706E6E0-86DA-11D1-B706-00A024DDAFD1"))
AIHouse : public IAIHouse, /*public IPersistStream,*/ public IDescription
{
    public:
        // TODO: Check this, but it seems these classes don't use IPersistStream...
        /*
        // IPersist.
        virtual HRESULT __stdcall GetClassID(CLSID *lpClassID) override;

        // IPersistStream.
        virtual LONG __stdcall IsDirty() override;
        virtual HRESULT __stdcall Load(IStream *pStm) override;
        virtual HRESULT __stdcall Save(IStream *pStm, BOOL fClearDirty) override;
        virtual LONG __stdcall GetSizeMax(ULARGE_INTEGER *pcbSize) override; // Retrieves the size of the stream needed to save the object.
        */

        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        IFACEMETHOD_(LONG, Link_House)(IHouse *house) PURE;
        IFACEMETHOD_(LONG, AI)(long *framedelay) PURE;

        IFACEMETHOD_(BSTR, Short_Name)() { return L"Brain dead general"; }
        IFACEMETHOD_(BSTR, Description)() { return L"No behavior defined"; }

    public:
        AIHouse() {}
        virtual ~AIHouse() {}
};


/**
 *  General Meade - Strong defense, weak attack.
 */
// General Meade - Strong defense, weak attack.
class __declspec(uuid("9E0F6120-87C1-11D1-B707-00A024DDAFD1"))
AIMeade : public IAIHouse, /*public IPersistStream,*/ public IDescription
{
    public:
        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        IFACEMETHOD_(LONG, Link_House)(IHouse *house) PURE;
        IFACEMETHOD_(LONG, AI)(long *framedelay) PURE;

        IFACEMETHOD_(BSTR, Short_Name)() { return L"General Meade"; }
        IFACEMETHOD_(BSTR, Description)() { return L"Strong defense, weak attack"; }

    public:
        AIMeade() {}
        virtual ~AIMeade() {}

    private:
        IHouse *House;
};


/**
 *  General Jackson - Strong defense, strong attack.
 */
class __declspec(uuid("C6004D80-87D1-11D1-B707-00A024DDAFD1"))
AIJackson : public IAIHouse, /*public IPersistStream,*/ public IDescription
{
    public:
        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        IFACEMETHOD_(LONG, Link_House)(IHouse *house) PURE;
        IFACEMETHOD_(LONG, AI)(long *framedelay) PURE;

        IFACEMETHOD_(BSTR, Short_Name)() { return L"General Jackson"; }
        IFACEMETHOD_(BSTR, Description)() { return L"Strong defense, strong attack"; }

    public:
        AIJackson() {}
        virtual ~AIJackson() {}

    private:
        IHouse *House;
};


/**
 *  General Grant - Weak defense, frequent attack.
 */
class __declspec(uuid("FBE6D4A0-87D1-11D1-B707-00A024DDAFD1"))
AIGrant : public IAIHouse, /*public IPersistStream,*/ public IDescription
{
    public:
        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        IFACEMETHOD_(LONG, Link_House)(IHouse *house) PURE;
        IFACEMETHOD_(LONG, AI)(long *framedelay) PURE;

        IFACEMETHOD_(BSTR, Short_Name)() { return L"General Grant"; }
        IFACEMETHOD_(BSTR, Description)() { return L"Weak defense, frequent attack"; }

    public:
        AIGrant() {}
        virtual ~AIGrant() {}

    private:
        IHouse *House;
};


/**
 *  General Hooker - Unpredictable.
 */
class __declspec(uuid("FBE6D4A1-87D1-11D1-B707-00A024DDAFD1"))
AIHooker : public IAIHouse, /*public IPersistStream,*/ public IDescription
{
    public:
        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        IFACEMETHOD_(LONG, Link_House)(IHouse *house) PURE;
        IFACEMETHOD_(LONG, AI)(long *framedelay) PURE;

        IFACEMETHOD_(BSTR, Short_Name)() { return L"General Hooker"; }
        IFACEMETHOD_(BSTR, Description)() { return L"Unpredictable"; }

    public:
        AIHooker() {}
        virtual ~AIHooker() {}

    private:
        IHouse *House;
};
