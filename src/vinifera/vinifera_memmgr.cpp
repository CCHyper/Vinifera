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


/**
 *  It is important to note here the order in which the files are included to avoid
 *  syntax and linking errors. First you should include all of the required standard
 *  header files followed by the vinifera_memmgr.h header. All other custom file should
 *  be included after the vinifera_memmgr.h header. 
 */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctime>
#include <new>              // Required for malloc() and free()
 
#include "debughandler.h"
#include "asserthandler.h"

#include "vinifera_memmgr.h"


/**
 *  Only execute if the memory manager has been enabled.
 */
#ifdef ACTIVATE_MEMORY_MANAGER


/*******************************************************************************************/
// Turn off the defined macros to avoid confusion.  We want to avoid circular definition, 
// it is also not desired to track memory allocations within the memory manager module.
#include "vinifera_new_off.h"


/**
 *  Define our own version of assert to simply set a break point.
 */
#define MEM_DEBUGBREAK()         __asm { int 3 } // Set a break point

#ifndef NDEBUG
#define MEM_ASSERT(x)       { if ((x) == false) { MEM_DEBUGBREAK(); } }
#define MEM_ASSERT_PRINT(x) { if ((x) == false) { DEBUG_ERROR("\n***** Memory breakpoint hit! *****\n\n"); MEM_DEBUGBREAK(); } }
#else
#define MEM_ASSERT(x)       ((void)0)
#define MEM_ASSERT_PRINT(x) ((void)0)
#endif


/**
 *  Global Variable definitions
 */
typedef enum
{
    EMPTY                = 0x00000000,
    PADDING              = 0xDEADC0DE,
    BODY                 = 0xBAADC0DE,
    OPT_NONE             = 0x0,
    OPT_BREAK_ON_DEALLOC = 0x1,
    OPT_BREAK_ON_REALLOC = 0x2,
    HASH_SIZE            = 1024,
};

static int  NumAllocations             = 0;
static char LOGFILE[256];//              = "memory.log"; 
static const char * const AllocationTypes[] = { "Unknown", "new", "new[]", "malloc", "calloc",
                                                "realloc", "delete", "delete[]", "free" };


/**
 *  Here are the containers that make up the memory manager.
 */
static struct StackNode {             // This struct is used to hold the file name and line
    const char *fileName;             // number of the file that is requesting a deallocation.
    const char *functionName;
    unsigned short lineNumber;        // Only deallocations are recorded since the allocation
    StackNode *next;                  // routines accept these additional parameters.
};

static struct MemoryNode              // This struct defines the primary container for tracking
{                                     // all memory allocations.  It holds information that 
    std::size_t    actualSize;        // will be used to track memory violations and information
    std::size_t    reportedSize;      // to help the user track down specific problems reported
    void         * actualAddress;     // to the Log file upon termination of the program.
    void         * reportedAddress;   
    char           sourceFile[30];    // I have tried to keep the physical size of this struct
    char           sourceFunction[256];
    unsigned short sourceLine;        // to a minimum, to reduce the memory tracking overhead.  
    unsigned short paddingSize;       // At the same time I have tried to allow for as much
    char           options;           // flexibility and information holding as possible.
    long           predefinedBody;
    ViniferaMemoryManager::MemAllocType     allocationType;
    MemoryNode   * next;
    MemoryNode   * prev;

    void Initialize_Memory(long body = BODY); // Initailize the nodes memory for interrogation.
};


/**
 *  Forward declrations of helper functions.
 */
bool __cdecl Initialize_Memory_Manager();
void __cdecl Release_Memory_Manager();
char * __cdecl Format_Owner_String(const char * file, const char * function, int line);
char * __cdecl Source_File_Stripper(const char * sourceFile);
void __cdecl Log(const char * s, ...);
char * __cdecl Insert_Commas(unsigned long value);
char *__cdecl Memory_Size_String(unsigned long size, bool lengthen_string = true);


// This class implements a basic stack for record keeping.  It is necessary to use this class
// instead of the STL class since we need to explicitly call the init() method to initialize
// the data members.  This is due to the restriction of only using the malloc() method within
// this file to avoid calling our own new() method and creating circular definitions.  It is 
// necessary to create this stack for Logging file information when deallocating memory due to
// to order in which memory is deallocated and the effect of the delete macro.  To understand
// this better refer to the sample illustration below.
// 
//    Sample Code     file1 => delete classAObject;
//                    file2 => ~classA() { delete[] intArray; }
//    
//    Function Calls      1.  Set_Owner(file1, line);
//                        2.  Set_Owner(file2, line);
//                        3.  Deallocate(intArray, MM_DELETE_ARRAY);
//                        4.  Deallocate(classAObject, MM_DELETE);
//
// The order of operations requires a stack for proper file tracking.
static class MyStack
{
    public:
        MyStack() :
            Head(nullptr),
            Count(0)
        {
        }

        __forceinline void Init() 
        {
            Head = nullptr;
            Count = 0;
        }

        __forceinline bool Empty() 
        {
            return Count == 0;
        }

        __forceinline StackNode * Top() 
        {
            return Head;
        }

        __forceinline void Push(StackNode * n) 
        {
            if (!n) {
                return;
            }
            
            n->next = Head;
            Head = n;
            Count++;
        }

        __forceinline StackNode * Pop()  
        {
            StackNode * n = Head;

            if (n) {
                Head = Head->next;
                Count--;
            }
            
            return n;
        }

    private:
        StackNode * Head;
        int Count;
};


// This class provides the implementation for the Memory Manager.  I created this class to 
// act as a container to centeralize the control instead of allowing everyone to become 
// intertangled.  Be design I have also left a lot of data members public for ease of access
// since this file is the only one that can ever create a MemoryManager object.
static class ViniferaMemoryManagerClass
{
    public:
        ViniferaMemoryManagerClass() {}     // Default Constructor.
        ~ViniferaMemoryManagerClass() {}    // Destructor.

        void Initialize();      // Initailize internal memory.
        void Release();         // Release internal memory.

        // Hash Table Routines
        void Insert_Memory_Node(MemoryNode *node);      // Insert a new memory node.
        MemoryNode *Get_Memory_Node(void *address);     // Retrieve a memory node.
        MemoryNode *Remove_Memory_Node(void *address);  // Remove a memory node.
        bool Validate_Memory_Unit(MemoryNode *node);    // Validate a memory node's memory.

        // Memory Caching to minimize allocations within the memory manager
        void Deallocate_Memory(MemoryNode *node);
        MemoryNode* Allocate_Memory();

        // Error Reporting Routines
        void Dump_Log_Report();
        void Dump_Memory_Allocations();
        void Log(const char *s, ...);
    
