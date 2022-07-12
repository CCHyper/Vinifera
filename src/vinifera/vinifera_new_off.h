/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_NEW_OFF.H
 *
 *  @author        Peter Dalton, Game Programming Gems 2, 2001.
 *
 *  @brief         This header allows for the Memory Manager to be turned on/off
 *                 seamlessly. Including this header will turn the Memory Manager off.
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
 *  Undefine all symbols defined by the Memory Manager.  This is important when including 
 *  header files that also overwrite the new/delete operators to avoid circular definitions
 *  that will result in unexpected behavior and syntax errors from the compiler.
 */
#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free
