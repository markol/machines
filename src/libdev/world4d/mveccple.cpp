/*
 * M V E C C P L E . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"

#include "world4d/mveccple.hpp"

#include "render/matvec.hpp"

bool W4dMaterialVecPtr_SetLess::operator()
(
    const Ren::MaterialVecPtr& lhs, const Ren::MaterialVecPtr& rhs
) const
{
    return (*lhs) < (*rhs);
}
/* End MVECCPLE.CPP *************************************************/
