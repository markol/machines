/*
 * F W M A T E R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Forward declarations for classes related to materials and vectors of em etc
*/

#ifndef _WORLD4D_FWMATERI_HPP
#define _WORLD4D_FWMATERI_HPP

#include "render/render.hpp"
#include "render/material.hpp"

template < class T > class ctl_vector;
template < class KEY, class COMPARE > class ctl_set;
template < class KEY, class T, class COMPARE > class ctl_map;

//Duplicates matmap.hpp declarations
typedef  ctl_set< RenMaterial, 				RenMaterial::Less > RenMaterialSet;
typedef  ctl_map< RenMaterial, RenMaterial, RenMaterial::Less > RenMaterialMap;

typedef ctl_vector< Ren::MaterialVecPtr > W4dMaterialVecPtrs;
class W4dMaterialVecPtr_SetLess;

//A set of counted pointers to material vectors, sorted on the material vector value
//rather than pointer address.
typedef ctl_set< Ren::MaterialVecPtr, W4dMaterialVecPtr_SetLess > W4dMaterialVecPtrSet;

#endif

/* End FWMATERI.HPP *************************************************/
