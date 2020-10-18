/*
 * O P N U K E A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include <iostream>

#include "phys/cspace2.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox3d.hpp"

#include "sim/manager.hpp"

#include "world4d/subject.hpp"
#include "world4d/entity.hpp"

#include "phys/motchunk.hpp"

#include "machphys/nuclweap.hpp"

#include "machlog/canattac.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/opnukeat.hpp"
#include "machlog/planet.hpp"
#include "machlog/nuclweap.hpp"
#include "machlog/races.hpp"
#include "machlog/weapon.hpp"

PER_DEFINE_PERSISTENT( MachLogNukeAttackOperation );

/* //////////////////////////////////////////////////////////////// */
MachLogNukeAttackOperation::MachLogNukeAttackOperation( MachLogMissileEmplacement * pActor, const MexPoint3d& targetPosition )
:	MachLogOperation( "NUKE_ATTACK_OPERATION", MachLogOperation::NUKE_ATTACK_OPERATION ),
	pActor_( pActor ),
  	targetPosition_( targetPosition )
{
	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
}

MachLogNukeAttackOperation::MachLogNukeAttackOperation( MachLogMissileEmplacement * pActor, MachActor * pDirectObject )
:	MachLogOperation( "NUKE_ATTACK_OPERATION", MachLogOperation::NUKE_ATTACK_OPERATION ),
	pActor_( pActor ),
  	targetPosition_( pDirectObject->position() )
{
	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
}

MachLogNukeAttackOperation::~MachLogNukeAttackOperation()
{
	// deliberately left blank
}

void MachLogNukeAttackOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogNukeAttackOperation\n";
}

///////////////////////////////////

bool MachLogNukeAttackOperation::doStart()
{
	return true;
}

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogNukeAttackOperation::doUpdate()
{
	PRE( not isFinished() );
	PRE( pActor_ != NULL );
	if( pSubOperation() )
	{
		return 0.5;
	}

	PhysRelativeTime interval = 2.0;

	if( pActor_->inAngleRange( targetPosition_ ) )
		pActor_->attack( targetPosition_ );
	else
	{
		MexRadians turnBy = pActor_->angleToTurnToFace( targetPosition_ );
		subOperation( pActor_, _NEW( MachLogTurnAnimation( pActor_, turnBy ) ) );
	}

   	return interval;
}

void MachLogNukeAttackOperation::doFinish()
{
	//deliberately blank
}

bool MachLogNukeAttackOperation::doIsFinished() const
{
	bool result = not( _CONST_CAST( const MachLogMissileEmplacement&, *pActor_ ).weapons().front()->recharged() );

	return result;
}

bool MachLogNukeAttackOperation::doBeInterrupted()
{
	return true;
}



/////////////////////////////////////////////////// persistence ////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogNukeAttackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.targetPosition_;
}

void perRead( PerIstream& istr, MachLogNukeAttackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.targetPosition_;
}

MachLogNukeAttackOperation::MachLogNukeAttackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}




/* End OPNUKEAT.CPP *************************************************/
