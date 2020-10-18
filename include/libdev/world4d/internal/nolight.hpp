/*
 * N O L I G H T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _WORLD4D_NOLIGHT_HPP
#define _WORLD4D_NOLIGHT_HPP

#include "base/base.hpp"

class W4dEntity;

// Determine if the given entity will need lighting calculations.  If
// RenMaterial::needsLighting is false for every material, the return 
// value is true.  Put here rather than added to W4dEntity.  It doesn't
// change the entities doNotLight flag (the client should).
bool w4dNeedsLighting(const W4dEntity* entity);

#endif

/* End NOLIGHT.HPP **************************************************/