    public:
        // User programmable options
        int          BreakOnAllocationCount;
        unsigned int PaddingSize;
        bool         LogAlways;
//        bool         CleanLogFileOnFirstRun;

        // Statistical Information
        int          TotalMemoryAllocations;  // The number of allocations.
        unsigned int TotalMemoryAllocated;    // Number of bytes allocated.
        unsigned int TotalMemoryUsed;         // Number of bytes used.
    
        unsigned int PeakMemoryAllocation;    // The largest memory allocation.
        unsigned int PeakTotalNumAllocations; // The largest number of allocation existing simaltaniously.
        unsigned int OverheadMemoryCost;      // The current amount of memory required for memory tracking.
        unsigned int PeakOverHeadMemoryCost;  // The peak overhead memory cost.
        unsigned int TotalOverHeadMemoryCost; // The total overhead memory cost. 
        unsigned int AllocatedMemory;         // The current amount of allocated memory.
        unsigned int NumBoundsViolations;     // The number of memory bounds violations.

        // Stack for tracking file information for deallocations.
        MyStack TopStack;

        unsigned int NumAllocations;      // The number of entries within the hash table.

    private:
        int Get_Hash_Index(void *address);  // Given anaddressthis returns the hash table index
        int Calculate_Unallocated_Memory();   // Return the amount of unallocated memory.

        MemoryNode *HashTable[HASH_SIZE]; // Hash Table container for tracking memory allocations.

        MemoryNode *MemoryCache;          // Used for caching unused memory nodes.
};

static ViniferaMemoryManagerClass *MemoryManager = nullptr;      // Declaration of the one and only Memory Manager Object 


#if 0
ViniferaMemoryManagerClass::ViniferaMemoryManagerClass() :
        int          BreakOnAllocationCount;
        unsigned int PaddingSize;
        bool         LogAlways;
//        bool         CleanLogFileOnFirstRun;
        int          TotalMemoryAllocations;  // The number of allocations.
        unsigned int TotalMemoryAllocated;    // Number of bytes allocated.
        unsigned int TotalMemoryUsed;         // Number of bytes used.
        unsigned int PeakMemoryAllocation;    // The largest memory allocation.
        unsigned int PeakTotalNumAllocations; // The largest number of allocation existing simaltaniously.
        unsigned int OverheadMemoryCost;      // The current amount of memory required for memory tracking.
        unsigned int PeakOverHeadMemoryCost;  // The peak overhead memory cost.
        unsigned int TotalOverHeadMemoryCost; // The total overhead memory cost. 
        unsigned int AllocatedMemory;         // The current amount of allocated memory.
        unsigned int NumBoundsViolations;     // The number of memory bounds violations.
        MyStack TopStack;
        unsigned int NumAllocations;      // The number of entries within the hash table.
        MemoryNode *HashTable[HASH_SIZE]; // Hash Table container for tracking memory allocations.
        MemoryNode *MemoryCache;          // Used for caching unused memory nodes.
{
}
#endif


/**
 * ViniferaMemoryManagerClass::Initialize():
 *  This method is responsible for initializing the Memory Manager.  
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void ViniferaMemoryManagerClass::Initialize()
{
    BreakOnAllocationCount = -1;
    PaddingSize = 4;
    LogAlways = true;
    //CleanLogFileOnFirstRun = true;
    TotalMemoryAllocated = 0;
    TotalMemoryUsed = 0;
    TotalMemoryAllocations = 0;
    PeakMemoryAllocation = 0;
    NumAllocations = 0;
    PeakTotalNumAllocations = 0;
    OverheadMemoryCost = 0;
    TotalOverHeadMemoryCost = 0;
    PeakOverHeadMemoryCost = 0;
    AllocatedMemory = 0;
    NumBoundsViolations = 0;

    for (int i = 0; i < HASH_SIZE; ++i) {
        HashTable[i] = nullptr;
    }

    TopStack.Init();

    MemoryCache = nullptr;
}


/**
 * ViniferaMemoryManagerClass::Release():
 *  This method is responsible for releasing the Memory Manager.  It dumps the Log file and 
 *  cleans up any memory that has been left behind.
 * 
 *  Return Type : void 
 *    Arguments   : NONE
 */
void ViniferaMemoryManagerClass::Release()
{
    Dump_Log_Report();     // Dump the statistical information to the Log file.

    // If there are memory leaks, be sure to clean up memory that the memory manager allocated.
    // It would really look bad if the memory manager created memory leaks!!!
    if (NumAllocations != 0) {
        for (int ii = 0; ii < HASH_SIZE; ++ii) {
            while (HashTable[ii]) {
                MemoryNode *ptr = HashTable[ii];
                HashTable[ii] = HashTable[ii]->next;
                ViniferaMemory::Free(ptr->actualAddress);      // Free the memory left behind by the memory leak.
                ViniferaMemory::Free(ptr);                     // Free the memory used to create the Memory Node.
            }
        }
    }

    // Clean up the stack if it contains entries.
    while (!TopStack.Empty()) {
        ViniferaMemory::Free(TopStack.Top());
        TopStack.Pop();
    }

    // Clean the memory cache
    MemoryNode *ptr;
    while (MemoryCache) {
        ptr = MemoryCache;
        MemoryCache = ptr->next;
        ViniferaMemory::Free(ptr);
    }
}


/**
 * ViniferaMemoryManagerClass::Insert_Memory_Node():
 *  Inserts a memory node into the hash table and collects statistical information.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      MemoryNode *node : The memory node to be inserted into the hash table.
 */
void ViniferaMemoryManagerClass::Insert_Memory_Node(MemoryNode *node)
{
    int hashIndex = Get_Hash_Index(node->reportedAddress);
    node->next = HashTable[hashIndex];
    node->prev = nullptr;

    if (HashTable[hashIndex]) HashTable[hashIndex]->prev = node;

    HashTable[hashIndex] = node;
    
    // Collect Statistic Information.
    NumAllocations++;

    AllocatedMemory += node->reportedSize;

    if (AllocatedMemory > PeakMemoryAllocation)   PeakMemoryAllocation    = AllocatedMemory;
    if (NumAllocations > PeakTotalNumAllocations) PeakTotalNumAllocations = NumAllocations;

    TotalMemoryAllocated += node->reportedSize;
    TotalMemoryAllocations++;
}


/**
 * ViniferaMemoryManagerClass::Get_Memory_Node():
 *  Returns the memory node for the given memory address, if the node does not exist a 
 *  nullptr pointer is returned.
 * 
 *  Return Type : MemoryNode* -> A pointer to the requested memory node.
 *  Arguments   : 
 *      void *address    : Theaddressof the memory to be retrieved.
 */
MemoryNode* ViniferaMemoryManagerClass::Get_Memory_Node(void *address)
{
    MemoryNode *ptr = HashTable[Get_Hash_Index(address)];
    while (ptr && ptr->reportedAddress != address) {
        ptr = ptr->next;
    }
    return ptr;
}


