/*
 * G X T E X T U R E S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXTextures

*/

#ifndef _GXTEXTURES_HPP
#define _GXTEXTURES_HPP

#include "base/base.hpp"
#include "ctl/map.hpp"

#include "gxin/gxtexture.hpp"
#include "gxin/gxidpos.hpp"

typedef ctl_map<GXIdPos, GXTexture, std::less<GXIdPos> >	 GXTextures;

#endif

/* End GXTEXTURES.HPP ***********************************************/
