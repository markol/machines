/*
 * T I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/line2d.hpp"

#include "phys/absp2d.hpp"
class MexPolygon2d;

void PhysDummyFunction3()
{
	MexPoint2d minCorner, maxCorner;
	static PhysAlignedBSPTree2d< MexLine2d* > d1( minCorner, maxCorner, 10.0, 3, 0);
    static ctl_vector< MexPolygon2d* > d2;
    static ctl_vector< bool > d3;
}
/* End TI.CPP *****************************************************/
