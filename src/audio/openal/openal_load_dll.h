/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_LOAD_DLL.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 OpenAL library functions from the DLL.
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

#include <al.h>
#include <alc.h>


/**
 *  Are all the required imports loaded?
 */
extern bool OpenALImportsLoaded;


bool Load_OpenAL_DLL();
void Unload_OpenAL_DLL();
