/*
 * B A C K G R N D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _WORLD4D_BACKGRND_HPP
#define _WORLD4D_BACKGRND_HPP

#include "mathex/mathex.hpp"

class SysPathName;
class W4dEntity;
class MexDegrees;

// This auxilliary function reads a tree of generic entities to use as 
// backgound objects.  The objects fall into three animation categories:
//   1) no animation
//   2) animated as celestial bodies (rotating around the sky)
//   3) animated as clouds (translated around the sky)
// Although this function doesn't define the animations, it is assumed 
// that the caller will create appropriate animations for the differnt
// categories.  The caller must create all three roots, but not the
// object which represents the primary light source (the sun).
bool W4dReadBackground
(
	const SysPathName& pathname, 
	W4dEntity*  root,			// the root for unanimated objects
	W4dEntity** primaryLight,	// an object representing the main light source
	W4dEntity*  cloudRoot,		// root for objects animated as clouds
	W4dEntity*  celestialRoot,	// root for objects animated as suns, moons, etc.
	MATHEX_SCALAR* cloudSpeed,	// max translation speed of clouds per sec
	MexDegrees* rotSpeed		// max rotation speed of celestials per sec
);

#endif

/* End BACKGRND.HPP *************************************************/
