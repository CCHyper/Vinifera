/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Setup the crash handling functions to generate crash reports.
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "always.h"
#include "wstring.h"


#ifdef CRASHPAD_ENABLED

LONG Vinifera_Crashpad_Handler(unsigned int e_code, struct _EXCEPTION_POINTERS *e_info);

bool Crashpad_Setup();

extern bool IsCrashpadEnabled;
extern bool IsCrashpadUploadsAllowed;
extern Wstring CrashpadUploadURL;

#endif
