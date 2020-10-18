/*
 * O P A D L O C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/opadloc.hpp"

#include "ctl/pvector.hpp"

#include "machlog/internal/opadloci.hpp"

#include "phys/cspace2.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "machlog/oplocate.hpp"
#include "machlog/move.hpp"
#include "machlog/administ.hpp"
#include "machlog/minesite.hpp"
#include "machlog/planet.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/follow.hpp"
#include "machlog/convyoff.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/squad.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/strategy.hpp"

PER_DEFINE_PERSISTENT( MachLogAdminLocateOperation );

#define CB_MachLogAdminLocateOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogAdministrator*, pActor_ ); \
    	CB_DEPIMPL( bool, complete_ ); \
		CB_DEPIMPL( MexPoint3d, dest_ ); \
		CB_DEPIMPL( MachLogAdminLocateOperation::Path, path_ ); \
		CB_DEPIMPL( MachLogAdminLocateOperation::Path,	externalPath_ ); \
		CB_DEPIMPL( bool, hasExternalPath_ ); \
		CB_DEPIMPL( int, currentElement_ );

/* //////////////////////////////////////////////////////////////// */

MachLogAdminLocateOperation::MachLogAdminLocateOperation( MachLogAdministrator * pActor, const MexPoint3d& dest )
:	MachLogOperation( "ADMIN_LOCATE_OPERATION" , MachLogOperation::ADMIN_LOCATE_OPERATION ),
	pImpl_( _NEW( MachLogAdminLocateOperationImpl( pActor, dest ) ) )
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::CTOR single dest\n" );
	path_.push_back( dest_ );
	//HAL_STREAM("(" << pActor->id() << ") MLLocateOp::CTOR\n" );
}

MachLogAdminLocateOperation::MachLogAdminLocateOperation( MachLogAdministrator * pActor )
:	MachLogOperation( "ADMIN_LOCATE_OPERATION" , MachLogOperation::ADMIN_LOCATE_OPERATION ),
	pImpl_( _NEW( MachLogAdminLocateOperationImpl( pActor ) ) )
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::CTOR just actor\n" );
	//HAL_STREAM("(" << pActor->id() << ") MLLocateOp::CTOR\n" );
}

MachLogAdminLocateOperation::MachLogAdminLocateOperation( MachLogAdministrator * pActor,  const Path& externalPath )
:	MachLogOperation( "ADMIN_LOCATE_OPERATION" , MachLogOperation::ADMIN_LOCATE_OPERATION ),
	pImpl_( _NEW( MachLogAdminLocateOperationImpl( pActor, externalPath ) ) )
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::CTOR external path\n" );
	if( externalPath_.size() == 1 )
	{
		dest_ = externalPath_.front();
		path_.push_back( dest_ );
		hasExternalPath_ = false;
		while( externalPath_.size() > 0 )
			externalPath_.erase( externalPath_.begin() );
	}
	else
	{
		path_ = externalPath_;
		while( externalPath_.size() > 0 )
			externalPath_.erase( externalPath_.begin() );
	}
}

MachLogAdminLocateOperation::~MachLogAdminLocateOperation( )
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::DTOR\n" );
	while( path_.size() > 0 )
		path_.erase( path_.begin() );

	_DELETE( pImpl_ );
}


void MachLogAdminLocateOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminLocateOperation" << std::endl; 
}

///////////////////////////////////

bool MachLogAdminLocateOperation::doStart()
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pActor_ );
}

PhysRelativeTime MachLogAdminLocateOperation::doUpdate( )
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::doUpdate\n" );
	if( pActor_->squadron() == NULL )
	{
		complete_ = true;
		return 2.0;
	}

	PhysRelativeTime interval = 5.0;
    //Check we have a locator
	//HAL_STREAM(" lloking for a locator\n" );
	MachLogGeoLocator *pLoc = NULL;
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::GEO_LOCATOR )
				pLoc = &(*i )->asGeoLocator();

	//HAL_STREAM(" (void*)pLoc " << (void*)pLoc << std::endl );
    //If not, dissolve the squadron if AI
    if( MachLogRaces::instance().controller( pActor_->race() ).type() == MachLogController::AI_CONTROLLER and pLoc == NULL )
    {
		//HAL_STREAM(" AI controller no locator ending op.\n" );
        pActor_->squadron()->releaseAllMachines();
        complete_ = true;
        return 2.0;
    }

    //If not, finish the operation
    if( pLoc == NULL )
    {
		//HAL_STREAM(" player controller no locator ending op.\n" );
        complete_ = true;
        return 2.0;
    }

	int index = 0;
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
		if( (*i)->id() != pActor_->id() and ( (*i)->isIdle() or (*i)->strategy().currentOperationType() == MachLogOperation::FOLLOW_OPERATION ) )
		{
			if( (*i)->objectType() == MachLog::GEO_LOCATOR )
			{
				//HAL_STREAM(" assigning a locator a locate op\n" );
				if( MachLogRaces::instance().controller( pActor_->race() ).type() == MachLogController::AI_CONTROLLER )
					assignLocatorTask( &(*i )->asGeoLocator() );
				else
					(*i)->newOperation( _NEW( MachLogLocateOperation( &(*i)->asGeoLocator() , path_ ) ) );
			}
			else
			{
				//HAL_STREAM(" issuing follow for a " << (*i)->objectType() << std::endl );
				(*i)->newOperation( _NEW( MachLogFollowOperation( (MachLogMachine*)*i, pLoc , MachLogConvoyOffsets::convoyOffset( MachLogConvoyOffsets::LOCATOR_CONVOY, index++ , 15 ) ) ) );
			}
		}

