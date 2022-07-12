#include "stdlib/string.hpp"
#include "machlog/internal/firedata.hpp"

#include "ctl/algorith.hpp"
#include "ctl/vector.hpp"

#include "world4d/manager.hpp"
#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "mathex/line3d.hpp"
#include "mathex/abox3d.hpp"

#include "sim/manager.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/aggrdata.hpp"
#include "machphys/admndata.hpp"
#include "machphys/msemdata.hpp"
#include "machphys/attack.hpp"
#include "machphys/random.hpp"
#include "machphys/punch.hpp"

#include "machlog/canattac.hpp"
#include "machlog/internal/canattai.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/missilem.hpp"
#include "machlog/pod.hpp"
#include "machlog/races.hpp"
#include "machlog/attack.hpp"
#include "machlog/strategy.hpp"
#include "machlog/opmeatta.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/weapon.hpp"
#include "machlog/spacial.hpp"
#include "machlog/charger.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/attafilt.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/squad.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"

PER_DEFINE_PERSISTENT( MachLogCanAttack );

#pragma GCC diagnostic ignored "-Wunused-variable"
//derferencing all these will cause a hit in the debug version - in the release version the compiler will remove
//any unreferenced variables.
#define CB_MachLogCanAttack_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachActor*, pMe_ ) \
    	CB_DEPIMPL( MachPhysCanAttack*, pPhysCanAttack_ ); \
		CB_DEPIMPL( MachActor* ,pCurrentTarget_ ); \
		CB_DEPIMPL( bool ,currentlyAttached_ ); \
		CB_DEPIMPL( MachPhys::WeaponCombo ,weaponCombo_); \
		CB_DEPIMPL( MachLogCanAttack::Weapons ,weapons_ ); \
		CB_DEPIMPL( ulong ,lastFireFrame_); \
		CB_DEPIMPL( MATHEX_SCALAR ,minimumInaccuracy_); \
		CB_DEPIMPL( MATHEX_SCALAR ,currentInaccuracy_); \
		CB_DEPIMPL( PhysAbsoluteTime ,lastFireTime_); \
		CB_DEPIMPL( MATHEX_SCALAR ,lastFireDiceRoll_); \
		CB_DEPIMPL( int, alertness_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, lastAlertnessDiminishTime_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, nextTimeAllowedToCheckAndAttack_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, nextHitByTestTime_ ); \
		CB_DEPIMPL( MATHEX_SCALAR, maxWeaponRange_ );


MachLogCanAttack::MachLogCanAttack( MachActor* pMe, MachPhysCanAttack* pPhysCanAttack, MachPhys::WeaponCombo wc )
:	pImpl_( _NEW( MachLogCanAttackImpl( pMe, pPhysCanAttack, wc ) ) )
{
	CB_MachLogCanAttack_DEPIMPL();
    weapons_.reserve( 3 );
	resetInaccuracyToDefault();
	aimSpoiled();				// ensures first shot does not refer to any previous shots for aim value
}

MachLogCanAttack::~MachLogCanAttack()
{
	CB_MachLogCanAttack_DEPIMPL();
	if(pMe_ and pMe_->objectIsMachine())
        pMe_->asMachine().squadron(NULL);
	while( weapons_.size() > 0 )
	{
		_DELETE( weapons_.front() );
		weapons_.erase( weapons_.begin() );
	}
	if( currentlyAttached_ and pCurrentTarget_ )
	{
		currentlyAttached_ = false;
		pCurrentTarget_->detach( this );
        pPhysCanAttack_->stopTrackingTarget();
	}
	_DELETE( pImpl_ );
}

void MachLogCanAttack::currentTarget( MachActor* p )
{
	CB_MachLogCanAttack_DEPIMPL();

	if( currentlyAttached_ and pCurrentTarget_ )
	{
		pCurrentTarget_->detach( this );
		currentlyAttached_ = false;

        pPhysCanAttack_->stopTrackingTarget();
	}
	pCurrentTarget_ = p;
	if( pCurrentTarget_ )
	{
		currentlyAttached_ = true;
		pCurrentTarget_->attach( this );

        pPhysCanAttack_->trackTarget( _CONST_CAST( const MachActor*, p)->physObject() );
	}

	MachLogNetwork& network = MachLogNetwork::instance();
    if( network.isNetworkGame() and network.remoteStatus( pMe_->race() ) == MachLogNetwork::LOCAL_PROCESS )
    {
            UtlId otherId = 0;
            if( pCurrentTarget_ )
                    otherId = pCurrentTarget_->id();
            network.messageBroker().sendCurrentTargetMessage( pMe_->id(), otherId );
    }
}

const MachActor& MachLogCanAttack::currentTarget() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return *pCurrentTarget_;
}

MachActor& MachLogCanAttack::currentTarget()
{
	CB_MachLogCanAttack_DEPIMPL();
	return *pCurrentTarget_;
}

bool MachLogCanAttack::hasCurrentTarget() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return pCurrentTarget_ != NULL;
}

bool MachLogCanAttack::currentTargetIsOneOfThese( const Actors& strongThreats )
{
	CB_MachLogCanAttack_DEPIMPL();
	ASSERT( hasCurrentTarget(), "Calling MachLogCanAttack::currentTargetIsOneOfThese when we don't even HAVE a current target." );

	Actors::const_iterator i = find( strongThreats.begin(), strongThreats.end(), currentTarget().id() );

	return( i != strongThreats.end() );
}

//virtual
bool MachLogCanAttack::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int )
{
	CB_MachLogCanAttack_DEPIMPL();
	switch( event )
	{
	case W4dSubject::DELETED:
		if( pSubject == (W4dSubject*)pCurrentTarget_ )
		{
			currentlyAttached_ = false;
			pCurrentTarget_ = NULL;
            pPhysCanAttack_->stopTrackingTarget();
			return false;
		}
		break;
	default:
		;
	}
	return true;
}