/**
 * ViniferaMemoryManagerClass::Remove_Memory_Node():
 *  Returns the memory node for the given memory address, if the node does not exist, a nullptr
 *  pointer is returned.  This method also removes the memory node from the hash table.
 * 
 *  Return Type : MemoryNode* -> A pointer to the requested memory node.
 *  Arguments   : 
 *      void *address    : Theaddressof the memory to be retrieved.
 */
MemoryNode* ViniferaMemoryManagerClass::Remove_Memory_Node(void *address)
{
    int hashIndex = Get_Hash_Index(address);
    
    if (hashIndex == 17) 
        int ttt = 0;
    
    MemoryNode *ptr = HashTable[hashIndex];
    while (ptr && ptr->reportedAddress != address) {
        ptr = ptr->next;
    }

    if (ptr) {
        if (ptr->next) ptr->next->prev = ptr->prev;
        if (ptr->prev) ptr->prev->next = ptr->next;
        else           HashTable[hashIndex] = ptr->next;

        // Update Statistical Information.
        NumAllocations--;
        AllocatedMemory -= ptr->reportedSize;
    }
    return ptr;
}


/**
 * ViniferaMemoryManagerClass::Validate_Memory_Unit():
 *  Given a Memory Node, this method will interrogate its memory looking for bounds violations
 *  and the number of bytes that were actually used.  This method should only be called before
 *  deleting a Memory Node to generate statistical information.  This method will report all 
 *  errors to the Log file.  Returns TRUE if no bounds violations where found, otherwise FALSE.
 * 
 *  Return Type : bool -> True if no bounds violations, otherwise False.
 *  Arguments   : 
 *      MemoryNode *node : The memory node to be interrogated.
 */
bool ViniferaMemoryManagerClass::Validate_Memory_Unit(MemoryNode *node)
{
    bool success = true;
    unsigned int ii;
    unsigned int totalBytesUsed = 0, boundViolations = 0;

    // Detect bounds violations
    long *beginning = (long*)node->actualAddress;
    long *ending    = (long*)((char*)node->actualAddress + node->actualSize - node->paddingSize*sizeof(long));
    for (ii = 0; ii < node->paddingSize; ++ii) {
        if (beginning[ii] != PADDING || ending[ii]!= PADDING) {
            success = false;  // Report the bounds violation.
            boundViolations++;
        }
    }

    if (!success) NumBoundsViolations++;

    // Attempt to determine how much of the allocated memory was actually used.
    // Initialize the memory padding for detecting bounds violations.
    long *lptr       = (long*)node->reportedAddress;
    unsigned int len = node->reportedSize / sizeof(long);
    unsigned int cnt;
    for (cnt = 0; cnt < len; ++cnt) {                        
        if (lptr[cnt] != node->predefinedBody)       totalBytesUsed += sizeof(long);
    }
    char *cptr = (char*)(&lptr[cnt]);
    len = node->reportedSize - len*sizeof(long);
    for (cnt = 0; cnt < len; ++cnt) {    
        if (cptr[cnt] != (char)node->predefinedBody) totalBytesUsed++;
    }

    TotalMemoryUsed += totalBytesUsed;

    if (LogAlways && totalBytesUsed != node->reportedSize) {          // Report the percentage 
        this->Log("Unused Memory Detected   : %-40s %8s(0x%08p) : %s",   //  of waisted memory space.
                     Format_Owner_String(node->sourceFile, node->sourceFunction, node->sourceLine),
                     AllocationTypes[node->allocationType], node->reportedAddress, 
                             Memory_Size_String(node->reportedSize - totalBytesUsed));
    }

    if (LogAlways && !success) {                                      // Report the memory 
        this->Log("Bounds Violation Detected: %-40s %8s(0x%08p)",        //  bounds violation.
                     Format_Owner_String(node->sourceFile, node->sourceFunction, node->sourceLine),
                     AllocationTypes[node->allocationType], node->reportedAddress);
    }

    return success;
}


/**
 * ViniferaMemoryManagerClass::Deallocate_Memory():
 *  This method adds the MemoryNode to the memory cache for latter use.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      MemoryNode *node : The MemoryNode to be released.
 */
void ViniferaMemoryManagerClass::Deallocate_Memory(MemoryNode *node)
{
    OverheadMemoryCost -= (node->paddingSize * 2 * sizeof(long));
    node->next = MemoryCache;
    MemoryCache = node;
}


/**
 * ViniferaMemoryManagerClass::Allocate_Memory():
 *  This method checks the memory cache for unused MemoryNodes, if one exists it is removed
 *  from the cache and returned.  Otherwise, new memory is allocated for the MemoryNode and
 *  returned.
 * 
 *  Return Type : MemoryNode* -> The allocated MemoryNode.
 *  Arguments   : NONE
 */
MemoryNode* ViniferaMemoryManagerClass::Allocate_Memory()
{
    if (!MemoryCache) {
        int overhead = PaddingSize * 2 * sizeof(long) + sizeof(MemoryNode);
        OverheadMemoryCost += overhead;
        TotalOverHeadMemoryCost += overhead;

        if (OverheadMemoryCost > PeakOverHeadMemoryCost) {
            PeakOverHeadMemoryCost =  OverheadMemoryCost;
        }

        return (MemoryNode*)ViniferaMemory::Alloc(sizeof(MemoryNode));
    }
    else {
        int overhead = PaddingSize * 2 * sizeof(long);
        OverheadMemoryCost += overhead;
        TotalOverHeadMemoryCost += overhead;

        if (OverheadMemoryCost > PeakOverHeadMemoryCost) {
            PeakOverHeadMemoryCost =  OverheadMemoryCost;
        }

        MemoryNode *ptr = MemoryCache;
        MemoryCache = MemoryCache->next;
        return ptr;
    }
}


