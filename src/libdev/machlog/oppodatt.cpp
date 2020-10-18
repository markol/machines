/*
 * O P P O D A T T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>

#include "ctl/pvector.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox3d.hpp"

#include "sim/manager.hpp"

#include "world4d/subject.hpp"
#include "world4d/entity.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/pod.hpp"

#include "machlog/canattac.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/planet.hpp"
#include "machlog/pod.hpp"
#include "machlog/races.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"
#include "machlog/weapon.hpp"

PER_DEFINE_PERSISTENT( MachLogPodAttackOperation );
PER_DEFINE_PERSISTENT( MachLogPodAttackAnimation );

/* //////////////////////////////////////////////////////////////// */
MachLogPodAttackOperation::MachLogPodAttackOperation( MachLogPod * pActor, const MexPoint3d& targetPosition )
:	MachLogOperation( "POD_ATTACK_OPERATION", MachLogOperation::POD_ATTACK_OPERATION ),
	pActor_( pActor ),
  	targetPosition_( targetPosition )
{
	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
	dealWithVoiceMails();
}

MachLogPodAttackOperation::MachLogPodAttackOperation( MachLogPod * pActor, MachActor * pDirectObject )
:	MachLogOperation( "POD_ATTACK_OPERATION", MachLogOperation::POD_ATTACK_OPERATION ),
	pActor_( pActor ),
  	targetPosition_( pDirectObject->position() )
{
	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
	dealWithVoiceMails();
}

MachLogPodAttackOperation::~MachLogPodAttackOperation()
{
	// deliberately left blank
}

void MachLogPodAttackOperation::dealWithVoiceMails()
{
	// now for the voicemail alerts - on this node if this is an AI race
	// send voicemail if it wasn't me initiating this launch and I have the capability to detect it

	if( MachLogRaces::instance().pcController().race() != pActor_->race() )
	{
 		MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_ION_CANNON, MachLogRaces::instance().pcController().race() );
	}


	// ........and whizz the warning round the network if it's a network game.
	if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendWeaponInformationMessage( MachLogMessageBroker::ION_CANNON_FIRED, pActor_->race() );

}

void MachLogPodAttackOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogPodAttackOperation\n";
}

///////////////////////////////////

bool MachLogPodAttackOperation::doStart()
{
	return true;
}

MexRadians
angleToTurnToFace( const MachLogPod& actor, const MexPoint3d& pos );

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogPodAttackOperation::doUpdate()
{
	PRE( not isFinished() );
	PRE( pActor_ != NULL );
	if( pSubOperation() )
	{
		return 0.5;
	}

	PhysRelativeTime interval = 0.1;

	/*
	//The owning aggressor is busy shooting something else right now.
	if( pActor_->hasCurrentTarget() and &pActor_->currentTarget() != &directObject() )
	{
		return 0.5;
	}
	*/

	subOperation( pActor_, _NEW( MachLogPodAttackAnimation( pActor_, targetPosition_ ) ) );


   	return interval;
}

void MachLogPodAttackOperation::doFinish()
{

}

bool MachLogPodAttackOperation::doIsFinished() const
{
	bool result = not _CONST_CAST( const MachLogPod&, *pActor_ ).weapons().front()->recharged();

	return result;
}

bool MachLogPodAttackOperation::doBeInterrupted()
{
	return true;
}

/* //////////////////////////////////////////////////////////////// */

MachLogPodAttackAnimation::MachLogPodAttackAnimation( MachLogPod * pActor, const MexPoint3d& targetPosition )
: pActor_( pActor ),
  targetPosition_( targetPosition )
{
   // deliberately left blank
}

void MachLogPodAttackAnimation::doOutputOperator( ostream& o ) const
{
	o << "MachLogPodAttackAnimation ";
}

MachLogPodAttackAnimation::~MachLogPodAttackAnimation()
{
	// deliberately left blank
}

///////////////////////////////

PhysRelativeTime MachLogPodAttackAnimation::doStartAnimation()
{
	return pActor_->attack( targetPosition_ );
}

MexRadians
angleToTurnToFace( const MachLogPod& actor, const MexPoint3d& pos )
{
	MexPoint2d actorPos( actor.position() );

	MexVec2 bearingToTarget( pos.x() - actorPos.x(), pos.y() - actorPos.y() );

	MexVec3 xBasis3;
	actor.pPhysPod()->globalTransform().xBasis( &xBasis3 );

	MexVec2 xBasis2( xBasis3 );
	return xBasis2.angleBetween( bearingToTarget );
}


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogPodAttackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.targetPosition_;
    ostr << op.lastTargetPosition_;
}

void perRead( PerIstream& istr, MachLogPodAttackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.targetPosition_;
    istr >> op.lastTargetPosition_;
}

MachLogPodAttackOperation::MachLogPodAttackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

void perWrite( PerOstream& ostr, const MachLogPodAttackAnimation& op )
{
	const MachLogAnimation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.targetPosition_;
}

void perRead( PerIstream& istr, MachLogPodAttackAnimation& op )
{
	MachLogAnimation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.targetPosition_;
}

MachLogPodAttackAnimation::MachLogPodAttackAnimation( PerConstructor con )
:	MachLogAnimation( con )
{
}


/* End OPPODATT.CPP *************************************************/