void MachLogCanAttack::checkAndAttackCloserTarget( MachLogMachine* pActor, MachActor* pFiredAtMe )
{
	CB_MachLogCanAttack_DEPIMPL();

    //Nothing to do in 1st person
    if( pActor->isIn1stPersonView() )
        return;

	//Never do checkandattack if evading
    if( pActor->evading() )
        return;

	if( MachLogNetwork::instance().isNetworkGame() )
	{
		if( MachLogNetwork::REMOTE_PROCESS == MachLogNetwork::instance().remoteStatus( pActor->race() ) )
			return;
	}
	//Do some quick DefCon checks first.
	MachLog::DefCon defCon = pActor->virtualDefCon();

	//if high defcon and we are not classified as being fully free to attack then return out
	if( 	defCon == MachLog::DEFCON_HIGH
		and not pActor->isFreeToAttack()   )
	{
		return;
	}

	MachLogRaces& races = MachLogRaces::instance();

	//Look for a closer target
	MachActor* pMach = NULL;
	//did something fire at us?
	bool found;

	if( not pFiredAtMe
		and hasCurrentTarget() )
	{
		// I already have a target. Chances are nothing much has happened between then and now.
		// Probably don't need to bother checking for a change in circumstances. Let us do something really smart.
		// Let us take the hwlevel of the machine, subtract it from 6, call that X, then make the chances of actually
		// bothering to check for a change in circumstance 1 chance in X.

		// So hwlevel 1 machines do full checks 16% of the time, whereas hwlevel 5 machines do full checks 84%
		// of the time.

		int ignoreChance = MachPhysRandom::randomInt(0, 6 );

		if( ignoreChance >= pActor->hwLevel() )
		{
			// stay on full alert....
			setMinimumAlertness( 125 );

			//....but don't retarget
			found = false;
		}
		else
		{
			// let's take a look at the best alternative
			pMach = &currentTarget();
			found = races.findAlternativeTargetClosestTo( *pActor, &pMach );
		}
	}
	else if( pFiredAtMe )
	{
		pMach = pFiredAtMe;
		found = true;
	}
	else
	{
		found = races.findTargetClosestTo( *pActor, &pMach );
	}

	if( found and pMach->objectIsMachine() )
	{
		// our target is a machine. Therefore, it might be inside a building.
		MachLogMachine& targetMachine = pMach->asMachine();

		if( targetMachine.insideBuilding() )
		{
			// yes, it IS inside a building. If we're able to
			// enter it, or are currently inside it, no problems. Otherwise, sod this as a target.

			const MachLogConstruction& buildingTargetIsInside = targetMachine.insideWhichBuilding();

			bool okayToAttack = ( pActor->insideBuilding() and pActor->insideWhichBuilding().id() == buildingTargetIsInside.id() )
								or pActor->canEnterConstructionNow( buildingTargetIsInside );

			if( not okayToAttack )
				found = false;
		}
		else if( pActor->insideBuilding() and pActor->race() == races.pcController().race() )
		{
			// player race machines shouldn't leave buildings of their own volition to attack targets outside
			found = false;
		}

	}

	if( found )
	{
		bool isStandingGround = pActor->isStandingGround();

		//MATHEX_SCALAR sqrScannerRange = sqr( pActor->effectiveScannerRange() );
		// TODO fix ambiguous sqr
		MATHEX_SCALAR sqrScannerRange = ( pActor->effectiveScannerRange()*pActor->effectiveScannerRange() );
		MATHEX_SCALAR sqrDistanceToTarget = pActor->position().sqrEuclidianDistance( pMach->position() );
		// if something less than scanner range meters away then target it for attack.
		// UNLESS the distance between us is more than maximumEngagementDistance( currently 150m), in which case
		// he's too far away and is probably one of those horrible big large missile launchers
		// Note that we rescind the scanner range requirement( but not the maximumEngagementDistance ) if we were
		// actually hit by the other actor's fire - we're able to "zero his position" even if beyond scanner range, by
		// detecting the direction from which the shot came.
		if( ( sqrDistanceToTarget < sqrScannerRange or pFiredAtMe )
			and sqrDistanceToTarget <= sqrMaximumEngagementDistance() )
		{
			// go on full alert
			setMinimumAlertness( 125 );

			bool retarget = true;

			// this clause prevents machines on higher defcons from moving out of their way to attack canAttack machines
			// that are currently outside engagement range/behind cover for both parties, or (in the case of non-attack-capable
			// machines), targets which are currently outside my weapon range or behind cover.

			if( defCon != MachLog::DEFCON_LOW )
			{
				if( actorIsDirectThreat( *pMach ) )
				{
					retarget = pFiredAtMe != NULL
							   or acceptableAggressiveTargetForHigherDefconsMachine( pMach, sqrDistanceToTarget );
				}
				else
				{
					// not an aggressive machine - if I can't shoot it from where I'm standing, forget about it.
					if( not canTurnToAttackAndNoCover( pMach ) )
					{
						retarget = false;
					}
				}
			}
			// end if( defCon != MachLog::DEFCON_LOW )


			// =================  The "What to do if we already have a target" section ===================

			if( retarget and hasCurrentTarget() )
			{
				// note that this operation MAY change *pMach to an alternative actor under certain conditions
				retarget = switchFromExistingTargetToThisOne( pMach, sqrDistanceToTarget, pFiredAtMe );
			}
			// ================= END "What to do if we already have a target" section ===================

			if( retarget )
			{
				bool initiateAttack = false;

				if( hasCurrentTarget() and not currentTarget().isDead() )
				{
					if( pMach->id() != currentTarget().id() )
						initiateAttack = true;
				}
				else
					initiateAttack = true;

				if( initiateAttack )
				{
					currentTarget( pMach );
					if( pActor->strategy().currentOperationType() != MachLogOperation::COUNTERATTACK_OPERATION
						and not isStandingGround
						and not pActor->strategy().isUninterruptable() )
					{
						pActor->strategy().changeToCounterattackMode( pMach );
					}
					else
					{
						pActor->strategy().newOperation( _NEW( MachLogAttackOperation( pActor, pMach, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ), true );
					}


					// don't check again for another target in the next three seconds (this will be cancelled if the target is destroyed)
					suppressAttackingUrges( 3.0 );
				}
			}
		}
	}
}


void MachLogCanAttack::checkAndAttackCloserTarget( MachLogPod* pActor, MachActor* pFiredAtMe )
{
	CB_MachLogCanAttack_DEPIMPL();
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		if( MachLogNetwork::REMOTE_PROCESS == MachLogNetwork::instance().remoteStatus( pActor->race() ) )
			return;
	}
	//Look for a closer target
	MachActor* pMach = NULL;
	//did something fire at us?
	bool found;
	MATHEX_SCALAR closest = 1000000000;
	if( pFiredAtMe )
	{
		pMach = pFiredAtMe;
		found = true;
	}
	else
	{
		found = MachLogRaces::instance().findTargetClosestTo( *pActor, &pMach );
		if( found )
			if(  pMach->objectIsMachine() )
				if(  pMach->asMachine().insideBuilding() )
					found = false;
	}

	if( found )
	{
		MATHEX_SCALAR sqrScannerRange = pActor->getMaximumWeaponRange();
		sqrScannerRange *= sqrScannerRange;
		closest = pActor->position().sqrEuclidianDistance( pMach->position() );
		//if something less than scanner range meters away then shoot it as well as correct target
		if( closest < sqrScannerRange )
		{
			bool retarget = true;
			//check for normal defcon setting here.
			//if normal then only target if new target is lessequal in hwlevel

			//if the current target is also in scanner range and can attack then do NOT retarget
			if( retarget and /*pActor->asCanAttack().*/hasCurrentTarget() )
			{
				if( currentTarget().objectIsCanAttack() and
					pActor->position().sqrEuclidianDistance( /*pActor->asCanAttack().*/currentTarget().position() ) < sqrScannerRange )
					retarget = false;
			}

			if( retarget )
			{
				if( hasCurrentTarget() and MachLogRaces::instance().objectExists( &/*pActor->asCanAttack().*/currentTarget() ) )
				{
					if( pMach->id() != currentTarget().id() )
					{
						currentTarget( pMach );
						pActor->strategy().newOperation( _NEW( MachLogPodAttackOperation( pActor, pMach ) ), true );
					}
				}
				else
				{
					currentTarget( pMach );
					pActor->strategy().newOperation( _NEW( MachLogPodAttackOperation( pActor, pMach ) ), true );
				}
			}
		}
	}
}

void MachLogCanAttack::checkAndAttackCloserTarget( MachLogMissileEmplacement* pActor, MachActor* pFiredAtMe )
{
	CB_MachLogCanAttack_DEPIMPL();
    //Nothing to do in 1st person
    if( pActor->isIn1stPersonView() )
        return;

	HAL_STREAM("(" << pActor->id() << ") MachLogCanAttack::checkAndAttackCloserTarget\n" );
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		if( MachLogNetwork::REMOTE_PROCESS == MachLogNetwork::instance().remoteStatus( pActor->race() ) )
			return;
	}
	//Look for a closer target
	MachActor * pMach;

	MachLogRaces& races = MachLogRaces::instance();
	bool found = false;
	// launchers at anything more than 25% of full health will not attack targets within 25m of themselves
	MATHEX_SCALAR minSqrFiringRange = 0.0;
	if( pActor->subType() == MachPhys::LAUNCHER and pActor->hpRatio() > 0.25 )
		minSqrFiringRange = 625.0;

	////////////////////////////////////////
	if( not pFiredAtMe
		and hasCurrentTarget() )
	{
		// if we already have a target, only a 20% chance of checking for an alternative
		int ignoreChance = MachPhysRandom::randomInt(0, 5 );

		if( ignoreChance != 0 )
		{
			// stay on full alert....
			setMinimumAlertness( 125 );

			//....but don't retarget
			found = false;
		}
		else
		{
			// let's take a look at the best alternative
			pMach = &currentTarget();
			found = races.findAlternativeTargetClosestTo( *pActor, &pMach, minSqrFiringRange );
		}
	}
	else if( pFiredAtMe )
	{
		if( pActor->position().sqrEuclidianDistance( pFiredAtMe->position() ) >= minSqrFiringRange )
		{
			pMach = pFiredAtMe;
			found = true;
		}
	}
	else
	{
		found = races.findTargetClosestTo( *pActor,
										   &pMach,
										   MachLog::TARGET_NORMAL,
										   MachLog::N_OBJECT_TYPES,
										   pActor->position(),
										   minSqrFiringRange );
	}

	// never bother with machines inside buildings
	if( found
		and pMach->objectIsMachine()
		and pMach->asMachine().insideBuilding() )
	{
		found = false;
	}

	////////////////////////////////////////

	if( found )
	{
		HAL_STREAM(" found something new " << pMach->objectType() << std::endl );
	}
	if( found and pActor->canDetect( *pMach ) )
	{
		// go to full alert.
		setMinimumAlertness( 125 );

		HAL_STREAM("(" << pActor->id() << ") MachLogCanAttack::checkAndAttackCloserTarget found closer target\n" );
		HAL_STREAM(" hasCurrentTarget " << hasCurrentTarget() << " we can detect other target\n" );
		bool retarget = true;
		//if we have a current target.........
		if( hasCurrentTarget() )
		{
			MachActor& currentTargetActor = currentTarget();

			HAL_STREAM(" currentTarget id is " << currentTargetActor.id() << " new id is " << pMach->id() << std::endl );
			if( currentTargetActor.id() == pMach->id() )
				retarget = false;
			else
			{
				HAL_STREAM(" checking to see if I should retarget\n" );
				HAL_STREAM(" inWeaponRange new target is " << inWeaponRange( *pMach, NONE ) << std::endl );
				HAL_STREAM(" new target behind softCover is " << targetBehindSoftCover( pMach, SAME_RACE_OK ) << std::endl );
				// .....don't bother retargetting if we can't see and attack the rival target candidate
				if( not canTurnToAttackAndNoCover( pMach ) )
				{
					retarget = false;
					HAL_STREAM(" new target is not valid\n" );
				}
				else
				{
					HAL_STREAM(" new target is valid verifying that old target is also valid\n" );
					if( canTurnToAttackAndNoCover( &currentTargetActor ) )
					{
						// both our current and the rival target are in range and shootable. Which shall we choose?

						// use different rules for PC and AI races. PC (player) races will always stick with their
						// current target if it remains viable......AI races will always switch target
						// if rival target poses attacking threat and current target does not.
						if( races.controller( pActor->race() ).type() != MachLogController::AI_CONTROLLER )
						{
							if( actorIsDirectThreat( currentTargetActor	) or not actorIsDirectThreat( *pMach ) )
							{
								retarget = false;
							}
						}
						else
						{
							// no real point in switching targets
							retarget = false;
						}
					}
				}
			}
		}

		if( retarget )
		{
			HAL_STREAM(" retargetting\n" );
			currentTarget( pMach );
			pActor->strategy().newOperation( _NEW( MachLogMissileEmplacementAttackOperation( pActor, pMach, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ), false );
		}
	}
}


const MachLogCanAttack::Weapons&	MachLogCanAttack::weapons() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return weapons_;
}

MachLogCanAttack::Weapons&	MachLogCanAttack::weapons()
{
	CB_MachLogCanAttack_DEPIMPL();
	return weapons_;
}

