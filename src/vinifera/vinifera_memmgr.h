/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_MEMMGR.H
 *
 *  @author        Peter Dalton, Game Programming Gems 2, 2001.
 *
 *  @brief         This Memory Manager software provides the following functionality
 *                   1. Seamless interface.
 *                   2. Tracking all memory allocations and deallocations.
 *                   3. Reporting memory leaks, unallocated memory.
 *                   4. Reporting memory bounds violations.
 *                   5. Reporting the percentage of allocated memory that is actually being used.
 *                   6. Customizable tracking.
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
 *  @note          This source file is heavily modified from the original file.
 *
 ******************************************************************************/
#pragma once


// Enable the memory manager
#define ACTIVATE_MEMORY_MANAGER

// Use the Windows Heap API for allocation over standard C functions.
#define USE_WIN_HEAPAPI


/**
 *  Make sure that the new/delete are not declared to avoid circular definitions.
 */
#include "vinifera_new_off.h"


/**
 *  Only activate the memory manager if the flag has been defined. This allows for the
 *  performance hit to be avoided if desired.
 */
#ifdef ACTIVATE_MEMORY_MANAGER


/*******************************************************************************************/
// ***** User interface, these methods can be used to set parameters within the Memory 
// ***** Manager to control the type and extent of the memory tests that are performed.  Note 
// ***** that it is not necessary to call any of these methods, you will get the default 
// ***** Memory Manager automatically.
namespace ViniferaMemoryManager
{

/**
 *  Dump the log report to the file, this is the same method that is automatically called
 *  upon the programs termination to report all statistical information.
 */
void __cdecl Dump_Log_Report();          
 
/**
 *  Report all allocated memory to the log file.
 */
void __cdecl Dump_Memory_Allocations();  
 
/**
 *  Allows for the log file to be changed from the default.
 */
void __cdecl Set_Log_File(const char * file); 
 
/**
 *  This method allows for exhaustive testing. It has the same functionality as the following
 *  function calls => Set_Log_Always(true); Set_Padding_Size(1024); 
 */
void __cdecl Set_Exhaustive_Testing(bool test = true);
 
/**
 *  Sets the flag for exhaustive information logging. All information is sent to the log file.
 */
void __cdecl Set_Log_Always(bool log = true);
 
/**
 *  Sets the padding size for memory bounds checks.
 */
void __cdecl Set_Padding_Size(int size = 4);
 
/**
 *  Cleans out the log file by deleting it.
 */
void __cdecl Clean_Log_File(bool clean = true);
 
/**
 *  Allows you to set a break point on the n-th allocation.
 */
void __cdecl Break_On_Allocation(int allocation_count);
 
/**
 *  Sets a flag that will set a break point when the specified memory is deallocated.
 */
void __cdecl Break_On_Deallocation(void *address); 

/**
 *  Sets a flag that will set a break point when the specified memory is reallocated by
 *  using the realloc() method.
 */
void __cdecl Break_On_Reallocation(void * address);

/**
 *  Possible allocation/deallocation types. 
 */
typedef enum MemAllocType : unsigned char // Declared as char to minimize memory footprint.
{
    MM_UNKNOWN      = 0,
    MM_NEW          = 1,
    MM_NEW_ARRAY    = 2,
    MM_MALLOC       = 3,
    MM_CALLOC       = 4,
    MM_REALLOC      = 5,
    MM_DELETE       = 6,
    MM_DELETE_ARRAY = 7,
    MM_FREE         = 8
} MemAllocType;

/**
 *  This is the main memory allocation routine, this is called by all of the other
 *  memory allocation routines to allocate and track memory.
 */
void * __cdecl Allocate_Memory(const char * file, const char * function, int line, std::size_t size, MemAllocType type, void *address = NULL); 

/**
 *  This is the main memory deallocation routine. This method is used by all of the
 *  other de-allocation routines for de-allocating and tracking memory.
 */
void __cdecl Deallocate_Memory(void * address, MemAllocType type);

/**
 *  This method is used by the deallocation methods to record the source file and line
 *  number that is requesting the allocation.  Note that it is important to create a
 *  seperate method for deallocation since we can not pass the addition parameters to
 */
void __cdecl Set_Owner(const char * file, const char * function, int line);

} // ViniferaMemoryManager namespace end


/**
 *  Here we define a static class that will be responsible for initializing the Memory
 *  Manager.  It is critical that it is placed here within the header file to ensure
 *  that this static object will be created before any other static objects are 
 *  intialized.  This will ensure that the Memory Manager is alive when other static
 *  objects allocate and deallocate memory.  Note that static objects are deallocated
 *  in the reverse order in which they are allocated, thus this class will be 
 *  deallocated last.
 */
class ViniferaMemoryManagerInitialize { public: ViniferaMemoryManagerInitialize(); };
static ViniferaMemoryManagerInitialize InitMemoryManager;


namespace ViniferaMemory
{
    void * __cdecl Alloc(unsigned int size);
    void * __cdecl CountAlloc(unsigned int count, unsigned int size);
    void * __cdecl Realloc(void *ptr, unsigned int size);
    void __cdecl Free(void *ptr);

    unsigned int __cdecl MemSize(void *ptr);
} // ViniferaMemory namespace end


/**
 *  These #defines are the core of the memory manager.  This overrides standard memory 
 *  allocation and de-allocation routines and replaces them with the memory manager's versions.
 *  This allows for memory tracking and statistics to be generated.  These #defines are 
 *  included in the new_on.h header so that they are listed only once.
 */
#include "vinifera_new_on.h"


#endif /* ACTIVATE_MEMORY_MANAGER */
