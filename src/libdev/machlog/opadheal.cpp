/*
 * O P A D H E A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "machlog/opadheal.hpp"
#include "machlog/opheal.hpp"
#include "machlog/administ.hpp"
#include "machlog/follow.hpp"
#include "machlog/convyoff.hpp"
#include "machlog/squad.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogAdminHealOperation );

MachLogAdminHealOperation::MachLogAdminHealOperation( MachLogAdministrator * pActor, MachActor* pTarget )
: 	MachLogOperation( "ADMIN_HEAL_OPERATION", MachLogOperation::ADMIN_HEAL_OPERATION ),
	pActor_( pActor ),
	pDirectObject_( pTarget ),
	complete_( false ),
	currentlyAttached_( true )
{
	pDirectObject_->attach( this );
}


//virtual
MachLogAdminHealOperation::~MachLogAdminHealOperation()
{
	pActor_->stopAllHealing( *pActor_ );
	if( currentlyAttached_ )
		pDirectObject_->detach( this );
}

void MachLogAdminHealOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminHealOperation" << std::endl; 
	o << " pDirectObject_.objecttype() (" << pDirectObject_->id() << ") " << pDirectObject_->objectType() << std::endl;
}

///////////////////////////////////

bool MachLogAdminHealOperation::doStart()
{
	return true;
}

PhysRelativeTime MachLogAdminHealOperation::doUpdate( )
{
	HAL_STREAM("(" << pActor_->id() << ") MLAdminHealOperation::doUpdate\n" );
	//check the range to the targetted object
	//less than 150 metres? If so go on the rampage
	PhysRelativeTime interval = 20.0;

	int index = 0;
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
		 {

			MachLogMachine* pSquadronMachine = (*i);

			if( not pSquadronMachine->evading() and			 // don't interrupt actors who are currently evading aggressors
				not pSquadronMachine->isStandingGround() and // machines standing ground are exempt from squad orders
				pSquadronMachine->id() != pActor_->id() )
			{
				if( pSquadronMachine->objectType() == MachLog::ADMINISTRATOR and pSquadronMachine->asCanAttack().hasHealingWeapon() )
				{
					HAL_STREAM(" issuing heal operation to " << pSquadronMachine->id() << std::endl );
					pSquadronMachine->newOperation( _NEW( MachLogHealOperation( &pSquadronMachine->asAdministrator(), pDirectObject_ ) ) );
				}
				else
				{
					HAL_STREAM(" issuing follow operation to " << pSquadronMachine->id() << std::endl );
					pSquadronMachine->newOperation( _NEW( MachLogFollowOperation( (MachLogMachine*)*i, pActor_ , MachLogConvoyOffsets::convoyOffset( MachLogConvoyOffsets::KILLER_CONVOY, index++, 15 ) ) ) );
				}
			}
		}
	HAL_STREAM(" issuing heal operation for myself and setting complete to true\n" );
	pActor_->newOperation( _NEW( MachLogHealOperation( pActor_, pDirectObject_ ) ) );
	complete_ = true;

	HAL_STREAM(" exit.\n" );
	return interval;
}

void MachLogAdminHealOperation::doFinish()
{

}
	
bool MachLogAdminHealOperation::doIsFinished() const
{
	return complete_;
}

bool MachLogAdminHealOperation::doBeInterrupted()
{
	return true;
}

//virtual
bool MachLogAdminHealOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int  )
{
	PRE ( currentlyAttached_ and pSubject == (W4dSubject*)pDirectObject_ );

	switch( event )
	{
	case W4dSubject::DELETED:
		{
			//construction has been deleted
			pDirectObject_ = NULL;
			currentlyAttached_ = false;
			complete_ = true;
		}
		break;
	default:
		;
	}
	//HAL_STREAM(" returning true\n" );
	return currentlyAttached_;
}

void perWrite( PerOstream& ostr, const MachLogAdminHealOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pDirectObject_;
    ostr << op.complete_;
	ostr << op.currentlyAttached_;
}

void perRead( PerIstream& istr, MachLogAdminHealOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pDirectObject_;
    istr >> op.complete_;
	istr >> op.currentlyAttached_;
	if( op.currentlyAttached_ )
		op.pDirectObject_->attach( &op );	
}

MachLogAdminHealOperation::MachLogAdminHealOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