void MachLogCanAttack::addWeapon( MachLogWeapon* pNewWeapon )
{
	CB_MachLogCanAttack_DEPIMPL();
	weapons_.push_back( pNewWeapon );

	// adjust range
	if( pNewWeapon->range() > maxWeaponRange_ )
		maxWeaponRange_ = pNewWeapon->range();
}

const MATHEX_SCALAR MachLogCanAttack::getMaximumWeaponRange() const
{
	CB_DEPIMPL( MATHEX_SCALAR, maxWeaponRange_ );

	return maxWeaponRange_;
}

const MATHEX_SCALAR MachLogCanAttack::getMaximumHealWeaponRange() const
{
//	CB_MachLogCanAttack_DEPIMPL();
	CB_DEPIMPL( const Weapons, weapons_ );
	MATHEX_SCALAR result = 0;
	for( Weapons::const_iterator i = weapons_.begin(); i != weapons().end(); ++i )
	{
		const MachLogWeapon& weapon = (**i);
		MachPhys::WeaponType type = weapon.physWeapon().type();
		if( (type == MachPhys::SUPERCHARGE_ADVANCED or
			 type == MachPhys::SUPERCHARGE_SUPER ) and
			 weapon.range() > result )
			result = weapon.range();
	}
	return result;
}

const MATHEX_SCALAR MachLogCanAttack::getMaximumTreacheryWeaponRange() const
{
//	CB_MachLogCanAttack_DEPIMPL();
	CB_DEPIMPL( const Weapons, weapons_ );
	MATHEX_SCALAR result = 0;
	for( Weapons::const_iterator i = weapons_.begin(); i != weapons().end(); ++i )
	{
		const MachLogWeapon& weapon = (**i);
		MachPhys::WeaponType type = weapon.physWeapon().type();
		if( type == MachPhys::ORB_OF_TREACHERY )
			result = weapon.range();
	}
	return result;
}

bool MachLogCanAttack::hasTreacheryWeapon() const
{
//	CB_MachLogCanAttack_DEPIMPL();
	CB_DEPIMPL( const Weapons, weapons_ );
	bool result = false;
	for( Weapons::const_iterator i = weapons_.begin(); i != weapons().end(); ++i )
	{
		MachPhys::WeaponType type = _CONST_CAST( const MachLogWeapon*, (*i) )->physWeapon().type();
		if( type == MachPhys::ORB_OF_TREACHERY )
			result = true;
	}
	return result;
}

PhysRelativeTime MachLogCanAttack::attackWithTreachery( MachActor* pTarget )
{
	CB_MachLogCanAttack_DEPIMPL();
	PhysRelativeTime rt = 0;
	MachLogFireData fireData = createFireData();
	for( MachLogCanAttack::Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		if( _CONST_CAST( const MachLogWeapon*, (*i) )->physWeapon().type() == MachPhys::ORB_OF_TREACHERY )
		{
			lastFireFrame_ = W4dManager::instance().frameNumber();
			PhysRelativeTime wft = (*i)->fire( pTarget, fireData );
			if(wft > rt) rt = wft;
			improveAim();
		}
	}
	return rt;
}

void MachLogCanAttack::doOutputOperator( ostream& o ) const
{
//	CB_MachLogCanAttack_DEPIMPL();
	CB_DEPIMPL( const Weapons, weapons_ );
	CB_DEPIMPL( int, alertness_ );

	o << "Alertness: " << alertness_<< std::endl;

	if( hasCurrentTarget() )
   		o << "Has current target. Target is of id [" << currentTarget().id() << "]" << std::endl;
   	else
   		o << "No current target." << std::endl;

	o << "Weapons_.size() " << weapons_.size() << std::endl;
	for( Weapons::const_iterator i = weapons_.begin(); i != weapons_.end(); ++i )
	{
		const MachPhysWeapon& pw = (const MachPhysWeapon&)((const MachLogWeapon*)(*i))->physWeapon();
		o << pw.type() << " " << (*i)->percentageRecharge() << std::endl;
	}

	o << "Inaccuracy rating: " << minimumInaccuracy() << "->" << inaccuracy() << std::endl;
}

MachPhys::WeaponCombo MachLogCanAttack::weaponCombo() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return weaponCombo_;
}


bool MachLogCanAttack::canAttack( const MachActor& other ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	if( pMe_->position().sqrEuclidianDistance( other.position() ) == 0 )
	{
		return true;
	}
	bool result = inWeaponRange( other, NONE )
				and inAngleRange( other.physObject() );
	return result;
	                // and hasSightOf( other )
}

bool MachLogCanAttack::canTurnToAttack( const MachActor& other ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	return inWeaponRange( other, NONE );
}

bool MachLogCanAttack::inWeaponRange( const MachActor& other, MachLogCanAttack::SpecialWeaponRange specialType ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	MATHEX_SCALAR weaponRange = 0;
	switch( specialType )
	{
		case NONE: 				weaponRange = getMaximumWeaponRange() * 0.95; break;
		case ORB_OF_TREACHERY: 	weaponRange = getMaximumTreacheryWeaponRange() * 0.95; break;
		case SUPERCHARGE: 		weaponRange = getMaximumHealWeaponRange() * 0.95; break;
		case GORILLA_PUNCH:		weaponRange = getMaximumPunchWeaponRange() * 0.95; break;
		default:
		ASSERT_INFO( specialType );
		ASSERT_BAD_CASE;
	}

	bool result = false;

	if( other.objectIsConstruction() )
	{
		result = MachLogSpacialManipulation::withinRangeOfMexAlignedBox2d( other.asConstruction().globalBoundary(), pMe_->position(), weaponRange );
	}
	else
	{
		MexPoint3d targetPos = other.position();
		result = pMe_->position().sqrEuclidianDistance( targetPos ) <= weaponRange * weaponRange;
	}

	return result;
}

