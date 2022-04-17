/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_DEBUG.H
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
#include "debughandler.h"


/**
 *  
 */
#ifndef NDEBUG
#define XAUDIO2_DEBUG_INFO(x, ...) DEBUG_INFO(x, ##__VA_ARGS__)
#define XAUDIO2_DEBUG_WARNING(x, ...) DEBUG_WARNING(x, ##__VA_ARGS__)
#define XAUDIO2_DEBUG_ERROR(x, ...) DEBUG_ERROR(x, ##__VA_ARGS__)
#define XAUDIO2_DEBUG_FATAL(x, ...) DEBUG_FATAL(x, ##__VA_ARGS__)
#define XAUDIO2_DEBUG_TRACE(x, ...) DEBUG_TRACE(x, ##__VA_ARGS__)
#else
#define XAUDIO2_DEBUG_INFO(x, ...) ((void)0)
#define XAUDIO2_DEBUG_WARNING(x, ...) ((void)0)
#define XAUDIO2_DEBUG_ERROR(x, ...) ((void)0)
#define XAUDIO2_DEBUG_FATAL(x, ...) ((void)0)
#define XAUDIO2_DEBUG_TRACE(x, ...) ((void)0)
#endif
