/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CRT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Setup all the hooks to take control of the basic CRT.
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
#include "crt_hooks.h"
#include <fenv.h>
#include "vinifera_newdel.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"

#include <fenv.h>
#include <cctype>


/**
 *  Redirect atexit() to use use the DLL's instance of std::atexit.
 */
static int __cdecl vinifera_atexit(void (__cdecl *Func)(void))
{
    return std::atexit(Func);
}


/**
 *  Redirect msize() to use HeapSize as we now control all memory allocations.
 */
static unsigned int __cdecl vinifera_msize(void *ptr)
{
    return HeapSize(GetProcessHeap(), 0, ptr);
}


/**
 *  Reimplementation of strdup() to use our allocator.
 */
static char * __cdecl vinifera_strdup(const char *string)
{
    char *str;
    char *p;
    int len = 0;

    while (string[len]) {
        len++;
    }
    str = (char *)vinifera_allocate(len + 1);
    p = str;
    while (*string) {
        *p++ = *string++;
    }
    *p = '\0';
    return str;
}


/**
 *  Set the FPU mode to match the game (rounding towards zero [chop mode]).
 */
DECLARE_PATCH(_set_fp_mode)
{
    // Call to "store_fpu_codeword"
    _asm { mov edx, 0x006B2314 };
    _asm { call edx };

    /**
     *  Set the FPU mode to match the game (rounding towards zero [chop mode]).
     */
    _set_controlfp(_RC_CHOP, _MCW_RC);

    /**
     *  And this is required for the std c++ lib.
     */
    fesetround(FE_TOWARDZERO);

    JMP(0x005FFDB0);
}


/**
 *  Main function for patching the hooks.
 */