bool MachLogCanAttack::withinSqrMaximumWeaponRange( MATHEX_SCALAR sqrDistance ) const
{
	MATHEX_SCALAR weaponRange = getMaximumWeaponRange();
	return sqrDistance < weaponRange * weaponRange;
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogCanAttack::inAngleRange( const W4dEntity& entity ) const
{
	CB_MachLogCanAttack_DEPIMPL();

    //Compute this' inverse global transform
    MexTransform3d inverse;
 	pMe_->physObject().globalTransform().invert( &inverse );

    //Hence get coordinates of entity in local frame of this
    MexPoint3d entityPosition( entity.globalTransform().position() );
    inverse.transform( &entityPosition );

    //Assume 30 degrees either side of straight on is good enough
    //NB sin( 30 ) = 0.5
	bool result = abs( entityPosition.y() ) < MachLogAdministrator::sinAngleRange() * entityPosition.x();
	return result;
}

// static
MexRadians MachLogCanAttack::angleRange()
{
	static MexRadians result_( Mathex::PI / 6 );
	return result_;
}

// static
MATHEX_SCALAR MachLogCanAttack::sinAngleRange()
{
	static MATHEX_SCALAR result_ = sin( MachLogCanAttack::angleRange().asScalar() );
	return result_;
}

bool MachLogCanAttack::hasHealingWeapon() const
{
	CB_MachLogCanAttack_DEPIMPL();
	bool result = false;
	for( Weapons::const_iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = ( ( const MachLogWeapon&)(**i) ).physWeapon().type();
		if( type == MachPhys::SUPERCHARGE_ADVANCED or
			type == MachPhys::SUPERCHARGE_SUPER )
		{
			result = true;
			break;
		}
	}
	return result;
}

void MachLogCanAttack::stopAllHealing( const MachActor& me )
{
	CB_MachLogCanAttack_DEPIMPL();
    PRE( &me == pMe_ );

	//remove any healing effect caused by administrators guns.
	for( Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = ( ( const MachLogWeapon&)(**i) ).physWeapon().type();
		if( type == MachPhys::SUPERCHARGE_ADVANCED or
			type == MachPhys::SUPERCHARGE_SUPER )
		{
			MachLogSuperCharger& charger = MachLogSuperCharger::asSuperCharger( (*i) );
            if( charger.healing() )
            {
    			charger.stopAllHealing();
    			if( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().remoteStatus( me.race() ) == MachLogNetwork::LOCAL_PROCESS )
    				MachLogNetwork::instance().messageBroker().sendHealMessage( me.id(), 0, MachLogMessageBroker::STOP_HEALING, false );
            }
		}
	}
}

//virtual
PhysRelativeTime MachLogCanAttack::attack( MachActor* pTarget )
{
	CB_MachLogCanAttack_DEPIMPL();
	// ensure that the guns miss together, fire similar (or same if same weapon) numbers of bursts together etc.
	MachLogFireData fireData = createFireData();

	PhysRelativeTime rt = 0;
	for( Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = _CONST_CAST( const MachLogWeapon*, (*i) )->physWeapon().type();
		if( type != MachPhys::SUPERCHARGE_ADVANCED and
			type != MachPhys::SUPERCHARGE_SUPER and
			type != MachPhys::GORILLA_PUNCH
			and not( type == MachPhys::ORB_OF_TREACHERY and not pTarget->objectIsMachine() ) )
		{
			lastFireFrame_ = W4dManager::instance().frameNumber();
			PhysRelativeTime wft = (*i)->fire( pTarget, fireData );
			if(wft > rt) rt = wft;

			improveAim();
		}
	}
	return rt;
}

PhysRelativeTime MachLogCanAttack::heal( MachActor* pTarget )
{
	CB_MachLogCanAttack_DEPIMPL();

	PhysRelativeTime rt = 0;
	for( Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = _CONST_CAST( const MachLogWeapon*, (*i) )->physWeapon().type();
		if( type == MachPhys::SUPERCHARGE_ADVANCED or
			type == MachPhys::SUPERCHARGE_SUPER )
		{
			lastFireFrame_ = W4dManager::instance().frameNumber();
			PhysRelativeTime wft = (*i)->fire( pTarget );
			if(wft > rt) rt = wft;
			if( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().remoteStatus( pMe_->race() ) == MachLogNetwork::LOCAL_PROCESS )
				MachLogNetwork::instance().messageBroker().sendHealMessage( pMe_->id(), pTarget->id(), MachLogMessageBroker::BEGIN_HEALING, false );
		}
	}
	return rt;
}

bool MachLogCanAttack::targetBehindSoftCover( MachActor* pTarget, MachLogCanAttack::SoftCoverAccuracy precision )
{
	CB_MachLogCanAttack_DEPIMPL();
	DEBUG_STREAM( DIAG_HAL, "(" << pMe_->id() << ") MLAttackOp::targetBehindSoftCover\n" );
	//3d targetting slower than the 2d intersections testing but only just.
	MexPoint3d startLine( pMe_->position() );
	MexPoint3d endLine( pTarget->position() );
	startLine.z( startLine.z() + 1 );
	endLine.z( endLine.z() + 1 );
	MexLine3d line( startLine, endLine );
	MATHEX_SCALAR range = line.length();

    W4dDomain* pDomain = pMe_->pCurrentDomain();

    W4dEntity*      pIntersectingEntity;
    MATHEX_SCALAR   distanceToIntersection;

    //  Check for construction or terrain intersection

    if( pDomain->findNearerEntity(
      line, range, W4dEntity::nextCheckId(),
      W4dEntity::HIGH, &pIntersectingEntity, &distanceToIntersection,
      W4dDomain::EXCLUDE_NOT_SOLID ) )
    {
        if( pIntersectingEntity->hasSubject() )
		{
			MachActor& intersectingActor = MachLogRaces::instance().actor( pIntersectingEntity->id() );
			{
				if( precision == MachLogCanAttack::SAME_RACE_OK and intersectingActor.race() == pTarget->race() )
					return false;
			}
		}
		return true;
    }
	else
		return false;

/*
	//determine if target is visible to gun
	PhysConfigSpace2d::PolygonId junk;

	PhysConfigSpace2d* pcSpace = &MachLogPlanet::instance().configSpace();
	if( pMe_->objectIsMachine() )
		pcSpace = &pMe_->asMachine().motionSeq().currentConfigSpace();//MachLogPlanet::instance().configSpace();
	PhysConfigSpace2d& cSpace = *pcSpace;
	DEBUG_STREAM( DIAG_HAL," configSpace we are going to use " << (void*)&cSpace << std::endl );
	bool planetSpace = ( &cSpace == &MachLogPlanet::instance().configSpace() );
	DEBUG_STREAM( DIAG_HAL," planet config space " << (void*)&MachLogPlanet::instance().configSpace() << std::endl );
	DEBUG_STREAM( DIAG_HAL," in plantSpace " << planetSpace << std::endl );

    //Disable any obstacle polygon around the target object
    bool objectPolygonDisabled = false;
    PhysConfigSpace2d::PolygonId objectPolygonId;

	if( pTarget->objectIsConstruction() )
	{
        objectPolygonId = pTarget->asConstruction().obstaclePolygonId();
	    objectPolygonDisabled = cSpace.exists( objectPolygonId );
        if( objectPolygonDisabled )
    		cSpace.isPolygonEnabled( objectPolygonId, false );
    }
	else if( pTarget->objectIsMachine() )
	{
        objectPolygonId = pTarget->asMachine().motionSeq().currentObstacleId();
	    objectPolygonDisabled = cSpace.exists( objectPolygonId );
        if( objectPolygonDisabled )
    		cSpace.isPolygonEnabled( objectPolygonId, false );
    }
    else if( pTarget->objectIsArtefact() )
	{
        objectPolygonId = pTarget->asArtefact().obstaclePolygonId();
	    objectPolygonDisabled = cSpace.exists( objectPolygonId );
        if( objectPolygonDisabled )
    		cSpace.isPolygonEnabled( objectPolygonId, false );
    }

	if( pMe_->objectIsConstruction() )
		cSpace.isPolygonEnabled( pMe_->asConstruction().obstaclePolygonId(), false );
	else
	{
		cSpace.isPolygonEnabled( pMe_->asMachine().motionSeq().currentObstacleId(), false );
		if( pMe_->asMachine().insideBuilding() and not planetSpace )
		{
			DEBUG_STREAM( DIAG_HAL," actor is inside building and not in planet Space so remove interior polys\n" );
			for( int i = 0; i < pMe_->asMachine().insideWhichBuilding().nEntrances(); ++i )
			{
				cSpace.isPolygonEnabled( pMe_->asMachine().insideWhichBuilding().entrance( i ).interiorPolygonId(), false );
				HAL_STREAM("  remove polygon " << pMe_->asMachine().insideWhichBuilding().entrance( i ).interiorPolygonId() << " for entrance " << i << std::endl );
			}
		}
	}

	bool result = not cSpace.contains( pMe_->position(), pTarget->position(), &junk, PhysConfigSpace2d::USE_ALL );
	DEBUG_STREAM( DIAG_HAL," result " << result << " junk Id " << junk << std::endl );

    //re-enable any obstacle polygon around the target object
    if( objectPolygonDisabled )
        cSpace.isPolygonEnabled( objectPolygonId, true );

	if( pMe_->objectIsConstruction() )
		cSpace.isPolygonEnabled( pMe_->asConstruction().obstaclePolygonId(), true );
	else
	{
		cSpace.isPolygonEnabled( pMe_->asMachine().motionSeq().currentObstacleId(), true );
		if( pMe_->asMachine().insideBuilding() and not planetSpace )
		{
			DEBUG_STREAM( DIAG_HAL," actor is inside building and not in planet Space so adding back interior polys\n" );
			for( int i = 0; i < pMe_->asMachine().insideWhichBuilding().nEntrances(); ++i )
			{
				cSpace.isPolygonEnabled( pMe_->asMachine().insideWhichBuilding().entrance( i ).interiorPolygonId(), true );
				DEBUG_STREAM( DIAG_HAL,"  add polygon " << pMe_->asMachine().insideWhichBuilding().entrance( i ).interiorPolygonId() << " for entrance " << i << std::endl );
			}
		}
	}


	DEBUG_STREAM( DIAG_HAL," result: behind cover " << result << " poygonId " << junk << std::endl );
	*/
}

//virtual
void MachLogCanAttack::domainDeleted( W4dDomain* )
{
	//inentionally empty...override as necessary

}

void MachLogCanAttack::dt( const char* text, bool start )
{
	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( start )
		startTime = Phys::time();
	else
	{
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_MISC, ",," << text << "," << endTime - startTime << std::endl );
	}
}

