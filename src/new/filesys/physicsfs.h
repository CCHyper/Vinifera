/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PHYSFSFILE.H
 *
 *  @author        CCHyper
 *
 *  @brief         Interface for loading files from zip files.
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

#if defined(VINIFERA_USE_PHYSICSFS)

#include "always.h"
#include "wstring.h"

 /* Define this, so the compiler doesn't complain about using old APIs. */
#define PHYSFS_DEPRECATED

extern "C" {
#include <physfs.h>
}


bool PhysicsFS_Init();
bool PhysicsFS_Shutdown();

inline bool PhysicsFS_AddArchive(Wstring archive, Wstring path = Wstring(), Wstring mount = Wstring(), bool append = true) { return false; }
Wstring PhysicsFS_AddArchive2(Wstring archive, Wstring path = Wstring(), Wstring mount = Wstring(), bool append = true);
bool PhysicsFS_RemoveArchive(Wstring archive);

bool PhysicsFS_AddPath(Wstring path, bool append = true);
bool PhysicsFS_RemovePath(Wstring archive);

bool PhysicsFS_IsMounted(Wstring filename);
bool PhysicsFS_IsFile(Wstring filename);
bool PhysicsFS_IsDirectory(Wstring filename);

bool PhysicsFS_IsAvailable(Wstring filename);
int PhysicsFS_Size(Wstring filename);
Wstring PhysicsFS_ModTime(Wstring filename);
Wstring PhysicsFS_CreateTime(Wstring filename);
Wstring PhysicsFS_AccessTime(Wstring filename);

bool PhysicsFS_Read(Wstring filename, void *buffer, int buffer_length);
bool PhysicsFS_Delete(Wstring filename);

void PhysicsFS_LogSearchPaths();



extern void PhysicsFS_Test();

#endif
