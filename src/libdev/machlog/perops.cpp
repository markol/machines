/*
 * P E R W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"
#include "machlog/perops.hpp"

#include "machlog/attack.hpp"
#include "machlog/animatio.hpp"
#include "machlog/animres.hpp"
#include "machlog/follow.hpp"
#include "machlog/inout.hpp"
#include "machlog/move.hpp"
#include "machlog/patrol.hpp"
#include "machlog/seekdest.hpp"
#include "machlog/opadatta.hpp"
#include "machlog/opadcapt.hpp"
#include "machlog/opadcon.hpp"
#include "machlog/opaddeco.hpp"
#include "machlog/opadheal.hpp"
#include "machlog/opadloc.hpp"
#include "machlog/opadmove.hpp"
#include "machlog/opadrepa.hpp"
#include "machlog/opadsupc.hpp"
#include "machlog/opautscv.hpp"
#include "machlog/opcaptur.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/opcounte.hpp"
#include "machlog/opdecons.hpp"
#include "machlog/opdapc.hpp"
#include "machlog/opeapc.hpp"
#include "machlog/opevade.hpp"
#include "machlog/opgolab.hpp"
#include "machlog/opheal.hpp"
#include "machlog/oplmine.hpp"
#include "machlog/oplocate.hpp"
#include "machlog/opmeatta.hpp"
#include "machlog/oppickup.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/opputdwn.hpp"
#include "machlog/oprecycl.hpp"
#include "machlog/oprefill.hpp"
#include "machlog/oprepair.hpp"
#include "machlog/opscav.hpp"
#include "machlog/opstandg.hpp"
#include "machlog/opsupcon.hpp"
#include "machlog/opsupcon.hpp"
#include "machlog/optransp.hpp"
#include "machlog/optreach.hpp"
#include "machlog/optskatt.hpp"
#include "machlog/optskcon.hpp"
#include "machlog/optskdrm.hpp"
#include "machlog/optskloc.hpp"
#include "machlog/optskpat.hpp"


MachLogOperationPersistence::MachLogOperationPersistence()
{

    TEST_INVARIANT;
}

MachLogOperationPersistence::~MachLogOperationPersistence()
{
    TEST_INVARIANT;

}

void MachLogOperationPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogOperationPersistence& t )
{

    o << "MachLogOperationPersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogOperationPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

//static 
void MachLogOperationPersistence::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogMoveToOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogMoveAndTurnOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAttackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogFollowOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogSeekAndDestroyOperation );
//	PER_REGISTER_DERIVED_CLASS( MachLogAnimation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminAttackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminConstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminHealOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminLocateOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogCaptureOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogConstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogDeployAPCOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogEnterAPCOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogGotoLabOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogHealOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogDropLandMineOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogLocateOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogMissileEmplacementAttackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogPickUpOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogPodAttackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogPodAttackAnimation );
	PER_REGISTER_DERIVED_CLASS( MachLogPutDownOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogRefillLandMinesOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogRepairOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTransportOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTreacheryOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskAttackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskConstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskDropLandMineOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskLocateOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskPatrolOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogPatrolOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogEnterBuildingOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogLeaveBuildingOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogMoveAndTurnOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogDeconstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogRecycleOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminDeconstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminCaptureOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogSuperConstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAdminSuperConstructOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogStandGroundOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogTurnAnimation );
	//PER_REGISTER_DERIVED_CLASS( MachLogConstructAnimation );
	PER_REGISTER_DERIVED_CLASS( MachLogBeDestroyedAnimation );
	PER_REGISTER_DERIVED_CLASS( MachLogResearchAnimation );
	PER_REGISTER_DERIVED_CLASS( MachLogCounterattackOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogScavengeOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogAutoScavengeOperation );
	PER_REGISTER_DERIVED_CLASS( MachLogEvadeOperation );
}

// Forced recompile 12/1/99 CPS

/* End PERWEAP.CPP **************************************************/
