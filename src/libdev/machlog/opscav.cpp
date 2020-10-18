/*
 * O P S C A V . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opscav.hpp"
#include "machlog/internal/opscavi.hpp"

#include "ctl/algorith.hpp"
#include "ctl/pvector.hpp"

#include "sim/manager.hpp"

#include "world4d/soundman.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/snddata.hpp"

#include "machlog/races.hpp"
#include "machlog/debris.hpp"
#include "machlog/move.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogScavengeOperation );

#define CB_MachLogScavengeOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogResourceCarrier*, pActor_ ); \
    	CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( MachLogScavengeOperation::Suppliers, suppliers_ ); \
		CB_DEPIMPL( size_t, currentElement_ ); \
		CB_DEPIMPL( MexPoint3d, dest_ );

/* //////////////////////////////////////////////////////////////// */

MachLogScavengeOperation::~MachLogScavengeOperation()
{
	TEST_INVARIANT;

	CB_MachLogScavengeOperation_DEPIMPL();
	
	while( suppliers_.size() > 0 )
	{
		suppliers_.front()->detach( this );		
		suppliers_.erase( suppliers_.begin() );
	}   

	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////// */

MachLogScavengeOperation::MachLogScavengeOperation( MachLogResourceCarrier * pActor , MachLogDebris* targetDebris )
:	MachLogOperation( "SCAVENGE_OPERATION", MachLogOperation::SCAVENGE_OPERATION ),
	pImpl_( _NEW( MachLogScavengeOperationImpl( pActor ) ) )	
{	
	CB_MachLogScavengeOperation_DEPIMPL();

	PRE( targetDebris != NULL );
	PRE( pActor_->isScavenger() );

	//register with target supplier
	suppliers_.push_back( targetDebris );
	targetDebris->attach( this );
	
	TEST_INVARIANT;
 }

MachLogScavengeOperation::MachLogScavengeOperation( MachLogResourceCarrier * pActor , const Suppliers& listOfSuppliers )
:	MachLogOperation( "SCAVENGE_OPERATION", MachLogOperation::SCAVENGE_OPERATION ),
	pImpl_( _NEW( MachLogScavengeOperationImpl( pActor ) ) )		
{
	CB_MachLogScavengeOperation_DEPIMPL();

	PRE( listOfSuppliers.size() > 0 );
	PRE( pActor_->isScavenger() );	

	//copy list of suppliers into internal list and register with each one
	for( Suppliers::const_iterator i = listOfSuppliers.begin(); i!= listOfSuppliers.end(); ++i )
	{
		suppliers_.push_back( *i );
		(*i)->attach( this );
	}
	
	TEST_INVARIANT;
}

void MachLogScavengeOperation::CLASS_INVARIANT
{
	CB_MachLogScavengeOperation_DEPIMPL();

    INVARIANT( this != NULL );
	INVARIANT( pActor_->amountCarried() <= pActor_->data().capacity() );
}


//virtual
bool MachLogScavengeOperation::beNotified( W4dSubject* pSubject,
                 W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	//HAL_STREAM("(" << pActor_->id() << ") MLScavengeOp::beNotified\n" );
	//HAL_STREAM(" suppliers_isze " << suppliers_.size() << std::endl );
	CB_MachLogScavengeOperation_DEPIMPL();

	bool stayAttached = true;

	switch( event )
	{
		case W4dSubject::DELETED:
		{
			Suppliers::iterator i = find( suppliers_.begin(), suppliers_.end(), pSubject );
			
			if( i != suppliers_.end() )
			{			
				suppliers_.erase( i );
				stayAttached = false;
			}
		}		
		break;
	
	default:
		;
	}

	return stayAttached;
}


void MachLogScavengeOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogScavengeOperation_DEPIMPL();

	o << "MachLogScavengeOperation ";
	o << " Suppliers_.size() " << suppliers_.size() << std::endl;
	o << " current element " << currentElement_ << std::endl;
}

bool MachLogScavengeOperation::doStart()
{
	CB_MachLogScavengeOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pActor_ );
}

///////////////////////////////////

void MachLogScavengeOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogScavengeOperation::doIsFinished() const
{
	CB_MachLogScavengeOperation_DEPIMPL();

	return ( finished_ and not pActor_->motionSeq().hasDestination() );
}

PhysRelativeTime
MachLogScavengeOperation::doUpdate( )
{
	CB_MachLogScavengeOperation_DEPIMPL();

//	if( pSubOperation() )
//		return 2.0;
	
	// am I still moving?
//	if( pActor_->motionSeq().hasDestination() )
//		return 2.0;	

	if( finished_ )
	{
		if( pSubOperation() )
			pSubOperation()->doBeInterrupted();
		return 0.0;
	}

	// do I still have suppliers left?	
	if( currentElement_ >= suppliers_.size() )
	{
		//I have run out of suppliers
		finished_ = true;
		if( pSubOperation() )
			pSubOperation()->doBeInterrupted();
		return 0.0;
	}
	
	MexPoint2d actorPos = pActor_->position();	
	MexPoint2d destPos = dest_;	

	// P I C K I N G   U P    D E B R I S
	
	MachLogDebris& debris =  *suppliers_[ currentElement_ ];
	
	dest_ = debris.position();
	destPos = dest_;

	PhysRelativeTime interval = 0.0;

	//are we too far from the debris to pick it up?
	if( actorPos.sqrEuclidianDistance( destPos ) > 4 )
	{	
		if( pSubOperation() )
			interval = 2.0;
		else
		{
			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest_ ) ) );
			TEST_INVARIANT;
			interval = 2.0;
		}		
	}
	else
	{
		//well, we're at the debris....let's pick this baby up.

		interval = pActor_->doLoading();					
			  
		MachPhys::BuildingMaterialUnits amountToAdd = debris.quantity();

		// now we need to autosmelt it. Note that anything that clips over the race limit is simply wasted.
		MachLogRaces::instance().smartAddBMUs( pActor_->race(), amountToAdd );
			
		//remove the debris object from the game
		debris.hasBeenPickedUp();
		
		//don't need to advance element count as debris element is erased via deletion notification	
	}
	
	
	
	return interval;	
}

//virtual
bool MachLogScavengeOperation::doBeInterrupted()
{
	return true;
}



/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogScavengeOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;	
}

void perRead( PerIstream& istr, MachLogScavengeOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

	
	for( MachLogScavengeOperation::Suppliers::iterator i = op.suppliers().begin(); i!= op.suppliers().end(); ++i )
		(*i)->attach( &op );
}

MachLogScavengeOperation::Suppliers& MachLogScavengeOperation::suppliers()
{
	CB_MachLogScavengeOperation_DEPIMPL();

	return suppliers_;
}	

MachLogScavengeOperation::MachLogScavengeOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */

/* End OPSCAV.CPP *************************************************/