ulong MachLogCanAttack::lastFireFrame() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return lastFireFrame_;
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogCanAttack::canDetect( const MachActor& other )
{
	CB_MachLogCanAttack_DEPIMPL();

	if ( pMe_->objectType() == MachLog::MISSILE_EMPLACEMENT )
		return pMe_->asMissileEmplacement().canDetect( other );
	else
		return true;
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogCanAttack::canTurnHead() const
{
	CB_MachLogCanAttack_DEPIMPL();
    return pPhysCanAttack_->canTrackWeaponBase();
}

bool MachLogCanAttack::atTiltLimits() const
{
	CB_MachLogCanAttack_DEPIMPL();
    return pPhysCanAttack_->atTiltLimits( 0 );
}
/*
bool MachLogCanAttack::inAngleRange( const MachActor& target, bool* pInVerticalAngleRange ) const
{
    //Get the tan of the minimum horizontal deviation angle for the mounted weapons.
    //TBD: This value should be cached.
    MATHEX_SCALAR tanAngle = tan( 0.1 );

    //Get the object we are aiming at and its bounding volume and global transform.
    const W4dEntity& targetEntity = target.physObject();
    const MexTransform3d& targetGlobalTransform = targetEntity.globalTransform();
    const MexAlignedBox3d& targetBoundary = ( targetEntity.isComposite() ?
                                              targetEntity.asComposite().compositeBoundingVolume() :
                                              targetEntity.boundingVolume() );

    MexPoint3d lowTargetPosition( targetGlobalTransform.position() );

    //Get the inverse of the weapon aspect transform
    MexTransform3d inverse;
    pPhysCanAttack_->globalWeaponAspectTransform().invert( &inverse );

    //Hence get coordinates of the target base in local frame of this
    inverse.transform( &lowTargetPosition );

    //Compute maximum offset from direct line, using tolerance since we are using an
    //approximate location for the weapon firing point.
    MATHEX_SCALAR xLowTarget = lowTargetPosition.x();
    MATHEX_SCALAR maxOffset = xLowTarget * tanAngle * 0.85;
    MATHEX_SCALAR offset = Mathex::abs( lowTargetPosition.y() );
    bool inHorizontalAngleRange = offset < maxOffset;

    //If required, check the vertical angle range
    bool inVerticalAngleRange = false;
    if( inHorizontalAngleRange )
    {
        //We need to check if any part of the line from the base up to the top of the target's
        //bounding volume is in vertical angle range.
        //We already have the base of this line in lowTargetPosition.
        //Compute the top of the line in coordiante system of aspect transform.
        MexPoint3d highTargetPosition( 0.0, 0.0, targetBoundary.maxCorner().z() );
        targetGlobalTransform.transform( &highTargetPosition );
        inverse.transform( &highTargetPosition );

        //Project this point into the x = xLowTarget plane
        MATHEX_SCALAR zHighTarget = highTargetPosition.z() * (xLowTarget / highTargetPosition.x());

        //TBD: This value should be cached.
        MATHEX_SCALAR tanVerticalAngle = tan( 0.1 );
        maxOffset = xLowTarget * tanVerticalAngle * 0.85;

        MATHEX_SCALAR zLowTarget = lowTargetPosition.z();

        if( zLowTarget < zHighTarget )
            inVerticalAngleRange = zLowTarget < maxOffset and zHighTarget > -maxOffset;
        else
            inVerticalAngleRange = zHighTarget < maxOffset and zLowTarget > -maxOffset;
    }

    *pInVerticalAngleRange = inVerticalAngleRange;

    return inHorizontalAngleRange;
}
*/
//static
MachLogCanAttack::WeaponDisposition MachLogCanAttack::rangeData
(
    const MexTransform3d& attackFrame, const W4dEntity& target, const MachPhysWeaponData& weaponData,
    MATHEX_SCALAR heightModifier, MexPoint3d* pAimPoint
)
{
MISC_STREAM( "MachLogCanAttack::rangeData entry" << std::endl );
    WeaponDisposition disposition;

    //Get the bounding volume of the target
    const MexAlignedBox3d& targetBoundary = (target.isComposite() ? target.asComposite().compositeBoundingVolume()
                                                                  : target.boundingVolume());

    //Construct the transform from the target frame to the attacker frame
    MexTransform3d targetInverse, targetToAttackerTransform;
    const MexTransform3d& targetGlobalTransform = target.globalTransform();
    targetGlobalTransform.invert( &targetInverse );
    targetInverse.transform( attackFrame, &targetToAttackerTransform );

    //Get the weapon range. Modify it for any height advantage/disadvantage of the attacker,
    //and reduce to allow for rounding/algorithm errors.
    MATHEX_SCALAR weaponRange = weaponData.range() * heightModifier * 0.95;
    MATHEX_SCALAR sqrWeaponRange = weaponRange * weaponRange;

    //get the square distance of the attacker from the target boundary, and hence determine if in range.
    MATHEX_SCALAR sqrAttackerDistance = targetBoundary.sqrEuclidianDistance( targetToAttackerTransform.position() );
MISC_STREAM( "  weaponRange " << weaponRange << " sqrAttackerDistance " << sqrAttackerDistance << std::endl );
    if( sqrWeaponRange < sqrAttackerDistance )
        disposition = NOT_IN_DISTANCE_RANGE;
    else
    {
        //Compute the offset of the target point we actually aim at in the target's frame.

		MexPoint3d targetPointInTargetFrame;

		if( targetBoundary.minCorner().z() >= 0.0 )
        	targetPointInTargetFrame = targetBoundary.centroid();
		else
		{
			// whoah! This fella is actually (partially) below the ground! We must ensure that
			// we only get the centroid of the portion that is above the ground.
			MexAlignedBox3d aboveGroundBox( targetBoundary );
			MexPoint3d newMinCorner( aboveGroundBox.minCorner().x(), aboveGroundBox.minCorner().y(), 0.0 );
			aboveGroundBox.minCorner( newMinCorner );
			targetPointInTargetFrame = aboveGroundBox.centroid();
		}

        //Need the transform from the attacker to the target
        MexTransform3d attackerToTargetTransform;
        targetToAttackerTransform.invert( &attackerToTargetTransform );

        //Hence compute the target point in the attacker's frame
        MexPoint3d targetPointInAttackerFrame = targetPointInTargetFrame;
        attackerToTargetTransform.transform( &targetPointInAttackerFrame );
MISC_STREAM( " targetPointInAttackerFrame " << targetPointInAttackerFrame << std::endl );

        //Ensure it is in front of the attacker
        MATHEX_SCALAR x = targetPointInAttackerFrame.x();
        if( x <= 0.01 )
            disposition = NOT_IN_HORIZONTAL_ANGLE_RANGE;
        else
        {
            //Construct a line from the attacker to the target point, but modified by the deviation constraints.
            //This runs from attacker frame origin to (x,y,z).
            MATHEX_SCALAR maxYOffset = x * tan( weaponData.horizontalDeviationAngle() ) * 0.85;
            MATHEX_SCALAR y = targetPointInAttackerFrame.y();
            if( y > maxYOffset )
                y = maxYOffset;
            else if( y < -maxYOffset )
                y = -maxYOffset;

            MATHEX_SCALAR maxZOffset = x * tan( weaponData.verticalDeviationAngle() ) * 0.85;
            MATHEX_SCALAR z = targetPointInAttackerFrame.z();
            if( z > maxZOffset )
                z = maxZOffset;
            else if( z < -maxZOffset )
                z = -maxZOffset;

            //Get this line in the frame of the target
            MexPoint3d constrainedTargetPoint( x, y, z );
MISC_STREAM( "  constrainedTargetPoint " << constrainedTargetPoint << std::endl );
            targetToAttackerTransform.transform( &constrainedTargetPoint );
            MexLine3d projectilePath( targetToAttackerTransform.position(), constrainedTargetPoint );

            //Construct a very tall bounding volume from the actual target bounding volume
            MexPoint3d tallMinCorner( targetBoundary.minCorner() );
            MexPoint3d tallMaxCorner( targetBoundary.maxCorner() );
            tallMinCorner.z( -1000.0 );
            tallMaxCorner.z( 1000.0 );
            MexAlignedBox3d tallBoundary( tallMinCorner, tallMaxCorner );

            //Intersect the projectile with the tall volume. If no intersection, then we aren't in
            //horizontal angular range. If there is, we can check the distance to the target to ensure
            //in weapon range at the angle we are facing.
            MATHEX_SCALAR entryDistance = -1.0;
            MATHEX_SCALAR exitDistance;
            bool doesIntersect = tallBoundary.intersects( projectilePath, &entryDistance, &exitDistance );
MISC_STREAM( "  doesIntersect " << doesIntersect << "  entryDistance " << entryDistance << std::endl );
            if( not doesIntersect )
                disposition = NOT_IN_HORIZONTAL_ANGLE_RANGE;
            else if( entryDistance * entryDistance > sqrWeaponRange )
                disposition = NOT_IN_DISTANCE_RANGE;
            else
            {
                //Compute the actual point of intersection with the tall bounding volume
                MexPoint3d entryPoint = projectilePath.pointAtDistance( entryDistance );

                //If this is outside the z boundary of the target, we are not in vertical angular range
                MATHEX_SCALAR zEntry = entryPoint.z();
                MATHEX_SCALAR zUpperLimit = targetBoundary.maxCorner().z();
				MATHEX_SCALAR zLowerLimit = targetBoundary.minCorner().z();
                bool highEntry = zEntry > zUpperLimit;
				bool lowEntry = zEntry < zLowerLimit;

				// for the moment, assume vertical angle is bad if entering above or below the box
                bool verticalAngleBad = highEntry  or lowEntry;

                // we may rescind the "vertical angle bad" decision if we can determine that the line
				// actually passes through the box via its lower or upper face
                if( verticalAngleBad )
                {
					if( highEntry )
					{
						//We have entered above the object, but if we don't exit above, we have passed
						// through its upper face and are thus actually ok for the hit
	                    MATHEX_SCALAR zExit = projectilePath.pointAtDistance( exitDistance ).z();
	                    if( zExit < zUpperLimit )
	                        verticalAngleBad = false;
					}
					else
					{
						//( lowEntry )
						//We have entered below the object, but if we don't exit below, we have passed
						// through its lower face and are thus actually ok for the hit
	                    MATHEX_SCALAR zExit = projectilePath.pointAtDistance( exitDistance ).z();
	                    if( zExit > zLowerLimit )
	                        verticalAngleBad = false;

					}
                }

                if( verticalAngleBad )
                    disposition = NOT_IN_VERTICAL_ANGLE_RANGE;
                else
                {
                    //Looks like we're in range. Return the actual target point in global coords
                    disposition = IN_RANGE;
                    *pAimPoint = constrainedTargetPoint;
                    targetGlobalTransform.transform( pAimPoint );
                }
            }
        }
    }

MISC_STREAM( "MachLogCanAttack::rangeData exit " << disposition << std::endl );
    return disposition;
}

MachLogCanAttack::WeaponDisposition MachLogCanAttack::behindCover
(
    const MexLine3d& projectilePath, SoftCoverAccuracy precision
) const
{
	CB_MachLogCanAttack_DEPIMPL();

	PRE( hasCurrentTarget() );

	return testActorBehindCover( pMe_, pCurrentTarget_, projectilePath, precision );
}

// static
MachLogCanAttack::WeaponDisposition MachLogCanAttack::testActorBehindCover
(
    MachActor* pSubjectActor, MachActor* pTestActor, const MexLine3d& projectilePath, SoftCoverAccuracy precision
)
{
    //Set up a domain based line intersection test to find the first object
    //along the projectile path.

    //Get the domain containing the start point. Note that the attacking actor's domain is inadequate.
    //A weapon may extend across a portal, or the actor may be in a different domain, and intersect the
    //one it is currently located in.

    W4dDomain* pDomain = NULL;

    if( 	pSubjectActor->objectIsMachine()
		and pSubjectActor->asMachine().insideBuilding() )
	{
		// must use the building's domain, not that of the outside world
		pDomain = &( pSubjectActor->asMachine().insideWhichBuilding().interiorDomain() );
	}
	else
    	pDomain = MachLogPlanetDomains::pDomainAt( MexPoint2d( projectilePath.end1() ) );

    //Set up a new check id, and assign it to the attacker and test actor, so we don't waste
    //time checking them.
    ulong checkId = W4dEntity::nextCheckId();
    pSubjectActor->physObject().checkId( checkId );



	if( pTestActor )
   		pTestActor->physObject().checkId( checkId );

    //Use a filter which will check for intervening objects, other than those of
    //enemy races, depending on the softcover accuracy.
    MachPhys::Race attackerRace = pSubjectActor->race();
    MachLogAttackFilter attackFilter( attackerRace, precision );

    W4dEntity* pCoverEntity;
    MATHEX_SCALAR distance;
    bool hit = pDomain->findNearerEntity( projectilePath, projectilePath.length(), checkId, W4dEntity::MEDIUM,
                                          &pCoverEntity, &distance, &attackFilter );

    WeaponDisposition disposition;

    if( hit )
    {
        //There is some cover. If it is a fristd::endly/allied machine regard this as soft cover.
        //Otherwise hard cover.
        UtlId coverActorId = pCoverEntity->id();
        MachLogRaces& races = MachLogRaces::instance();
        disposition = BEHIND_COVER;

		//tbr========================
		//pCoverEntity->visible( not pCoverEntity->visible() );
		//tbr========================

        if( pCoverEntity->hasSubject() and races.actorExists( coverActorId ) )
        {
            MachActor& coverActor = races.actor( coverActorId );
            if( coverActor.objectIsMachine() )
            {
				MachLogRaces::DispositionToRace disposition = races.dispositionToRace( attackerRace, coverActor.race() );

                if( disposition != MachLogRaces::ENEMY and disposition != MachLogRaces::NEUTRAL )
                    //disposition = BEHIND_FRIENDLY_COVER;
                    disposition = _STATIC_CAST(MachLogRaces::DispositionToRace, BEHIND_FRIENDLY_COVER);
            }
        }
    }
    else
        disposition = IN_RANGE;

    return disposition;
}


const MATHEX_SCALAR MachLogCanAttack::getMaximumPunchWeaponRange() const
{
	return MachPhysPunchBlast::range();
}

bool MachLogCanAttack::hasPunchWeapon() const
{
	bool found = false;
	for( Weapons::const_iterator i = weapons().begin(); not found and i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = ( ( const MachLogWeapon&)(**i) ).physWeapon().type();
		if( type == MachPhys::GORILLA_PUNCH )
		{
			found = true;
			break;
		}
	}
	return found;
}

bool MachLogCanAttack::canPunch( const MachActor& other ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	if( pMe_->position().sqrEuclidianDistance( other.position() ) == 0 )
		return true;

	return inWeaponRange( other, GORILLA_PUNCH )
				and inAngleRange( other.physObject() );
}

// virtual
PhysRelativeTime MachLogCanAttack::attackWithPunch( MachActor* pTarget )
{
	CB_MachLogCanAttack_DEPIMPL();
	PRE( hasPunchWeapon() );

	PhysRelativeTime rt = 0;

	bool found = false;

	for( Weapons::iterator i = weapons().begin(); not found and i != weapons().end() ; ++i )
	{
		MachPhys::WeaponType type = ( ( const MachLogWeapon&)(**i) ).physWeapon().type();
		if( type == MachPhys::GORILLA_PUNCH )
		{
			found = true;
			lastFireFrame_ = W4dManager::instance().frameNumber();
			if( (*i)->recharged() )
			{
				ASSERT( pMe_->objectIsMachine(), "A non-machine actor has been given a ground punch."  );
            	pMe_->asMachine().motionSeq().stop();

				// give "smash" voicemail
				switch( MachPhysRandom::randomInt( 0, 4 ) )
				{
					case 0:
					{
						MachLogVoiceMailManager::instance().postNewMail( VID_NINJA_SMASH0, pMe_->id(), pMe_->race() );
					}
					break;

					default:
						;
				}
				rt = (*i)->fire( pTarget );
			}
		}
	}

	POST( found );

	return rt;
}


void MachLogCanAttack::resetInaccuracyToDefault()
{
	CB_MachLogCanAttack_DEPIMPL();
	MATHEX_SCALAR initialAccuracy = 0;

	ASSERT( pMe_->objectIsMachine() or pMe_->objectIsConstruction(),
				"Unexpected actor type." );

	ASSERT_INFO( pMe_->objectType() );

	if( pMe_->objectIsMachine() )
	{
		ASSERT( pMe_->objectType() == MachLog::AGGRESSOR
				or pMe_->objectType() == MachLog::ADMINISTRATOR,
				"Unexpected machine type." );

		if( pMe_->objectType() == MachLog::AGGRESSOR )
			initialAccuracy = pMe_->asAggressor().data().accuracy();
		else
			// must be an administrator
			initialAccuracy = pMe_->asAdministrator().data().accuracy();
	}
	else
	// must be a construction, and should be a missile emplacement
	{
		ASSERT( pMe_->objectType() == MachLog::MISSILE_EMPLACEMENT or pMe_->objectType() == MachLog::POD,
				"Unexpected construction type." );

		if( pMe_->objectType() == MachLog::MISSILE_EMPLACEMENT )
			initialAccuracy = pMe_->asMissileEmplacement().data().accuracy();
		else
			// dummy value for pods - not actually important what it is - 100 just looks nice.
			initialAccuracy = 100.0;
	}

	minimumInaccuracy_ = (100.0 - initialAccuracy );
	currentInaccuracy_ = minimumInaccuracy_;

}

MATHEX_SCALAR MachLogCanAttack::inaccuracy() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return currentInaccuracy_;
}

