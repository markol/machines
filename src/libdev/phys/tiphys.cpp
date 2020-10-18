/*
 * T I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES
//#define _INSTANTIATE_TEMPLATE_FUNCTIONS

#include "base/base.hpp"
#include "stdlib/memory.hpp"
#include "phys/eulertx.hpp"
#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "mathex/point2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/transf3d.hpp"
//#include "phys/boundvol.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "phys/internal/linemtii.hpp"
#include "phys/timespin.hpp"
#include "phys/accetumb.hpp"
#include "phys/asclplan.hpp"

#include <memory>
//Forward refs
class MexPolygon3d;

void PhysDummyFunction1()
{
    //static ctl_vector<PhysBoundingVolume::Entry*> dummy1;
    static ctl_vector<PhysLinearMotionPlanImpl::Entry*> dummy2;
    static PhysMotionPlanPtr dummy3;
    static ctl_vector< const MexPolygon3d* > dummy5;
    static ctl_vector< const MexPolygon2d* > dummy6;
    static ctl_vector< void* > dummy7;
    static std::auto_ptr< MexPolygon2d > dummy8;
    static ctl_vector< bool > dummy9;
	static ctl_vector<PhysRampAcceleration> dummy10;
	static ctl_vector<MATHEX_SCALAR> dummy11;

    static PhysAccelerateTumblePlan::EulerTransformsPtr dummy12;
}

/* End TI.CPP *****************************************************/
