/*
 * T I 2. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES
//#define _INSTANTIATE_TEMPLATE_FUNCTIONS

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "mathex/poly2d.hpp"
#include "phys/cspace2.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2domai.hpp"
#include "phys/internal/cs2vigra.hpp"
#include "phys/internal/cs2dmfnd.hpp"
#include "phys/internal/cs2dmcac.hpp"
#include "phys/internal/cs2mochu.hpp"

//Forward refs

void PhysDummyFunction2()
{
    static FtlSerialPMap< PhysCS2dDomain > dummy9( 10 );
    static FtlSerialMap< PhysConfigSpace2d::PolygonId > dummy13;
    static PhysCS2dImpl::DomainTree dummy10( MexPoint2d(0,0), MexPoint2d(1,1),1,1,1 );
    static PhysConfigSpace2d dummy11( MexPoint2d(0,0), MexPoint2d(1,1),
                                      PhysConfigSpace2d::SUBTRACTIVE, 100, 1, 10, 0 );
    static PhysCS2dExpansionSpace* pExpansionSpace = NULL;
    static PhysCS2dVisibilityGraph dummy12( &dummy11, &dummy11, pExpansionSpace, 2.0 );
    static ctl_vector< PhysCS2dDomainFindPathCache::PathData >  dummy14;
}

/* End TI2.CPP *****************************************************/