MATHEX_SCALAR MachLogCanAttack::minimumInaccuracy() const
{
	CB_MachLogCanAttack_DEPIMPL();
	return minimumInaccuracy_;
}

void MachLogCanAttack::decreaseMinimumInaccuracy( MATHEX_SCALAR amountToDecreaseBy )
{
	CB_MachLogCanAttack_DEPIMPL();
	minimumInaccuracy_ -= amountToDecreaseBy;
	if( minimumInaccuracy_ < 0 )
		minimumInaccuracy_ = 0;
}

void MachLogCanAttack::increaseCurrentInaccuracy( MATHEX_SCALAR amountToIncreaseBy )
{
	CB_MachLogCanAttack_DEPIMPL();
	currentInaccuracy_ += amountToIncreaseBy;
	if( currentInaccuracy_ > 75.0 )
		currentInaccuracy_ = 75.0;

/*	TBD: removed for now to make machines more accurate for alpha this is expected to go back in.
	// is hit sufficient to disturb our aim?
	if( amountToIncreaseBy > 2 )
		aimSpoiled();

*/
	// this may not be the most logically appropriate place to put this call, but it is a pretty good bottleneck
	// to catch all instances of aggressive machines being hit.
	cancelAnyAttackingUrgeSuppression();
}

void MachLogCanAttack::decreaseCurrentInaccuracy( MATHEX_SCALAR amountToDecreaseBy )
{
	CB_MachLogCanAttack_DEPIMPL();
	currentInaccuracy_ -= amountToDecreaseBy;
	if( currentInaccuracy_ < minimumInaccuracy_ )
		currentInaccuracy_ = minimumInaccuracy_;
}

void MachLogCanAttack::update()
{
	// do nothing
}

void MachLogCanAttack::aimSpoiled()
{
	CB_MachLogCanAttack_DEPIMPL();
	lastFireTime_ = SimManager::instance().currentTime() - MachLogCanAttack::improveAimInterval();
}

MachLogFireData MachLogCanAttack::createFireData()
{
	// used to ensure that the guns miss together, fire similar (or same if same weapon) numbers of bursts together etc.

	CB_MachLogCanAttack_DEPIMPL();
	MachLogFireData fireData;

	//random value that will be used to synchronise burst values
	fireData.burstSeed = MachPhysRandom::randomDouble( 0, 0.9999 );

	// is this a completely accurate shot?
	MATHEX_SCALAR chance;

	// is this a fresh aim, or one improving upon a previous aim?

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	PhysRelativeTime fireInterval = timeNow - lastFireTime_;
	if( fireInterval < MachLogCanAttack::improveAimInterval() )
	{
		// no need for improvement if last shot was direct hit
		if( lastFireDiceRoll_ >= inaccuracy() )
			chance = lastFireDiceRoll_;
		else
		{
			// guaranteed improvement upon previous shot
			chance = lastFireDiceRoll_ + ( fireInterval / 10.0 )*80.0;

			//improve the machine's accuracy for the experience of improving your aim.
			decreaseMinimumInaccuracy( fireInterval * 4 );
		}
	}
	else
	{
		// aiming afresh

		MATHEX_SCALAR penalty = 0.0;

		if( pMe_->objectIsMachine() )
		{
			MachLogMachine& meAsMachine = pMe_->asMachine();

			// Are there modifications for light?
			penalty += ( meAsMachine.perceivedDarkness() * 40.0 );

			//standard penalty against flying units
			if( hasCurrentTarget() )
			{
				MachActor& targetActor = currentTarget();
				if( targetActor.objectIsMachine() and targetActor.asMachine().machineIsGlider() )
				{
					penalty += 35.0;
				}
			}

			// general accuracy improves if in an administrator-led squad and the guv'nor's in 100m range.
			if( meAsMachine.objectType() != MachLog::ADMINISTRATOR
				and meAsMachine.squadron() != NULL
				and meAsMachine.squadron()->hasCommander()
				and meAsMachine.position().sqrEuclidianDistance( meAsMachine.squadron()->commander().position() ) <= 10000.0  )
			{
				penalty -= 20.0;
			}
		}

		chance = std::min( 100.0, MachPhysRandom::randomDouble( 0.0, 100.0 ) - penalty );

		// 10% chance that even a 0% inaccurate actor cannot legislate for - force majeure.

		chance -= MachPhysRandom::randomDouble( 0.0, 10.0 );
	}

	lastFireDiceRoll_ = chance;

	if ( chance < inaccuracy() )
	{
		// we haven't got a spot-on hit, so we have to add some drift
		MATHEX_SCALAR totalDrift = inaccuracy() - chance;

		// don't double-penalise....we have a total index of inaccuracy, and now if
		// the a-drift is high, the b-drift is low and vice versa

		MATHEX_SCALAR aDrift = MachPhysRandom::randomDouble( 0, totalDrift );
		MATHEX_SCALAR bDrift = totalDrift - aDrift;

		if( MachPhysRandom::randomInt( 0, 2 ) == 0 )
			aDrift *= -1;
		if( MachPhysRandom::randomInt( 0, 2 ) == 0 )
			bDrift *= -1;

		fireData.missed = true;
		fireData.firstDrift = aDrift;
		fireData.secondDrift = bDrift;
	}

	return fireData;
}