/**
 * ViniferaMemoryManagerClass::Dump_Log_Report():
 *  This method implements the main reporting system.  It reports all of the statistical
 *  information to the desired Log file.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void ViniferaMemoryManagerClass::Dump_Log_Report()
{
#if 0
    if (CleanLogFileOnFirstRun) {      // Cleanup the Log?
        unlink(LOGFILE);                 // Delete the existing Log file.
        CleanLogFileOnFirstRun = false;  // Toggle the flag.
    }
#endif

    FILE    *fp = std::fopen(LOGFILE, "ab"); // Open the Log file
    if (!fp) return;

    time_t t = std::time(nullptr);
    tm *time = std::localtime(&t);
    
    int memoryLeak = Calculate_Unallocated_Memory();
    int totalMemoryDivider = TotalMemoryAllocated != 0 ? TotalMemoryAllocated : 1;

    // Header Information
    std::fprintf(fp, "\r\n");
    std::fprintf(fp, "******************************************************************************* \r\n");
    std::fprintf(fp, "*********           Memory report for: %02d/%02d/%04d %02d:%02d:%02d            ********* \r\n", time->tm_mon + 1, time->tm_mday, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec);
    std::fprintf(fp, "******************************************************************************* \r\n");
    std::fprintf(fp, "\r\n");

    // Report summary
    std::fprintf(fp, "                     T O T A L  M E M O R Y  U S A G E                          \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "           Total Number of Dynamic Allocations: %10s\r\n", Insert_Commas(TotalMemoryAllocations));
    std::fprintf(fp, "      Reported Memory usage to the Application: %s\r\n", Memory_Size_String(TotalMemoryAllocated));
    std::fprintf(fp, "          Actual Memory use by the Application: %s\r\n", Memory_Size_String(TotalOverHeadMemoryCost + TotalMemoryAllocated));
    std::fprintf(fp, "                      Memory Tracking Overhead: %s\r\n", Memory_Size_String(TotalOverHeadMemoryCost));
    std::fprintf(fp, "\r\n");

    std::fprintf(fp, "                      P E A K  M E M O R Y  U S A G E                           \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "            Peak Number of Dynamic Allocations: %10s\r\n", Insert_Commas(PeakTotalNumAllocations));
    std::fprintf(fp, " Peak Reported Memory usage to the application: %s\r\n", Memory_Size_String(PeakMemoryAllocation));
    std::fprintf(fp, "     Peak Actual Memory use by the Application: %s\r\n", Memory_Size_String(PeakOverHeadMemoryCost + PeakMemoryAllocation));
    std::fprintf(fp, "                 Peak Memory Tracking Overhead: %s\r\n", Memory_Size_String(PeakOverHeadMemoryCost));
    std::fprintf(fp, "\r\n");

    std::fprintf(fp, "                          U N U S E D  M E M O R Y                              \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "  Percentage of Allocated Memory Actually Used: %10.2f %%\r\n", (float)(1 - (TotalMemoryAllocated - TotalMemoryUsed)/(float)totalMemoryDivider) * 100);
    std::fprintf(fp, "       Percentage of Allocated Memory Not Used: %10.2f %%\r\n", (float)(TotalMemoryAllocated - TotalMemoryUsed)/(float)totalMemoryDivider * 100);
    std::fprintf(fp, "        Memory Allocated but not Actually Used: %s\r\n", Memory_Size_String(TotalMemoryAllocated - TotalMemoryUsed));
    std::fprintf(fp, "\r\n");

    std::fprintf(fp, "                      B O U N D S  V I O L A T I O N S                          \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "            Number of Memory Bounds Violations: %10s\r\n", Insert_Commas(NumBoundsViolations));
    std::fprintf(fp, "\r\n");

    std::fprintf(fp, "                           M E M O R Y  L E A K S                               \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "                        Number of Memory Leaks: %10s\r\n", Insert_Commas(NumAllocations));
    std::fprintf(fp, "                 Amount of Memory Un-Allocated: %s\r\n", Memory_Size_String(memoryLeak));
    std::fprintf(fp, "   Percentage of Allocated Memory Un-Allocated: %10.2f %%\r\n", (float)(1 - (TotalMemoryAllocated - memoryLeak)/(float)totalMemoryDivider) * 100);
    std::fprintf(fp, "\r\n");

    if (NumAllocations != 0) {  // Are there memory leaks?
        fclose(fp);               // Close the Log file.
        Dump_Memory_Allocations();    // Display any memory leaks.
    }
    else {
        std::fclose(fp);
    }
}


/**
 * ViniferaMemoryManagerClass::Dump_Memory_Allocations():
 *  This method is responsible for reporting all memory that is currently allocated.  This is
 *  achieved by reporting all memory that is still within the hash table.  
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void ViniferaMemoryManagerClass::Dump_Memory_Allocations()
{
#if 0
    if (CleanLogFileOnFirstRun) {      // Cleanup the Log?
        unlink(LOGFILE);                 // Delete the existing Log file.
        CleanLogFileOnFirstRun = false;  // Toggle the flag.
    }
#endif

    FILE    *fp = std::fopen(LOGFILE, "ab"); // Open the Log file
    if (!fp) return;

    std::fprintf(fp, "              C U R R E N T L Y  A L L O C A T E D  M E M O R Y                 \r\n");
    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");

    for (int ii = 0, cnt = 1; ii < HASH_SIZE; ++ii) {
        for (MemoryNode *ptr = HashTable[ii]; ptr; ptr = ptr->next) {
            std::fprintf(fp, "** Allocation # %2d\r\n", cnt++);
            std::fprintf(fp, "Total Memory Size : %s\r\n", Memory_Size_String(ptr->reportedSize, false));
            std::fprintf(fp, "Source File       : %s\r\n", ptr->sourceFile);
            std::fprintf(fp, "Source Function   : %s\r\n", ptr->sourceFunction);
            std::fprintf(fp, "Source Line       : %d\r\n", ptr->sourceLine);
            std::fprintf(fp, "Allocation Type   : %s\r\n", AllocationTypes[ptr->allocationType]);
            std::fprintf(fp, "\r\n");
        }
    }

    std::fprintf(fp, "------------------------------------------------------------------------------- \r\n");
    std::fprintf(fp, "******************************************************************************* \r\n");
    std::fprintf(fp, "\r\n");

    std::fclose(fp);
}


/**
 * ViniferaMemoryManagerClass::Log():
 *  Dumps a specific string to the Log file.  Used for error reporting during runtime.  This
 *  method accepts a variable argument lenght such as printf() for ease of reporting.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      char *s    : The string to be written to the Log file.
 *      ...        : The parameters to be placed within the string, simular to say: printf(s, ...)
 */
void ViniferaMemoryManagerClass::Log(const char *s, ...)
{
#if 0
    if (CleanLogFileOnFirstRun) {      // Cleanup the Log?
        unlink(LOGFILE);                 // Delete the existing Log file.
        CleanLogFileOnFirstRun = false;  // Toggle the flag.
    }
#endif

    static char buffer[2048];    // Create the buffer
    va_list    list;                // Replace the strings variable arguments with the provided
    va_start(list, s);         //  arguments.
    std::vsnprintf(buffer, sizeof(buffer), s, list);
    va_end(list);

    FILE    *fp = std::fopen(LOGFILE, "ab");  // Open the Log file
    if (!fp) return;

    std::fprintf(fp, "%s\r\n", buffer);     // Write the data to the Log file
    std::fclose(fp);                        // Close the file
}


/**
 * ViniferaMemoryManagerClass::Get_Hash_Index():
 *  Returns the hash index for the given memory address.
 * 
 *  Return Type : int -> The hash table index.
 *  Arguments   : 
 *      void *address    : Theaddressto determine the hash table index for.
 */
