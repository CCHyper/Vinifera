/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_NEW_ON.H
 *
 *  @author        Peter Dalton, Game Programming Gems 2, 2001.
 *
 *  @brief         This header allows for the Memory Manager to be turned on/off
 *                 seamlessly. Including this header will turn the Memory Manager on.
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


/**
 *  These #defines are the core of the memory manager. This overrides standard memory
 *  allocation and de-allocation routines and replaces them with the memory manager's versions.
 *  This allows for memory tracking and statistics to be generated.
 */
#define new              ViniferaMemoryManager::Allocate_Memory(__FILE__, "<todo>", __LINE__, ViniferaMemoryManager::MM_MALLOC) // new(__FILE__, "<todo>", __LINE__)
#define delete           (ViniferaMemoryManager::Set_Owner(__FILE__, "<todo>", __LINE__), false) ? ViniferaMemoryManager::Set_Owner("", "<todo>", 0) : delete
#define malloc(sz)       ViniferaMemoryManager::Allocate_Memory(__FILE__, "<todo>", __LINE__, sz, ViniferaMemoryManager::MM_MALLOC)
#define calloc(num, sz)  ViniferaMemoryManager::Allocate_Memory(__FILE__, "<todo>", __LINE__, sz * num, ViniferaMemoryManager::MM_CALLOC)
#define realloc(ptr, sz) ViniferaMemoryManager::Allocate_Memory(__FILE__, "<todo>", __LINE__, sz, ViniferaMemoryManager::MM_REALLOC, ptr)
#define free(sz)         ViniferaMemoryManager::Deallocate_Memory(__FILE__, "<todo>", __LINE__, sz, ViniferaMemoryManager::MM_FREE)