void MachLogCanAttack::improveAim()
{
	CB_MachLogCanAttack_DEPIMPL();
	lastFireTime_ = SimManager::instance().currentTime();
}


void MachLogCanAttack::setPhysCanAttack( MachPhysCanAttack* pPhysCanAttack )
{
	CB_MachLogCanAttack_DEPIMPL();
    pPhysCanAttack_ = pPhysCanAttack;
}

MachPhysCanAttack& MachLogCanAttack::physCanAttack()
{
	CB_MachLogCanAttack_DEPIMPL();
    return *pPhysCanAttack_;
}

const MachPhysCanAttack& MachLogCanAttack::physCanAttack() const
{
	CB_MachLogCanAttack_DEPIMPL();
    return *pPhysCanAttack_;
}


/////////////////////////////////////////////////// persistence ////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogCanAttack& actor )
{
	const W4dObserver& base1 = actor;

	ostr << base1;

	ostr << actor.pImpl_;
}

void perRead( PerIstream& istr, MachLogCanAttack& actor )
{
	W4dObserver& base1 = actor;

	istr >> base1;

	istr >> actor.pImpl_;

	if( actor.currentlyAttached() and actor.hasCurrentTarget() )
		actor.currentTarget().attach( &actor );
}

MachLogCanAttack::MachLogCanAttack( PerConstructor )
{
}

ostream& operator <<( ostream& o, MachLogCanAttack::WeaponDisposition disp )
{
    switch( disp )
    {
        case MachLogCanAttack:: NOT_IN_DISTANCE_RANGE:
            o << "NOT_IN_DISTANCE_RANGE";
            break;
        case MachLogCanAttack:: NOT_IN_HORIZONTAL_ANGLE_RANGE:
            o << "NOT_IN_HORIZONTAL_ANGLE_RANGE";
            break;
        case MachLogCanAttack:: NOT_IN_VERTICAL_ANGLE_RANGE:
            o << "NOT_IN_VERTICAL_ANGLE_RANGE";
            break;
        case MachLogCanAttack:: BEHIND_COVER:
            o << "BEHIND_COVER";
			break;
		case MachLogCanAttack:: BEHIND_FRIENDLY_COVER:
            o << "BEHIND_FRIENDLY_COVER";
            break;
        case MachLogCanAttack:: IN_RANGE:
            o << "IN_RANGE";
            break;

  		DEFAULT_ASSERT_BAD_CASE( disp );
    }

    return o;
}


bool MachLogCanAttack::canTurnToAttackAndNoCover( MachActor* pTargetActor )
{
	return ( canTurnToAttack( *pTargetActor ) and IHaveClearWeaponPathToHim( pTargetActor ) ) ;
}

bool MachLogCanAttack::IHaveClearWeaponPathToHim( MachActor* pTargetActor ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	return IHaveClearWeaponPathToHim( pMe_, pTargetActor );
}

// static
bool MachLogCanAttack::IHaveClearWeaponPathToHim( MachActor* pSubjectActor, MachActor* pTargetActor )
{
	// actually obtains a LOCAL transform wrt the "me" actor
	const W4dEntity& meEntity = pSubjectActor->physObject();
	MexPoint3d myPosition = ( meEntity.isComposite() ? meEntity.asComposite().compositeBoundingVolume().centroid()
													 : meEntity.boundingVolume().centroid() );
	// ....now convert this into a genuine global
	pSubjectActor->globalTransform().transform( &myPosition );

	// actually obtains a LOCAL transform wrt the target actor

	const W4dEntity& targetEntity = pTargetActor->physObject();
	MexPoint3d globalAimPoint = ( targetEntity.isComposite() ? targetEntity.asComposite().compositeBoundingVolume().centroid()
															 : targetEntity.boundingVolume().centroid() );

	// ....now convert this into a genuine global
	pTargetActor->globalTransform().transform( &globalAimPoint );

	MexLine3d projectilePath( myPosition, globalAimPoint );
   	MachLogCanAttack::WeaponDisposition disposition = testActorBehindCover( pSubjectActor, pTargetActor, projectilePath, MachLogCanAttack::SAME_RACE_OK );

	return( disposition == IN_RANGE );
}

const MATHEX_SCALAR MachLogCanAttack::improveAimInterval()
{
	static MATHEX_SCALAR result = 3.5;
	return result;
}


bool MachLogCanAttack::currentlyAttached() const
{
	CB_DEPIMPL( bool, currentlyAttached_ );
	return currentlyAttached_;
}

bool MachLogCanAttack::acceptableAggressiveTargetForHigherDefconsMachine( MachActor* pTarget, MATHEX_SCALAR sqrDistanceToTarget ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	PRE( pMe_->objectIsMachine() );
	//PRE( pMe_->asMachine().defCon() != MachLog::DEFCON_LOW ); //TODO fails after load game

	bool acceptableTarget = false;

	bool acceptableRange = false;

	// are either or both of us in the other's weapons range?
	// Note that if the enemy is not in my range, but I am in his, yet the distance between us is

	if( withinSqrMaximumWeaponRange( sqrDistanceToTarget ) )
	{
		// He's in my range - I don't even have to move. No problems, let's shoot him.
		acceptableRange = true;
	}
	// if I'm on defcon normal, will move to attack him if he currently threatens me.
	else if( pMe_->asMachine().virtualDefCon() == MachLog::DEFCON_NORMAL
			 and pMe_->hasThreats()
			 and pMe_->hasThisActorAsAThreat( pTarget->id() )
			 and pTarget->asCanAttack().withinSqrMaximumWeaponRange( sqrDistanceToTarget ) )
	{
		acceptableRange = true;
	}

	if( acceptableRange )
	{
		// if so, is there an obstruction between us?
		if( IHaveClearWeaponPathToHim( pTarget ) )
			acceptableTarget = true;
	}

	return acceptableTarget;
}

bool MachLogCanAttack::willCheckForTargets() const
{
	CB_MachLogCanAttack_DEPIMPL();

	int doCheckDiceRoll = MachPhysRandom::randomInt(0, 100 );

	return doCheckDiceRoll < alertness_
		   and allowedToCheckForNewTarget();
}

void MachLogCanAttack::diminishAlertnessAndInaccuracy()
{
	CB_MachLogCanAttack_DEPIMPL();

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	PhysRelativeTime timeSinceLastDiminish = timeNow - lastAlertnessDiminishTime_;

	int reduceAlertnessBy = (int)( timeSinceLastDiminish * 4.0 );

	if( reduceAlertnessBy > 0 )
	{
		lastAlertnessDiminishTime_ = timeNow;

		alertness_ -= reduceAlertnessBy;		// arbitrary for now. Should really be passed relative time parameter and decrease accordingly.

		int hwLevelMinimum = minimumAlertnessForActor();


		if( alertness_ < hwLevelMinimum )
			alertness_ = hwLevelMinimum;
	}

	// this may be a temporary fudge.
	decreaseCurrentInaccuracy( timeSinceLastDiminish / 4.0 );
}

void MachLogCanAttack::setMinimumAlertness( int minimumNewLevel )
{
	CB_MachLogCanAttack_DEPIMPL();

	if( alertness_ < minimumNewLevel )
		alertness_ = minimumNewLevel;
}

void MachLogCanAttack::suppressAttackingUrges( PhysRelativeTime forHowLong )
{
	CB_MachLogCanAttack_DEPIMPL();

	nextTimeAllowedToCheckAndAttack_ = SimManager::instance().currentTime() + forHowLong;
}

void MachLogCanAttack::cancelAnyAttackingUrgeSuppression()
{
	CB_MachLogCanAttack_DEPIMPL();

	nextTimeAllowedToCheckAndAttack_ = SimManager::instance().currentTime();
}

