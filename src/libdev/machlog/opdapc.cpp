/*
 * O P D A P C . C P P
 * (c) Charybdis Limited, 1997/98. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opdapc.hpp"

#include "machlog/internal/opdapci.hpp"

#include "ctl/pvector.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"

#include "machlog/animatio.hpp"
#include "machlog/apc.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"
#include "machlog/strategy.hpp"



PER_DEFINE_PERSISTENT( MachLogDeployAPCOperation );

#define CB_MachLogDeployAPCOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogAPC*, pAPC_ ); \
    	CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( MachLogDeployAPCOperation::Status, status_ ); \

MachLogDeployAPCOperation::~MachLogDeployAPCOperation()
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	pAPC_->removedDeployOp();

	_DELETE( pImpl_ );
}


/* //////////////////////////////////////////////////////////////// */

MachLogDeployAPCOperation::MachLogDeployAPCOperation( MachLogAPC* pAPC, const MexPoint3d& dest )
:	MachLogOperation( "DEPLOY_APC_OPERATION", MachLogOperation::DEPLOY_APC_OPERATION ),
	pImpl_( _NEW( MachLogDeployAPCOperationImpl( pAPC ) ) )
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	pAPC_->addedDeployOp();
	pAPC_->dropzonePosition( dest );
}

/////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogDeployAPCOperation::beNotified( W4dSubject*,
                 W4dSubject::NotificationEvent, int)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////

void MachLogDeployAPCOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	o << "MachLogDeployAPCOperation " << std::endl;
	switch( status_ )
	{
		case GOTO_DESTINATION:
			o << "GOTO_DESTINATION\n";
			break;
		case DEPLOY_MACHINES:
			o << "DEPLOY_MACHINES\n";
			break;
		default:
			o << "Unknown status\n";
	}
}

bool MachLogDeployAPCOperation::doStart()
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pAPC_ );
}

/////////////////////////////////////////////////////////////////////////

void MachLogDeployAPCOperation::doFinish()
{
	/* Intentionally Empty	*/
}

/////////////////////////////////////////////////////////////////////////

bool MachLogDeployAPCOperation::doIsFinished() const
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	return finished_;
}

/////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogDeployAPCOperation::doUpdate( )
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

   	//have we got an active sub operation at the moment?
   	if( pSubOperation() )
	{
		return 0.5;
	}

	//if the APC is inside a building then exit
	if( pAPC_->insideBuilding() )
	{
		finished_ = true;
		return 0.0;
	}

	if( pAPC_->machines().size() == 0 )
	{
		finished_ = true;
		return 0.0;
	}

	if( status_ == GOTO_DESTINATION )
	{
		const MexPoint3d& dest = pAPC_->dropzonePosition();

		//HAL_STREAM(" status_ == GOTO_DESTINATION checking range from APC\n" );
		//are we more than 10 metres away from out final destination?
        //This test must be performed in 2d so we don't get discrepancies on high terrain.
        MexPoint2d apcPosition2d = pAPC_->position();
        MexPoint2d dropPosition2d = dest;
		if( apcPosition2d.sqrEuclidianDistance( dropPosition2d ) > 100 )
		{
			subOperation( pAPC_, _NEW( MachLogMoveToOperation( pAPC_, dest ) ) );
		}
		else
		{
			//HAL_STREAM(" close enough set status to DEPLOY_MACHINES\n" );
			status_ = DEPLOY_MACHINES;
		}
	}

	//deploy one machine every second
	if( status_ == DEPLOY_MACHINES )
	{
		if( pAPC_->machines().size() == 0 )
		{
			finished_ = true;
			return 0.0;
		}
		else
		{
			pAPC_->motionSeq().stop();

			// We deploy using a LIFO principle.
			MachLogAPC::Machines::const_iterator i = pAPC_->machines().end();

			//check to see if we have deployed all the machines
			//next time around the iterator will point to the end again.
			//if there is no space for the machines
			// then the operation will never end.
			if( i == pAPC_->machines().begin() )
				finished_ = true;
			else
			{
				i--;	// We want the last iterator position, not the "beyond end" position

				MexPoint3d deployPoint;

				if (!pAPC_->nextDeployPoint( &deployPoint, (*i)->highClearence()))	 // Returns updated deployPoint as side-effect if found.
				{
					// No space was found ANYWHERE nearby for a deployment. Oh bugger. Do something, maybe?
				}

				else

				{
					// First, deploy the machine from the APC
					pAPC_->remove( (*i), deployPoint );

					// then make them tootle out away from the APC
					//pushFurtherOut( (*i), ( ( pAPC_->nMachinesCarried() + 8 )/8 ) * 20 );
				}
			}
		}
	}
	return 1.0;
}

/////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogDeployAPCOperation::doBeInterrupted()
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	pAPC_->motionSeq().stop();
	return not pAPC_->motionSeq().hasDestination();
}

/////////////////////////////////////////////////////////////////////////

//virtual
void MachLogDeployAPCOperation::pushFurtherOut( MachLogMachine* pMachine, MATHEX_SCALAR additionalDistance )
{
	CB_MachLogDeployAPCOperation_DEPIMPL();

	PRE (pMachine != NULL );

	static PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();

	// Calculate position along the line from the centre of the APC to the newly-deployed machine that is
	// additionalDistance metres further out than the machine's current position.

	MATHEX_SCALAR oldDistance = pAPC_->position().euclidianDistance( pMachine->position() );
	MexPoint2d dest2d( pAPC_->position(), pMachine->position(),  ( oldDistance + additionalDistance)/oldDistance );

	// Send the machine to any location with adequate space on or near the target point

	if (cs.findSpace( pMachine->position(),dest2d, pMachine->highClearence(), 2, pMachine->obstacleFlags(), &dest2d ) )
	{
		pMachine->strategy().newOperation( _NEW(MachLogMoveToOperation( pMachine, dest2d, false ) ) ); // Last param == false as no possibility of leaving a building during move
	}
}

/////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogDeployAPCOperation& t )
{

    o << "MachLogDeployAPCOperation " << (void*)&t << " start" << std::endl;
    o << "MachLogDeployAPCOperation " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogDeployAPCOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogDeployAPCOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

MachLogDeployAPCOperation::MachLogDeployAPCOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/////////////////////////////////////////////////////////////////////////

/* End OPDAPC.CPP *************************************************/
