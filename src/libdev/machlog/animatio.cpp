/*
 * A N I M A T I O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
 
#include "sim/manager.hpp"

//#include "machlog/actor.hpp"
#include "machlog/animatio.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/construc.hpp"
#include "machlog/constron.hpp"
#include "machlog/races.hpp"
//#include "machlog/mobile.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT_ABSTRACT( MachLogAnimation )

MachLogAnimation::MachLogAnimation()
: MachLogOperation(" ANIMATION", MachLogOperation::ANIMATION )
{
	/* Intentionally Empty	*/
}

MachLogAnimation::~MachLogAnimation() 
{
	/* Intentionally Empty	*/
}

///////////////////////////////////

bool MachLogAnimation::doStart()
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	endTime_ = doStartAnimation() + timeNow;
	return true;
}

PhysRelativeTime MachLogAnimation::doUpdate()
{
	return 0.0;
}

void MachLogAnimation::doFinish()
{
	/* Intentionally Empty	*/
}

///////////////////////////////////

bool MachLogAnimation::doIsFinished() const
{
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	return timeNow > endTime_;
}

bool MachLogAnimation::doBeInterrupted()
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	bool result = ( endTime_ - timeNow ) <= 0.0;
	return result;
}

void perWrite( PerOstream& ostr, const MachLogAnimation& anim )
{
	const MachLogOperation& base1 = anim;

	ostr << base1;
	ostr << anim.endTime_;
}

void perRead( PerIstream& istr, MachLogAnimation& anim )
{
	MachLogOperation& base1 = anim;

	istr >> base1;
	istr >> anim.endTime_;
}

MachLogAnimation::MachLogAnimation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogTurnAnimation )

MachLogTurnAnimation::MachLogTurnAnimation( MachLogCanTurn * pActor, MexRadians radians )
: pActor_( pActor ),
  turnBy_( radians )
{
	/* Intentionally Empty	*/
}

void MachLogTurnAnimation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTurnAnimation " << turnBy_;
}

MachLogTurnAnimation::~MachLogTurnAnimation()
{
	/* Intentionally Empty	*/
}

PhysRelativeTime MachLogTurnAnimation::doStartAnimation()
{
	return pActor_->turn( turnBy_.asScalar() );
}

void perWrite( PerOstream& ostr, const MachLogTurnAnimation& anim )
{
	const MachLogAnimation& base1 = anim;

	ostr << base1;
	ostr << anim.pActor_;
	ostr << anim.turnBy_;
}

void perRead( PerIstream& istr, MachLogTurnAnimation& anim )
{
	MachLogAnimation& base1 = anim;

	istr >> base1;
	istr >> anim.pActor_;
	istr >> anim.turnBy_;
}

MachLogTurnAnimation::MachLogTurnAnimation( PerConstructor con )
:	MachLogAnimation( con )
{
}

/* //////////////////////////////////////////////////////////////// */


/*
PER_DEFINE_PERSISTENT( MachLogConstructAnimation )

MachLogConstructAnimation::MachLogConstructAnimation( MachLogConstructor * pActor, 
												MachLogConstruction * pConstruction,
												MachPhys::BuildingMaterialUnits units )
: pActor_( pActor ),
  pConstruction_( pConstruction ),
  units_( units ),
  lastUpdateTime_( SimManager::instance().currentTime() )
{
	// Intentionally empty
}
	
PhysRelativeTime MachLogConstructAnimation::doUpdate()
{
//In operation//	MachPhys::BuildingMaterialUnits amountToAdd = ( units_ * ( SimManager::instance().currentTime() - lastUpdateTime_ ) / 60 );
//In operation//	MachPhys::BuildingMaterialUnits& nBMU = MachLogRaces::instance().nBuildingMaterialUnits( pActor_->race() );
//In operation//
//In operation//	if( amountToAdd > nBMU )
//In operation//		amountToAdd = nBMU;
//In operation//
//In operation//	if( amountToAdd + pConstruction_->constructedUnits() > pConstruction_->objectData().cost() )
//In operation//		amountToAdd = pConstruction_->objectData().cost() - pConstruction_->constructedUnits();
//In operation//
//In operation//	pConstruction_->advanceConstructionState( amountToAdd );
//In operation//	nBMU -= amountToAdd;
	MachLogAnimation::doUpdate( );
	//only update time if added something on.
//inoperation//	if( not ( amountToAdd == 0 and nBMU != 0 ) )
//inoperation//		lastUpdateTime_ = SimManager::instance().currentTime();

	return max( 1, 60 / units_ );
}

void MachLogConstructAnimation::doOutputOperator( ostream& o ) const
{
	o << "MachLogConstructAnimation ";
}

MachLogConstructAnimation::~MachLogConstructAnimation()
{
	// Intentionally empty	
}

PhysRelativeTime MachLogConstructAnimation::doStartAnimation()
{
	PhysRelativeTime constructTime = pActor_->construct();
//	if( constructTime == 0 )
//		return 1.0;
//	else
		return constructTime;
}

void perWrite( PerOstream& ostr, const MachLogConstructAnimation& anim )
{
	const MachLogAnimation& base1 = anim;

	ostr << base1;
	ostr << anim.pActor_;
	ostr << anim.pConstruction_;
	ostr << anim.units_;
	ostr << anim.lastUpdateTime_;
}

void perRead( PerIstream& istr, MachLogConstructAnimation& anim )
{
	MachLogAnimation& base1 = anim;

	istr >> base1;
	istr >> anim.pActor_;
	istr >> anim.pConstruction_;
	istr >> anim.units_;
	istr >> anim.lastUpdateTime_;
}

MachLogConstructAnimation::MachLogConstructAnimation( PerConstructor con )
:	MachLogAnimation( con )
{
}
*/


/* //////////////////////////////////////////////////////////////// */

PER_DEFINE_PERSISTENT( MachLogBeDestroyedAnimation )

MachLogBeDestroyedAnimation::MachLogBeDestroyedAnimation( MachActor * pActor )
: pActor_( pActor )
{
	/* Intentionally Empty	*/
}

void MachLogBeDestroyedAnimation::doOutputOperator( ostream& o ) const
{
	o << "MachLogBeDestroyedAnimation ";
}

MachLogBeDestroyedAnimation::~MachLogBeDestroyedAnimation()
{
	/* Intentionally Empty	*/
}

///////////////////////////////

PhysRelativeTime MachLogBeDestroyedAnimation::doStartAnimation()
{
	return pActor_->startExplodingAnimation();
}

void MachLogBeDestroyedAnimation::doFinish()
{
	pActor_->endExplodingAnimation();
}

void perWrite( PerOstream& ostr, const MachLogBeDestroyedAnimation& anim )
{
	const MachLogAnimation& base1 = anim;

	ostr << base1;
	ostr << anim.pActor_;
}

void perRead( PerIstream& istr, MachLogBeDestroyedAnimation& anim )
{
	MachLogAnimation& base1 = anim;

	istr >> base1;
	istr >> anim.pActor_;
}

MachLogBeDestroyedAnimation::MachLogBeDestroyedAnimation( PerConstructor con )
:	MachLogAnimation( con )
{
}


/* //////////////////////////////////////////////////////////////// */

/* End ANIMATIO.CPP *************************************************/