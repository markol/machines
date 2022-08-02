/*
 * F W M A T M A P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_FWMATMAP_HPP
#define _RENDER_FWMATMAP_HPP

// Forward declarations of material maps and sets.  These classes appear in
// lots of interfaces, so forward declarations are useful in several places.
#include "render/material.hpp"

template <class T,			class C> class ctl_set;
template <class K, class T, class C> class ctl_map;

// Note: less<RenMaterial> isn't used simply to reduce STL dependencies.
typedef  ctl_set< RenMaterial, 				RenMaterial::Less > RenMaterialSet;
typedef  ctl_map< RenMaterial, RenMaterial, RenMaterial::Less > RenMaterialMap;

#endif

/* End FWMATMAP.HPP ***************************************************/