int ViniferaMemoryManagerClass::Get_Hash_Index(void *address)
{
    return ((unsigned int)address >> 4) & (HASH_SIZE -1);
}


/**
 * ViniferaMemoryManagerClass::Calculate_Unallocated_Memory():
 *  Returns the amount of unallocated memory in BYTES.
 * 
 *  Return Type : int -> The number of BYTES of unallocated memory.
 *  Arguments   : NONE
 */
int ViniferaMemoryManagerClass::Calculate_Unallocated_Memory()
{
    int memory = 0;
    for (int ii = 0; ii < HASH_SIZE; ++ii) {
        for (MemoryNode *ptr = HashTable[ii]; ptr; ptr = ptr->next) {
            memory += ptr->reportedSize;
        }
    }
    return memory;
}


/**
 * MemoryNode::Initialize_Memory():
 *  Initialize the padding and the body of the allocated memory so that it can be interrogated
 *  upon deallocation.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      long body    : The value to which the body of the allocated memory should be intialized too.
 */
void MemoryNode::Initialize_Memory(long body)
{
    // Initialize the memory padding for detecting bounds violations.
    long *beginning = (long*)actualAddress;
    long *ending    = (long*)((char*)actualAddress + actualSize - paddingSize*sizeof(long));
    for (unsigned short ii = 0; ii < paddingSize; ++ii) {
        beginning[ii] = ending[ii] = PADDING;
    }

    // Initialize the memory body for detecting unused memory.
    beginning        = (long*)reportedAddress;
    unsigned int len = reportedSize / sizeof(long);
    unsigned int cnt;
    for (cnt = 0; cnt < len; ++cnt) {                         // Initialize the majority of the memory
        beginning[cnt] = body;
    }
    char *cptr = (char*)(&beginning[cnt]);
    len = reportedSize - len*sizeof(long);
    for (cnt = 0; cnt < len; ++cnt) {    // Initialize the remaining memory
        cptr[cnt] = (char)body;
    }

    predefinedBody = body;
}


/**
 * Initialize_Memory_Manager():
 *  This method is responsible for creating a Memory Manager Object.  If the object already
 *  exists or is successfully created TRUE is returned.  Otherwise if the object was 
 *  previously created and has been destroyed FALSE is returned.  The goal is to guarantee
 *  that the Memory Manager is the first object to be created and the last to be destroyed.
 * 
 *  Return Type : bool -> True if intialized, otherwise False.
 *  Arguments   : NONE
 */
bool __cdecl Initialize_Memory_Manager()
{
    static bool hasBeenInitialized = false;

    if (MemoryManager) {            // The memory manager object already exists.
        return true;
    }

    if (hasBeenInitialized) {       // The memory manager object has already been created
        return false;               // once, however it was release before everyone was done.
    }

    // Create the memory manager object.
    MemoryManager = (ViniferaMemoryManagerClass *)ViniferaMemory::Alloc(sizeof(ViniferaMemoryManagerClass));
    MemoryManager->Initialize();
    std::atexit(Release_Memory_Manager);    // Log this function to be called upon program shut down.
    hasBeenInitialized = true;
    return true;
}


/**
 * Release_Memory_Manager():
 *  This method is automatically called when the application is terminated.  It is important
 *  that this is the last function called to perform application cleanup since the memory 
 *  manager object should be the last object to be destoryed, thus this must be the first 
 *  method Logged to perform application clean up.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void __cdecl Release_Memory_Manager() 
{
    NumAllocations = MemoryManager->NumAllocations;
    MemoryManager->Release();  // Dump the Log report and free remaining memory.
    ViniferaMemory::Free(MemoryManager);
    MemoryManager = nullptr;
}


/**
 * Format_Owner_String():
 *  This method is responsible for formating the owner string.  This string states the file
 *  name and line number within the specified file.
 * 
 *  Return Type : char* -> A pointer to the string representing the owner string. 
 *  Arguments   : 
 *      const char *file : The files name
 *      int line           : The line number within the specified file.
 */
char * __cdecl Format_Owner_String(const char *file, const char * function, int line)
{
  static char    str[256];
    std::memset(str, 0, sizeof(str));
    std::snprintf(str, sizeof(str), "%s(%05d):%s", Source_File_Stripper(file), line, function);
    return str;
}


/**
 * Source_File_Stripper():
 *  This method takes a file name and strips off all directory information.
 * 
 *  Return Type : char* -> A pointer to the actual file minus all directory information. 
 *  Arguments   : 
 *      const char *sourceFile : The file to strip.
 */
char * __cdecl Source_File_Stripper(const char *sourceFile)
{
    if (!sourceFile) {
        return nullptr;
    }

    char * ptr = (char *)std::strrchr(sourceFile, '\\');
    if (ptr) {
        return ptr + 1;
    }

    ptr = (char *)std::strrchr(sourceFile, '/');
    if (ptr) {
        return ptr + 1;
    }

    return (char *)sourceFile;
}


/**
 * Log():
 *  Dumps a specific string to the Log file.  Used for error reporting during runtime.  This
 *  method accepts a variable argument lenght such as printf() for ease of reporting.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      char *s    : The string to be written to the Log file.
 *      ...        : The parameters to be placed within the string, simular to say: printf(s, ...)
 */
void __cdecl Log(const char *s, ...)
{
    static char buffer[2048];            // Create the buffer
    va_list    list;
    va_start(list, s);
    std::vsnprintf(buffer, sizeof(buffer), s, list);
    va_end(list);

    FILE    *fp = std::fopen(LOGFILE, "ab");  // Open the Log file
    if (!fp) return;

    std::fprintf(fp, "%s\r\n", buffer);     // Write the data to the Log file
    std::fclose(fp);                        // Close the file
}


/**
 * Insert_Commas():
 *  This method takes a value and inserts commas, creating a nicely formated string.  Thus
 *  the value => 23456 would be converted to the following string => 23,456.
 * 
 *  Return Type : char* -> A string representing the provided value with commas inserted. 
 *  Arguments   : 
 *      unsigned long value    : The value to insert commas into.
 */
char * __cdecl Insert_Commas(unsigned long value)
{
    static char str[30];
    for (int ii = 0; ii < 30; ++ii) str[ii] = '\0';
    snprintf(str, sizeof(str), "%d", value);
    if (std::strlen(str) > 3) {
        std::memmove(&str[std::strlen(str)-3], &str[std::strlen(str)-4], 4);
        str[std::strlen(str) - 4] = ',';
    }
    if (std::strlen(str) > 7) {
        std::memmove(&str[std::strlen(str)-7], &str[std::strlen(str)-8], 8);
        str[std::strlen(str) - 8] = ',';
    }
    if (std::strlen(str) > 11) {
        std::memmove(&str[std::strlen(str)-11], &str[std::strlen(str)-12], 12);
        str[std::strlen(str) - 12] = ',';
    }
    return str;
}


