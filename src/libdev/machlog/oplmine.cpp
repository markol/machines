/*
 * O P L M I N E . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/oplmine.hpp"

#include "machlog/internal/oplminei.hpp"

#include "mathex/line2d.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/slocdata.hpp"

#include "machlog/minesite.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"
#include "machlog/spy.hpp"

PER_DEFINE_PERSISTENT( MachLogDropLandMineOperation );

#define CB_MachLogDropLandMineOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogSpyLocator*, pActor_ ); \
    	CB_DEPIMPL( MexPoint3d, dest_ ); \
		CB_DEPIMPL( MachLogDropLandMineOperation::Path, path_ ); \
		CB_DEPIMPL( int, failedAttempts_ ); \
		CB_DEPIMPL( bool, moved_ );


/* //////////////////////////////////////////////////////////////// */

MachLogDropLandMineOperation::MachLogDropLandMineOperation( MachLogSpyLocator * pActor, const MexPoint3d& dest )
:	MachLogOperation( "DROPLANDMINE_OPERATION", MachLogOperation::DROPLANDMINE_OPERATION ),
	pImpl_( _NEW( MachLogDropLandMineOperationImpl( pActor, dest ) ) )	
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	//path_.push_back( pActor_->position() );
	path_.push_back( dest );
	//HAL_STREAM("(" << pActor->id() << ") MLDropLandMineOp::CTOR\n" );
}

MachLogDropLandMineOperation::MachLogDropLandMineOperation( MachLogSpyLocator * pActor,  const Path& externalPath )
:	MachLogOperation( "DROPLANDMINE_OPERATION", MachLogOperation::DROPLANDMINE_OPERATION ),
	pImpl_( _NEW( MachLogDropLandMineOperationImpl( pActor, externalPath ) ) )	
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	//path_.push_front( pActor_->position() );
}

MachLogDropLandMineOperation::~MachLogDropLandMineOperation( )
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	while( path_.size() > 0 )
		path_.erase( path_.begin() );

	_DELETE( pImpl_ );
}

void MachLogDropLandMineOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	o << "MachLogDropLandMineOperation Dest " << dest_ << " position " << pActor_->position() << std::endl; 
	o << " Actor mines " << pActor_->nMines() << std::endl;
	o << " total path size " << path_.size() << std::endl;

	if( path_.size() )
	{
		/*
		const Path::const_iterator& k = path_.end();
		for( Path::const_iterator& i = path_.begin(); i != k ; ++i )
			o << (*i) << " ";
		o << std::endl;
		*/

		for( Path::iterator i = path_.begin(); i != path_.end() ; ++i )
			o << (*i) << " ";
		o << std::endl;
	}
}

///////////////////////////////////

bool MachLogDropLandMineOperation::doStart()
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	if( not checkNeedAndDoLeaveOperation( pActor_ ) )
	{
		dest_ = path_.front();
		return true;
	}

	return false;
}


void MachLogDropLandMineOperation::doFinish()
{
}
	
bool MachLogDropLandMineOperation::doIsFinished() const
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	return path_.empty() or pActor_->nMines() == 0 or failedAttempts_ == 10;
}

//virtual
bool MachLogDropLandMineOperation::doBeInterrupted()
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

//virtual
PhysRelativeTime MachLogDropLandMineOperation::doUpdate( )
{
	CB_MachLogDropLandMineOperation_DEPIMPL();

	if( pSubOperation() )
	{
		// check to see we can still reach this current location without any hassles.
		if( not MachLogSpacialManipulation::pointIsFree( path_.front(), pActor_->motionSeq().highClearance() * 1.5 ) )	
		{
			// if this counter reaches 5, it is pretty clear that this destination location isn't currently viable
			++failedAttempts_;

			if( failedAttempts_ > 5 )
			{
				// hmmm, that point doesn't seem like it's viable. Kick the location off the front of the path 
				// to the back and try the next point (it might be the same one).
				path_.push_back( path_.front() );
				path_.erase( path_.begin() );

				// cancel the current move subop
				pSubOperation()->doBeInterrupted();

				failedAttempts_ = 0;
			}
		}
		else 
			failedAttempts_ = 0;

		return 1.0;
	}
		
    
	PhysRelativeTime interval = 1.0;

    const MexPoint2d& actorPos = pActor_->position();
    if( actorPos == path_.front() )
	{
		// ah, good! We're at the point we want to be at to drop a mine.

		MachPhys::Race race = pActor_->race();

		pActor_->placeMine();

		// reset failed attempts counter
		failedAttempts_ = 0;

		// boot this mine's location off the front of the queue
		path_.erase( path_.begin() );

		// now, any more places to go to?
		if( pActor_->nMines() > 0 and not path_.empty() )
		{
			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, MexPoint3d( path_.front() ) ) ) );
			interval = 2.0;	
		}
	}
	else
	{
		// Hmmm.....we're not actually where we want to be. Let's try and move towards it.

		if( MachLogSpacialManipulation::pointIsFree( path_.front(), pActor_->motionSeq().highClearance() * 1.5 ) )
		{
			// no apparent problems reaching that point....let's go.

			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, MexPoint3d( path_.front() ) ) ) );
			interval = 2.0;	
		}
		else
		{
			// hmmm, that point isn't viable. Kick the location off the front of the path 
			// to the back and try the next point (it might be the same one).
			path_.push_back( path_.front() );
			path_.erase( path_.begin() );

			// if this counter reaches 10, the operation will spontaneously abort as it is pretty clear that
			// none of the target locations will be clear to drop mines at in the foreseeable future.
			++failedAttempts_;
		}		
	}
	return interval;
}

void perWrite( PerOstream& ostr, const MachLogDropLandMineOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;	
}

void perRead( PerIstream& istr, MachLogDropLandMineOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;	
}

MachLogDropLandMineOperation::MachLogDropLandMineOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
