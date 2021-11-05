/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWSWIZZLE.H
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Replacement pointer swizzling interface for debugging save load issues.
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
#include "iswizzle.h"
#include "vector.h"
#include "tibsun_defines.h"
#include "vinifera_defines.h"
#include <cstdio>


/**
 *  Wrappers for the new swizzle manager for providing debug information.
 */
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER

#define VINIFERA_SWIZZLE_RESET(func) \
        { \
            static char buffer[256]; \
            std::snprintf(buffer, sizeof(buffer), "SwizzleManager::Reset() - %s.\n", func); \
            DEBUG_INFO(buffer); \
            ViniferaSwizzleManager.Reset(); \
        }

#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(pointer, var)                                       ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Swizzle_Dbg((void **)pointer, __FILE__, __LINE__, __FUNCTION__ "()", var);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_VECTOR(pointer, var, vector)                        for (int i = 0; i < vector.Count(); ++i) { ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Swizzle_Dbg((void **)&vector[i], __FILE__, __LINE__, __FUNCTION__ "()", var); }
#define VINIFERA_SWIZZLE_FETCH_POINTER_ID(pointer, id, var)                                        ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Fetch_Swizzle_ID_Dbg((void *)pointer, id, __FILE__, __LINE__, __FUNCTION__ "()", var);
#define VINIFERA_SWIZZLE_HERE_I_AM(id, pointer, var)                                               ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Here_I_Am_Dbg(id, (void *)pointer, __FILE__, __LINE__, __FUNCTION__ "()", var);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, file, line, func, var)                 ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Swizzle_Dbg((void **)pointer, file, line, func, var);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_VECTOR_DBG(pointer, file, line, func, var, vector)  for (int i = 0; i < vector.Count(); ++i) { ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Swizzle_Dbg((void **)&vector[i], file, line, func, var);  }
#define VINIFERA_SWIZZLE_FETCH_POINTER_ID_DBG(pointer, id, file, line, func, var)                  ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Fetch_Swizzle_ID_Dbg((void *)pointer, id, file, line, func, var);
#define VINIFERA_SWIZZLE_HERE_I_AM_DBG(id, pointer, file, line, func, var)                         ViniferaSwizzleManager.ViniferaSwizzleManagerClass::Here_I_Am_Dbg(id, (void *)pointer, file, line, func, var);

#else

#define VINIFERA_SWIZZLE_RESET(func) \
        { \
            static char buffer[256]; \
            std::snprintf(buffer, sizeof(buffer), "SwizzleManager::Reset() - %s.\n", func); \
            DEBUG_INFO(buffer); \
            SwizzleManager.Reset(); \
        }

#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(pointer, var)                                       SwizzleManager.SwizzleManagerClass::Swizzle((void **)pointer);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_VECTOR(pointer, var, vector)                        for (int i = 0; i < vector.Count(); ++i) { SwizzleManager.SwizzleManagerClass::Swizzle((void **)&vector[i]); }
#define VINIFERA_SWIZZLE_FETCH_POINTER_ID(pointer, id, var)                                        SwizzleManager.SwizzleManagerClass::Fetch_Swizzle_ID((void *)pointer, id);
#define VINIFERA_SWIZZLE_HERE_I_AM(id, pointer, var)                                               SwizzleManager.SwizzleManagerClass::Here_I_Am(id, (void *)pointer);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, file, line, func, var)                 SwizzleManager.SwizzleManagerClass::Swizzle((void **)pointer);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_VECTOR_DBG(pointer, file, line, func, var, vector)  for (int i = 0; i < vector.Count(); ++i) { SwizzleManager.SwizzleManagerClass::Swizzle((void **)&vector[i]); }
#define VINIFERA_SWIZZLE_FETCH_POINTER_ID_DBG(pointer, id, file, line, func, var)                  SwizzleManager.SwizzleManagerClass::Fetch_Swizzle_ID((void *)pointer, id);
#define VINIFERA_SWIZZLE_HERE_I_AM_DBG(id, pointer, file, line, func, var)                         SwizzleManager.SwizzleManagerClass::Here_I_Am(id, (void *)pointer);