/**
 * Memory_Size_String():
 *  This method takes a memory size and creates a user friendly string that displays the 
 *  memory size in bytes, K or M. 
 * 
 *  Return Type : char* -> The final memory size string. 
 *  Arguments   : 
 *      unsigned long size    : The size of the memory.
 *      bool lengthen_string : Whether or not to pad the string with white spaces.
 */
char * __cdecl Memory_Size_String(unsigned long size, bool lengthen_string /* = true */)
{
    static char str[90];
    if (lengthen_string) {
        if (size > (1024*1024))    std::snprintf(str, sizeof(str), "%10s (%7.2fM)", Insert_Commas(size), size / (1024.0 * 1024.0));
        else if (size > 1024)        std::snprintf(str, sizeof(str), "%10s (%7.2fK)", Insert_Commas(size), size / 1024.0);
        else                            std::snprintf(str, sizeof(str), "%10s bytes     ", Insert_Commas(size));
    }
    else {
        if (size > (1024*1024))    std::snprintf(str, sizeof(str), "%s (%7.2fM)", Insert_Commas(size), size / (1024.0 * 1024.0));
        else if (size > 1024)        std::snprintf(str, sizeof(str), "%s (%7.2fK)", Insert_Commas(size), size / 1024.0);
        else                            std::snprintf(str, sizeof(str), "%s bytes     ", Insert_Commas(size));
    }
    return str;
}


/**
 *  Implement wrappers for memory allocation and deallocation.
 */
void * __cdecl ViniferaMemory::Alloc(unsigned int size)
{
#ifdef USE_WIN_HEAPAPI
    void *block_ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    ASSERT_PRINT(block_ptr != nullptr, "Failed to allocate memory!");
#else
    void *block_ptr = std::malloc(size);
#endif

    return block_ptr;
}

void * __cdecl ViniferaMemory::CountAlloc(unsigned int count, unsigned int size)
{
#ifdef USE_WIN_HEAPAPI
    void *block_ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size * count);
    ASSERT_PRINT(block_ptr != nullptr, "Failed to allocate array memory!");
#else
    void *block_ptr = std::calloc(count, size);
#endif

    return block_ptr;
}

void * __cdecl ViniferaMemory::Realloc(void *ptr, unsigned int size)
{
#ifdef USE_WIN_HEAPAPI
    void *block_ptr = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr, size);
    ASSERT_PRINT(block_ptr != nullptr, "Failed to re-allocate memory!");
#else
    void *block_ptr = std::realloc(ptr, r_size);
#endif

    return block_ptr;
}

void __cdecl ViniferaMemory::Free(void *ptr)
{
    if (!ptr) {
        return;
    }

#ifdef USE_WIN_HEAPAPI
    bool freed = HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr);
    ASSERT_PRINT(freed, "Failed to free memory, possible memory leak!");
#else
    std::free(ptr);
#endif
}

unsigned int __cdecl ViniferaMemory::MemSize(void *ptr)
{
#ifndef USE_WIN_HEAPAPI
    return HeapSize(GetProcessHeap(), 0, ptr);
#else
    return _msize(ptr);
#endif
}


/**
 * Dump_Log_Report():
 *  Dump the Log report to the file, this is the same method that is automatically called 
 *  upon the programs termination to report all statistical information.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void __cdecl ViniferaMemoryManager::Dump_Log_Report()
{
    if (MemoryManager) {
        MemoryManager->Dump_Log_Report();
    }
}


/**
 * Dump_Memory_Allocations():
 *  Report all allocated memory to the Log file.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void __cdecl ViniferaMemoryManager::Dump_Memory_Allocations()
{
    if (MemoryManager) {
        MemoryManager->Dump_Memory_Allocations();
    }
}


/**
 * Set_Log_File():
 *  Allows for the Log file to be changed from the default.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      char *file : The name of the new Log file.
 */
void __cdecl ViniferaMemoryManager::Set_Log_File(const char * file)
{
    if (file) {
        std::strncpy(LOGFILE, file, sizeof(LOGFILE));
    }
}


/**
 * Set_Exhaustive_Testing():
 *  This method allows for exhaustive testing.  It has the same functionality as the following
 *  function calls => Set_Log_Always(true); Set_Padding_Size(1024); 
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      bool test : Whether or not to turn exhaustive testing on or off.
 */
void __cdecl ViniferaMemoryManager::Set_Exhaustive_Testing(bool test /* = true */)
{
    if (!MemoryManager) {
        return;
    }

    if (test) {
        Set_Padding_Size(1024);
        Set_Log_Always(true);
    } else {
        Set_Padding_Size(1024);
        Set_Log_Always(false);
    }
}


/**
 * Set_Log_Always():
 *  Sets the flag for exhaustive information Logging.  All information is sent to the Log file.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      bool Log     : Whether or not to Log all information.
 */
void __cdecl ViniferaMemoryManager::Set_Log_Always(bool log /* = true */)
{
    if (MemoryManager) {
        MemoryManager->LogAlways = log;
    }
} 


/**
 * Set_Padding_Size():
 *  Sets the padding size for memory bounds checks.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      int size     : The new padding size.
 */
void __cdecl ViniferaMemoryManager::Set_Padding_Size(int size /* = 4 */)
{
    if (MemoryManager && size > 0) {
        MemoryManager->PaddingSize = size;
    }
}


#if 0
/**
 * Clean_Log_File():
 *  Cleans out the Log file by deleting it.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      bool clean : Whether or not to clean the Log file.
 */
void __cdecl ViniferaMemoryManager::Clean_Log_File(bool clean /* = true */)
{
    if (MemoryManager) {
        MemoryManager->CleanLogFileOnFirstRun = true;
    }
}
#endif


/**
 * Break_On_Allocation():
 *  Allows you to set a break point on the n-th allocation.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      int allocation_count    : The allocation count to break on.
 */
void __cdecl ViniferaMemoryManager::Break_On_Allocation(int allocation_count)
{
    if (MemoryManager && allocation_count > 0) {
        MemoryManager->BreakOnAllocationCount = allocation_count;
    }
}


/**
 * Break_On_Deallocation():
 *  Sets a flag that will set a break point when the specified memory is deallocated.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *      void * address    : Theaddressto break on when it is deallocated.
 */
void __cdecl ViniferaMemoryManager::Break_On_Deallocation(void * address)
{
    if (!MemoryManager || !address) {
        return;
    }

    MemoryNode *node = MemoryManager->Get_Memory_Node(address);
    node->options |= OPT_BREAK_ON_DEALLOC;
}


