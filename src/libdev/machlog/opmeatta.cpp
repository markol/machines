/*
 * O P M E A T T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/opmeatta.hpp"

#include <iostream>

#include "ctl/pvector.hpp"

#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/abox3d.hpp"

#include "sim/manager.hpp"

#include "world4d/subject.hpp"
#include "world4d/entity.hpp"

#include "machphys/missemp.hpp"
#include "machphys/weapon.hpp"

#include "machlog/canattac.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/machine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/races.hpp"
#include "machlog/weapon.hpp"

#include "machlog/internal/firedata.hpp"
#include "machlog/internal/opmeatti.hpp"


PER_DEFINE_PERSISTENT( MachLogMissileEmplacementAttackOperation );

#define CB_MachLogMissileEmplacementAttackOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( bool, currentlyAttached_ ); \
    	CB_DEPIMPL( MachLogMissileEmplacement*, pActor_ ); \
		CB_DEPIMPL( MachActor*, pDirectObject_ ); \
		CB_DEPIMPL( MexPoint2d, lastTargetPosition_ ); \
		CB_DEPIMPL( bool, targetBehindCover_ ); \
		CB_DEPIMPL( MachLogAttackOperation::RaceChangeTerminate, raceChangeTerminate_ );

/* //////////////////////////////////////////////////////////////// */
MachLogMissileEmplacementAttackOperation::MachLogMissileEmplacementAttackOperation( MachLogMissileEmplacement * pActor, MachActor * pDirectObject, MachLogAttackOperation::RaceChangeTerminate rct )
:	MachLogOperation( "MISSILE_EMPLACEMENT_ATTACK_OPERATION" , MachLogOperation::MISSILE_EMPLACEMENT_ATTACK_OPERATION ),
	pImpl_( _NEW( MachLogMissileEmplacementAttackOperationImpl( pActor, pDirectObject, rct ) ) )
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );

	pDirectObject_->attach( this );
	currentlyAttached_ = true;

	// if I'm the only one currently initiating an attack against this target, it's quite probable I'm the
	// first one to spot it. Dispatch a cavee call to set all nearby chums on alert.
	if( not pDirectObject_->hasThreats() )
		pActor_->dispatchCaveeCall();

	// now signal to the target that the subject machine is threatening it,
		pDirectObject_->addThreat( pActor_->id() );

	TEST_INVARIANT;
}

MachLogMissileEmplacementAttackOperation::~MachLogMissileEmplacementAttackOperation()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	TEST_INVARIANT;

	//HAL_STREAM("(" << pActor_->id() << ") MLAttackOp::DTOR\n" );
	if( pDirectObject_ )
    {
		pDirectObject_->detach( this );

		// now signal to the target that the subject turret is no longer threatening it,
		// and if the current target is the direct object, cancel the target
		stopTargetting();
    }

	_DELETE( pImpl_ );
}

void MachLogMissileEmplacementAttackOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	o << "MachLogMissileEmplacementAttackOperation\n";
	if( pDirectObject_ )
	{
		o << " (" << directObject().id() << ") [" << directObject().objectType() << "] ";
		o << directObject().position() << std::endl;
		o << " hp " << directObject().hp() << " armour " << directObject().armour() << std::endl;
		o << " SQR range to target " << pActor_->position().sqrEuclidianDistance( directObject().position() ) << std::endl;
	}
	else
		o << "target not there anymore" << std::endl;

}

///////////////////////////////////

MachActor& MachLogMissileEmplacementAttackOperation::directObject()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	return *pDirectObject_;
}

const MachActor& MachLogMissileEmplacementAttackOperation::directObject() const
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	return *pDirectObject_;
}

///////////////////////////////////

bool MachLogMissileEmplacementAttackOperation::doStart()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	if( pDirectObject_ )
	{
		pActor_->currentTarget( pDirectObject_ );
	}

	return true;
}

/* //////////////////////////////////////////////////////////////// */


PhysRelativeTime MachLogMissileEmplacementAttackOperation::doUpdate()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	PRE( not isFinished() );
	PRE( pActor_ != NULL );

    //Do nothing if delegated to a subop
	if( pSubOperation() )
		return 0.1;

    //Need a view of the subject actor as a canAttack
    MachLogCanAttack& attacker = pActor_->asCanAttack();
    MachActor& target = directObject();

	//If the attacker is busy shooting something else right now, try again in a bit.
    //Otherwise ensure the direct object is the current target.
	if( attacker.hasCurrentTarget() )
	{
	    if( &attacker.currentTarget() != &target )
    		return 0.5;
    }
    else
        attacker.currentTarget( &target );

    //If the target is a machine inside a building, try again in a bit
    if( target.objectIsMachine() and target.asMachine().insideBuilding() )
        return 1.0;

    //Set up useful local variables
   	PhysRelativeTime interval = 0.25;
    const W4dEntity& targetObject = _CONST_CAST( const MachActor&, target).physObject();
    MATHEX_SCALAR heightModifier = 1.0;

    //Check each weapon separately
    const MachLogCanAttack::Weapons& weapons = _CONST_CAST( const MachLogCanAttack&, attacker).weapons();

	MachLogFireData fireData = pActor_->asCanAttack().createFireData();

    for( MachLogCanAttack::Weapons::const_iterator it = weapons.begin(); it != weapons.end(); ++it )
    {
        //store the logical and physical weapon refs
        MachLogWeapon& weapon = *(*it);
        const MachPhysWeapon& physWeapon = _CONST_CAST( const MachLogWeapon&, weapon).physWeapon();

        //Get the disposition of this weapon with respect to position (not cover)
        MexPoint3d globalAimPoint;

        const MexTransform3d& globalWeaponTransform = physWeapon.globalTransform();
        MachLogCanAttack::WeaponDisposition disposition =
            MachLogCanAttack::rangeData( globalWeaponTransform, targetObject, physWeapon.weaponData(),
                                         heightModifier, &globalAimPoint );

        //If in range, need to check for cover. Set up the global line from the weapon to the aim point
        if( disposition == MachLogCanAttack::IN_RANGE )
        {
            MexLine3d projectilePath( globalWeaponTransform.position(), globalAimPoint );
            disposition = attacker.behindCover( projectilePath, MachLogCanAttack::SAME_RACE_OK );

			/*
            //For some reason the GDT decided that soft cover should be igmored for MEs.
            if( disposition == MachLogCanAttack::BEHIND_FRIstd::endlY_COVER )
                disposition = MachLogCanAttack::IN_RANGE;
			*/
			// Revoked 9/12/98 HA/CS with GDT approval, permanent removal likely, pending playtesting.
        }

        //If this weapon can fire, do so
        if( disposition == MachLogCanAttack::IN_RANGE )
            weapon.fire( &target, fireData );
    }

	TEST_INVARIANT;

	return interval;
}

