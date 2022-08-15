/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 FMOD library functions from the DLL.
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
#include "fmod_load_dll.h"
#include "fmod_globals.h"
#include "wstring.h"
#include "winutil.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>


/**
 *  fmodL has debug logging enabled, this is only required for Debug builds.
 */
#ifndef NDEBUG
#define FMOD_DLL_NAME "fmodL.dll"
#else
#define FMOD_DLL_NAME "fmod.dll"
#endif


#define LOAD_DLL_IMPORT(dll, type, func) \
    func = (type)GetProcAddress(dll, #func); \
    if (!func) { \
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", #func, GetLastError()); \
        /* return false; */ \
    }


/**
 *  Loaded FMOD library.
 */
static HMODULE FMODDLL = nullptr;


/**
 *  We successfully loaded all imports?
 */
bool FMODImportsLoaded = false;


/**
 *  Load the FMOD DLL and any imports we need.
 * 
 *  @author: CCHyper
 */
bool Load_FMOD_DLL()
{
    /**
     *  We already performed a successful one-time init, return success.
     */
    if (FMODImportsLoaded) {
        return true;
    }

    Wstring lib_name = FMOD_DLL_NAME;
    
    /**
     *  Look for the FMOD DLL.
     */
    if (GetFileAttributesA(lib_name.Peek_Buffer()) != INVALID_FILE_ATTRIBUTES) {
        FMODDLL = LoadLibrary(lib_name.Peek_Buffer());
    }

    if (!FMODDLL) {
        DEBUG_ERROR("Audio: Failed to load FMOD library!\n");
        FMODImportsLoaded = false;
        return false;
    }

    DEBUG_INFO("Audio: FMOD library \"%s\" found.\n", lib_name.Peek_Buffer());

    DEV_DEBUG_INFO("Load_FMOD_DLL()\n");

    // FMOD5_ChannelGroup_AddDSP(x,x,x)	100E66D0	330
    // FMOD5_ChannelGroup_AddFadePoint(x,x,x,x)	100E66E0	331
    // FMOD5_ChannelGroup_AddGroup(x,x,x,x)	100E6700	332
    // FMOD5_ChannelGroup_Get3DAttributes(x,x,x)	100E6710	333
    // FMOD5_ChannelGroup_Get3DConeOrientation(x,x)	100E6720	334
    // FMOD5_ChannelGroup_Get3DConeSettings(x,x,x,x)	100E6730	335
    // FMOD5_ChannelGroup_Get3DCustomRolloff(x,x,x)	100E6740	336
    // FMOD5_ChannelGroup_Get3DDistanceFilter(x,x,x,x)	100E6750	337
    // FMOD5_ChannelGroup_Get3DDopplerLevel(x,x)	100E6760	338
    // FMOD5_ChannelGroup_Get3DLevel(x,x)	100E6770	339
    // FMOD5_ChannelGroup_Get3DMinMaxDistance(x,x,x)	100E6780	340
    // FMOD5_ChannelGroup_Get3DOcclusion(x,x,x)	100E6790	341
    // FMOD5_ChannelGroup_Get3DSpread(x,x)	100E67A0	342
    // FMOD5_ChannelGroup_GetAudibility(x,x)	100E67B0	343
    // FMOD5_ChannelGroup_GetChannel(x,x,x)	100E67C0	344
    // FMOD5_ChannelGroup_GetDSP(x,x,x)	100E67D0	345
    // FMOD5_ChannelGroup_GetDSPClock(x,x,x)	100E67E0	346
    // FMOD5_ChannelGroup_GetDSPIndex(x,x,x)	100E67F0	347
    // FMOD5_ChannelGroup_GetDelay(x,x,x,x)	100E6800	348
    // FMOD5_ChannelGroup_GetFadePoints(x,x,x,x)	100E6810	349
    // FMOD5_ChannelGroup_GetGroup(x,x,x)	100E6820	350
    // FMOD5_ChannelGroup_GetLowPassGain(x,x)	100E6830	351
    // FMOD5_ChannelGroup_GetMixMatrix(x,x,x,x,x)	100E6840	352
    // FMOD5_ChannelGroup_GetMode(x,x)	100E6850	353
    // FMOD5_ChannelGroup_GetMute(x,x)	100E6860	354
    // FMOD5_ChannelGroup_GetName(x,x,x)	100E6870	355
    // FMOD5_ChannelGroup_GetNumChannels(x,x)	100E6880	356
    // FMOD5_ChannelGroup_GetNumDSPs(x,x)	100E6890	357
    // FMOD5_ChannelGroup_GetNumGroups(x,x)	100E68A0	358
    // FMOD5_ChannelGroup_GetParentGroup(x,x)	100E68B0	359
    // FMOD5_ChannelGroup_GetPaused(x,x)	100E68C0	360
    // FMOD5_ChannelGroup_GetPitch(x,x)	100E68D0	361
    // FMOD5_ChannelGroup_GetReverbProperties(x,x,x)	100E68E0	362
    // FMOD5_ChannelGroup_GetSystemObject(x,x)	100E68F0	363
    // FMOD5_ChannelGroup_GetUserData(x,x)	100E6900	364
    // FMOD5_ChannelGroup_GetVolume(x,x)	100E6910	365
    // FMOD5_ChannelGroup_GetVolumeRamp(x,x)	100E6920	366
    // FMOD5_ChannelGroup_IsPlaying(x,x)	100E6930	367
    // FMOD5_ChannelGroup_Release(x)	100E6940	368
    // FMOD5_ChannelGroup_RemoveDSP(x,x)	100E6950	369
    // FMOD5_ChannelGroup_RemoveFadePoints(x,x,x,x,x)	100E6960	370
    // FMOD5_ChannelGroup_Set3DAttributes(x,x,x)	100E6980	371
    // FMOD5_ChannelGroup_Set3DConeOrientation(x,x)	100E6990	372
    // FMOD5_ChannelGroup_Set3DConeSettings(x,x,x,x)	100E69A0	373
    // FMOD5_ChannelGroup_Set3DCustomRolloff(x,x,x)	100E69E0	374
    // FMOD5_ChannelGroup_Set3DDistanceFilter(x,x,x,x)	100E69F0	375
    // FMOD5_ChannelGroup_Set3DDopplerLevel(x,x)	100E6A20	376
    // FMOD5_ChannelGroup_Set3DLevel(x,x)	100E6A40	377
    // FMOD5_ChannelGroup_Set3DMinMaxDistance(x,x,x)	100E6A60	378
    // FMOD5_ChannelGroup_Set3DOcclusion(x,x,x)	100E6A90	379
    // FMOD5_ChannelGroup_Set3DSpread(x,x)	100E6AC0	380
    // FMOD5_ChannelGroup_SetCallback(x,x)	100E6AE0	381
    // FMOD5_ChannelGroup_SetDSPIndex(x,x,x)	100E6AF0	382
    // FMOD5_ChannelGroup_SetDelay(x,x,x,x,x,x)	100E6B00	383
    // FMOD5_ChannelGroup_SetFadePointRamp(x,x,x,x)	100E6B20	384
    // FMOD5_ChannelGroup_SetLowPassGain(x,x)	100E6B40	385
    // FMOD5_ChannelGroup_SetMixLevelsInput(x,x,x)	100E6B60	386
    // FMOD5_ChannelGroup_SetMixLevelsOutput(x,x,x,x,x,x,x,x,x)	100E6B70	387
    // FMOD5_ChannelGroup_SetMixMatrix(x,x,x,x,x)	100E6BE0	388
    // FMOD5_ChannelGroup_SetMode(x,x)	100E6BF0	389
    // FMOD5_ChannelGroup_SetMute(x,x)	100E6C00	390
    // FMOD5_ChannelGroup_SetPan(x,x)	100E6C10	391
    // FMOD5_ChannelGroup_SetPaused(x,x)	100E6C30	392
    // FMOD5_ChannelGroup_SetPitch(x,x)	100E6C40	393
    // FMOD5_ChannelGroup_SetReverbProperties(x,x,x)	100E6C60	394
    // FMOD5_ChannelGroup_SetUserData(x,x)	100E6C80	395
    // FMOD5_ChannelGroup_SetVolume(x,x)	100E6C90	396
    // FMOD5_ChannelGroup_SetVolumeRamp(x,x)	100E6CB0	397
    // FMOD5_ChannelGroup_Stop(x)	100E6CC0	398
    // FMOD5_Channel_AddDSP(x,x,x)	100E6CD0	399
    // FMOD5_Channel_AddFadePoint(x,x,x,x)	100E6CE0	400
    // FMOD5_Channel_Get3DAttributes(x,x,x)	100E6D00	401
    // FMOD5_Channel_Get3DConeOrientation(x,x)	100E6D10	402
    // FMOD5_Channel_Get3DConeSettings(x,x,x,x)	100E6D20	403
    // FMOD5_Channel_Get3DCustomRolloff(x,x,x)	100E6D30	404
    // FMOD5_Channel_Get3DDistanceFilter(x,x,x,x)	100E6D40	405
    // FMOD5_Channel_Get3DDopplerLevel(x,x)	100E6D50	406
    // FMOD5_Channel_Get3DLevel(x,x)	100E6D60	407
    // FMOD5_Channel_Get3DMinMaxDistance(x,x,x)	100E6D70	408
    // FMOD5_Channel_Get3DOcclusion(x,x,x)	100E6D80	409
    // FMOD5_Channel_Get3DSpread(x,x)	100E6D90	410
    // FMOD5_Channel_GetAudibility(x,x)	100E6DA0	411
    // FMOD5_Channel_GetChannelGroup(x,x)	100E6DB0	412
    // FMOD5_Channel_GetCurrentSound(x,x)	100E6DC0	413
    // FMOD5_Channel_GetDSP(x,x,x)	100E6DD0	414
    // FMOD5_Channel_GetDSPClock(x,x,x)	100E6DE0	415
    // FMOD5_Channel_GetDSPIndex(x,x,x)	100E6DF0	416
    // FMOD5_Channel_GetDelay(x,x,x,x)	100E6E00	417
    // FMOD5_Channel_GetFadePoints(x,x,x,x)	100E6E10	418
    // FMOD5_Channel_GetFrequency(x,x)	100E6E20	419
    // FMOD5_Channel_GetIndex(x,x)	100E6E30	420
    // FMOD5_Channel_GetLoopCount(x,x)	100E6E40	421
    // FMOD5_Channel_GetLoopPoints(x,x,x,x,x)	100E6E50	422
    // FMOD5_Channel_GetLowPassGain(x,x)	100E6E60	423
    // FMOD5_Channel_GetMixMatrix(x,x,x,x,x)	100E6E70	424
    // FMOD5_Channel_GetMode(x,x)	100E6E80	425
    // FMOD5_Channel_GetMute(x,x)	100E6E90	426
    // FMOD5_Channel_GetNumDSPs(x,x)	100E6EA0	427
    // FMOD5_Channel_GetPaused(x,x)	100E6EB0	428
    // FMOD5_Channel_GetPitch(x,x)	100E6EC0	429
    // FMOD5_Channel_GetPosition(x,x,x)	100E6ED0	430
    // FMOD5_Channel_GetPriority(x,x)	100E6EE0	431
    // FMOD5_Channel_GetReverbProperties(x,x,x)	100E6EF0	432
    // FMOD5_Channel_GetSystemObject(x,x)	100E6F00	433
    // FMOD5_Channel_GetUserData(x,x)	100E6F10	434
    // FMOD5_Channel_GetVolume(x,x)	100E6F20	435
    // FMOD5_Channel_GetVolumeRamp(x,x)	100E6F30	436
    // FMOD5_Channel_IsPlaying(x,x)	100E6F40	437
    // FMOD5_Channel_IsVirtual(x,x)	100E6F50	438
    // FMOD5_Channel_RemoveDSP(x,x)	100E6F60	439
    // FMOD5_Channel_RemoveFadePoints(x,x,x,x,x)	100E6F70	440
    // FMOD5_Channel_Set3DAttributes(x,x,x)	100E6F90	441
    // FMOD5_Channel_Set3DConeOrientation(x,x)	100E6FA0	442
    // FMOD5_Channel_Set3DConeSettings(x,x,x,x)	100E6FB0	443
    // FMOD5_Channel_Set3DCustomRolloff(x,x,x)	100E6FF0	444
    // FMOD5_Channel_Set3DDistanceFilter(x,x,x,x)	100E7000	445
    // FMOD5_Channel_Set3DDopplerLevel(x,x)	100E7030	446
    // FMOD5_Channel_Set3DLevel(x,x)	100E7050	447
    // FMOD5_Channel_Set3DMinMaxDistance(x,x,x)	100E7070	448
    // FMOD5_Channel_Set3DOcclusion(x,x,x)	100E70A0	449
    // FMOD5_Channel_Set3DSpread(x,x)	100E70D0	450
    // FMOD5_Channel_SetCallback(x,x)	100E70F0	451
    // FMOD5_Channel_SetChannelGroup(x,x)	100E7100	452
    // FMOD5_Channel_SetDSPIndex(x,x,x)	100E7110	453
    // FMOD5_Channel_SetDelay(x,x,x,x,x,x)	100E7120	454
    // FMOD5_Channel_SetFadePointRamp(x,x,x,x)	100E7140	455
    // FMOD5_Channel_SetFrequency(x,x)	100E7160	456
    // FMOD5_Channel_SetLoopCount(x,x)	100E7180	457
    // FMOD5_Channel_SetLoopPoints(x,x,x,x,x)	100E7190	458
    // FMOD5_Channel_SetLowPassGain(x,x)	100E71A0	459
    // FMOD5_Channel_SetMixLevelsInput(x,x,x)	100E71C0	460
    // FMOD5_Channel_SetMixLevelsOutput(x,x,x,x,x,x,x,x,x)	100E71D0	461
    // FMOD5_Channel_SetMixMatrix(x,x,x,x,x)	100E7240	462
    // FMOD5_Channel_SetMode(x,x)	100E7250	463
    // FMOD5_Channel_SetMute(x,x)	100E7260	464
    // FMOD5_Channel_SetPan(x,x)	100E7270	465
    // FMOD5_Channel_SetPaused(x,x)	100E7290	466
    // FMOD5_Channel_SetPitch(x,x)	100E72A0	467
    // FMOD5_Channel_SetPosition(x,x,x)	100E72C0	468
    // FMOD5_Channel_SetPriority(x,x)	100E72D0	469
    // FMOD5_Channel_SetReverbProperties(x,x,x)	100E72E0	470
    // FMOD5_Channel_SetUserData(x,x)	100E7300	471
    // FMOD5_Channel_SetVolume(x,x)	100E7310	472
    // FMOD5_Channel_SetVolumeRamp(x,x)	100E7330	473
    // FMOD5_Channel_Stop(x)	100E7340	474
    // FMOD5_DSPConnection_GetInput(x,x)	100E7350	475
    // FMOD5_DSPConnection_GetMix(x,x)	100E7360	476
    // FMOD5_DSPConnection_GetMixMatrix(x,x,x,x,x)	100E7370	477
    // FMOD5_DSPConnection_GetOutput(x,x)	100E7380	478
    // FMOD5_DSPConnection_GetType(x,x)	100E7390	479
    // FMOD5_DSPConnection_GetUserData(x,x)	100E73A0	480
    // FMOD5_DSPConnection_SetMix(x,x)	100E73B0	481
    // FMOD5_DSPConnection_SetMixMatrix(x,x,x,x,x)	100E73D0	482
    // FMOD5_DSPConnection_SetUserData(x,x)	100E73E0	483
    // FMOD5_DSP_AddInput(x,x,x,x)	100E73F0	484
    // FMOD5_DSP_DisconnectAll(x,x,x)	100E7400	485
    // FMOD5_DSP_DisconnectFrom(x,x,x)	100E7410	486
    // FMOD5_DSP_GetActive(x,x)	100E7420	487
    // FMOD5_DSP_GetBypass(x,x)	100E7430	488
    // FMOD5_DSP_GetCPUUsage(x,x,x)	100E7440	489
    // FMOD5_DSP_GetChannelFormat(x,x,x,x)	100E7450	490
    // FMOD5_DSP_GetDataParameterIndex(x,x,x)	100E7460	491
    // FMOD5_DSP_GetIdle(x,x)	100E7470	492
    // FMOD5_DSP_GetInfo(x,x,x,x,x,x)	100E7480	493
    // FMOD5_DSP_GetInput(x,x,x,x)	100E7490	494
    // FMOD5_DSP_GetMeteringEnabled(x,x,x)	100E74A0	495
    // FMOD5_DSP_GetMeteringInfo(x,x,x)	100E74B0	496
    // FMOD5_DSP_GetNumInputs(x,x)	100E74C0	497
    // FMOD5_DSP_GetNumOutputs(x,x)	100E74D0	498
    // FMOD5_DSP_GetNumParameters(x,x)	100E74E0	499
    // FMOD5_DSP_GetOutput(x,x,x,x)	100E74F0	500
    // FMOD5_DSP_GetOutputChannelFormat(x,x,x,x,x,x,x)	100E7500	501
    // FMOD5_DSP_GetParameterBool(x,x,x,x,x)	100E7510	502
    // FMOD5_DSP_GetParameterData(x,x,x,x,x,x)	100E7520	503
    // FMOD5_DSP_GetParameterFloat(x,x,x,x,x)	100E7530	504
    // FMOD5_DSP_GetParameterInfo(x,x,x)	100E7540	505
    // FMOD5_DSP_GetParameterInt(x,x,x,x,x)	100E7550	506
    // FMOD5_DSP_GetSystemObject(x,x)	100E7560	507
    // FMOD5_DSP_GetType(x,x)	100E7570	508
    // FMOD5_DSP_GetUserData(x,x)	100E7580	509
    // FMOD5_DSP_GetWetDryMix(x,x,x,x)	100E7590	510
    // FMOD5_DSP_Release(x)	100E75A0	511
    // FMOD5_DSP_Reset(x)	100E75B0	512
    // FMOD5_DSP_SetActive(x,x)	100E75C0	513
    // FMOD5_DSP_SetBypass(x,x)	100E75D0	514
    // FMOD5_DSP_SetChannelFormat(x,x,x,x)	100E75E0	515
    // FMOD5_DSP_SetMeteringEnabled(x,x,x)	100E75F0	516
    // FMOD5_DSP_SetParameterBool(x,x,x)	100E7600	517
    // FMOD5_DSP_SetParameterData(x,x,x,x)	100E7610	518
    // FMOD5_DSP_SetParameterFloat(x,x,x)	100E7620	519
    // FMOD5_DSP_SetParameterInt(x,x,x)	100E7640	520
    // FMOD5_DSP_SetUserData(x,x)	100E7650	521
    // FMOD5_DSP_SetWetDryMix(x,x,x,x)	100E7660	522
    // FMOD5_DSP_ShowConfigDialog(x,x,x)	100E76A0	523
    // FMOD5_Debug_Initialize(x,x,x,x)	100E76B0	524
    // FMOD5_File_GetDiskBusy(x)	100E76C0	525
    // FMOD5_File_SetDiskBusy(x)	100E76D0	526
    // FMOD5_Geometry_AddPolygon(x,x,x,x,x,x,x)	100E76E0	527
    // FMOD5_Geometry_GetActive(x,x)	100E7720	528
    // FMOD5_Geometry_GetMaxPolygons(x,x,x)	100E7730	529
    // FMOD5_Geometry_GetNumPolygons(x,x)	100E7740	530
    // FMOD5_Geometry_GetPolygonAttributes(x,x,x,x,x)	100E7750	531
    // FMOD5_Geometry_GetPolygonNumVertices(x,x,x)	100E7760	532
    // FMOD5_Geometry_GetPolygonVertex(x,x,x,x)	100E7770	533
    // FMOD5_Geometry_GetPosition(x,x)	100E7780	534
    // FMOD5_Geometry_GetRotation(x,x,x)	100E7790	535
    // FMOD5_Geometry_GetScale(x,x)	100E77A0	536
    // FMOD5_Geometry_GetUserData(x,x)	100E77B0	537
    // FMOD5_Geometry_Release(x)	100E77C0	538
    // FMOD5_Geometry_Save(x,x,x)	100E77D0	539
    // FMOD5_Geometry_SetActive(x,x)	100E77E0	540
    // FMOD5_Geometry_SetPolygonAttributes(x,x,x,x,x)	100E77F0	541
    // FMOD5_Geometry_SetPolygonVertex(x,x,x,x)	100E7820	542
    // FMOD5_Geometry_SetPosition(x,x)	100E7830	543
    // FMOD5_Geometry_SetRotation(x,x,x)	100E7840	544
    // FMOD5_Geometry_SetScale(x,x)	100E7850	545
    // FMOD5_Geometry_SetUserData(x,x)	100E7860	546
    // FMOD5_Memory_GetStats(x,x,x)	100E7870	547
    // FMOD5_Memory_Initialize(x,x,x,x,x,x)	100E7880	548
    // FMOD5_Reverb3D_Get3DAttributes(x,x,x,x)	100E7890	549
    // FMOD5_Reverb3D_GetActive(x,x)	100E78A0	550
    // FMOD5_Reverb3D_GetProperties(x,x)	100E78B0	551
    // FMOD5_Reverb3D_GetUserData(x,x)	100E78C0	552
    // FMOD5_Reverb3D_Release(x)	100E78D0	553
    // FMOD5_Reverb3D_Set3DAttributes(x,x,x,x)	100E78E0	554
    // FMOD5_Reverb3D_SetActive(x,x)	100E7910	555
    // FMOD5_Reverb3D_SetProperties(x,x)	100E7920	556
    // FMOD5_Reverb3D_SetUserData(x,x)	100E7930	557
    // FMOD5_SoundGroup_GetMaxAudible(x,x)	100E7940	558
    // FMOD5_SoundGroup_GetMaxAudibleBehavior(x,x)	100E7950	559
    // FMOD5_SoundGroup_GetMuteFadeSpeed(x,x)	100E7960	560
    // FMOD5_SoundGroup_GetName(x,x,x)	100E7970	561
    // FMOD5_SoundGroup_GetNumPlaying(x,x)	100E7980	562
    // FMOD5_SoundGroup_GetNumSounds(x,x)	100E7990	563
    // FMOD5_SoundGroup_GetSound(x,x,x)	100E79A0	564
    // FMOD5_SoundGroup_GetSystemObject(x,x)	100E79B0	565
    // FMOD5_SoundGroup_GetUserData(x,x)	100E79C0	566
    // FMOD5_SoundGroup_GetVolume(x,x)	100E79D0	567
    // FMOD5_SoundGroup_Release(x)	100E79E0	568
    // FMOD5_SoundGroup_SetMaxAudible(x,x)	100E79F0	569
    // FMOD5_SoundGroup_SetMaxAudibleBehavior(x,x)	100E7A00	570
    // FMOD5_SoundGroup_SetMuteFadeSpeed(x,x)	100E7A10	571
    // FMOD5_SoundGroup_SetUserData(x,x)	100E7A30	572
    // FMOD5_SoundGroup_SetVolume(x,x)	100E7A40	573
    // FMOD5_SoundGroup_Stop(x)	100E7A60	574
    // FMOD5_Sound_AddSyncPoint(x,x,x,x,x)	100E7A70	575
    // FMOD5_Sound_DeleteSyncPoint(x,x)	100E7A80	576
    // FMOD5_Sound_Get3DConeSettings(x,x,x,x)	100E7A90	577
    // FMOD5_Sound_Get3DCustomRolloff(x,x,x)	100E7AA0	578
    // FMOD5_Sound_Get3DMinMaxDistance(x,x,x)	100E7AB0	579
    // FMOD5_Sound_GetDefaults(x,x,x)	100E7AC0	580
    // FMOD5_Sound_GetFormat(x,x,x,x,x)	100E7AD0	581
    // FMOD5_Sound_GetLength(x,x,x)	100E7AE0	582
    // FMOD5_Sound_GetLoopCount(x,x)	100E7AF0	583
    // FMOD5_Sound_GetLoopPoints(x,x,x,x,x)	100E7B00	584
    // FMOD5_Sound_GetMode(x,x)	100E7B10	585
    // FMOD5_Sound_GetMusicChannelVolume(x,x,x)	100E7B20	586
    // FMOD5_Sound_GetMusicNumChannels(x,x)	100E7B30	587
    // FMOD5_Sound_GetMusicSpeed(x,x)	100E7B40	588
    // FMOD5_Sound_GetName(x,x,x)	100E7B50	589
    // FMOD5_Sound_GetNumSubSounds(x,x)	100E7B60	590
    // FMOD5_Sound_GetNumSyncPoints(x,x)	100E7B70	591
    // FMOD5_Sound_GetNumTags(x,x,x)	100E7B80	592
    // FMOD5_Sound_GetOpenState(x,x,x,x,x)	100E7B90	593
    // FMOD5_Sound_GetSoundGroup(x,x)	100E7BA0	594
    // FMOD5_Sound_GetSubSound(x,x,x)	100E7BB0	595
    // FMOD5_Sound_GetSubSoundParent(x,x)	100E7BC0	596
    // FMOD5_Sound_GetSyncPoint(x,x,x)	100E7BD0	597
    // FMOD5_Sound_GetSyncPointInfo(x,x,x,x,x,x)	100E7BE0	598
    // FMOD5_Sound_GetSystemObject(x,x)	100E7BF0	599
    // FMOD5_Sound_GetTag(x,x,x,x)	100E7C00	600
    // FMOD5_Sound_GetUserData(x,x)	100E7C10	601
    // FMOD5_Sound_Lock(x,x,x,x,x,x,x)	100E7C20	602
    // FMOD5_Sound_ReadData(x,x,x,x)	100E7C30	603
    // FMOD5_Sound_Release(x)	100E7C40	604
    // FMOD5_Sound_SeekData(x,x)	100E7C50	605
    // FMOD5_Sound_Set3DConeSettings(x,x,x,x)	100E7C60	606
    // FMOD5_Sound_Set3DCustomRolloff(x,x,x)	100E7CA0	607
    // FMOD5_Sound_Set3DMinMaxDistance(x,x,x)	100E7CB0	608
    // FMOD5_Sound_SetDefaults(x,x,x)	100E7CE0	609
    // FMOD5_Sound_SetLoopCount(x,x)	100E7D00	610
    // FMOD5_Sound_SetLoopPoints(x,x,x,x,x)	100E7D10	611
    // FMOD5_Sound_SetMode(x,x)	100E7D20	612
    // FMOD5_Sound_SetMusicChannelVolume(x,x,x)	100E7D30	613
    // FMOD5_Sound_SetMusicSpeed(x,x)	100E7D50	614
    // FMOD5_Sound_SetSoundGroup(x,x)	100E7D70	615
    // FMOD5_Sound_SetUserData(x,x)	100E7D80	616
    // FMOD5_Sound_Unlock(x,x,x,x,x)	100E7D90	617
    // FMOD5_System_AttachChannelGroupToPort(x,x,x,x,x,x)	100E7DA0	618
    // FMOD5_System_AttachFileSystem(x,x,x,x,x)	100E7DC0	619
    // FMOD5_System_Close(x)	100E7DD0	620
    // FMOD5_System_Create(x,x)	100E7DE0	621
    // FMOD5_System_CreateChannelGroup(x,x,x)	100E7DF0	622
    // FMOD5_System_CreateDSP(x,x,x)	100E7E00	623
    // FMOD5_System_CreateDSPByPlugin(x,x,x)	100E7E10	624
    // FMOD5_System_CreateDSPByType(x,x,x)	100E7E20	625
    // FMOD5_System_CreateGeometry(x,x,x,x)	100E7E30	626
    // FMOD5_System_CreateReverb3D(x,x)	100E7E40	627
    // FMOD5_System_CreateSound(x,x,x,x,x)	100E7E50	628
    // FMOD5_System_CreateSoundGroup(x,x,x)	100E7E60	629
    // FMOD5_System_CreateStream(x,x,x,x,x)	100E7E70	630
    // FMOD5_System_DetachChannelGroupFromPort(x,x)	100E7E80	631
    // FMOD5_System_Get3DListenerAttributes(x,x,x,x,x,x)	100E7E90	632
    // FMOD5_System_Get3DNumListeners(x,x)	100E7EA0	633
    // FMOD5_System_Get3DSettings(x,x,x,x)	100E7EB0	634
    // FMOD5_System_GetAdvancedSettings(x,x)	100E7EC0	635
    // FMOD5_System_GetCPUUsage(x,x)	100E7ED0	636
    // FMOD5_System_GetChannel(x,x,x)	100E7EE0	637
    // FMOD5_System_GetChannelsPlaying(x,x,x)	100E7EF0	638
    // FMOD5_System_GetDSPBufferSize(x,x,x)	100E7F00	639
    // FMOD5_System_GetDSPInfoByPlugin(x,x,x)	100E7F10	640
    // FMOD5_System_GetDSPInfoByType(x,x,x)	100E7F20	641
    // FMOD5_System_GetDefaultMixMatrix(x,x,x,x,x)	100E7F30	642
    // FMOD5_System_GetDriver(x,x)	100E7F40	643
    // FMOD5_System_GetDriverInfo(x,x,x,x,x,x,x,x)	100E7F50	644
    // FMOD5_System_GetFileUsage(x,x,x,x)	100E7F60	645
    // FMOD5_System_GetGeometryOcclusion(x,x,x,x,x)	100E7F70	646
    // FMOD5_System_GetGeometrySettings(x,x)	100E7F80	647
    // FMOD5_System_GetMasterChannelGroup(x,x)	100E7F90	648
    // FMOD5_System_GetMasterSoundGroup(x,x)	100E7FA0	649
    // FMOD5_System_GetNestedPlugin(x,x,x,x)	100E7FB0	650
    // FMOD5_System_GetNetworkProxy(x,x,x)	100E7FC0	651
    // FMOD5_System_GetNetworkTimeout(x,x)	100E7FD0	652
    // FMOD5_System_GetNumDrivers(x,x)	100E7FE0	653
    // FMOD5_System_GetNumNestedPlugins(x,x,x)	100E7FF0	654
    // FMOD5_System_GetNumPlugins(x,x,x)	100E8000	655
    // FMOD5_System_GetOutput(x,x)	100E8010	656
    // FMOD5_System_GetOutputByPlugin(x,x)	100E8020	657
    // FMOD5_System_GetOutputHandle(x,x)	100E8030	658
    // FMOD5_System_GetPluginHandle(x,x,x,x)	100E8040	659
    // FMOD5_System_GetPluginInfo(x,x,x,x,x,x)	100E8050	660
    // FMOD5_System_GetRecordDriverInfo(x,x,x,x,x,x,x,x,x)	100E8060	661
    // FMOD5_System_GetRecordNumDrivers(x,x,x)	100E8070	662
    // FMOD5_System_GetRecordPosition(x,x,x)	100E8080	663
    // FMOD5_System_GetReverbProperties(x,x,x)	100E8090	664
    // FMOD5_System_GetSoftwareChannels(x,x)	100E80A0	665
    // FMOD5_System_GetSoftwareFormat(x,x,x,x)	100E80B0	666
    // FMOD5_System_GetSpeakerModeChannels(x,x,x)	100E80C0	667
    // FMOD5_System_GetSpeakerPosition(x,x,x,x,x)	100E80D0	668
    // FMOD5_System_GetStreamBufferSize(x,x,x)	100E80E0	669
    // FMOD5_System_GetUserData(x,x)	100E80F0	670
    // FMOD5_System_GetVersion(x,x)	100E8100	671
    // FMOD5_System_Init(x,x,x,x)	100E8110	672
    // FMOD5_System_IsRecording(x,x,x)	100E8120	673
    // FMOD5_System_LoadGeometry(x,x,x,x)	100E8130	674
    // FMOD5_System_LoadPlugin(x,x,x,x)	100E8140	675
    // FMOD5_System_LockDSP(x)	100E8150	676
    // FMOD5_System_MixerResume(x)	100E8160	677
    // FMOD5_System_MixerSuspend(x)	100E8170	678
    // FMOD5_System_PlayDSP(x,x,x,x,x)	100E8180	679
    // FMOD5_System_PlaySound(x,x,x,x,x)	100E8190	680
    // FMOD5_System_RecordStart(x,x,x,x)	100E81A0	681
    // FMOD5_System_RecordStop(x,x)	100E81B0	682
    // FMOD5_System_RegisterCodec(x,x,x,x)	100E81C0	683
    // FMOD5_System_RegisterDSP(x,x,x)	100E81D0	684
    // FMOD5_System_RegisterOutput(x,x,x)	100E81E0	685
    // FMOD5_System_Release(x)	100E81F0	686
    // FMOD5_System_Set3DListenerAttributes(x,x,x,x,x,x)	100E8200	687
    // FMOD5_System_Set3DNumListeners(x,x)	100E8210	688
    // FMOD5_System_Set3DRolloffCallback(x,x)	100E8220	689
    // FMOD5_System_Set3DSettings(x,x,x,x)	100E8230	690
    // FMOD5_System_SetAdvancedSettings(x,x)	100E8270	691
    // FMOD5_System_SetCallback(x,x,x)	100E8280	692
    // FMOD5_System_SetDSPBufferSize(x,x,x)	100E8290	693
    // FMOD5_System_SetDriver(x,x)	100E82A0	694
    // FMOD5_System_SetFileSystem(x,x,x,x,x,x,x,x)	100E82B0	695
    // FMOD5_System_SetGeometrySettings(x,x)	100E82C0	696
    // FMOD5_System_SetNetworkProxy(x,x)	100E82E0	697
    // FMOD5_System_SetNetworkTimeout(x,x)	100E82F0	698
    // FMOD5_System_SetOutput(x,x)	100E8300	699
    // FMOD5_System_SetOutputByPlugin(x,x)	100E8310	700
    // FMOD5_System_SetPluginPath(x,x)	100E8320	701
    // FMOD5_System_SetReverbProperties(x,x,x)	100E8330	702
    // FMOD5_System_SetSoftwareChannels(x,x)	100E8340	703
    // FMOD5_System_SetSoftwareFormat(x,x,x,x)	100E8350	704
    // FMOD5_System_SetSpeakerPosition(x,x,x,x,x)	100E8360	705
    // FMOD5_System_SetStreamBufferSize(x,x,x)	100E8390	706
    // FMOD5_System_SetUserData(x,x)	100E83A0	707
    // FMOD5_System_UnloadPlugin(x,x)	100E83B0	708
    // FMOD5_System_UnlockDSP(x)	100E83C0	709
    // FMOD5_System_Update(x)	100E83D0	710
    // FMOD5_Thread_SetAttributes(x,x,x,x,x)	100E83E0	711

    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_AddDSP, FMOD_ChannelGroup_AddDSP	100B6050	712
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_AddFadePoint, FMOD_ChannelGroup_AddFadePoint	100B6070	713
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_AddGroup, FMOD_ChannelGroup_AddGroup	100B60A0	714
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_Get3DAttributes, FMOD_ChannelGroup_Get3DAttributes	100B60D0	715
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_Get3DConeOrientation, FMOD_ChannelGroup_Get3DConeOrientation	100B60F0	716
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_Get3DConeSettings, FMOD_ChannelGroup_Get3DConeSettings	100B6110	717
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_Get3DCustomRolloff, FMOD_ChannelGroup_Get3DCustomRolloff	100B6130	718
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_ChannelGroup_Get3DDistanceFilter, FMOD_ChannelGroup_Get3DDistanceFilter	100B6150	719
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Get3DDopplerLevel	100B6190	720
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Get3DLevel	100B61B0	721
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Get3DMinMaxDistance	100B61D0	722
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Get3DOcclusion	100B61F0	723
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Get3DSpread	100B6210	724
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetAudibility	100B6230	725
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetChannel	100B6250	726
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetDSP	100B6270	727
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetDSPClock	100B6290	728
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetDSPIndex	100B62B0	729
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetDelay	100B62D0	730
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetFadePoints	100B6310	731
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetGroup	100B6330	732
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetLowPassGain	100B6350	733
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetMixMatrix	100B6370	734
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetMode	100B6390	735
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetMute	100B63B0	736
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetName	100B63F0	737
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetNumChannels	100B6410	738
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetNumDSPs	100B6430	739
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetNumGroups	100B6450	740
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetParentGroup	100B6470	741
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetPaused	100B6490	742
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetPitch	100B64D0	743
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetReverbProperties	100B64F0	744
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetSystemObject	100B6510	745
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetUserData	100B6530	746
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetVolume	100B6550	747
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_GetVolumeRamp	100B6570	748
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_IsPlaying	100B65B0	749
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Release	100B65F0	750
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_RemoveDSP	100B6610	751
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_RemoveFadePoints	100B6630	752
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DAttributes	100B6660	753
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DConeOrientation	100B6680	754
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DConeSettings	100B66A0	755
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DCustomRolloff	100B66E0	756
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DDistanceFilter	100B6700	757
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DDopplerLevel	100B6740	758
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DLevel	100B6770	759
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DMinMaxDistance	100B67A0	760
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DOcclusion	100B67E0	761
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Set3DSpread	100B6820	762
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetCallback	100B6850	763
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetDSPIndex	100B6870	764
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetDelay	100B6890	765
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetFadePointRamp	100B68D0	766
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetLowPassGain	100B6900	767
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetMixLevelsInput	100B6930	768
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetMixLevelsOutput	100B6950	769
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetMixMatrix	100B69D0	770
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetMode	100B69F0	771
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetMute	100B6A10	772
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetPan	100B6A40	773
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetPaused	100B6A70	774
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetPitch	100B6AA0	775
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetReverbProperties	100B6AD0	776
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetUserData	100B6B00	777
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetVolume	100B6B20	778
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_SetVolumeRamp	100B6B50	779
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_ChannelGroup_Stop	100B6B80	780
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_AddDSP	100B6BA0	781
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_AddFadePoint	100B6BC0	782
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DAttributes	100B6BF0	783
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DConeOrientation	100B6C10	784
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DConeSettings	100B6C30	785
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DCustomRolloff	100B6C50	786
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DDistanceFilter	100B6C70	787
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DDopplerLevel	100B6CB0	788
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DLevel	100B6CD0	789
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DMinMaxDistance	100B6CF0	790
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DOcclusion	100B6D10	791
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Get3DSpread	100B6D30	792
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetAudibility	100B6D50	793
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetChannelGroup	100B6D70	794
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetCurrentSound	100B6D90	795
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetDSP	100B6DB0	796
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetDSPClock	100B6DD0	797
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetDSPIndex	100B6DF0	798
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetDelay	100B6E10	799
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetFadePoints	100B6E50	800
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetFrequency	100B6E70	801
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetIndex	100B6E90	802
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetLoopCount	100B6EB0	803
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetLoopPoints	100B6ED0	804
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetLowPassGain	100B6EF0	805
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetMixMatrix	100B6F10	806
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetMode	100B6F30	807
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetMute	100B6F50	808
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetNumDSPs	100B6F90	809
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetPaused	100B6FB0	810
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetPitch	100B6FF0	811
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetPosition	100B7010	812
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetPriority	100B7030	813
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetReverbProperties	100B7050	814
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetSystemObject	100B7070	815
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetUserData	100B7090	816
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetVolume	100B70B0	817
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_GetVolumeRamp	100B70D0	818
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_IsPlaying	100B7110	819
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_IsVirtual	100B7150	820
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_RemoveDSP	100B7190	821
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_RemoveFadePoints	100B71B0	822
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DAttributes	100B71E0	823
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DConeOrientation	100B7200	824
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DConeSettings	100B7220	825
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DCustomRolloff	100B7260	826
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DDistanceFilter	100B7280	827
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DDopplerLevel	100B72C0	828
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DLevel	100B72F0	829
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DMinMaxDistance	100B7320	830
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DOcclusion	100B7360	831
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Set3DSpread	100B73A0	832
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetCallback	100B73D0	833
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetChannelGroup	100B73F0	834
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetDSPIndex	100B7410	835
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetDelay	100B7430	836
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetFadePointRamp	100B7470	837
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetFrequency	100B74A0	838
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetLoopCount	100B74D0	839
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetLoopPoints	100B74F0	840
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetLowPassGain	100B7510	841
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetMixLevelsInput	100B7540	842
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetMixLevelsOutput	100B7560	843
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetMixMatrix	100B75E0	844
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetMode	100B7600	845
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetMute	100B7620	846
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetPan	100B7650	847
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetPaused	100B7680	848
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetPitch	100B76B0	849
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetPosition	100B76E0	850
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetPriority	100B7700	851
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetReverbProperties	100B7720	852
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetUserData	100B7750	853
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetVolume	100B7770	854
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_SetVolumeRamp	100B77A0	855
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Channel_Stop	100B77D0	856
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetInput	100B77F0	857
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetMix	100B7810	858
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetMixMatrix	100B7830	859
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetOutput	100B7850	860
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetType	100B7870	861
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_GetUserData	100B7890	862
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_SetMix	100B78B0	863
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_SetMixMatrix	100B78E0	864
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSPConnection_SetUserData	100B7900	865
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_AddInput	100B7920	866
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_DisconnectAll	100B7940	867
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_DisconnectFrom	100B7980	868
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetActive	100B79A0	869
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetBypass	100B79E0	870
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetCPUUsage	100B7A20	871
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetChannelFormat	100B7A40	872
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetDataParameterIndex	100B7A60	873
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetIdle	100B7A80	874
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetInfo	100B7AC0	875
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetInput	100B7AE0	876
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetMeteringEnabled	100B7B00	877
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetMeteringInfo	100B7B60	878
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetNumInputs	100B7B80	879
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetNumOutputs	100B7BA0	880
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetNumParameters	100B7BC0	881
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetOutput	100B7BE0	882
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetOutputChannelFormat	100B7C00	883
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetParameterBool	100B7C20	884
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetParameterData	100B7C60	885
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetParameterFloat	100B7C80	886
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetParameterInfo	100B7CA0	887
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetParameterInt	100B7CC0	888
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetSystemObject	100B7CE0	889
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetType	100B7D00	890
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetUserData	100B7D20	891
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_GetWetDryMix	100B7D40	892
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_Release	100B7D60	893
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_Reset	100B7D80	894
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetActive	100B7DA0	895
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetBypass	100B7DD0	896
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetChannelFormat	100B7E00	897
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetMeteringEnabled	100B7E20	898
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetParameterBool	100B7E60	899
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetParameterData	100B7E90	900
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetParameterFloat	100B7EB0	901
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetParameterInt	100B7EE0	902
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetUserData	100B7F00	903
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_SetWetDryMix	100B7F20	904
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_DSP_ShowConfigDialog	100B7F60	905
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Debug_Initialize	100A13D0	906
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_File_GetDiskBusy	100A3830	907
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_File_SetDiskBusy	100A3860	908
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_AddPolygon	100B7F90	909
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetActive	100B7FE0	910
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetMaxPolygons	100B8020	911
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetNumPolygons	100B8040	912
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetPolygonAttributes	100B8060	913
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetPolygonNumVertices	100B80A0	914
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetPolygonVertex	100B80C0	915
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetPosition	100B80E0	916
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetRotation	100B8100	917
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetScale	100B8120	918
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_GetUserData	100B8140	919
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_Release	100B8160	920
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_Save	100B8180	921
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetActive	100B81A0	922
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetPolygonAttributes	100B81D0	923
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetPolygonVertex	100B8220	924
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetPosition	100B8240	925
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetRotation	100B8260	926
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetScale	100B8280	927
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Geometry_SetUserData	100B82A0	928
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Memory_GetStats	100B82C0	929
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Memory_Initialize	100B8430	930
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_Get3DAttributes	100B85A0	931
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_GetActive	100B85C0	932
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_GetProperties	100B8600	933
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_GetUserData	100B8620	934
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_Release	100B8640	935
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_Set3DAttributes	100B8660	936
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_SetActive	100B86A0	937
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_SetProperties	100B86D0	938
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Reverb3D_SetUserData	100B86F0	939
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetMaxAudible	100B8710	940
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetMaxAudibleBehavior	100B8730	941
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetMuteFadeSpeed	100B8750	942
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetName	100B8770	943
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetNumPlaying	100B8790	944
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetNumSounds	100B87B0	945
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetSound	100B87D0	946
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetSystemObject	100B87F0	947
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetUserData	100B8810	948
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_GetVolume	100B8830	949
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_Release	100B8850	950
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_SetMaxAudible	100B8870	951
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_SetMaxAudibleBehavior	100B8890	952
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_SetMuteFadeSpeed	100B88B0	953
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_SetUserData	100B88E0	954
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_SetVolume	100B8900	955
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_SoundGroup_Stop	100B8930	956
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_AddSyncPoint	100B8950	957
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_DeleteSyncPoint	100B8970	958
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Get3DConeSettings	100B8990	959
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Get3DCustomRolloff	100B89B0	960
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Get3DMinMaxDistance	100B89D0	961
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetDefaults	100B89F0	962
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetFormat	100B8A10	963
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetLength	100B8A30	964
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetLoopCount	100B8A50	965
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetLoopPoints	100B8A70	966
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetMode	100B8A90	967
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetMusicChannelVolume	100B8AB0	968
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetMusicNumChannels	100B8AD0	969
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetMusicSpeed	100B8AF0	970
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetName	100B8B10	971
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetNumSubSounds	100B8B30	972
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetNumSyncPoints	100B8B50	973
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetNumTags	100B8B70	974
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetOpenState	100B8B90	975
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSoundGroup	100B8BF0	976
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSubSound	100B8C10	977
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSubSoundParent	100B8C30	978
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSyncPoint	100B8C50	979
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSyncPointInfo	100B8C70	980
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetSystemObject	100B8C90	981
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetTag	100B8CB0	982
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_GetUserData	100B8CD0	983
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Lock	100B8CF0	984
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_ReadData	100B8D10	985
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Release	100B8D30	986
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SeekData	100B8D50	987
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Set3DConeSettings	100B8D70	988
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Set3DCustomRolloff	100B8DB0	989
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Set3DMinMaxDistance	100B8DD0	990
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetDefaults	100B8E10	991
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetLoopCount	100B8E40	992
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetLoopPoints	100B8E60	993
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetMode	100B8E80	994
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetMusicChannelVolume	100B8EA0	995
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetMusicSpeed	100B8ED0	996
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetSoundGroup	100B8F00	997
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_SetUserData	100B8F20	998
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_Sound_Unlock	100B8F40	999
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_AttachChannelGroupToPort	100B8F60	1000
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_AttachFileSystem	100B8FA0	1001
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_Close	100B8FC0	1002
    //LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Create, FMOD_System_Create);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateChannelGroup, FMOD_System_CreateChannelGroup	100B9130	1004
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateDSP, FMOD_System_CreateDSP	100B9150	1005
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateDSPByPlugin, FMOD_System_CreateDSPByPlugin	100B9170	1006
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateDSPByType, FMOD_System_CreateDSPByType	100B9190	1007
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateGeometry, FMOD_System_CreateGeometry	100B91B0	1008
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateReverb3D, FMOD_System_CreateReverb3D	100B91D0	1009
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateSound, FMOD_System_CreateSound	100B91F0	1010
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateSoundGroup, FMOD_System_CreateSoundGroup	100B9210	1011
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_CreateStream, FMOD_System_CreateStream	100B9230	1012
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_DetachChannelGroupFromPort, FMOD_System_DetachChannelGroupFromPort	100B9250	1013
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Get3DListenerAttributes, FMOD_System_Get3DListenerAttributes	100B9270	1014
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Get3DNumListeners, FMOD_System_Get3DNumListeners	100B9290	1015
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Get3DSettings, FMOD_System_Get3DSettings	100B92B0	1016
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetAdvancedSettings	100B92D0	1017
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetCPUUsage	100B92F0	1018
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetChannel	100B9310	1019
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetChannelsPlaying	100B9330	1020
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDSPBufferSize	100B9350	1021
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDSPInfoByPlugin	100B9370	1022
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDSPInfoByType	100B9390	1023
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDefaultMixMatrix	100B93B0	1024
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDriver	100B93D0	1025
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetDriverInfo	100B93F0	1026
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetFileUsage	100B9410	1027
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetGeometryOcclusion	100B9430	1028
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetGeometrySettings	100B9450	1029
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetMasterChannelGroup	100B9470	1030
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetMasterSoundGroup	100B9490	1031
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNestedPlugin	100B94B0	1032
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNetworkProxy	100B94D0	1033
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNetworkTimeout	100B94F0	1034
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNumDrivers	100B9510	1035
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNumNestedPlugins	100B9530	1036
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetNumPlugins	100B9550	1037
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetOutput	100B9570	1038
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetOutputByPlugin	100B9590	1039
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetOutputHandle	100B95B0	1040
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetPluginHandle	100B95D0	1041
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetPluginInfo	100B95F0	1042
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetRecordDriverInfo	100B9610	1043
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetRecordNumDrivers	100B9630	1044
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetRecordPosition	100B9650	1045
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetReverbProperties	100B9670	1046
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetSoftwareChannels	100B9690	1047
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetSoftwareFormat	100B96B0	1048
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetSpeakerModeChannels	100B96D0	1049
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetSpeakerPosition	100B96F0	1050
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetStreamBufferSize	100B9730	1051
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetUserData	100B9750	1052
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_GetVersion	100B9770	1053
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_Init	100B9790	1054
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_IsRecording	100B97B0	1055
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_LoadGeometry	100B97F0	1056
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_LoadPlugin	100B9810	1057
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_LockDSP	100B9830	1058
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_MixerResume	100B9850	1059
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_MixerSuspend	100B9870	1060
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_PlayDSP	100B9890	1061
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_PlaySound	100B98C0	1062
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_RecordStart	100B98F0	1063
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_RecordStop	100B9920	1064
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_RegisterCodec	100B9940	1065
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_RegisterDSP	100B9960	1066
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_RegisterOutput	100B9980	1067
    // LOAD_DLL_IMPORT(FMODDLL, LP_, FMOD_System_Release	100B99A0	1068
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Set3DListenerAttributes, FMOD_System_Set3DListenerAttributes);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Set3DNumListeners, FMOD_System_Set3DNumListeners);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Set3DRolloffCallback, FMOD_System_Set3DRolloffCallback);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Set3DSettings, FMOD_System_Set3DSettings);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetAdvancedSettings, FMOD_System_SetAdvancedSettings);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetCallback, FMOD_System_SetCallback);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetDSPBufferSize, FMOD_System_SetDSPBufferSize);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetDriver, FMOD_System_SetDriver);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetFileSystem, FMOD_System_SetFileSystem);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetGeometrySettings, FMOD_System_SetGeometrySettings);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetNetworkProxy, FMOD_System_SetNetworkProxy);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetNetworkTimeout, FMOD_System_SetNetworkTimeout);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetOutput, FMOD_System_SetOutput);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetOutputByPlugin, FMOD_System_SetOutputByPlugin);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetPluginPath, FMOD_System_SetPluginPath);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetReverbProperties, FMOD_System_SetReverbProperties);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetSoftwareChannels, FMOD_System_SetSoftwareChannels);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetSoftwareFormat, FMOD_System_SetSoftwareFormat);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetSpeakerPosition, FMOD_System_SetSpeakerPosition);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetStreamBufferSize, FMOD_System_SetStreamBufferSize);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_SetUserData, FMOD_System_SetUserData);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_UnloadPlugin, FMOD_System_UnloadPlugin);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_UnlockDSP, FMOD_System_UnlockDSP);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_System_Update, FMOD_System_Update);
    // LOAD_DLL_IMPORT(FMODDLL, LP_FMOD_Thread_SetAttributes, FMOD_Thread_SetAttributes);
    
    /**
     *  All loaded and ready to go.
     */
    FMODImportsLoaded = true;
    
    return true;
}


/**
 *  Free the FMOD library and all loaded pointers.
 * 
 *  @author: CCHyper
 */
void Unload_FMOD_DLL()
{
    DEV_DEBUG_INFO("Unload_FMOD_DLL()\n");

    FreeLibrary(FMODDLL);
    
    //FMOD_System_Create = nullptr;
    
    FMODImportsLoaded = false;
}


#undef LOAD_DLL_IMPORT
