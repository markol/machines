/*
 * I L L U M I N E . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//#include "render/internal/d3d.hpp"

/*_CODE_INLINE
void RenITexBody::applyD3DHandle(IDirect3DDevice2* dev) const
{
	PRE(dev);
	PRE_INFO((void*) handleDev_);
	PRE_INFO((void*) dev);
	PRE(!handleDev_ || handleDev_ == dev);

    TRYRENDX(dev->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, handle_));

	// Detailed debugging: there's a large overhead in writing this many times per frame.
	// RENDER_STREAM("Using tex handle " << handle_ << endl);
}
*/
/* End MATERIAL.IPP **************************************************/
