/*
 * T T F P O L Y I . I P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#include "render/internal/devicei.hpp"
#include "render/device.hpp"

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
void RenITTFImpl::renderGL
(
    RenIVertex* vtx,
    const RenMaterial& mat,
    GLenum mode
) const
{
    glDisable(GL_CULL_FACE);
    Ren::VertexIdx* crufty = _CONST_CAST(Ren::VertexIdx*, indices_);
    RenDevice::current()->renderIndexedScreenspace(vtx, nVertices_, crufty, nIndices_, mat, mode);
    glEnable(GL_CULL_FACE);
}

/* End TTFPOLYI.IPP *************************************************/