void MachLogMissileEmplacementAttackOperation::doFinish()
{

}

bool MachLogMissileEmplacementAttackOperation::doIsFinished() const
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

    //Continue the op as long as we have a target and it's in scanner range
    bool result = (pDirectObject_ == NULL)
    				or not pActor_->canDetect( *pDirectObject_ )
    				or not pActor_->isComplete()	// defensive programming - catchall to prevent illegal attacks by incomplete turrets
					or( pActor_->subType() == MachPhys::LAUNCHER // if I'm a launcher at more than 25% health, don't continue to attack targets less than 25m from me
						and pActor_->hpRatio() > 0.25
						and pActor_->position().sqrEuclidianDistance( pDirectObject_->position() ) < 625 );

	if( not result and pDirectObject_ )
	{
		// still have to do a check for AI races now being allied

		MachLogRaces& races = MachLogRaces::instance();
		MachPhys::Race attackerRace = pActor_->race();

		if( races.controller( attackerRace ).type() == MachLogController::AI_CONTROLLER )
		{
			// If we're now allies of this race, better halt the attack.
			if( races.dispositionToRace( attackerRace, pDirectObject_->race() ) == MachLogRaces::ALLY )
			{
				result = true;
			}
		}
	}

	return result;
}

bool MachLogMissileEmplacementAttackOperation::doBeInterrupted()
{
	return true;
}

//virtual
bool MachLogMissileEmplacementAttackOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	ASSERT( pSubject == (W4dSubject*)pDirectObject_, "Notified by an unexpected subject." );
	ASSERT( currentlyAttached_, "Notified when object believed itself to be unattached." );

	switch( event )
	{
	case W4dSubject::DELETED:
			// if the current target is the direct object, cancel the target
			stopTargetting();
			pDirectObject_ = NULL;
			nextCallBackTime( SimManager::instance().currentTime() );
			currentlyAttached_ = false;
			break;

	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
				if( raceChangeTerminate_ == MachLogAttackOperation::TERMINATE_ON_CHANGE )
				{
					MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( pDirectObject_->race(), pActor_->race() );

					if( disposition == MachLogRaces::OUR_RACE or disposition == MachLogRaces::ALLY )
					{
						// now signal to the target that the subject machine is no longer threatening it,
						// and if the current target is the direct object, cancel the target
						stopTargetting();
						pDirectObject_ = NULL;
						nextCallBackTime( SimManager::instance().currentTime() );
						currentlyAttached_ = false;
					}
				}
				break;
			case MachLog::TELEPORTED_OUT_OF_WORLD:
				// now signal to the target that the subject machine is no longer threatening it,
				// and if the current target is the direct object, cancel the target
				stopTargetting();
				pDirectObject_ = NULL;
				nextCallBackTime( SimManager::instance().currentTime() );
				currentlyAttached_ = false;
				break;
			default:
			;
		}
	default:
		;
	}

	TEST_INVARIANT;

	return currentlyAttached_;
}

void MachLogMissileEmplacementAttackOperation::stopTargetting()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	PRE( pDirectObject_ );

	pDirectObject_->removeThreat( pActor_->id() );

	MachLogCanAttack& attacker = pActor_->asCanAttack();
    if( &(attacker.currentTarget()) == pDirectObject_ )
    	attacker.currentTarget( NULL );
}


void MachLogMissileEmplacementAttackOperation::CLASS_INVARIANT
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

    INVARIANT( this != NULL );
	INVARIANT( iff( pDirectObject_ != NULL, currentlyAttached_ ) );
}

bool MachLogMissileEmplacementAttackOperation::currentlyAttached() const
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	return currentlyAttached_;
}

MachActor* MachLogMissileEmplacementAttackOperation::pDirectObject()
{
	CB_MachLogMissileEmplacementAttackOperation_DEPIMPL();

	return pDirectObject_;
}


///////////////////////////////////////////////////// persistence ///////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogMissileEmplacementAttackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogMissileEmplacementAttackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

	if( op.currentlyAttached() )
		op.pDirectObject()->attach( &op );
}

MachLogMissileEmplacementAttackOperation::MachLogMissileEmplacementAttackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

// Forced recompile 16/2/99 CPS
/* End OPERATIO.CPP *************************************************/
