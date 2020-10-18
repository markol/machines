/*
 * T I 4 . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

//  These next two lines are a hack to get this file to compile - Bob
#define _MACH_ACTOR_HPP
class MachActor;

#include "ctl/fixedvec.hpp"
#include "ctl/stack.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "ctl/utility.hpp"
#include "utility/bidpmap.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "phys/phys.hpp"
#include "world4d/entity.hpp"
#include "machlog/internal/grpmovei.hpp"


class W4dEntity;
class PhysConfigSpace2d;
class MachLogEntrance;

void ti4()
{
    static ctl_fixed_vector<unsigned> dummyX;

    static ctl_stack< ctl_vector< pair< W4dEntity*, PhysConfigSpace2d* > > >   dummyY;
    
    static  ctl_vector< MachLogEntrance* > dummyZ;
    static UtlBoundedIdPMap< MachActor > dummyA( 12 );
    static ctl_vector< ctl_pvector< MachActor > > dummy;

    static ctl_vector< MachLogGroupSimpleMoveImplementation::MachineData >   dummy0;
}

/* End TI4.CPP *******************************************************/

