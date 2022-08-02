/*
 * M V E C C P L E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dMaterialVecPtr_SetLess

    Implements a comparator for a pair of Ren::MaterialVecPtr which compares the
    referenced material vectors rather than the pointers.
*/

#ifndef _WORLD4D_MVECCPLE_HPP
#define _WORLD4D_MVECCPLE_HPP

#include "base/base.hpp"
#include "render/render.hpp"

class W4dMaterialVecPtr_SetLess
{
public:
    bool operator() ( const Ren::MaterialVecPtr& lhs, const Ren::MaterialVecPtr& rhs ) const;
};


#endif

/* End MVECCPLE.HPP *************************************************/