#endif


#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER

/**
 *  Reimplementation of SwizzleManagerClass.
 * 
 *  #WARNING: Do not add any additional members to the class, we much match
 *            expected class size otherwise everything will break!
 */
class ViniferaSwizzleManagerClass : public ISwizzle
{
    private:
        struct SwizzlePointerStruct
        {
            SwizzlePointerStruct() :
                ID(0), Pointer(nullptr), File(nullptr), Line(-1), Function(nullptr), Variable(nullptr)
            {
            }

            SwizzlePointerStruct(LONG id, void *pointer) :
                ID(id), Pointer(pointer), File(nullptr), Line(-1), Function(nullptr), Variable(nullptr)
            {}

            SwizzlePointerStruct(LONG id, void *pointer, const char *file, const int line, const char *func, const char *var) :
                ID(id), Pointer(pointer), File(file), Line(line), Function(func), Variable(var)
            {}

            ~SwizzlePointerStruct() {}

            void operator=(const SwizzlePointerStruct &that)
            {
                ID = that.ID;
                Pointer = that.Pointer;
                File = that.File;
                Line = that.Line;
                Function = that.Function;
                Variable = that.Variable;
            }

            bool operator==(const SwizzlePointerStruct &that) const { return ID == that.ID; }
            bool operator!=(const SwizzlePointerStruct &that) const { return ID != that.ID; }
            bool operator<(const SwizzlePointerStruct &that) const { return ID < that.ID; }
            bool operator>(const SwizzlePointerStruct &that) const { return ID > that.ID; }

            /**
             *  The id of the pointer to remap.
             */
            LONG ID;

            /**
             *  The pointer to fixup.
             */
            void *Pointer;
            
            /**
             *  Debugging information.
             */
            const char *File;
            /*const*/ int Line;
            const char *Function;
            const char *Variable;
        };

    public:
        /**
         *  IUnknown
         */
        STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj) override;
        STDMETHOD_(ULONG, AddRef)() override;
        STDMETHOD_(ULONG, Release)() override;

        /**
         *  ISwizzle
         */
        STDMETHOD_(LONG, Reset)() override;
        STDMETHOD_(LONG, Swizzle)(void **pointer) override;
        STDMETHOD_(LONG, Fetch_Swizzle_ID)(void *pointer, LONG *id) override;
        STDMETHOD_(LONG, Here_I_Am)(LONG id, void *pointer) override;
        STDMETHOD(Save_Interface)(IStream *stream, IUnknown *pointer) override;
        STDMETHOD(Load_Interface)(IStream *stream, CLSID *riid, void **pointer) override;
        STDMETHOD_(LONG, Get_Save_Size)(LONG *size) override;

        /**
         *  New debug routines.
         */
        STDMETHOD_(LONG, Swizzle_Dbg)(void **pointer, const char *file, const int line, const char *func = nullptr, const char *var = nullptr);
        STDMETHOD_(LONG, Fetch_Swizzle_ID_Dbg)(void *pointer, LONG *id, const char *file, const int line, const char *func = nullptr, const char *var = nullptr);
        STDMETHOD_(LONG, Here_I_Am_Dbg)(LONG id, void *pointer, const char *file, const int line, const char *func = nullptr, const char *var = nullptr);

    public:
        ViniferaSwizzleManagerClass();
        ~ViniferaSwizzleManagerClass();

    private:
        void Sort_Tables();
        void Process_Tables();

    private:
        /**
         *  List of all the pointers that need remapping.
         */
        DynamicVectorClass<SwizzlePointerStruct> RequestTable;

        /**
         *  List of all the new pointers.
         */
        DynamicVectorClass<SwizzlePointerStruct> PointerTable;

    private:
        friend static int __cdecl ptr_compare_function(const void *ptr1, const void *ptr2);
};

extern ViniferaSwizzleManagerClass ViniferaSwizzleManager;

#endif
