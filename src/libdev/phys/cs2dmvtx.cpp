/*
 * C S 2 D M V T X . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2dmvtx.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2dmvtx.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainVertex::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dDomainVertex& t )
{
    o << "portal id " << t.portalId_.asScalar();
    o << " point " << t.point_;
    o << " clearance " << t.clearance_;
    o << " flags " << t.flags_;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DMVTX.CPP *************************************************/