void CRT_Hooks()
{
    /**
     *  Call the games fpmath to make sure we init 
     */
    Patch_Jump(0x005FFDAB, &_set_fp_mode);

    /**
     *  dynamic init functions call _msize indirectly.
     *  They call __onexit, so we need to patch this.
     */
    Hook_Function(0x006B80AA, &vinifera_msize);

    /**
     *  Redirect atexit() to use use the DLL's instance of std::atexit.
     */
    Hook_Function(0x006B4E5C, &std::atexit);

//___onexitinit	.text	006B4E6E	0000002F	00000000	00000000	R	.	L	.	.	.	.	.
//sub_6B4E9D	.text	006B4E9D	00000017	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B4EB4	.text	006B4EB4	00000001	00000000	00000000	R	.	.	.	.	.	.	.
//__cfltcvt_init_9_0	.text	006B4EB5	00000038	00000000	00000000	R	.	L	.	.	.	.	.
//sub_6B4EED	.text	006B4EED	00000010	00000000	00000004	R	.	.	.	.	.	.	.
//__CIpow	.text	006B4F00	00000019	00000010	00000000	R	.	L	.	.	.	.	.
//_pow	.text	006B4F19	00000009	00000000	00000010	R	.	L	.	.	.	T	.
//sub_6B4F22	.text	006B4F22	000001D3	0000008C	00000010	R	.	L	.	.	.	.	.
//sub_6B50F5	.text	006B50F5	00000028	00000000	00000000	R	.	L	.	.	.	.	.

//Hook_Function(0x006B51E5, &__purecall); // no found.

Hook_Function(0x006B51F0, &std::strncpy);
Hook_Function(0x006B52EE, &std::sprintf);
//Hook_Function(0x006B5340, &__toupper);
//Hook_Function(0x006B5348, &_toupper);
//Hook_Function(0x006B53B7, &_toupper_0); // huh?
Hook_Function(0x006B5483, &std::isalpha);
Hook_Function(0x006B54B1, &std::isupper);
Hook_Function(0x006B54D9, &std::islower);
Hook_Function(0x006B5501, &std::isdigit);
Hook_Function(0x006B5529, &std::isxdigit);
Hook_Function(0x006B5556, &std::isspace);
Hook_Function(0x006B557E, &std::ispunct);
Hook_Function(0x006B55A6, &std::isalnum);
Hook_Function(0x006B55D4, &std::isprint);
Hook_Function(0x006B5602, &std::isgraph);
Hook_Function(0x006B5630, &std::iscntrl);
//Hook_Function(0x006B5658, &isascii); // depeciated?
//Hook_Function(0x006B5665, &toascii); // depeciated?
//Hook_Function(0x006B566D, &iscsymf); // depeciated?
//Hook_Function(0x006B56AA, &iscsym); // depeciated?

//___RTCastToVoid	.text	006B56E7	00000050	00000038	00000004	R	.	L	.	.	B	T	.
//__thiscall __non_rtti_object::__non_rtti_object(char const *)	.text	006B5769	00000022	00000008	00000004	R	.	L	.	.	B	.	.
//sub_6B578B	.text	006B578B	0000001C	00000004	00000001	R	.	.	.	.	.	T	.
//sub_6B57A7	.text	006B57A7	00000005	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B57AC	.text	006B57AC	00000018	00000004	00000004	R	.	.	.	.	.	.	.
//sub_6B57C4	.text	006B57C4	00000018	00000004	00000004	R	.	.	.	.	.	.	.
//sub_6B57DC	.text	006B57DC	0000001C	00000004	00000001	R	.	.	.	.	.	T	.
//sub_6B57F8	.text	006B57F8	00000005	00000000	00000000	R	.	.	.	.	.	.	.
//___RTtypeid	.text	006B57FD	00000094	00000054	00000004	R	.	L	.	.	B	.	.
//sub_6B5891	.text	006B5891	00000014	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B58A5	.text	006B58A5	0000001E	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B58C3	.text	006B58C3	00000019	00000004	00000001	R	.	.	.	.	.	.	.
//___RTDynamicCast	.text	006B58DC	000000E5	0000005C	00000011	R	.	L	.	.	B	T	.
//sub_6B59C1	.text	006B59C1	00000014	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B59D5	.text	006B59D5	0000001E	00000000	00000000	R	.	.	.	.	.	.	.
//__thiscall bad_cast::bad_cast(char const * const &)	.text	006B59F3	00000018	00000004	00000004	R	.	L	.	.	.	.	.
//__thiscall bad_cast::`vector deleting destructor'(uint)	.text	006B5A0B	0000001C	00000004	00000001	R	.	.	.	.	.	T	.
//sub_6B5A27	.text	006B5A27	00000005	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B5A2C	.text	006B5A2C	00000018	00000004	00000004	R	.	.	.	.	.	.	.
//__cdecl FindCompleteObject(void * *)	.text	006B5A44	0000001A	00000000	00000004	R	.	L	.	S	.	T	.
//__cdecl FindSITargetTypeInstance(void *,_s_RTTICompleteObjectLocator const *,TypeDescriptor *,int,TypeDescriptor *)	.text	006B5A5E	0000005A	00000014	00000014	R	.	L	.	S	B	T	.
//__cdecl FindMITargetTypeInstance(void *,_s_RTTICompleteObjectLocator const *,TypeDescriptor *,int,TypeDescriptor *)	.text	006B5AB8	000000F9	0000001C	00000014	R	.	L	.	S	B	T	.
//__cdecl FindVITargetTypeInstance(void *,_s_RTTICompleteObjectLocator const *,TypeDescriptor *,int,TypeDescriptor *)	.text	006B5BB1	00000138	00000020	00000014	R	.	L	.	S	B	T	.
//__cdecl PMDtoOffset(void *,PMD const &)	.text	006B5CE9	00000023	00000000	00000008	R	.	L	.	S	.	T	.
    Hook_Function(0x006B5D0C, &std::qsort);
    //Hook_Function(0x006B5E60, &std::_shortsort); // not found?
    Hook_Function(0x006B5EAE, &std::swap);
    Hook_Function(0x006B5EDA, &std::atol);
    Hook_Function(0x006B5F65, &std::atoi);
    Hook_Function(0x006B5F70, &_atoi64);
    Hook_Function(0x006B602A, &std::strtok);
    Hook_Function(0x006B60CE, &std::atof);
    //Hook_Function(0x006B6125, &strtoxl_);
    //Hook_Function(0x006B613C, &_strtoxl);
    Hook_Function(0x006B6341, &std::strtoul);
    Hook_Function(0x006B6358, &_makepath);
    Hook_Function(0x006B63F0, &std::memcpy);
    //Hook_Function(0x006B6730, &std::strstr);
    Hook_Function(0x006B67B0, &std::sscanf);
    Hook_Function(0x006B67E4, &std::free);
    Hook_Function(0x006B6830, &std::strncmp);
    //Hook_Function(0x006B6868, &std::time);
    Hook_Function(0x006B6944, &std::fclose);
    //Hook_Function(0x006B6975, &__fclose_lk);
    Hook_Function(0x006B69C1, &std::fprintf);
    //Hook_Function(0x006B69FD, &__fsopen);
    Hook_Function(0x006B6A2E, &std::fopen);
    Hook_Function(0x006B6A41, &std::vsprintf);
    //Hook_Function(0x006B6AA0, &std::strchr___found_bx);
    //Hook_Function(0x006B6AB0, &std::strchr);
    //Hook_Function(0x006B6B70, &__alloca_probe);
    Hook_Function(0x006B6BA0, &std::strncat);
    //Hook_Function(0x006B6CC3, &__tolower);
    Hook_Function(0x006B6CCB, &_tolower);
    //Hook_Function(0x006B6D3A, &_tolower_0);
    Hook_Function(0x006B6E05, &std::puts);
    //Hook_Function(0x006B6E7D, &__cinit);
    Hook_Function(0x006B6EAA, &std::exit);
    //Hook_Function(0x006B6EBB, &__exit);
    //Hook_Function(0x006B6ECC, &__cexit);
    //Hook_Function(0x006B6EDB, &__c_exit);
    //Hook_Function(0x006B6EEA, &_doexit);
    //Hook_Function(0x006B6F8F, &__lockexit);
    //Hook_Function(0x006B6F98, &__unlockexit);
    //Hook_Function(0x006B6FA1, &__initterm);
    //Hook_Function(0x006B6FBB, &_put_6B6FBB);
    //Hook_Function(0x006B6FF6, &___initstdio);
    //Hook_Function(0x006B709E, &___endstdio);
    //Hook_Function(0x006B70B2, &__lock_file);
    //Hook_Function(0x006B70E1, &__lock_file2);
    //Hook_Function(0x006B7104, &__unlock_file);
    //Hook_Function(0x006B7133, &__unlock_file2);
    Hook_Function(0x006B7156, &std::srand);
    Hook_Function(0x006B7163, &std::rand);
    //Hook_Function(0x006B7185, &__splitpath);
    //Hook_Function(0x006B72CC, &_malloc);
    //Hook_Function(0x006B72DE, &_nh_malloc);
    //Hook_Function(0x006B730A, &_heap_alloc);
    Hook_Function(0x006B7358, &std::fgetc);
    Hook_Function(0x006B7389, &_fgetchar);
    Hook_Function(0x006B73A0, &_strcmpi);
    //Hook_Function(0x006B7470, &_ftime);
    Hook_Function(0x006B7555, &std::wcslen);
    Hook_Function(0x006B7572, &std::getenv);
    //Hook_Function(0x006B7593, &_getenv);
    Hook_Function(0x006B7610, &_statusfp);
    Hook_Function(0x006B7623, &_clearfp);
//__control87	.text	006B7637	00000035	0000000C	00000005	R	.	L	.	.	B	T	.
//__controlfp	.text	006B766C	00000016	00000000	00000005	R	.	L	.	.	.	T	.
//__fpreset	.text	006B7682	0000002B	00000004	00000000	R	.	L	.	.	.	T	.
//__abstract_cw	.text	006B76AD	00000092	0000000C	00000004	R	.	L	.	S	.	.	.
//__hw_cw	.text	006B773F	00000089	00000008	00000004	R	.	L	.	S	.	.	.
//__abstract_sw	.text	006B77C8	00000035	00000000	00000001	R	.	L	.	S	.	.	.
//__global_unwind2	.text	006B7800	00000020	00000014	00000001	R	.	L	.	.	B	T	.
//__unwind_handler	.text	006B7820	00000022	00000000	00000010	R	.	L	.	S	.	.	.
//__local_unwind2	.text	006B7842	00000068	00000014	00000008	R	.	L	.	.	.	.	.
//__abnormal_termination	.text	006B78AA	00000023	00000000	00000000	R	.	L	.	.	.	T	.
//__NLG_Notify1	.text	006B78CD	00000009	00000004	00000000	R	.	L	.	.	.	.	.
//__NLG_Notify	.text	006B78D6	00000018	00000004	00000000	R	.	L	.	.	.	.	.
//sub_6B78F0	.text	006B78F0	00000008	00000004	00000000	R	.	L	.	.	.	.	.
//__except_handler3	.text	006B78F8	000000BD	0000001C	0000000C	R	.	L	.	.	B	T	.
// _seh_longjmp_unwind(x)	.text	006B79B5	0000001B	00000004	00000004	R	.	L	.	.	.	.	.
//_floor	.text	006B79D0	000000CF	00000028	00000008	R	.	L	.	.	B	T	.
    //Hook_Function(0x006B7AA0, &std::strrchr); // needs casting
//_remove	.text	006B7AC7	0000002A	00000000	00000001	R	.	L	.	.	.	T	.
//__unlink	.text	006B7AF1	0000000B	00000000	00000001	R	.	L	.	.	.	T	.
//_tmpnam	.text	006B7AFC	0000007E	0000000C	00000001	R	.	L	.	.	.	T	.
//_tmpfile	.text	006B7B7A	000000D6	00000010	00000000	R	.	L	.	.	.	T	.
//_init_namebuf	.text	006B7C50	00000061	00000004	00000004	R	.	L	.	S	.	.	.
//_genfname	.text	006B7CB1	00000046	00000008	00000001	R	.	L	.	S	B	T	.
//sub_6B7CF7	.text	006B7CF7	00000007	00000000	00000000	R	.	L	.	.	.	.	.
//__make_time_t	.text	006B7CFE	00000019	00000000	00000001	R	.	L	.	.	.	T	.
//__strnicmp	.text	006B7D20	00000101	00000014	00000009	R	.	L	.	.	B	T	.
//sub_6B7F0A	.text	006B7F0A	00000014	00000000	00000000	R	.	.	.	.	.	.	.
//sub_6B7F1E	.text	006B7F1E	0000000B			R	.	.	.	.	.	.	.
//__amsg_exit	.text	006B7F29	00000022	00000000	00000001	R	.	L	.	.	.	.	.
//_fast_error_exit	.text	006B7F4E	00000024	00000000	00000001	R	.	L	.	S	.	.	.
//__setdefaultprecision	.text	006B80EF	00000012	00000000	00000000	R	.	L	.	.	.	.	.
//__ms_p5_test_fdiv	.text	006B8101	0000003E	0000001C	00000000	R	.	L	.	.	B	.	.
//__ms_p5_mp_test_fdiv	.text	006B813F	00000029	00000000	00000000	R	.	L	.	.	.	.	.
//__forcdecpt	.text	006B8168	0000005A	00000004	00000004	R	.	L	.	.	.	.	.
//__fassign	.text	006B8228	0000003E	0000000C	0000000C	R	.	L	.	.	B	.	.
//__cftoe	.text	006B8266	00000061	00000040	00000010	R	.	L	.	.	B	.	.
//__cftoe2	.text	006B82C7	000000C2	00000010	00000011	R	.	L	.	S	B	.	.
//__cftof	.text	006B8389	00000055	00000040	00000009	R	.	L	.	.	B	.	.
//__cftof2	.text	006B83DE	000000A7	00000010	0000000D	R	.	L	.	S	B	T	.
//__cftog	.text	006B8485	00000093	00000048	00000010	R	.	L	.	.	B	T	.
//__cfltcvt	.text	006B8518	00000051	00000004	00000014	R	.	L	.	.	B	T	.
//__shift	.text	006B8569	00000025	00000004	00000008	R	.	L	.	S	.	T	.
//zerotoxdone	.text	006B8677	00000001	00000000	00000000	R	.	.	.	.	.	.	.
//__ffexpm1___ExpArgOutOfRange	.text	006B86A0	0000002F	00000000	00000000	R	.	.	.	.	.	.	.
//__ffexpm1	.text	006B86EE	00000043	00000000	00000000	R	.	L	.	.	.	.	.
//_isintTOS	.text	006B8731	00000034	00000000	00000000	R	.	L	.	.	.	.	.
//noerror	.text	006B8796	00000001	00000000	00000000	R	.	.	.	.	.	.	.
//__trandisp1	.text	006B87A0	00000067	00000000	00000000	R	.	L	.	.	.	.	.
//__rttosnpop	.text	006B889C	00000001	00000000	00000000	R	.	L	.	.	.	.	.
//__ffexpm1_____rtzeronpop	.text	006B88A6	00000005	00000000	00000000	R	.	L	.	.	.	.	.
//__rttosnpopde	.text	006B8959	0000000A	00000000	00000000	R	.	L	.	.	.	.	.
//chsifnegret	.text	006B8969	00000001	00000000	00000000	R	.	L	.	.	.	.	.
//__startTwoArgErrorHandling	.text	006B8970	00000017	00000024	00000018	R	.	L	.	.	B	.	.
//__startOneArgErrorHandling	.text	006B8987	0000003C	00000024	00000010	R	.	L	.	.	B	.	.
//__twoToTOS	.text	006B89D0	00000015	00000000	00000000	R	.	L	.	.	.	.	.
//__load_CW	.text	006B89E5	00000017	00000000	00000004	R	.	L	.	.	.	.	.
//__fload_withFB	.text	006B8A15	00000043	0000000C	00000000	R	.	L	.	.	.	.	.
//__check_overflow_exit	.text	006B8AA5	00000014	00000008	00000000	R	.	L	.	.	.	.	.
//__check_range_exit	.text	006B8AB9	000000A3	00000008	00000000	R	.	L	.	.	.	.	.
//__powhlp	.text	006B8B5C	00000130	00000018	00000014	R	.	L	.	.	B	T	.
//__d_inttype	.text	006B8C8C	00000065	00000014	00000008	R	.	L	.	.	B	T	.
//__mtinitlocks	.text	006B8CF1	00000029	00000004	00000000	R	.	L	.	.	.	.	.
//__mtdeletelocks	.text	006B8D1A	0000006C	00000008	00000000	R	.	L	.	.	.	.	.
//__lock	.text	006B8D86	00000061	00000008	00000004	R	.	L	.	.	B	.	.
//__unlock	.text	006B8DE7	00000015	00000004	00000004	R	.	L	.	.	B	.	.
//sub_6B8E08	.text	006B8E08	0000000B	00000000	00000000	R	.	L	.	.	.	.	.
//sub_6B8E13	.text	006B8E13	00000001	00000000	00000000	R	.	L	.	.	.	.	.
    Hook_Function(0x006B8E20, &std::strcmp);
//__flsbuf	.text	006B8EA4	00000118	0000000C	00000005	R	.	L	.	.	B	T	.
//__output	.text	006B8FBC	00000721	00000258	0000000C	R	.	L	.	.	B	T	.
//_write_char	.text	006B96FD	00000035	00000004	0000000C	R	.	L	.	S	B	T	.
//_write_multi_char	.text	006B9732	00000031	00000008	00000010	R	.	L	.	S	.	T	.
//_write_string	.text	006B9763	00000038	0000000C	00000010	R	.	L	.	S	.	T	.
//_get_int_arg	.text	006B979B	0000000D	00000000	00000004	R	.	L	.	S	.	.	.
//_get_int64_arg	.text	006B97A8	00000010	00000000	00000004	R	.	L	.	S	.	.	.
//_get_short_arg	.text	006B97B8	0000000E	00000000	00000004	R	.	L	.	S	.	.	.
//_setlocale	.text	006B97C6	00000246	00000094	00000005	R	.	L	.	.	B	T	.
//__setlocale_set_cat	.text	006B9A0C	0000011B	000000B4	00000005	R	.	L	.	S	B	T	.
//__setlocale_get_all	.text	006B9B27	000000B9	00000010	00000000	R	.	L	.	S	.	.	.
//__expandlocale	.text	006B9BE0	00000118	00000098	00000010	R	.	L	.	.	B	T	.
//___init_dummy	.text	006B9CF8	00000003	00000000	00000000	R	.	L	.	.	.	.	.
//__strcats	.text	006B9CFB	00000025	00000004	00000008	R	.	L	.	.	.	T	.
//___lc_strtolc	.text	006B9D20	000000CC	00000010	00000005	R	.	L	.	.	B	T	.
//___lc_lctostr	.text	006B9DEC	00000053	00000004	00000005	R	.	L	.	.	.	T	.
//___crtLCMapStringA	.text	006B9E3F	00000224	0000003C	00000020	R	.	L	.	.	B	T	.
//_strncnt	.text	006BA063	0000002B	00000004	00000008	R	.	L	.	S	.	.	.
//__isctype	.text	006BA08E	00000075	00000008	00000005	R	.	L	.	.	B	T	.
//__allmul	.text	006BA240	00000034	00000000	00000010	R	.	L	.	.	.	.	.
//__mtinit	.text	006BA274	00000054	00000004	00000000	R	.	L	.	.	.	.	.
//__initptd	.text	006BA2E6	00000013	00000000	00000004	R	.	L	.	.	.	.	.
//__getptd	.text	006BA2F9	00000067	00000008	00000000	R	.	L	.	.	.	.	.
//sub_6BA400	.text	006BA400	00000006	00000000	00000000	R	.	.	.	.	.	T	.
//sub_6BA406	.text	006BA406	00000006	00000000	00000000	R	.	.	.	.	.	T	.
//__fltin2	.text	006BA40C	0000007F	00000028	00000008	R	.	L	.	.	B	.	.
//_strlen	.text	006BA490	0000007B	00000000	00000001	R	.	L	.	.	.	T	.
//__dosmaperr	.text	006BA50B	00000073	00000004	00000004	R	.	L	.	.	.	.	.
//__errno	.text	006BA57E	00000009	00000000	00000000	R	.	L	.	.	.	T	.
//___doserrno	.text	006BA587	00000009	00000000	00000000	R	.	L	.	.	.	T	.
//__mbsdec	.text	006BA590	0000006D	00000008	00000008	R	.	L	.	.	.	T	.
//__input	.text	006BA5FD	00000A25	000001D4	0000000C	R	.	L	.	.	B	T	.
//__hextodec	.text	006BB022	00000037	00000004	00000001	R	.	L	.	S	.	T	.
//_fgetc_0	.text	006BB059	0000001A	00000000	00000001	R	.	L	.	.	.	T	.
//__un_inc	.text	006BB073	00000017	00000000	00000005	R	.	L	.	S	.	T	.
//__whiteout	.text	006BB08A	00000024	00000008	00000005	R	.	L	.	S	.	T	.
//__heap_init	.text	006BB0AE	0000003C	00000000	00000004	R	.	L	.	.	.	.	.
//__heap_term	.text	006BB0EA	00000075	00000008	00000000	R	.	L	.	.	.	.	.
//sub_6BB15F	.text	006BB15F	00000006	00000000	00000000	R	.	.	.	.	.	.	.
//__set_sbh_threshold	.text	006BB165	00000017	00000000	00000001	R	.	L	.	.	.	T	.
//___sbh_heap_init	.text	006BB17C	0000003E	00000000	00000000	R	.	L	.	.	.	.	.
//___sbh_find_block	.text	006BB1BA	0000002B	00000000	00000004	R	.	L	.	.	.	.	.
//___sbh_free_block	.text	006BB1E5	0000032B	00000024	00000008	R	.	L	.	.	B	.	.
//___sbh_alloc_block	.text	006BB510	00000309	00000024	00000004	R	.	L	.	.	B	.	.
//___sbh_alloc_new_region	.text	006BB819	000000B1	00000008	00000000	R	.	L	.	.	.	.	.
//___sbh_alloc_new_group	.text	006BB8CA	000000FB	00000014	00000004	R	.	L	.	.	B	.	.
//___sbh_resize_block	.text	006BB9C5	000002F6	0000001C	0000000C	R	.	L	.	.	B	.	.
//___sbh_heap_check	.text	006BBD8C	0000032F	0000014C	00000000	R	.	L	.	.	B	.	.
//___loctotime_t	.text	006BC0BB	000000C2	0000002C	0000001C	R	.	L	.	.	B	.	.
//__close	.text	006BC17D	0000005D	00000004	00000004	R	.	L	.	.	.	T	.
//__close_lk	.text	006BC1DA	00000083	00000008	00000004	R	.	L	.	.	.	.	.
//__freebuf	.text	006BC25D	0000002B	00000004	00000004	R	.	L	.	.	.	.	.
//_fflush	.text	006BC288	0000002F	00000004	00000001	R	.	L	.	.	.	T	.
//__fflush_lk	.text	006BC2B7	0000002E	00000004	00000004	R	.	L	.	.	.	.	.
//__flush	.text	006BC2E5	0000005C	0000000C	00000004	R	.	L	.	.	.	.	.
//__flushall	.text	006BC341	00000009	00000000	00000000	R	.	L	.	.	.	T	.
//_flsall	.text	006BC34A	000000A4	0000000C	00000004	R	.	L	.	S	.	.	.
//__stbuf	.text	006BC3EE	0000008D	00000004	00000004	R	.	L	.	.	.	.	.
//__ftbuf	.text	006BC47B	0000002A	00000004	00000008	R	.	L	.	.	.	.	.
//__openfile	.text	006BC4A5	00000170	00000018	00000010	R	.	L	.	.	B	.	.
//__getstream	.text	006BC615	000000C8	0000000C	00000000	R	.	L	.	.	.	.	.
//sub_6BC6DD	.text	006BC6DD	0000002F	00000008	0000000D	R	.	.	.	.	B	.	.
    Hook_Function(0x006BC70C, &std::fwrite);
//__ioinit	.text	006BC816	000001BC	00000058	00000000	R	.	L	.	.	B	.	.
//__ioterm	.text	006BC9D2	00000054	0000000C	00000000	R	.	L	.	.	.	.	.
//__fcloseall	.text	006BCAB3	00000081	00000008	00000000	R	.	L	.	.	.	T	.
//__setmbcp	.text	006BCB34	000001AD	00000028	00000004	R	.	L	.	.	B	T	.
//_getSystemCP	.text	006BCCE1	0000004A	00000000	00000004	R	.	L	.	S	.	.	.
//_CPtoLCID	.text	006BCD2B	00000033	00000000	00000004	R	.	L	.	S	.	.	.
//_setSBCS	.text	006BCD5E	00000029	00000004	00000000	R	.	L	.	S	.	.	.
//_setSBUpLow	.text	006BCD87	00000185	0000051C	00000000	R	.	L	.	S	B	.	.
//__getmbcp	.text	006BCF0C	00000010	00000000	00000000	R	.	L	.	.	.	T	.
//___initmbctable	.text	006BCF1C	0000001C	00000000	00000000	R	.	L	.	.	.	.	.
//__mbsnbcpy	.text	006BCF38	0000009A	00000008	00000009	R	.	L	.	.	B	T	.
//__cdecl _set_new_handler(int (__cdecl *)(uint))	.text	006BCFD2	00000024	00000004	00000004	R	.	L	.	.	.	T	.
//sub_6BCFF6	.text	006BCFF6	00000006	00000000	00000000	R	.	.	.	.	.	.	.
//__callnewh	.text	006BCFFC	0000001B	00000000	00000004	R	.	L	.	.	.	T	.
//__filbuf	.text	006BD017	000000DC	00000004	00000001	R	.	L	.	.	.	T	.
//___tzset	.text	006BD0F3	0000002E	00000000	00000000	R	.	L	.	.	.	.	.
//__tzset	.text	006BD121	00000016	00000000	00000000	R	.	L	.	.	.	T	.
//__tzset_lk	.text	006BD137	00000287	0000001C	00000000	R	.	L	.	S	B	.	.
//__isindst	.text	006BD3BE	00000021	00000004	00000004	R	.	L	.	.	.	.	.
//__isindst_0	.text	006BD3DF	000001AC	0000000C	00000004	R	.	L	.	.	.	.	.
//_cvtdate	.text	006BD58B	00000140	0000000C	0000002C	R	.	L	.	S	B	.	.
//__aullrem	.text	006BD6D0	00000075	00000004	00000010	R	.	L	.	.	.	.	.
//__aulldiv	.text	006BD750	00000068	00000008	00000010	R	.	L	.	.	.	.	.
//__mbsnbicoll	.text	006BD7B8	0000003F	00000004	00000009	R	.	L	.	.	B	T	.
//___wtomb_environ	.text	006BD7F7	0000006E	00000014	00000000	R	.	L	.	.	.	.	.
//_signal	.text	006BD865	00000182	00000010	00000008	R	.	L	.	.	B	T	.
//__stdcall ctrlevent_capture(x)	.text	006BD9E7	00000062	0000000C	00000004	R	.	L	.	S	.	T	.
//_raise	.text	006BDA49	00000182	0000001C	00000004	R	.	L	.	.	B	T	.
//_siglookup	.text	006BDBCB	0000003D	00000008	00000008	R	.	L	.	S	.	.	.
//sub_6BDC08	.text	006BDC08	00000009	00000000	00000000	R	.	.	.	.	.	.	.
//___pxcptinfoptrs	.text	006BDC11	00000009	00000000	00000000	R	.	L	.	.	.	T	.
//__handle_qnan1	.text	006BDC1A	00000054	00000020	00000010	R	.	L	.	.	B	T	.
//__handle_qnan2	.text	006BDC6E	00000060	00000028	00000018	R	.	L	.	.	B	T	.
//__except1	.text	006BDCCE	00000098	00000078	0000001C	R	.	L	.	.	B	T	.
//__except2	.text	006BDD66	000000A5	00000078	00000024	R	.	L	.	.	B	T	.
//__raise_exc	.text	006BDE0B	000002B3	00000010	00000018	R	.	L	.	.	B	T	.
//__handle_exc	.text	006BE0BE	00000217	00000028	0000000C	R	.	L	.	.	B	.	.
//__umatherr	.text	006BE2D5	00000088	00000024	00000024	R	.	L	.	.	B	T	.
//__set_errno	.text	006BE35D	00000028	00000000	00000001	R	.	L	.	.	.	T	.
//__get_fname	.text	006BE385	00000025	00000000	00000004	R	.	L	.	S	.	.	.
//__errcode	.text	006BE3AA	0000002D	00000000	00000001	R	.	L	.	.	.	.	.
//__frnd	.text	006BE3D7	00000012	0000000C	00000008	R	.	L	.	.	B	T	.
//__set_exp	.text	006BE3E9	00000029	0000000C	0000000C	R	.	L	.	.	B	T	.
//__set_bexp	.text	006BE452	00000024	0000000C	0000000C	R	.	L	.	.	B	T	.
//__sptype	.text	006BE476	0000005A	00000004	00000008	R	.	L	.	.	B	.	.
//__decomp	.text	006BE4D0	000000C1	0000001C	0000000C	R	.	L	.	.	B	T	.
//__statfp	.text	006BE591	0000000E	00000006	00000000	R	.	L	.	.	B	.	.
//__clrfp	.text	006BE59F	0000000F	00000006	00000000	R	.	L	.	.	B	.	.
//__ctrlfp	.text	006BE5AE	00000023	00000008	00000008	R	.	L	.	.	B	.	.
//__set_statfp	.text	006BE5D1	00000056	0000000C	00000004	R	.	L	.	.	B	.	.
    Hook_Function(0x006BE630, &std::strcpy);
    Hook_Function(0x006BE640, &std::strcat);
//__access	.text	006BE720	00000046	00000000	00000005	R	.	L	.	.	.	T	.
    Hook_Function(0x006BE766, &vinifera_strdup);
//__open	.text	006BE791	00000017	00000000	0000000C	R	.	L	.	.	.	T	.
//__sopen	.text	006BE7A8	000002CF	0000002C	00000010	R	.	L	.	.	B	T	.
//__itoa	.text	006BEA77	0000002D	00000004	00000009	R	.	L	.	.	B	T	.
//_xtoa	.text	006BEAA4	0000005C	00000010	00000010	R	.	L	.	S	B	.	.
//__ltoa	.text	006BEB00	0000002A	00000004	00000009	R	.	L	.	.	B	T	.
//__ultoa	.text	006BEB2A	0000001B	00000000	00000009	R	.	L	.	.	.	T	.
//__i64toa	.text	006BEB45	00000031	00000004	0000000D	R	.	L	.	.	B	T	.
//__stdcall x64toa(x,x,x,x,x)	.text	006BEB76	00000086	00000010	00000014	R	.	L	.	S	B	.	.
//__ui64toa	.text	006BEBFC	0000001B	00000004	0000000D	R	.	L	.	.	B	T	.
//GetCurrentProcessId_0	.text	006BEC17	00000006	00000000	00000000	R	.	L	.	.	.	T	.
//__mbsrchr	.text	006BEC1D	00000072	00000008	00000008	R	.	L	.	.	B	T	.
//__rmtmp	.text	006BEC8F	0000006D	00000008	00000000	R	.	L	.	.	.	T	.
//_asctime	.text	006BECFC	000000CA	00000010	00000001	R	.	L	.	.	.	T	.
//_store_dt	.text	006BEDC6	00000028	00000004	00000008	R	.	L	.	S	.	.	.
//_localtime	.text	006BEDEE	00000160	00000008	00000001	R	.	L	.	.	B	T	.
//__XcptFilter	.text	006BEF4E	0000013E	00000010	00000005	R	.	L	.	.	B	T	.
//_xcptlookup	.text	006BF08C	0000003A	00000008	00000008	R	.	L	.	S	.	.	.
//__wincmdln	.text	006BF0C6	00000058	00000000	00000000	R	.	L	.	.	.	.	.
//__setenvp	.text	006BF11E	000000B9	0000000C	00000000	R	.	L	.	.	.	.	.
//__setargv	.text	006BF1D7	00000099	00000018	00000000	R	.	L	.	.	B	.	.
//_parse_cmdline	.text	006BF270	000001B4	00000010	00000014	R	.	L	.	S	B	.	.
//___crtGetEnvironmentStringsA	.text	006BF424	00000132	00000018	00000000	R	.	L	.	.	.	.	.
//__FF_MSGBANNER	.text	006BF556	00000039	00000000	00000000	R	.	L	.	.	.	.	.
//__NMSG_WRITE	.text	006BF58F	00000153	000001A8	00000001	R	.	L	.	.	B	T	.
//_memcpy_0_0	.text	006BF720	00000335	0000000C	00000009	R	.	L	.	.	B	T	.
//__ZeroTail	.text	006BFA55	00000049	00000004	00000008	R	.	L	.	.	.	.	.
//__IncMan	.text	006BFA9E	00000056	0000000C	00000008	R	.	L	.	.	.	.	.
//__RoundMan	.text	006BFAF4	0000008C	00000018	00000008	R	.	L	.	.	B	.	.
//__CopyMan	.text	006BFB80	0000001B	00000004	00000008	R	.	L	.	.	.	.	.
//__FillZeroMan	.text	006BFB9B	0000000C	00000004	00000004	R	.	L	.	.	.	.	.
//__IsZeroMan	.text	006BFBA7	0000001B	00000000	00000004	R	.	L	.	.	.	.	.
//__ShrMan	.text	006BFBC2	0000008D	0000001C	00000008	R	.	L	.	.	B	.	.
//__ld12cvt	.text	006BFC4F	0000016C	00000028	0000000C	R	.	L	.	.	B	.	.
//__atodbl	.text	006BFDBB	00000016	00000000	00000005	R	.	L	.	.	.	T	.
//__atoflt	.text	006BFDD1	00000016	00000000	00000005	R	.	L	.	.	.	T	.
//__ld12told	.text	006BFDE7	0000007A	0000001C	00000008	R	.	L	.	.	B	.	.
//sub_6BFE61	.text	006BFE61	0000002D	00000010	00000008	R	.	L	.	.	B	.	.
//__atoldbl	.text	006BFE8E	0000002E	00000010	00000005	R	.	L	.	.	B	T	.
//sub_6BFEBC	.text	006BFEBC	0000002D	00000010	00000008	R	.	L	.	.	B	.	.
//__fptostr	.text	006BFEE9	00000077	00000010	0000000C	R	.	L	.	.	B	T	.
//__fltout2	.text	006BFF60	0000005C	00000034	0000000D	R	.	L	.	.	B	T	.
//___dtold	.text	006BFFBC	000000B6	00000014	00000008	R	.	L	.	.	B	.	.
    Hook_Function(0x006C0080, &std::memset);
//__fptrap	.text	006C00D8	00000009	00000000	00000000	R	.	L	.	.	.	.	.
//sub_6C00F0	.text	006C00F0	00000117	00000000	00000020	R	.	L	.	.	.	T	.
//sub_6C0207	.text	006C0207	0000049F	0000002C	00000012	R	.	L	.	.	.	.	.
//__fdivp_sti_st	.text	006C06A6	00000013	0000002C	00000000	R	.	L	.	.	.	.	.
//__fdivrp_sti_st	.text	006C06B9	00000013	0000002C	00000000	R	.	L	.	.	.	.	.
//__adj_fdiv_m32	.text	006C06CC	0000004C	00000010	00000004	R	.	L	.	.	.	.	.
//__adj_fdiv_m64	.text	006C0718	0000004C	00000010	00000008	R	.	L	.	.	.	.	.
//__adj_fdiv_m16i	.text	006C0764	00000034	00000010	00000002	R	.	L	.	.	.	.	.
//__adj_fdiv_m32i	.text	006C0798	00000034	00000010	00000004	R	.	L	.	.	.	.	.
//__adj_fdivr_m32	.text	006C07CC	0000004C	00000010	00000004	R	.	L	.	.	.	.	.
//__adj_fdivr_m64	.text	006C0818	0000004C	00000010	00000008	R	.	L	.	.	.	.	.
//__adj_fdivr_m16i	.text	006C0864	00000034	00000010	00000002	R	.	L	.	.	.	.	.
//__adj_fdivr_m32i	.text	006C0898	00000034	00000010	00000004	R	.	L	.	.	.	.	.
//__safe_fdiv	.text	006C08CC	00000015	00000030	00000000	R	.	L	.	.	.	.	.
//__safe_fdivr	.text	006C08E1	00000015	00000030	00000000	R	.	L	.	.	.	.	.
//__fprem_common	.text	006C08F6	00000206	00000028	00000030	R	.	L	.	.	.	.	.
//__adj_fprem	.text	006C0AFC	000000B2	00000034	00000000	R	.	L	.	.	.	.	.
//__fprem1_common	.text	006C0BAE	00000206	00000028	00000030	R	.	L	.	.	.	.	.
//__adj_fprem1	.text	006C0DB4	000000B5	00000034	00000000	R	.	L	.	.	.	.	.
//__safe_fprem	.text	006C0E69	00000006	00000000	00000000	R	.	L	.	.	.	.	.
//__safe_fprem1	.text	006C0E6F	00000006	00000000	00000000	R	.	L	.	.	.	.	.
//__adj_fpatan	.text	006C0E75	00000003	00000000	00000000	R	.	L	.	.	.	.	.
//__adj_fptan	.text	006C0E78	00000003	00000000	00000000	R	.	L	.	.	.	.	.
//__87except	.text	006C0E7B	000000D0	00000068	0000000C	R	.	L	.	.	B	.	.
//__copysign	.text	006C0F4B	00000021	0000000C	00000010	R	.	L	.	.	B	T	.
//__chgsign	.text	006C0F6C	00000025	0000000C	00000001	R	.	L	.	.	B	T	.
//__scalb	.text	006C0F91	00000016	0000000C	00000009	R	.	L	.	.	.	T	.
//__logb	.text	006C0FA7	000000E7	00000024	00000001	R	.	L	.	.	B	T	.
//__nextafter	.text	006C108E	00000295	00000048	00000009	R	.	L	.	.	B	T	.
//__finite	.text	006C1323	00000014	00000000	0000000A	R	.	L	.	.	.	T	.
//__isnan	.text	006C1337	0000002E	00000000	00000001	R	.	L	.	.	.	T	.
//__fpclass	.text	006C1365	00000092	0000000C	00000001	R	.	L	.	.	B	T	.
//__lseek	.text	006C13F7	00000065	00000004	00000009	R	.	L	.	.	.	T	.
//__lseek_lk	.text	006C145C	00000073	00000008	00000009	R	.	L	.	.	.	T	.
//__write	.text	006C14CF	00000065	00000004	00000009	R	.	L	.	.	.	T	.
//__write_lk	.text	006C1534	0000018B	00000424	00000009	R	.	L	.	.	B	T	.
//__getbuf	.text	006C16BF	00000044	00000000	00000004	R	.	L	.	.	.	.	.
//__isatty	.text	006C1703	00000029	00000000	00000004	R	.	L	.	.	.	T	.
//_wctomb	.text	006C172C	00000059	00000010	00000005	R	.	L	.	.	B	T	.
//__wctomb_lk	.text	006C1785	00000069	00000004	00000005	R	.	L	.	.	B	T	.
//___init_time	.text	006C17EE	00000091	00000004	00000000	R	.	L	.	.	.	.	.
//__get_lc_time	.text	006C187F	0000034E	00000010	00000004	R	.	L	.	S	B	.	.
//__free_lc_time	.text	006C1BCD	00000190	00000004	00000004	R	.	L	.	.	.	.	.
//___init_numeric	.text	006C1D5D	000001D5	0000000C	00000000	R	.	L	.	.	.	.	.
//_fix_grouping	.text	006C1F32	00000037	00000000	00000004	R	.	L	.	.	.	.	.
//___init_monetary	.text	006C1F69	000000CB	00000004	00000000	R	.	L	.	.	.	.	.
//__get_lc_lconv	.text	006C2034	00000122	00000008	00000004	R	.	L	.	.	.	.	.
//_fix_grouping_0	.text	006C2156	00000037	00000000	00000004	R	.	L	.	.	.	.	.
//__free_lc_lconv	.text	006C218D	0000004E	00000004	00000004	R	.	L	.	.	.	.	.
//___init_ctype	.text	006C21DB	00000245	0000002C	00000000	R	.	L	.	.	B	.	.
//sub_6C2420	.text	006C2420	00000003	00000000	00000000	R	.	.	.	.	.	.	.
//_strcspn	.text	006C2430	0000003E	00000028	00000005	R	.	L	.	.	B	T	.
//_strpbrk	.text	006C2470	0000003A	00000028	00000008	R	.	L	.	.	B	T	.
//___get_qualified_locale	.text	006C24AA	0000017D	0000000C	0000000C	R	.	L	.	.	.	.	.
//_TranslateName	.text	006C2627	00000058	00000010	0000000C	R	.	L	.	S	B	.	.
//_GetLcidFromLangCountry	.text	006C267F	00000087	00000000	00000000	R	.	L	.	S	.	.	.
//__stdcall LangCountryEnumProc(x)	.text	006C2706	00000204	00000088	00000004	R	.	L	.	S	B	T	.
//_GetLcidFromLanguage	.text	006C290A	00000056	00000000	00000000	R	.	L	.	S	.	.	.
//__stdcall LanguageEnumProc(x)	.text	006C2960	000000BD	00000080	00000004	R	.	L	.	S	B	T	.
//_GetLcidFromCountry	.text	006C2A1D	00000037	00000000	00000000	R	.	L	.	S	.	.	.
//__stdcall CountryEnumProc(x)	.text	006C2A54	00000086	00000080	00000004	R	.	L	.	S	B	T	.
//_GetLcidFromDefault	.text	006C2ADA	0000001A	00000000	00000000	R	.	L	.	S	.	.	.
//_ProcessCodePage	.text	006C2AF4	00000066	00000010	00000001	R	.	L	.	S	B	T	.
//_TestDefaultCountry	.text	006C2B5A	0000001F	00000000	00000002	R	.	L	.	S	.	.	.
//_TestDefaultLanguage	.text	006C2B79	00000062	0000007C	00000008	R	.	L	.	S	B	.	.
//_IsThisWindowsNT	.text	006C2BDB	00000036	00000098	00000000	R	.	L	.	S	B	.	.
//__stdcall crtGetLocaleInfoA(x,x,x,x)	.text	006C2C11	000000E6	00000010	0000000D	R	.	L	.	S	B	T	.
//_LcidFromHexString	.text	006C2CF7	00000039	00000000	00000004	R	.	L	.	S	.	.	.
//_GetPrimaryLen	.text	006C2D30	00000021	00000000	00000004	R	.	L	.	S	.	.	.
//___crtGetStringTypeA	.text	006C2D51	00000149	00000038	0000001C	R	.	L	.	.	B	T	.
//__stdcall __CxxUnhandledExceptionFilter(_EXCEPTION_POINTERS *)	.text	006C2E9A	00000046	00000004	00000004	R	.	L	.	.	.	T	.
//__cdecl __CxxSetUnhandledExceptionFilter(void)	.text	006C2EE0	00000011	00000000	00000000	R	.	.	.	.	.	.	.
//__cdecl __CxxRestoreUnhandledExceptionFilter(void)	.text	006C2EF1	0000000D	00000000	00000000	R	.	.	.	.	.	.	.
//___strgtold12	.text	006C2EFE	000004A1	0000006C	0000001C	R	.	L	.	.	B	.	.
//___STRINGTOLD	.text	006C33CF	0000003C	00000014	00000010	R	.	L	.	.	B	.	.
//_mbtowc	.text	006C340B	0000005D	00000010	00000009	R	.	L	.	.	B	T	.
//__mbtowc_lk	.text	006C3468	000000C9	0000000C	0000000C	R	.	L	.	.	B	T	.
//__allshl	.text	006C3540	0000001F	00000000	00000000	R	.	L	.	.	.	.	.
//_ungetc	.text	006C355F	00000029	00000004	00000005	R	.	L	.	.	.	T	.
//_ungetc_0	.text	006C3588	0000006E	00000008	00000005	R	.	L	.	.	.	T	.
//__alloc_osfhnd	.text	006C35F6	00000123	00000018	00000000	R	.	L	.	.	.	.	.
//__set_osfhnd	.text	006C3719	0000007C	00000008	00000005	R	.	L	.	.	.	T	.
//__free_osfhnd	.text	006C3795	0000007F	00000008	00000004	R	.	L	.	.	.	.	.
//__get_osfhandle	.text	006C3814	00000042	00000000	00000004	R	.	L	.	.	.	T	.
//__open_osfhandle	.text	006C3856	000000A7	0000000C	00000006	R	.	L	.	.	B	T	.
//__lock_fhandle	.text	006C38FD	0000005F	0000000C	00000004	R	.	L	.	.	.	.	.
//__unlock_fhandle	.text	006C395C	00000022	00000000	00000004	R	.	L	.	.	.	.	.
//__commit	.text	006C397E	00000093	0000000C	00000004	R	.	L	.	.	.	T	.
//__read	.text	006C3A11	00000065	00000004	00000009	R	.	L	.	.	.	T	.
//__read_lk	.text	006C3A76	000001D9	0000001C	00000009	R	.	L	.	.	B	T	.
//___crtCompareStringA	.text	006C3C4F	0000027D	00000050	00000019	R	.	L	.	.	B	T	.
//_strncnt_0	.text	006C3ECC	0000002B	00000004	00000008	R	.	L	.	S	.	.	.
//___crtsetenv	.text	006C3EF7	00000187	00000018	00000008	R	.	L	.	.	B	T	.
//_findenv	.text	006C407E	00000058	00000008	00000005	R	.	L	.	S	.	.	.
//_copy_environ	.text	006C40D6	00000067	00000004	00000004	R	.	L	.	S	.	.	.
//__matherr	.text	006C413D	00000003	00000000	00000000	R	.	.	.	.	.	T	.
//__chsize	.text	006C4140	00000059	00000004	00000005	R	.	.	.	.	.	T	.
//__chsize_lk	.text	006C4199	00000125	00001010	00000005	R	.	L	.	.	B	T	.
//_gmtime	.text	006C42BE	0000010A	0000000C	00000001	R	.	L	.	.	.	T	.
//sub_6C43C8	.text	006C43C8	00000011	00000000	00000004	R	.	.	.	.	.	.	.
//__ismbbkprint	.text	006C43D9	00000011	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbkpunct	.text	006C43EA	00000011	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbalnum	.text	006C43FB	00000014	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbalpha	.text	006C440F	00000014	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbgraph	.text	006C4423	00000014	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbprint	.text	006C4437	00000014	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbpunct	.text	006C444B	00000011	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbblead	.text	006C445C	00000011	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbtrail	.text	006C446D	00000011	00000000	00000004	R	.	L	.	.	.	T	.
//__ismbbkana	.text	006C447E	00000027	00000000	00000004	R	.	L	.	.	.	T	.
//sub_6C44A5	.text	006C44A5	00000031	00000000	00000009	R	.	.	.	.	.	.	.
//___crtMessageBoxA	.text	006C44D6	00000089	0000000C	0000000C	R	.	L	.	.	.	.	.
//___addl	.text	006C455F	00000021	00000004	0000000C	R	.	L	.	.	.	.	.
//___add_12	.text	006C4580	0000005E	00000008	00000008	R	.	L	.	.	.	.	.
//___shl_12	.text	006C45DE	0000002E	00000008	00000004	R	.	L	.	.	.	.	.
//___shr_12	.text	006C460C	0000002D	00000008	00000004	R	.	L	.	.	.	.	.
//___mtold12	.text	006C4639	000000C7	0000001C	0000000C	R	.	L	.	.	B	.	.
//_$I10_OUTPUT	.text	006C4700	00000293	0000002C	00000018	R	.	L	.	.	B	.	.
//_ldexp	.text	006C4993	000001E6	00000034	00000009	R	.	L	.	.	B	T	.
//__Getdays	.text	006C4B79	0000008F	00000018	00000000	R	.	L	.	.	B	T	.
//__Getmonths	.text	006C4C08	00000095	00000018	00000000	R	.	L	.	.	B	T	.
//__Gettnames	.text	006C4C9D	00000232	00000020	00000000	R	.	L	.	.	B	T	.
//_strftime	.text	006C4ECF	0000001B	00000000	00000014	R	.	.	.	.	.	T	.
//__Strftime	.text	006C4EEA	000000FA	00000018	00000014	R	.	.	.	.	B	T	.
//__expandtime	.text	006C4FE4	000003EF	00000010	00000014	R	.	L	.	S	B	.	.
//__store_str	.text	006C53D3	00000027	00000008	0000000C	R	.	L	.	S	.	.	.
//__store_num	.text	006C53FA	0000006F	0000000C	00000010	R	.	L	.	S	B	.	.
//__store_number	.text	006C5469	00000049	00000010	0000000C	R	.	L	.	S	B	.	.
//__store_winword	.text	006C54B2	000002CF	00000014	00000014	R	.	L	.	S	B	T	.
//___getlocaleinfo	.text	006C5781	00000138	00000090	00000010	R	.	L	.	.	B	.	.
//_localeconv	.text	006C58B9	00000006	00000000	00000000	R	.	.	.	.	.	T	.
//___crtGetStringTypeW	.text	006C58BF	000001C5	0000003C	00000015	R	.	L	.	.	B	T	.
//__cdecl _ValidateRead(void const *,uint)	.text	006C5A84	0000001C	00000004	00000005	R	.	.	.	.	.	.	.
//__cdecl _ValidateWrite(void *,uint)	.text	006C5AA0	0000001C	00000004	00000005	R	.	.	.	.	.	.	.
//__cdecl _ValidateExecute(int (__stdcall *)(void))	.text	006C5ABC	00000018	00000004	00000001	R	.	L	.	.	.	T	.
//__cdecl terminate(void)	.text	006C5AD4	00000061	0000001C	00000000	R	.	L	.	.	B	T	.
//__cdecl unexpected(void)	.text	006C5B35	00000018	00000000	00000000	R	.	L	.	.	.	T	.
//__cdecl _inconsistency(void)	.text	006C5B4D	00000056	0000001C	00000000	R	.	.	.	.	B	.	.
//___ld12mul	.text	006C5BA3	00000220	00000034	00000008	R	.	L	.	.	B	.	.
//___multtenpow12	.text	006C5DC3	0000007C	00000014	0000000C	R	.	L	.	.	B	.	.
//__mbschr	.text	006C5E3F	00000097	0000000C	00000005	R	.	L	.	.	B	T	.
//__setmode	.text	006C5ED6	00000059	00000004	00000008	R	.	.	.	.	.	T	.
//__setmode_lk	.text	006C5F2F	00000061	00000004	00000008	R	.	L	.	.	.	.	.
//___crtGetLocaleInfoW	.text	006C5F90	00000113	00000034	00000011	R	.	L	.	.	B	T	.
//___crtGetLocaleInfoA	.text	006C60A3	0000011E	00000034	00000011	R	.	L	.	.	B	T	.
    Hook_Function(0x006C61C1, &std::abort);

    /**
     *  Standard functions.
     */
    Hook_Function(0x006BE766, &vinifera_strdup);

    /**
     *  C memory functions.
     */
    Hook_Function(0x006B72CC, &vinifera_allocate);
    Hook_Function(0x006BCA26, &vinifera_count_allocate);
    Hook_Function(0x006B7F72, &vinifera_reallocate);
    Hook_Function(0x006B67E4, &vinifera_free);

    /**
     *  C++ new and delete.
     */
    Hook_Function(0x006B51D7, &vinifera_allocate);
    Hook_Function(0x006B51CC, &vinifera_free);

    /**
     *  Redirect the games CRT functions to use use the DLL's CRT.
     */
    Hook_Function(0x006B602A, &std::strtok);
}
