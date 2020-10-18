//#define _INSTANTIATE_TEMPLATE_CLASSES

#include "internal/ren_pch.hpp"
#include "internal/comptr.ctp"

dummyFunction1( void )
{
	static COMPTR(IDirectDraw)				dd1;
	static COMPTR(IDirectDraw2)				dd2;
	static COMPTR(IDirectDraw4)				dd3;
	static COMPTR(IDirectDrawSurface)		dd4;
	static COMPTR(IDirectDrawSurface2)		dd5;
	static COMPTR(IDirectDrawPalette)		dd6;
	static COMPTR(IDirectDrawClipper)		dd7;
	static COMPTR(IDirectDrawGammaControl)	dd8;

	static COMPTR(IDirect3D2)				im1;
	static COMPTR(IDirect3DDevice2)			im2;
	static COMPTR(IDirect3DMaterial2)		im3;
	static COMPTR(IDirect3DLight)			im4;
	static COMPTR(IDirect3DTexture2)		im5;
	static COMPTR(IDirect3DViewport2)		im6;

	static COMPTR(IDirect3DRM)				rm1;
	static COMPTR(IDirect3DRMMeshBuilder)	rm2;
//	static COMPTR(IDirect3DRMMeshBuilder2)	rm3;
	static COMPTR(IDirect3DRMTexture)		rm4;
//	static COMPTR(IDirect3DRMFaceArray)		rm5;
//	static COMPTR(IDirect3DRMFace)			rm6;
	static COMPTR(IDirect3DRMMesh)			rm7;
	static COMPTR(IDirect3DRMFrame)			rm8;
//	static COMPTR(IDirect3DRMFrame2)		rm9;
	static COMPTR(IDirect3DRMVisual)		rm10;
	static COMPTR(IDirect3DRMFrameArray)	rm11;
	static COMPTR(IDirect3DRMVisualArray)	rm12;
	static COMPTR(IDirect3DRMMaterial)		rm13;
}