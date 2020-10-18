/*
 * B A C K G R N D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/device.hpp"
#include "render/backgrnd.hpp"

RenBackground::RenBackground():
//	vertices_(_NEW_ARRAY(D3DTLVERTEX, 4))
	vertices_(_NEW_ARRAY(RenIVertex, 4))
{
	/*vertices_[0].sx = D3DVAL(0);
	vertices_[1].sx = D3DVAL(639);
	vertices_[2].sx = D3DVAL(639);
	vertices_[3].sx = D3DVAL(0);

	vertices_[0].sy = D3DVAL(0);
	vertices_[1].sy = D3DVAL(0);
	vertices_[2].sy = D3DVAL(479);
	vertices_[3].sy = D3DVAL(479);

	vertices_[0].sz = D3DVAL(0.99);
	vertices_[1].sz = D3DVAL(0.99);
	vertices_[2].sz = D3DVAL(0.99);
	vertices_[3].sz = D3DVAL(0.99);

	vertices_[0].rhw = D3DVAL(0.01);
	vertices_[1].rhw = D3DVAL(0.01);
	vertices_[2].rhw = D3DVAL(0.01);
	vertices_[3].rhw = D3DVAL(0.01);

	vertices_[0].color = D3DRGBA(1, 0, 1, 1);
	vertices_[1].color = vertices_[0].color;
	vertices_[2].color = vertices_[0].color;
	vertices_[3].color = vertices_[0].color;

	vertices_[0].specular = D3DRGB(0, 0, 0);
	vertices_[1].specular = vertices_[0].specular;
	vertices_[2].specular = vertices_[0].specular;
	vertices_[3].specular = vertices_[0].specular;

	vertices_[0].tu = D3DVAL(0); vertices_[0].tv = D3DVAL(0);
	vertices_[1].tu = D3DVAL(0); vertices_[1].tv = D3DVAL(0);
	vertices_[2].tu = D3DVAL(0); vertices_[2].tv = D3DVAL(0);
	vertices_[3].tu = D3DVAL(0); vertices_[3].tv = D3DVAL(0);*/

	vertices_[0].x = (-1);
	vertices_[1].x = (1);
	vertices_[2].x = (1);
	vertices_[3].x = (-1);

	vertices_[0].y = (-1);
	vertices_[1].y = (-1);
	vertices_[2].y = (1);
	vertices_[3].y = (1);

	vertices_[0].z = (0.99);
	vertices_[1].z = (0.99);
	vertices_[2].z = (0.99);
	vertices_[3].z = (0.99);

	vertices_[0].w = (0.01);
	vertices_[1].w = (0.01);
	vertices_[2].w = (0.01);
	vertices_[3].w = (0.01);

	vertices_[0].color = packColourChecked(1, 0, 1, 1);
	vertices_[1].color = vertices_[0].color;
	vertices_[2].color = vertices_[0].color;
	vertices_[3].color = vertices_[0].color;

	vertices_[0].specular = packColourChecked(0, 0, 0);
	vertices_[1].specular = vertices_[0].specular;
	vertices_[2].specular = vertices_[0].specular;
	vertices_[3].specular = vertices_[0].specular;

	vertices_[0].tu = (0); vertices_[0].tv = (0);
	vertices_[1].tu = (0); vertices_[1].tv = (0);
	vertices_[2].tu = (0); vertices_[2].tv = (0);
	vertices_[3].tu = (0); vertices_[3].tv = (0);
}

RenBackground::~RenBackground()
{
	_DELETE_ARRAY(vertices_);
}

void RenBackground::render() const
{
	//IDirect3DDevice2* d3dDev = RenDevice::current()->device();

	// Perspective texture correction is unnecessary.  Turning it off will
	// give a big saving for software drivers.
/*	DWORD perspective;
	TRYRENDX(d3dDev->GetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, &perspective));
	TRYRENDX(d3dDev->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, false));

	static WORD indices[4] = { 0,1,2,3 };
	TRYRENDX(d3dDev->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
		  vertices_, 4, indices, 4, 0));

	// Reset these items of state to their previous values.
	TRYRENDX(d3dDev->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, perspective));*/
//	static Ren::VertexIdx indices[4] = { 0,1,2,3 };
//	RenDevice::current()->renderIndexedScreenspace(vertices_, 4, indices, mat, GL_TRIANGLE_FAN);
}

/* End BACKGRND.CPP *************************************************/