/**
 * Break_On_Reallocation():
 *  Sets a flag that will set a break point when the specified memory is reallocated by 
 *  using the realloc() method.
 *
 *  Return Type : void 
 *  Arguments   : 
 *      void * address    : Theaddressto break on when it is reallocated.
 */
void __cdecl ViniferaMemoryManager::Break_On_Reallocation(void * address)
{
    if (!MemoryManager || !address) {
        return;
    }

    MemoryNode *node = MemoryManager->Get_Memory_Node(address);
    node->options |= OPT_BREAK_ON_REALLOC;
}


/**
 * ViniferaMemoryManager::Allocate_Memory():
 *  This is the main memory allocation routine, this is called by all of the other 
 *  memory allocation routines to allocate and track memory.
 * 
 *  Return Type: void 
 *  Arguments: 
 *      const char * file      : The file requesting the deallocation.
 *      int line               : The line within the file requesting the deallocation.
 *      size_t size            : The size of the memory to be reallocated.
 *      MemAllocType type      : The type of reallocation being performed.
 */
void * __cdecl ViniferaMemoryManager::Allocate_Memory(const char * file, const char * function, int line, std::size_t size, MemAllocType type, void * address) 
{
    MemoryNode *memory;

    // If the memory manager has not yet been initialized due to the order in which static
    // variables are allocated, create the memory manager here.
    if (!MemoryManager && !Initialize_Memory_Manager()) {
        if (NumAllocations != 0) {
            Log("The Memory Manager has already been released from memory, however an allocation was requested");
            Log("\t%-40s", Format_Owner_String(file, function, line));
        }
        return ViniferaMemory::Alloc(size);  // Try to allocate the memory for the requesting process. 
    }

    if (size == 0) {
        size = 1; // ANSI states that allocation requests of size 0 should return a valid pointer.
    }
        
    // Has the user requested to break on the N-th allocation.
    MEM_ASSERT_PRINT(MemoryManager->TotalMemoryAllocations != MemoryManager->BreakOnAllocationCount);

    // If the type is UNKNOWN then this allocation was made from a source not set up to 
    // use memory tracking, include the MemoryManager header within the source to elimate
    // this error.
    MEM_ASSERT_PRINT(type != MM_UNKNOWN);

    if (type == MM_REALLOC) {
        memory = MemoryManager->Remove_Memory_Node(address);

        // Validate that the memory exists
        MEM_ASSERT_PRINT(memory != nullptr);
        if (!memory) {
            MemoryManager->Log("Request to reallocate RAM that was never allocated.");
        }

      // Validate that there is not a allocation/reallocation mismatch
        MEM_ASSERT_PRINT(memory->allocationType == MM_MALLOC ||
                         memory->allocationType == MM_CALLOC ||
                         memory->allocationType == MM_REALLOC);

        // Validate that a break point on reallocation has not been requested.
        MEM_ASSERT_PRINT((memory->options & OPT_BREAK_ON_REALLOC) == 0x0);

        memory->actualSize    = size + MemoryManager->PaddingSize * sizeof(long)*2;
        memory->reportedSize  = size;
        memory->actualAddress = ViniferaMemory::Realloc(memory->actualAddress, memory->actualSize);

    } else {
        // Create a new memory block for tracking the memory
        memory = MemoryManager->Allocate_Memory();

        // Validate the memory node allocation
        MEM_ASSERT_PRINT(memory != nullptr);
        if (memory == nullptr) {
            MemoryManager->Log("Could not allocate memory for memory tracking. Out of memory.");
        }

        memory->actualSize    = size + MemoryManager->PaddingSize * sizeof(long)*2;
        memory->reportedSize  = size;
        memory->actualAddress = ViniferaMemory::Alloc(memory->actualSize);
        memory->options       = OPT_NONE;
    }

    memory->reportedAddress   = (char*)memory->actualAddress + MemoryManager->PaddingSize * sizeof(long);
    memory->sourceLine        = line;
    memory->paddingSize       = MemoryManager->PaddingSize;
    memory->allocationType    = type;
    strncpy(memory->sourceFile, Source_File_Stripper(file), 30);
    strncpy(memory->sourceFunction, function, 256);
    memory->sourceFile[29] = '\0';
    memory->sourceFunction[255] = '\0';

    if (MemoryManager->LogAlways) {
        MemoryManager->Log("Memory Allocation        : %-40s %8s(0x%08p) : %s", Format_Owner_String(file, function, line),
                    AllocationTypes[type], memory->reportedAddress, Memory_Size_String(size));
    }

    // Validate the memory allocated
    MEM_ASSERT_PRINT(memory->actualAddress);
    if (!memory->actualAddress) {
        MemoryManager->Log("Request for allocation failed.  Out of memory.");
    }

    // Initialize the memory allocated for tracking upon deallocation
    if (type == MM_CALLOC) {
        memory->Initialize_Memory(EMPTY);
    } else {
        memory->Initialize_Memory(BODY);
    }

    // Insert the memory node into the hash table, this is a linked list hash table.
    MemoryManager->Insert_Memory_Node(memory);
    return memory->reportedAddress;
}

/*******************************************************************************************/

/**
 * ViniferaMemoryManager::Deallocate_Memory():
 *  This is the main memory de-allocation routine.  This method is used by all of the 
 *  other de-allocation routines for de-allocating and tracking memory.
 * 
 *  Return Type: void 
 *  Arguments: 
 *      void *address           : Theaddressof memory to be deallocated.
 *      MemAllocType type         : The type of deallocation being performed.
 */
