/*
 * T I S I M . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */


#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/deque.hpp"
#undef _INSTANTIATE_TEMPLATE_CLASSES

#include "phys/rampacce.hpp"

#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"

#include "mathex/transf3d.hpp"
#include "phys/motplan.hpp"
#include "render/mesh.hpp"
#include "render/matvec.hpp"
#include "render/uvanim.hpp"
#include "world4d/meshplan.hpp"
#include "world4d/matplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/compplan.hpp"
#include "world4d/entity.hpp"

#include "sim/disevent.hpp"
#include "sim/evdiary.hpp"
#include "sim/process.hpp"
#include "sim/sim.hpp"
#include "internal/manageri.hpp"

void SimDummyFunction1()
{
    static SimDiscreteEventPtr eventPtr;
	static SimEventDiary::SimEventQueue eventQueue;
    static SimManagerImpl::SimProcesses processes;
    static SimActors actors;
    static CtlConstCountedPtr< W4dEntity > dummy7;
}

/* End TISIM.CPP *****************************************************/
