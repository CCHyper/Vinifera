/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW7_UTIL.H
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
#include "ddraw7_support.h"
#include "d7surface.h"
#include "bsurface.h"
#include "tibsun_globals.h"
#include "options.h"
#include "ccini.h"
#include "rgb.h"
#include "textprint.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Gets the error description based on the result of a DD command and returns a
 *  pointer to a string describing the given DD, D3D or D3DRM error code.
 */
char const *Get_DD_Error_Description(HRESULT ddres)
{
    static char buffer[256];
    char const *errorstr = nullptr;

    switch (ddres) {

        /**
         *  Direct Draw
         */
        case DDERR_NODIRECTDRAWSUPPORT:
            errorstr = "No DirectDraw support possible with current display driver.";
            break;
        case DDERR_CANTLOCKSURFACE:
            errorstr = "Unable to lock surface because no driver exists which can supply a pointer to the surface.";
            break;
        case DDERR_INVALIDSURFACETYPE:
            errorstr = "The requested action could not be performed because the surface was of the wrong type.";
            break;
        case DDERR_ALREADYINITIALIZED:
            errorstr = "This object is already initialized.";
            break;
        case DDERR_BLTFASTCANTCLIP:
            errorstr = "Return if a clipper object is attached to the source surface passed into a BltFast call.";
            break;
        case DDERR_CANNOTATTACHSURFACE:
            errorstr = "This surface can not be attached to the requested surface.";
            break;
        case DDERR_CANNOTDETACHSURFACE:
            errorstr = "This surface can not be detached from the requested surface.";
            break;
        case DDERR_CANTCREATEDC:
            errorstr = "Windows can not create any more DCs.";
            break;
        case DDERR_CANTDUPLICATE:
            errorstr = "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.";
            break;
        case DDERR_CLIPPERISUSINGHWND:
            errorstr = "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.";
            break;
        case DDERR_COLORKEYNOTSET:
            errorstr = "No src color key specified for this operation.";
            break;
        case DDERR_CURRENTLYNOTAVAIL:
            errorstr = "Support is currently not available.";
            break;
        case DDERR_DIRECTDRAWALREADYCREATED:
            errorstr = "A DirectDraw object representing this driver has already been created for this process.";
            break;
        case DDERR_EXCEPTION:
            errorstr = "An exception was encountered while performing the requested operation.";
            break;
        case DDERR_EXCLUSIVEMODEALREADYSET:
            errorstr = "An attempt was made to set the cooperative level when it was already set to exclusive.";
            break;
        case DDERR_GENERIC:
            errorstr = "Generic failure.";
            break;
        case DDERR_HEIGHTALIGN:
            errorstr = "Height of rectangle provided is not a multiple of reqd alignment.";
            break;
        case DDERR_HWNDALREADYSET:
            errorstr = "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.";
            break;
        case DDERR_HWNDSUBCLASSED:
            errorstr = "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.";
            break;
        case DDERR_IMPLICITLYCREATED:
            errorstr = "This surface can not be restored because it is an implicitly created surface.";
            break;
        case DDERR_INCOMPATIBLEPRIMARY:
            errorstr = "Unable to match primary surface creation request with existing primary surface.";
            break;
        case DDERR_INVALIDCAPS:
            errorstr = "One or more of the caps bits passed to the callback are incorrect.";
            break;
        case DDERR_INVALIDCLIPLIST:
            errorstr = "DirectDraw does not support the provided cliplist.";
            break;
        case DDERR_INVALIDDIRECTDRAWGUID:
            errorstr = "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.";
            break;
        case DDERR_INVALIDMODE:
            errorstr = "DirectDraw does not support the requested mode.";
            break;
        case DDERR_INVALIDOBJECT:
            errorstr = "DirectDraw received a pointer that was an invalid DIRECTDRAW object.";
            break;
        case DDERR_INVALIDPARAMS:
            errorstr = "One or more of the parameters passed to the function are incorrect.";
            break;
        case DDERR_INVALIDPIXELFORMAT:
            errorstr = "The pixel format was invalid as specified.";
            break;
        case DDERR_INVALIDPOSITION:
            errorstr = "Returned when the position of the overlay on the destination is no longer legal for that destination.";
            break;
        case DDERR_INVALIDRECT:
            errorstr = "Rectangle provided was invalid.";
            break;
        case DDERR_LOCKEDSURFACES:
            errorstr = "Operation could not be carried out because one or more surfaces are locked.";
            break;
        case DDERR_NO3D:
            errorstr = "There is no 3D present.";
            break;
        case DDERR_NOALPHAHW:
            errorstr = "Operation could not be carried out because there is no alpha accleration hardware present or available.";
            break;
#if(0)
        case DDERR_NOANTITEARHW:
            errorstr = "Operation could not be carried out because there is no hardware support for synchronizing blts to avoid tearing.";
            break;
#endif
        case DDERR_NOBLTHW:
            errorstr = "No blitter hardware present.";
            break;
#if(0)
        case DDERR_NOBLTQUEUEHW:
            errorstr = "Operation could not be carried out because there is no hardware support for asynchronous blting.";
            break;
#endif
        case DDERR_NOCLIPLIST:
            errorstr = "No cliplist available.";
            break;
        case DDERR_NOCLIPPERATTACHED:
            errorstr = "No clipper object attached to surface object.";
            break;
        case DDERR_NOCOLORCONVHW:
            errorstr = "Operation could not be carried out because there is no color conversion hardware present or available.";
            break;
        case DDERR_NOCOLORKEY:
            errorstr = "Surface doesn't currently have a color key";
            break;
        case DDERR_NOCOLORKEYHW:
            errorstr = "Operation could not be carried out because there is no hardware support of the destination color key.";
            break;
        case DDERR_NOCOOPERATIVELEVELSET:
            errorstr = "Create function called without DirectDraw object method SetCooperativeLevel being called.";
            break;
        case DDERR_NODC:
            errorstr = "No DC was ever created for this surface.";
            break;
        case DDERR_NODDROPSHW:
            errorstr = "No DirectDraw ROP hardware.";
            break;
        case DDERR_NODIRECTDRAWHW:
            errorstr = "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.";
            break;
        case DDERR_NOEMULATION:
            errorstr = "Software emulation not available.";
            break;
        case DDERR_NOEXCLUSIVEMODE:
            errorstr = "Operation requires the application to have exclusive mode but the application does not have exclusive mode.";
            break;
        case DDERR_NOFLIPHW:
            errorstr = "Flipping visible surfaces is not supported.";
            break;
        case DDERR_NOGDI:
            errorstr = "There is no GDI present.";
            break;
        case DDERR_NOHWND:
            errorstr = "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.";
            break;
        case DDERR_NOMIRRORHW:
            errorstr = "Operation could not be carried out because there is no hardware present or available.";
            break;
        case DDERR_NOOVERLAYDEST:
            errorstr = "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.";
            break;
        case DDERR_NOOVERLAYHW:
            errorstr = "Operation could not be carried out because there is no overlay hardware present or available.";
            break;
        case DDERR_NOPALETTEATTACHED:
            errorstr = "No palette object attached to this surface.";
            break;
        case DDERR_NOPALETTEHW:
            errorstr = "No hardware support for 16 or 256 color palettes.";
            break;
        case DDERR_NORASTEROPHW:
            errorstr = "Operation could not be carried out because there is no appropriate raster op hardware present or available.";
            break;
        case DDERR_NOROTATIONHW:
            errorstr = "Operation could not be carried out because there is no rotation hardware present or available.";
            break;
        case DDERR_NOSTRETCHHW:
            errorstr = "Operation could not be carried out because there is no hardware support for stretching.";
            break;
        case DDERR_NOT4BITCOLOR:
            errorstr = "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.";
            break;
        case DDERR_NOT4BITCOLORINDEX:
            errorstr = "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.";
            break;
        case DDERR_NOT8BITCOLOR:
            errorstr = "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.";
            break;
        case DDERR_NOTAOVERLAYSURFACE:
            errorstr = "Returned when an overlay member is called for a non-overlay surface.";
            break;
        case DDERR_NOTEXTUREHW:
            errorstr = "Operation could not be carried out because there is no texture mapping hardware present or available.";
            break;
        case DDERR_NOTFLIPPABLE:
            errorstr = "An attempt has been made to flip a surface that is not flippable.";
            break;
        case DDERR_NOTFOUND:
            errorstr = "Requested item was not found.";
            break;
        case DDERR_NOTLOCKED:
            errorstr = "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.";
            break;
        case DDERR_NOTPALETTIZED:
            errorstr = "The surface being used is not a palette-based surface.";
            break;
        case DDERR_NOVSYNCHW:
            errorstr = "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.";
            break;
        case DDERR_NOZBUFFERHW:
            errorstr = "Operation could not be carried out because there is no hardware support for zbuffer blitting.";
            break;
        case DDERR_NOZOVERLAYHW:
            errorstr = "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.";
            break;
        case DDERR_OUTOFCAPS:
            errorstr = "The hardware needed for the requested operation has already been allocated.";
            break;
        case DDERR_OUTOFMEMORY:
            errorstr = "There is not enough memory to perform the operation.";
            break;
        case DDERR_OUTOFVIDEOMEMORY:
            errorstr = "DirectDraw does not have enough memory to perform the operation.";
            break;
        case DDERR_OVERLAYCANTCLIP:
            errorstr = "The hardware does not support clipped overlays.";
            break;
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            errorstr = "Can only have ony color key active at one time for overlays.";
            break;
        case DDERR_OVERLAYNOTVISIBLE:
            errorstr = "Returned when GetOverlayPosition is called on a hidden overlay.";
            break;
        case DDERR_PALETTEBUSY:
            errorstr = "Access to this palette is being refused because the palette is already locked by another thread.";
            break;
        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            errorstr = "This process already has created a primary surface.";
            break;
        case DDERR_REGIONTOOSMALL:
            errorstr = "Region passed to Clipper::GetClipList is too small.";
            break;
        case DDERR_SURFACEALREADYATTACHED:
            errorstr = "This surface is already attached to the surface it is being attached to.";
            break;
        case DDERR_SURFACEALREADYDEPENDENT:
            errorstr = "This surface is already a dependency of the surface it is being made a dependency of.";
            break;
        case DDERR_SURFACEBUSY:
            errorstr = "Access to this surface is being refused because the surface is already locked by another thread.";
            break;
        case DDERR_SURFACEISOBSCURED:
            errorstr = "Access to surface refused because the surface is obscured.";
            break;
        case DDERR_SURFACELOST:
            errorstr = "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.";
            break;
        case DDERR_SURFACENOTATTACHED:
            errorstr = "The requested surface is not attached.";
            break;
        case DDERR_TOOBIGHEIGHT:
            errorstr = "Height requested by DirectDraw is too large.";
            break;
        case DDERR_TOOBIGSIZE:
            errorstr = "Size requested by DirectDraw is too large, but the individual height and width are OK.";
            break;
        case DDERR_TOOBIGWIDTH:
            errorstr = "Width requested by DirectDraw is too large.";
            break;
        case DDERR_UNSUPPORTED:
            errorstr = "Function call not supported.";
            break;
        case DDERR_UNSUPPORTEDFORMAT:
            errorstr = "FOURCC format requested is unsupported by DirectDraw.";
            break;
        case DDERR_UNSUPPORTEDMASK:
            errorstr = "Bitmask in the pixel format requested is unsupported by DirectDraw.";
            break;
        case DDERR_VERTICALBLANKINPROGRESS:
            errorstr = "Vertical blank is in progress.";
            break;
        case DDERR_WASSTILLDRAWING:
            errorstr = "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.";
            break;
        case DDERR_WRONGMODE:
            errorstr = "This surface can not be restored because it was created in a different mode.";
            break;
        case DDERR_XALIGN:
            errorstr = "Rectangle provided was not horizontally aligned on required boundary.";
            break;

        /**
         *  Direct 3D
         */
#if 0
        case D3DERR_INVALIDCALL:
            errorstr = "Invalid Call";
            break;
        case D3DERR_NOTAVAILABLE:
            errorstr = "Not Available";
            break;
        case D3DERR_OUTOFVIDEOMEMORY: // Duplicate id number of DDERR_OUTOFVIDEOMEMORY.
            errorstr = "Out of Video Memory";
            break;
#endif

#if 0
        case D3DERR_BADMAJORVERSION:
            errorstr = "The service that you requested is unavailable in this major version of DirectX. (A major version denotes a primary release, such as DirectX 6.0..";
            break;
            break;
        case D3DERR_BADMINORVERSION:
            errorstr = "The service that you requested is available in this major version of DirectX, but not in this minor version. Get the latest version of the component run time from Microsoft. (A minor version denotes a secondary release, such as DirectX 6.1..";
            break;
        case D3DERR_COLORKEYATTACHED:
            errorstr = "The application attempted to create a texture with a surface that uses a color key for transparency.";
            break;
        case D3DERR_CONFLICTINGTEXTUREFILTER:
            errorstr = "The current texture filters cannot be used together.";
            break;
        case D3DERR_CONFLICTINGTEXTUREPALETTE:
            errorstr = "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palettized textures simultaneously enabled also share the same palette.";
            break;
        case D3DERR_CONFLICTINGRENDERSTATE:
            errorstr = "The currently set render states cannot be used together.";
            break;
        case D3DERR_DEVICEAGGREGATED:
            errorstr = "The IDirect3DDevice7::SetRenderTarget method was called on a device that was retrieved from the render target surface.";
            break;
        case D3DERR_EXECUTE_CLIPPED_FAILED:
            errorstr = "The execute buffer could not be clipped during execution.";
            break;
        case D3DERR_EXECUTE_CREATE_FAILED:
            errorstr = "The execute buffer could not be created. This typically occurs when no memory is available to allocate the execute buffer.";
            break;
        case D3DERR_EXECUTE_DESTROY_FAILED:
            errorstr = "The memory for the execute buffer could not be deallocated.";
            break;
        case D3DERR_EXECUTE_FAILED:
            errorstr = "The contents of the execute buffer are invalid and cannot be executed.";
            break;
        case D3DERR_EXECUTE_LOCK_FAILED:
            errorstr = "The execute buffer could not be locked.";
            break;
        case D3DERR_EXECUTE_LOCKED:
            errorstr = "The operation requested by the application could not be completed because the execute buffer is locked.";
            break;
        case D3DERR_EXECUTE_NOT_LOCKED:
            errorstr = "The execute buffer could not be unlocked because it is not currently locked.";
            break;
        case D3DERR_EXECUTE_UNLOCK_FAILED:
            errorstr = "The execute buffer could not be unlocked.";
            break;
        case D3DERR_INBEGIN:
            errorstr = "The requested operation cannot be completed while scene rendering is taking place. Try again after the scene is completed and the IDirect3DDevice7::EndScene method is called.";
            break;
        case D3DERR_INBEGINSTATEBLOCK:
            errorstr = "The operation cannot be completed while recording states for a state block. Complete recording by calling the IDirect3DDevice7::EndStateBlock method, and try again.";
            break;
        case D3DERR_INITFAILED:
            errorstr = "A rendering device could not be created because the new device could not be initialized.";
            break;
        case D3DERR_INVALID_DEVICE:
            errorstr = "The requested device type is not valid.";
            break;
        case D3DERR_INVALIDCURRENTVIEWPORT:
            errorstr = "The currently selected viewport is not valid.";
            break;
        case D3DERR_INVALIDMATRIX:
            errorstr = "The requested operation could not be completed because the combination of the currently set world, view, and projection matrices is invalid (the determinant of the combined matrix is 0).";
            break;
        case D3DERR_INVALIDPALETTE:
            errorstr = "The palette associated with a surface is invalid.";
            break;
        case D3DERR_INVALIDPRIMITIVETYPE:
            errorstr = "The primitive type specified by the application is invalid.";
            break;
        case D3DERR_INVALIDRAMPTEXTURE:
            errorstr = "Ramp mode is being used, and the texture handle in the current material does not match the current texture handle that is set as a render state.";
            break;
        case D3DERR_INVALIDSTATEBLOCK:
            errorstr = "The state block handle is invalid.";
            break;
        case D3DERR_INVALIDVERTEXFORMAT:
            errorstr = "The combination of flexible vertex format flags specified by the application is not valid.";
            break;
        case D3DERR_INVALIDVERTEXTYPE:
            errorstr = "The vertex type specified by the application is invalid.";
            break;
        case D3DERR_LIGHT_SET_FAILED:
            errorstr = "The attempt to set lighting parameters for a light object failed.";
            break;
        case D3DERR_LIGHTHASVIEWPORT:
            errorstr = "The requested operation failed because the light object is associated with another viewport.";
            break;
        case D3DERR_LIGHTNOTINTHISVIEWPORT:
            errorstr = "The requested operation failed because the light object has not been associated with this viewport.";
            break;
        case D3DERR_MATERIAL_CREATE_FAILED:
            errorstr = "The material could not be created. This typically occurs when no memory is available to allocate for the material.";
            break;
        case D3DERR_MATERIAL_DESTROY_FAILED:
            errorstr = "The memory for the material could not be deallocated.";
            break;
        case D3DERR_MATERIAL_GETDATA_FAILED:
            errorstr = "The material parameters could not be retrieved.";
            break;
        case D3DERR_MATERIAL_SETDATA_FAILED:
            errorstr = "The material parameters could not be set.";
            break;
        case D3DERR_MATRIX_CREATE_FAILED:
            errorstr = "The matrix could not be created. This can occur when no memory is available to allocate for the matrix.";
            break;
        case D3DERR_MATRIX_DESTROY_FAILED:
            errorstr = "The memory for the matrix could not be deallocated.";
            break;
        case D3DERR_MATRIX_GETDATA_FAILED:
            errorstr = "The matrix data could not be retrieved. This can occur when the matrix was not created by the current device.";
            break;
        case D3DERR_MATRIX_SETDATA_FAILED:
            errorstr = "The matrix data could not be set. This can occur when the matrix was not created by the current device.";
            break;
        case D3DERR_NOCURRENTVIEWPORT:
            errorstr = "The viewport parameters could not be retrieved because none have been set.";
            break;
        case D3DERR_NOTINBEGIN:
            errorstr = "The requested rendering operation could not be completed because scene rendering has not begun. Call IDirect3DDevice7::BeginScene to begin rendering, and try again.";
            break;
        case D3DERR_NOTINBEGINSTATEBLOCK:
            errorstr = "The requested operation could not be completed because it is only valid while recording a state block. Call the IDirect3DDevice7::BeginStateBlock method, and try again.";
            break;
        case D3DERR_NOVIEWPORTS:
            errorstr = "The requested operation failed because the device currently has no viewports associated with it.";
            break;
        case D3DERR_SCENE_BEGIN_FAILED:
            errorstr = "Scene rendering could not begin.";
            break;
        case D3DERR_SCENE_END_FAILED:
            errorstr = "Scene rendering could not be completed.";
            break;
        case D3DERR_SCENE_IN_SCENE:
            errorstr = "Scene rendering could not begin because a previous scene was not completed by a call to the IDirect3DDevice7::EndScene method.";
            break;
        case D3DERR_SCENE_NOT_IN_SCENE:
            errorstr = "Scene rendering could not be completed because a scene was not started by a previous call to the IDirect3DDevice7::BeginScene method.";
            break;
        case D3DERR_SETVIEWPORTDATA_FAILED:
            errorstr = "The viewport parameters could not be set.";
            break;
        case D3DERR_STENCILBUFFER_NOTPRESENT:
            errorstr = "The requested stencil buffer operation could not be completed because there is no stencil buffer attached to the render target surface.";
            break;
        case D3DERR_SURFACENOTINVIDMEM:
            errorstr = "The device could not be created because the render target surface is not located in video memory. (Hardware-accelerated devices require video-memory render target surfaces..";
            break;
        case D3DERR_TEXTURE_BADSIZE:
            errorstr = "The dimensions of a current texture are invalid. This can occur when an application attempts to use a texture that has dimensions that are not a power of 2 with a device that requires them.";
            break;
        case D3DERR_TEXTURE_CREATE_FAILED:
            errorstr = "The texture handle for the texture could not be retrieved from the driver.";
            break;
        case D3DERR_TEXTURE_DESTROY_FAILED:
            errorstr = "The device was unable to deallocate the texture memory.";
            break;
        case D3DERR_TEXTURE_GETSURF_FAILED:
            errorstr = "The DirectDraw surface used to create the texture could not be retrieved.";
            break;
        case D3DERR_TEXTURE_LOAD_FAILED:
            errorstr = "The texture could not be loaded.";
            break;
        case D3DERR_TEXTURE_LOCK_FAILED:
            errorstr = "The texture could not be locked.";
            break;
        case D3DERR_TEXTURE_LOCKED:
            errorstr = "The requested operation could not be completed because the texture surface is currently locked.";
            break;
        case D3DERR_TEXTURE_NO_SUPPORT:
            errorstr = "The device does not support texture mapping.";
            break;
        case D3DERR_TEXTURE_NOT_LOCKED:
            errorstr = "The requested operation could not be completed because the texture surface is not locked.";
            break;
        case D3DERR_TEXTURE_SWAP_FAILED:
            errorstr = "The texture handles could not be swapped.";
            break;
        case D3DERR_TEXTURE_UNLOCK_FAILED:
            errorstr = "The texture surface could not be unlocked.";
            break;
        case D3DERR_TOOMANYOPERATIONS:
            errorstr = "The application is requesting more texture-filtering operations than the device supports.";
            break;
        case D3DERR_TOOMANYPRIMITIVES:
            errorstr = "The device is unable to render the provided number of primitives in a single pass.";
            break;
        case D3DERR_UNSUPPORTEDALPHAARG:
            errorstr = "The device does not support one of the specified texture-blending arguments for the alpha channel.";
            break;
        case D3DERR_UNSUPPORTEDALPHAOPERATION:
            errorstr = "The device does not support one of the specified texture-blending operations for the alpha channel.";
            break;
        case D3DERR_UNSUPPORTEDCOLORARG:
            errorstr = "The device does not support one of the specified texture-blending arguments for color values.";
            break;
        case D3DERR_UNSUPPORTEDCOLOROPERATION:
            errorstr = "The device does not support one of the specified texture-blending operations for color values.";
            break;
        case D3DERR_UNSUPPORTEDFACTORVALUE:
            errorstr = "The specified texture factor value is not supported by the device.";
            break;
        case D3DERR_UNSUPPORTEDTEXTUREFILTER:
            errorstr = "The specified texture filter is not supported by the device.";
            break;
        case D3DERR_VBUF_CREATE_FAILED:
            errorstr = "The vertex buffer could not be created. This can happen when there is insufficient memory to allocate a vertex buffer.";
            break;
        case D3DERR_VERTEXBUFFERLOCKED:
            errorstr = "The requested operation could not be completed because the vertex buffer is locked.";
            break;
        case D3DERR_VERTEXBUFFEROPTIMIZED:
            errorstr = "The requested operation could not be completed because the vertex buffer is optimized. (The contents of optimized vertex buffers are driver-specific and considered private..";
            break;
        case D3DERR_VERTEXBUFFERUNLOCKFAILED:
            errorstr = "The vertex buffer could not be unlocked because the vertex buffer memory was overrun. Be sure that your application does not write beyond the size of the vertex buffer.";
            break;
        case D3DERR_VIEWPORTDATANOTSET:
            errorstr = "The requested operation could not be completed because viewport parameters have not yet been set. Set the viewport parameters by calling the IDirect3DDevice7::SetViewport method, and try again.";
            break;
        case D3DERR_VIEWPORTHASNODEVICE:
            errorstr = "The requested operation could not be completed because the viewport has not yet been associated with a device. Associate the viewport with a rendering device by calling the IDirect3DDevice3::AddViewport method, and try again.";
            break;
        case D3DERR_WRONGTEXTUREFORMAT:
            errorstr = "The pixel format of the texture surface is not valid.";
            break;
        case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
            errorstr = "The requested operation could not be completed because the specified device requires system-memory depth-buffer surfaces. (Software rendering devices require system-memory depth buffers..";
            break;
        case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
            errorstr = "The requested operation could not be completed because the specified device requires video-memory depth-buffer surfaces. (Hardware-accelerated devices require video-memory depth buffers..";
            break;
        case D3DERR_ZBUFFER_NOTPRESENT:
            errorstr = "The requested operation could not be completed because the render target surface does not have an attached depth buffer.";
            break;
#endif

        case DD_OK:
        //case D3D_OK:
        //case D3DRM_OK:
            errorstr = "Direct Draw request went ok.";
            break;

        default:
            errorstr = "Unrecognized error value.";
            break;

    };

    std::snprintf(buffer, sizeof(buffer), errorstr);
    return buffer;
}


/**
 *  Displays a message box containing the given formatted string.
 */
void DD_Error_Message_Box(HRESULT ddres, char const *fmt, ...)
{
    char buff[1024];

    va_list args;
    va_start(args, fmt);

    std::snprintf(buff, sizeof(buff), fmt, ddres, args);

    va_end(args);

    std::strcat(buff, "\r\n");
    std::strcat(buff, Get_DD_Error_Description(ddres));

    MessageBox(MainWindow, buff, "DirectX Error", MB_ICONEXCLAMATION|MB_OK);
}