void __cdecl ViniferaMemoryManager::Deallocate_Memory(void * address, MemAllocType type)
{
    // If the memory manager has not yet been initialized due to the order in which static
    // variables are allocated, create the memory manager here.
    if (!MemoryManager && !Initialize_Memory_Manager()) {
        ViniferaMemory::Free(address); // Release the memory
        if (NumAllocations != 0) {
            Log("The Memory Manager has already been released from memory, however a deallocation was requested");
        }
        return;
    }

    // The topStack contains the Logged information, such as file name and line number.
    StackNode *info = MemoryManager->TopStack.Empty() ? nullptr : MemoryManager->TopStack.Top();

    // Does the memory node exist within the hash table of the memory manager.
    MemoryNode *memory = MemoryManager->Remove_Memory_Node(address);

    if (!memory) {              // Validate that the memory was previously allocated.  If the memory was not Logged 
        ViniferaMemory::Free(address);  // by the memory manager simple free the memory and return.  We do not Log or 
        return;                 // create any errors since we want the memory manager to be as seemless as possible.
    }

    // Log the memory deallocation if desired.
    if (MemoryManager->LogAlways) {
          MemoryManager->Log("Memory Deallocation      : %-40s %8s(0x%08p) : %s", 
                            Format_Owner_String(info->fileName, info->functionName, info->lineNumber),
                            AllocationTypes[type], address, Memory_Size_String(memory->reportedSize));
    }

    // If the type is UNKNOWN then this allocation was made from a source not set up to 
    // use memory tracking, include the MemoryManager header within the source to elimate
    // this error.
    MEM_ASSERT_PRINT(type != MM_UNKNOWN);

    // Validate that no memory errors occured. If any errors have occured they will be written to the Log 
    // file by the Validate_Memory_Unit() method.
    MemoryManager->Validate_Memory_Unit(memory);

    // Validate that there is not a allocation/deallocation mismatch
    MEM_ASSERT_PRINT(type == MM_DELETE       && memory->allocationType == MM_NEW       ||
                     type == MM_DELETE_ARRAY && memory->allocationType == MM_NEW_ARRAY ||
                     type == MM_FREE         && memory->allocationType == MM_MALLOC    ||
                     type == MM_FREE         && memory->allocationType == MM_CALLOC    ||
                     type == MM_FREE         && memory->allocationType == MM_REALLOC);

    // Validate that a break on deallocate was not set
    MEM_ASSERT_PRINT((memory->options & OPT_BREAK_ON_DEALLOC) == 0x0);

    // Free the memory
    ViniferaMemory::Free(memory->actualAddress);

    // Free the memory used to create the Memory Node
    MemoryManager->Deallocate_Memory(memory);

    // Free the info node used to hold the file and line number information for this deallocation.
    if (info) {
        MemoryManager->TopStack.Pop();
        ViniferaMemory::Free(info);
    }
}


/**
 * Set_Owner():
 *  This method is only called by the delete macro defined within the MemoryManager.h header.
 *  It is responsible for Logging the file and line number for tracking information.  For
 *  an explanation for the stack implementation refer to the MemoryManager class definition.
 *
 *  Return Type : void 
 *  Arguments   : 
 *      const char * file  : The file requesting the deallocation.
 *      int line           : The line number within the file.
 */
void __cdecl ViniferaMemoryManager::Set_Owner(const char * file, const char * function, int line)
{
    if (MemoryManager) {
        StackNode *n = (StackNode *)ViniferaMemory::Alloc(sizeof(StackNode));
        n->fileName = file;
        n->functionName = function;
        n->lineNumber = line;
        MemoryManager->TopStack.Push(n);
    }
}


/**
 *  ViniferaMemMgrInitialize::ViniferaMemMgrInitialize():
 *    Initialize the Memory Manager Object.  This class is required to ensure that the Memory
 *    Manager has been created before dynamic allocation occure within other statically 
 *    allocated objects.
 * 
 *  Return Type : 
 *    Arguments   : NONE
 */
ViniferaMemoryManagerInitialize::ViniferaMemoryManagerInitialize()
{
    Initialize_Memory_Manager(); // Create the Memory Manager Object.
}


/**
 *  Overloaded New operators to use our memory manager functions.
 * 
 *  operator new() / operator new[]()
 *    Here is the overloaded new operator, responsible for allocating and tracking the requested memory.
 *
 *    Arguments: 
 *  	   size_t size	         : The size of memory requested in BYTES
 *  	   const char *file      : The file responsible for requesting the allocation.
 *  	   const char *function  : The function responsible for requesting the allocation.
 *  	   int line	             : The line number within the file requesting the allocation.
 * 
 *    Return Type:
 *         void* -> A pointer to the requested memory.
 */
void * __cdecl operator new(unsigned int size, const char * file, const char * function, int line) 
{
    return ViniferaMemoryManager::Allocate_Memory(file, function, line, size, ViniferaMemoryManager::MM_NEW); 
}

void * __cdecl operator new[](unsigned int size, const char * file, const char * function, int line)
{
    return ViniferaMemoryManager::Allocate_Memory(file, function, line, size, ViniferaMemoryManager::MM_NEW_ARRAY);
}


/**
 *  Overloaded Delete operators to use our memory manager functions.
 * 
 *  operator delete() / operator delete[]()
 *    This routine is responsible for de-allocating the requested memory.
 *
 *    Arguments: 
 *  	   void *address	: A pointer to the memory to be de-allocated.
 * 
 *    Return Type:
 *         void
 */
void __cdecl operator delete(void * address)
{
    /**
     *  ANSI states that delete will allow NULL pointers.
     */
    if (!address) {
        return;
    }

    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE);
}

void __cdecl operator delete[](void * address)
{
    /**
     *  ANSI states that delete will allow NULL pointers.
     */
    if (!address) {
        return;
    }
    
    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE_ARRAY);
}

void __cdecl operator delete(void * address, const std::nothrow_t &tag)
{
    /**
     *  ANSI states that delete will allow NULL pointers.
     */
    if (!address) {
        return;
    }

    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE);
}

void __cdecl operator delete[](void * address, const std::nothrow_t &tag)
{
    /**
     *  ANSI states that delete will allow NULL pointers.
     */
    if (!address) {
        return;
    }
    
    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE_ARRAY);
}


/**
 *  These routines should never get called when the memory manager is active, however
 *  they are provided here for clarity and to through.
 */
void * __cdecl operator new(unsigned int size)
{
    return ViniferaMemoryManager::Allocate_Memory(nullptr, nullptr, -1, size, ViniferaMemoryManager::MM_NEW);
}

void * __cdecl operator new[](unsigned int size)
{
    return ViniferaMemoryManager::Allocate_Memory(nullptr, nullptr, -1, size, ViniferaMemoryManager::MM_NEW_ARRAY);
}

void * __cdecl operator new(unsigned int size, const std::nothrow_t &tag)
{
    return ViniferaMemoryManager::Allocate_Memory(nullptr, nullptr, -1, size, ViniferaMemoryManager::MM_NEW);
}

void * __cdecl operator new[](unsigned int size, const std::nothrow_t &tag)
{
    return ViniferaMemoryManager::Allocate_Memory(nullptr, nullptr, -1, size, ViniferaMemoryManager::MM_NEW_ARRAY);
}

/**
 *  These two routines should never get called, unless an error occures during the 
 *  allocation process. These need to be defined to make Visual C++ 6.0 happy.
 *  If there was an allocation problem these method would be called automatically by 
 *  the operating system. C/C++ Users Journal (Vol. 19 No. 4 -> April 2001 pg. 60)  
 *  has an excellent explanation of what is going on here.
 */
void __cdecl operator delete(void * address, const char * file, const char * function, int line)
{
    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE);
}

void __cdecl operator delete[](void * address, const char * file, const char * function, int line)
{
    ViniferaMemoryManager::Deallocate_Memory(address, ViniferaMemoryManager::MM_DELETE_ARRAY);
}


#endif  /* ACTIVATE_MEMORY_MANAGER */