//	if( not pActor_->motionSeq().hasDestination() )
//	{
		pActor_->motionSeq().stop();
		pActor_->motionSeq().update( 0.01 );
		//HAL_STREAM(" issuing follow op for administrator\n" );
		if( not pActor_->motionSeq().isFollowing() and not pSubOperation() )
			subOperation( pActor_, _NEW( MachLogFollowOperation( pActor_, pLoc, MachLogConvoyOffsets::convoyOffset( MachLogConvoyOffsets::LOCATOR_CONVOY, 0, 15 ) ) ) );
		interval = 15.0;
//	}
	if( pActor_->motionSeq().isFollowing() )
		interval = 15.0;

	return interval;
}

void MachLogAdminLocateOperation::doFinish()
{
//	pActor_->isLocating( false );
}
	
bool MachLogAdminLocateOperation::doIsFinished() const
{
	CB_MachLogAdminLocateOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLAdminLocateOp::diIsFinished " << complete_ << "\n" );
	return complete_;
//	return dest_ == pActor_->position();
}

bool MachLogAdminLocateOperation::doBeInterrupted()
{
	return true;
}

void MachLogAdminLocateOperation::assignLocatorTask( MachLogGeoLocator* obj )
{
	bool found  = false;
	MachPhys::Race race = obj->race();
	// has the Locator discovered a site?

	// Locator moves on to the next site...

	found  = false;
	////HAL_STREAM("(" << pActor_->id() << ") MLAdminLocOp assign locator task\n" );
	bool foundAGoodOne = false;
	MATHEX_SCALAR sqrRange = 1000000000;
	MATHEX_SCALAR sqrRangeClosest = 1000000000;
	MachLogPlanet::Sites::const_iterator j =  MachLogPlanet::instance().sites().begin();
	MachLogPlanet::Sites::const_iterator i =  MachLogPlanet::instance().sites().begin();
	for( ; j != MachLogPlanet::instance().sites().end(); ++j )
	{
		found = not (*j)->hasBeenDiscovered();
		sqrRange = obj->position().sqrEuclidianDistance( (*j)->position() );
		////HAL_STREAM(" checking mineral sites sqrRange " << sqrRange << " amount of ore " << (*j)->amountOfOre() << std::endl );
		if( found and sqrRange < sqrRangeClosest and (*j)->amountOfOre() > 0 )
		{
			////HAL_STREAM("  saving this one for later...\n" );
			sqrRangeClosest = sqrRange;
			i = j;
			foundAGoodOne = true;
		}
	}
	if( foundAGoodOne )
	{
		////HAL_STREAM(" found a good one...assigning locate op pos " << (*i)->position() << std::endl );
		MexPoint3d dest( (*i)->position() );
		PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
		PhysConfigSpace2d::DomainId domain;
		dest.x( dest.x() + MachPhysRandom::randomDouble( -30, 30 ) );
		dest.y( dest.y() + MachPhysRandom::randomDouble( -30, 30 ) );
		while( not obj->motionSeq().targetPositionContainedInSpace( dest ) )
		{
			dest = (*i)->position();
			dest.x( dest.x() + MachPhysRandom::randomDouble( -30, 30 ) );
			dest.y( dest.y() + MachPhysRandom::randomDouble( -30, 30 ) );
		}
		obj->newOperation( _NEW( MachLogLocateOperation( obj , dest ) ) );
	}
}

void perWrite( PerOstream& ostr, const MachLogAdminLocateOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;	
}

void perRead( PerIstream& istr, MachLogAdminLocateOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;	
}

MachLogAdminLocateOperation::MachLogAdminLocateOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