bool MachLogCanAttack::canFireAt( const MachActor& potentialTarget ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	// this method has multiple exit points for efficiency

	// am I a ground-to-air missile emplacement?

	if( 	pMe_->objectType() == MachLog::MISSILE_EMPLACEMENT
		and	pMe_->asMissileEmplacement().subType() == MachPhys::SENTRY )
	{
		bool targetIsFlyingMachine = potentialTarget.objectIsMachine()
							   		 and potentialTarget.asMachine().machineIsGlider();

		// SAMs (level 4) can only attack flying machines, SSMs (level 3) can attack anything but flying machines
		bool isSAMsite = pMe_->asMissileEmplacement().level() == 4;
		return isSAMsite == targetIsFlyingMachine;
	}

	if(	potentialTarget.objectIsMachine() )
	{
		const MachLogMachine& machine = potentialTarget.asMachine();

		if( machine.machineIsGlider() )
		{
			// if my weapon range is really pants, don't consider me eligible to attack flying units
			if( maxWeaponRange_ <= 20.0 )
				return false;

			// am I a Bee? If so, I can't attack other airborne targets
			if( 	pMe_->objectType() == MachLog::ADMINISTRATOR
				and	pMe_->asAdministrator().subType() == MachPhys::BOSS
				and pMe_->asAdministrator().hwLevel() == 2 )
				return false;
		}
		else if( machine.insideBuilding() )
		{
			// don't consider this an eligible target if I'm not able to fit inside this building
			if( pMe_->objectIsMachine() )
			{
				if( pMe_->asMachine().canEnterConstruction( machine.insideWhichBuilding() ) )
					return true;
			}
			else
				return false;
		}
	}

	// otherwise....hey! Everything's fair game.
	return true;
}

int MachLogCanAttack::alertness() const
{
	CB_MachLogCanAttack_DEPIMPL();

	return alertness_;
}

// helper function for SOS call
bool MachLogCanAttack::recruitableToAttack( const MachActor& potentialTarget ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	// pc race machines inside a building don't respond to SOS calls targetting enemies outside that building
	if(	pMe_->objectIsMachine()
		and pMe_->asMachine().insideBuilding()
		and pMe_->race() == MachLogRaces::instance().pcController().race()
		and not( potentialTarget.objectIsMachine()
				 and potentialTarget.asMachine().insideBuilding()
				 and potentialTarget.asMachine().insideWhichBuilding().id() == pMe_->asMachine().insideWhichBuilding().id() ) )
	{
		// the target is outside, and I'm inside. Don't respond to the call.
		return false;
	}
	else
		return canFireAt( potentialTarget );
}

int MachLogCanAttack::minimumAlertnessForActor() const
{
	CB_MachLogCanAttack_DEPIMPL();

	int hwLevelMinimum = 0;

	ASSERT_INFO( pMe_->objectType() );
	ASSERT( pMe_->objectIsMachine() or pMe_->objectIsMissileEmplacement(), "Hey, that's not right." );

	if( pMe_->objectIsMachine() )
		hwLevelMinimum = pMe_->asMachine().hwLevel() * 4;
	else
	{
		// must be missile emplacement
		hwLevelMinimum = 40;
	}

	hwLevelMinimum += 10;

	return hwLevelMinimum;
}

bool MachLogCanAttack::allowedToCheckForNewTarget() const
{
	CB_MachLogCanAttack_DEPIMPL();

	return SimManager::instance().currentTime() >= nextTimeAllowedToCheckAndAttack_;
}

//static
MATHEX_SCALAR MachLogCanAttack::sqrMaximumEngagementDistance()
{
	static MATHEX_SCALAR maxDistance = sqr( 150 );
	return maxDistance;
}

bool MachLogCanAttack::willTestHitByCandidate( const MachActor& hitByActor ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	bool result = false;
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	if( not hasCurrentTarget() )
	{
		// well, yeah, we'll definitely test for this new fella!
		result = true;
		nextHitByTestTime_ = timeNow + 10.0;
	}
	else
	{
		// okay, we already have a target, but there are still some circumstances in which
		// we might consider switching targets to the guy who's just shot my ass up.

		// first up, if the guy who just hit us IS our current target, forget it.
		if( currentTarget().id() == hitByActor.id() )
		{
			result = false;
			nextHitByTestTime_ = timeNow + 10.0;
		}
		else
		{
			// We'll do a check anyway providng sufficient time has passed since our last hitBy-initiated check.

			if( timeNow >= nextHitByTestTime_ )
			{
				nextHitByTestTime_ = timeNow + 10.0;

				result = true;
			}
		}
	}

	return result;
}

MATHEX_SCALAR MachLogCanAttack::distanceBeyondMaximumWeaponRange( const MachActor& targetActor ) const
{
	CB_MachLogCanAttack_DEPIMPL();

	MATHEX_SCALAR distanceToTarget = pMe_->position().euclidianDistance( targetActor.position() );

	return std::max( 0.0, ( distanceToTarget - getMaximumWeaponRange() ) );
}

// static
bool MachLogCanAttack::actorIsDirectThreat( const MachActor& candidateActor )
{
	bool result = candidateActor.objectIsCanAttack()
				  and not( candidateActor.objectType() == MachLog::POD )
				  and not( candidateActor.objectIsMissileEmplacement()
				   			and ( candidateActor.asMissileEmplacement().subType() == MachPhys::ICBM
				   				  or not candidateActor.asMissileEmplacement().isComplete() )  );

	return result;
}

bool MachLogCanAttack::willingToRespondToSOS() const
{
	CB_MachLogCanAttack_DEPIMPL();

	bool result = true;

	if( pMe_->hpRatio() <= 0.6 )
	{
		// Hmmm, I'm a bit duffed up. I might pass on this one....
		int responseChance = pMe_->hpRatio() * 100;		// giving max of 60

		int d100DiceRoll = MachPhysRandom::randomInt( 0, 100 );

		if( d100DiceRoll > responseChance )
		{
			result = false;
		}
	}

	return result;
}

bool MachLogCanAttack::switchFromExistingTargetToThisOne( MachActor* pAlternativeActor, MATHEX_SCALAR sqrDistanceToCurrentTarget, MachActor* pFiredAtMe )
{
	CB_MachLogCanAttack_DEPIMPL();

	PRE( pMe_->objectIsMachine() );

	bool retarget = true;

	MachActor& currentTargetActor = currentTarget();
	MachLogMachine& meAsMachine = pMe_->asMachine();
	MachLogRaces& races = MachLogRaces::instance();
	MachLog::DefCon defCon = meAsMachine.virtualDefCon();

	bool myCurrentTargetIsADirectThreat = actorIsDirectThreat( currentTargetActor );

	//if this is a player race machine, check for normal defcon setting here.
	//if normal then only target if new target is lessequal in hwlevel
	//if idleing etc then actor will always retarget.
	if( races.controller( meAsMachine.race() ).type() != MachLogController::AI_CONTROLLER
		and defCon == MachLog::DEFCON_NORMAL
		and not meAsMachine.isFreeToAttack() )
	{
		//the new target is lessequal than we are so try to blow it up on the way
		if( pAlternativeActor->objectIsMachine() )
		{
			if( meAsMachine.thisActorIsStrongerThanMe( *pAlternativeActor ) )
				retarget = false;
		}
	}

	if( retarget )
	{
		// if the current target is in weapon range, we might not want to retarget
		if(	canTurnToAttack( currentTargetActor ) )
		{
			// provisional setting pending further tests
			retarget = false;

			// automatically retarget if
			// the target is now behind cover , or the target isn't a canattack and a viable
			// canattack alternative target is within range and not behind cover.


			if( not IHaveClearWeaponPathToHim( &currentTargetActor ) )
			{
				// note that this REPLACES pAlternativeActor with the alternative if the current pAlternativeActor is
				// already our current target, and findAlternativeTargetClosestTo then succeeds

				// must ensure alternative target being examined IS a genuine alternative
				if( pAlternativeActor->id() != currentTargetActor.id() )
				{
					if( canTurnToAttackAndNoCover( pAlternativeActor ) )
					{
						retarget = true;
					}
				}
				else if( races.findAlternativeTargetClosestTo( meAsMachine, &pAlternativeActor )
						 and pAlternativeActor->id() != currentTargetActor.id()
						 and canTurnToAttackAndNoCover( pAlternativeActor ) )
				{
					retarget = true;
				}
			}
			// if current target is NOT a can attack and an alternative canAttack CAN be found, switch
			// to that instead so long as it's not behind cover.
			else
			{
				if( not myCurrentTargetIsADirectThreat )
				{
					// note that this REPLACES pAlternativeActor with the alternative if the current pAlternativeActor is
					// already our current target, and findAlternativeTargetClosestTo then succeeds

					// must ensure alternative target being examined IS a genuine alternative
					if( pAlternativeActor->id() != currentTargetActor.id() )
					{
						if( actorIsDirectThreat( *pAlternativeActor ) and canTurnToAttackAndNoCover( pAlternativeActor ) )
							retarget = true;
					}
					else if( races.findAlternativeTargetClosestTo( meAsMachine, &pAlternativeActor )
							and pAlternativeActor->id() != currentTargetActor.id()
							and actorIsDirectThreat( *pAlternativeActor )
							and canTurnToAttackAndNoCover( pAlternativeActor ) )
					{
						retarget = true;
					}
				}
			}
		}
		else
		{
			// current target is not in range - but what about the alternative?
			if( canTurnToAttackAndNoCover( pAlternativeActor ) )
			{
				// don't retarget if this alternative is a non-attack-capable guy,
				// my current target IS attack-capable, and I'm within my current target's weapon range
				if( not actorIsDirectThreat( *pAlternativeActor )
					and myCurrentTargetIsADirectThreat
					and currentTargetActor.asCanAttack().withinSqrMaximumWeaponRange( sqrDistanceToCurrentTarget ) )
				{
					retarget = false;
				}
				else
				{
					retarget = true;
				}
			}
			else
			{
				// Well, the alternative ain't in range. Only retarget if it actually just shot us and our
				// current target is not a threat.
				if( pFiredAtMe
					and not myCurrentTargetIsADirectThreat )
				{
					retarget = true;
				}
				else
				{
					retarget = false;
				}
			}
		}
	}

	return retarget;
}

#pragma GCC diagnostic pop
