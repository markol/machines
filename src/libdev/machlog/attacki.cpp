/*
 * A T T A C K I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/attacki.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/attack.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"


PER_DEFINE_PERSISTENT( MachLogAttackOperationImpl );

MachLogAttackOperationImpl::MachLogAttackOperationImpl( MachLogMachine * pActor, MachActor * pDirectObject, MachLogAttackOperation::RaceChangeTerminate rct )
:	pActor_( pActor ),
  	pDirectObject_( pDirectObject ),
  	targetBehindCover_( false ),
	pAgg_( NULL ),
	pAdm_( NULL ),
	lastAction_( MachLogAttackOperation::WAIT_FOR_INTERVAL ),
	raceChangeTerminate_( rct ),
	currentlyAttached_( false ),
	lastTargetPosition_( -5000.0, -5000.0 ),
	hasGorillaPunch_( false ),
	commandId_( MachLogRaces::instance().AINoCommandID() )
{
	nextHardCoverCheckTime_ = SimManager::instance().currentTime() + MachLogAttackOperation::behindCoverMoveGracePeriod();
    TEST_INVARIANT;
}

MachLogAttackOperationImpl::MachLogAttackOperationImpl( MachLogMachine * pActor, MachActor * pDirectObject, size_t commandId, MachLogAttackOperation::RaceChangeTerminate rct )
:	pActor_( pActor ),
  	pDirectObject_( pDirectObject ),
  	targetBehindCover_( false ),
	pAgg_( NULL ),
	pAdm_( NULL ),
	lastAction_( MachLogAttackOperation::WAIT_FOR_INTERVAL ),
	raceChangeTerminate_( rct ),
	currentlyAttached_( false ),
	lastTargetPosition_( -5000.0, -5000.0 ),
	hasGorillaPunch_( false ),
	commandId_( commandId )
{
	nextHardCoverCheckTime_ = SimManager::instance().currentTime() + MachLogAttackOperation::behindCoverMoveGracePeriod();
    TEST_INVARIANT;
}


MachLogAttackOperationImpl::~MachLogAttackOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogAttackOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAttackOperationImpl& t )
{

    o << "MachLogAttackOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAttackOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAttackOperationImpl& attackOpImpl )
{
	ostr << attackOpImpl.currentlyAttached_;
	ostr << attackOpImpl.pActor_;
	ostr << attackOpImpl.pAgg_;
	ostr << attackOpImpl.pAdm_;
	ostr << attackOpImpl.pDirectObject_;
	ostr << attackOpImpl.lastTargetPosition_;
	ostr << attackOpImpl.targetBehindCover_;
	ostr << attackOpImpl.raceChangeTerminate_;
	ostr << attackOpImpl.commandId_;
	ostr << attackOpImpl.hasGorillaPunch_;	
}

void perRead( PerIstream& istr, MachLogAttackOperationImpl& attackOpImpl )
{
	istr >> attackOpImpl.currentlyAttached_;
	istr >> attackOpImpl.pActor_;
	istr >> attackOpImpl.pAgg_;
	istr >> attackOpImpl.pAdm_;
	istr >> attackOpImpl.pDirectObject_;
	istr >> attackOpImpl.lastTargetPosition_;
	istr >> attackOpImpl.targetBehindCover_;
	istr >> attackOpImpl.raceChangeTerminate_;
	istr >> attackOpImpl.commandId_;	
	istr >> attackOpImpl.hasGorillaPunch_;	

	attackOpImpl.nextHardCoverCheckTime_ = SimManager::instance().currentTime() + MachLogAttackOperation::behindCoverMoveGracePeriod();
    attackOpImpl.lastAction_ = MachLogAttackOperation::WAIT_FOR_INTERVAL;
}

MachLogAttackOperationImpl::MachLogAttackOperationImpl( PerConstructor )
{
}


/* End ATTACKI.CPP ***************************************************/
