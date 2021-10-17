/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PROGRESSSCREEN_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks and patches for ProgressScreenClass.
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
#include "progressscreen_hooks.h"
#include "progressscreen.h"
#include "tibsun_globals.h"
#include "colorscheme.h"
#include "rulesext.h"
#include "session.h"
#include "textprint.h"
#include "language.h"
#include "wwfont.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Draw the current text line to the loading screen. This is required as
 *  we can not allocate on the stack (see HiddenSurface->Get_Rect()).
 * 
 *  @author: CCHyper
 */
static void Draw_Loading_Screen_Text(const char *text, Point2D &point)
{
	ColorScheme *colorscheme;

	if (RulesClassExtension::UIControls.LoadingScreenTextColor != COLORSCHEME_NONE) {
		colorscheme = ColorSchemes[RulesClassExtension::UIControls.LoadingScreenTextColor];

	} else {
		colorscheme = ColorScheme::As_Pointer("Green", 1);
	}

	TextPrintType style = TPF_NOSHADOW;
	if (ScreenRect.Height <= 480) {
		style |= TPF_EFNT;

	} else {
		style |= TPF_6PT_GRAD;
	}

	/**
	 *  Kludge to compact the text so it fits in the box.
	 */
	Font_Ptr(TPF_6PT_GRAD)->Set_X_Spacing(0);

	Fancy_Text_Print(text, HiddenSurface, &HiddenSurface->Get_Rect(),
		&point, colorscheme, COLOR_TBLACK, style);

	Font_Ptr(TPF_6PT_GRAD)->Set_X_Spacing(1);
}


/**
 *  #issue-663
 * 
 *  Allow customisation of the loading screen text colour.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ProgressScreenClass_Draw_Bars_TextColor_Patch)
{
	GET_REGISTER_STATIC(ProgressScreenClass *, this_ptr, esi);
	GET_REGISTER_STATIC(int, row, edi);
	LEA_STACK_STATIC(Point2D *, textpoint, esp, 0x24);
	static int rowheight;

	if (ScreenRect.Height <= 480) {
		rowheight = 10;

	} else {
		rowheight = 14;
	}

	textpoint->X = this_ptr->XPos;
	textpoint->Y = rowheight * row + this_ptr->YPos;

	Draw_Loading_Screen_Text(Text_String(ProgressText[row].TextID), *textpoint);

	JMP_REG(ecx, 0x005ADF9C);
}


/**
 *  #issue-663
 * 
 *  Allow customisation of the loading screen text colour.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ProgressScreenClass_Draw_Graphics_TextColor_Patch)
{
	GET_REGISTER_STATIC(int, text_xpos, ecx);
	GET_REGISTER_STATIC(int, text_ypos, eax);
	GET_STACK_STATIC(int, player, esp, 0x48);
	LEA_STACK_STATIC(Point2D *, textpoint, esp, 0x4C);

	textpoint->X = text_xpos - 80;
	textpoint->Y = text_ypos;

	Draw_Loading_Screen_Text(Session.Players[player]->Name, *textpoint);

	JMP(0x005ADB7D);
}


/**
 *  Main function for patching the hooks.
 */
void ProgressScreenClassExtension_Hooks()
{
	Patch_Jump(0x005ADB24, &_ProgressScreenClass_Draw_Graphics_TextColor_Patch);
	Patch_Jump(0x005ADF3C, &_ProgressScreenClass_Draw_Bars_TextColor_Patch);
}
