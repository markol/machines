/*
 * W E A P O N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "sim/manager.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/random.hpp"
#include "machlog/actor.hpp"
#include "machlog/weapon.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"

#include "machlog/internal/firedata.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogWeapon );

MachLogWeapon::MachLogWeapon( MachLogRace* pLogRace, MachPhysWeapon* pPhysWeapon, MachActor* pOwner )
:	pPhysWeapon_( pPhysWeapon ),
	lastFireTime_( SimManager::instance().currentTime() ),
	pLogRace_( pLogRace ),
	pOwner_( pOwner ),
	pTarget_( NULL ),
	currentlyAttached_( false )
{

    TEST_INVARIANT;
}

MachLogWeapon::~MachLogWeapon()
{
    TEST_INVARIANT;

}

PhysRelativeTime MachLogWeapon::fire( MachActor* pTarget, const MachLogFireData& fireData )
{
  	PhysRelativeTime result = 0.0;	// meaningless default value

	//Is this weapon ready to fire again yet?
	if( percentageRecharge() < 100 )
	{
		//not charged - return interval until full recharge.
		result = lastFireTime_ + pPhysWeapon_->weaponData().reloadTime() - SimManager::instance().currentTime();
	}
	else
	{
		// weapon is charged and ready to fire
		//fire gun and retrun animation time.
		setChargeToZero();
		dt("",true );

		doFire( pTarget, fireData );
		dt("MLWeapon::doFire",false );
		dt("",true );

		int numberInBurst = (int)( fireData.burstSeed *(float)( physWeapon().weaponData().nRoundsPerBurst() ) ) + 1;
		result = pPhysWeapon_->fire( lastFireTime_, numberInBurst );

		MachLogRaces::instance().firedAt( pOwner_->race(), pTarget->race(), true );

		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendFireWeaponAnimationMessage( pOwner_->id(), mounting(), numberInBurst );

		dt("MLWeapon::physWeapon->fire",false);
	}

	return result;
}

PhysRelativeTime MachLogWeapon::fire( MachActor* pTarget )
{
	//Is this weapon ready to fire again yet?
	//if not return gap to recharge.
	if( percentageRecharge() < 100 )
	{
		return lastFireTime_ + pPhysWeapon_->weaponData().reloadTime() - SimManager::instance().currentTime();
	}
	//fire gun and retrun animation time.
	lastFireTime_ = SimManager::instance().currentTime();
	dt("",true );

	MachLogFireData dummyData;
	dummyData.burstSeed = DUMMY_SEED;

	doFire( pTarget, dummyData );
	dt("MLWeapon::doFire",false );
	dt("",true );
	PhysRelativeTime result = pPhysWeapon_->fire( lastFireTime_, 1 );

	if( MachLogNetwork::instance().isNetworkGame() )
		MachLogNetwork::instance().messageBroker().sendFireWeaponAnimationMessage( pOwner_->id(), mounting(), 1 );

	dt("MLWeapon::physWeapon->fire",false);
	return result;
}



PhysRelativeTime MachLogWeapon::fire( const MexPoint3d& position )
{
	//Is this weapon ready to fire again yet?
	//if not return gap to recharge.
	if( percentageRecharge() < 100 )
	{
		return lastFireTime_ + pPhysWeapon_->weaponData().reloadTime() - SimManager::instance().currentTime();
	}
	//fire gun and retrun animation time.
	lastFireTime_ = SimManager::instance().currentTime();
	dt("",true );
	doFire( position );
	dt("MLWeapon::doFire",false );
	dt("",true );
	PhysRelativeTime result = pPhysWeapon_->fire( lastFireTime_, 1 );

	if( MachLogNetwork::instance().isNetworkGame() )
		MachLogNetwork::instance().messageBroker().sendFireWeaponAnimationMessage( pOwner_->id(), mounting(), 1 );

	dt("MLWeapon::physWeapon->fire",false);
	return result;
}

MexVec3 MachLogWeapon::getDirection( const MexPoint3d& start, MachActor* pTarget )
{
	MexPoint3d targetPosition( pTarget->position() );
	if( targetPosition.z() < 1 )
		targetPosition.z( 1 );
	MexVec3 direction( start, targetPosition, start.euclidianDistance( pTarget->position() ) );
	//I want to fire with a flat trajectory.
//	direction.z( 0 );
	MATHEX_SCALAR x = direction.x();
	MATHEX_SCALAR y = direction.y();
	const MATHEX_SCALAR PERCENTAGE_DRIFT = 0.05;
	MATHEX_SCALAR yDelta = MachPhysRandom::randomDouble( -1, 1 ) * y * PERCENTAGE_DRIFT;
	MATHEX_SCALAR xDelta = MachPhysRandom::randomDouble( -1, 1 ) * x * PERCENTAGE_DRIFT;
	direction = MexVec3( x + xDelta , y + yDelta, direction.z() );
	direction.makeUnitVector();
	return direction;
}

bool MachLogWeapon::recharged() const
{
	return percentageRecharge() >= 100;
}

int	MachLogWeapon::percentageRecharge() const
{
	int result = (int)(
		100 * (
			( SimManager::instance().currentTime() - lastFireTime_ ) / pPhysWeapon_->weaponData().reloadTime()
			)
		);
	if( result > 100 )
		result = 100;

	return result;
}

void MachLogWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachLogRace&	MachLogWeapon::logRace()
{
	return *pLogRace_;
}

MachPhysWeapon& MachLogWeapon::physWeapon()
{
	ASSERT( pPhysWeapon_, "Unexpected NULL value for pPhysWeapon_" );
	return *pPhysWeapon_;
}

const MachPhysWeapon& MachLogWeapon::physWeapon() const
{
	ASSERT( pPhysWeapon_, "Unexpected NULL value for pPhysWeapon_" );
	return *pPhysWeapon_;
}

MATHEX_SCALAR MachLogWeapon::range() const
{
	return physWeapon().weaponData().range();
}

const MachActor& MachLogWeapon::owner() const
{
	return *pOwner_;
}

MachActor& MachLogWeapon::owner()
{
	return *pOwner_;
}

ostream& operator <<( ostream& o, const MachLogWeapon& t )
{

    o << "MachLogWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachLogWeapon::beNotified( W4dSubject* ,
	                         W4dSubject::NotificationEvent , int )
{
	return true;
}

//virtual
void MachLogWeapon::domainDeleted( W4dDomain* )
{
}

//Attaching methods
bool MachLogWeapon::currentlyAttached()
{
	return currentlyAttached_;
}

void MachLogWeapon::currentlyAttached( bool newAttachement )
{
	// Note - we have chosen "attachement" to appear French and sophisticated.

	currentlyAttached_ = newAttachement;

    //If we don't do this we get a crash writing the op, because pTarget is left dangling.
    if( not newAttachement )
        pTarget_ = NULL;
}

MachActor& MachLogWeapon::target()
{
	PRE( currentlyAttached_ );
	return *pTarget_;
}

void MachLogWeapon::target( MachActor* pTarget )
{
	pTarget_ = pTarget;
}
/* End WEAPON.CPP ***************************************************/


void MachLogWeapon::dt( const char* text, bool start )
{
	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( start )
		startTime = Phys::time();
	else
	{
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_MISC, pOwner_->id() << "," << pPhysWeapon_->type() << "," << text << "," << endTime - startTime << std::endl );
	}
}


MachPhys::Mounting MachLogWeapon::mounting() const
{
	return pPhysWeapon_->mounting();
}

MachPhys::WeaponType MachLogWeapon::type() const
{
	return pPhysWeapon_->type();
}


void MachLogWeapon::fireWithoutEcho( int numberInBurst )
{
	pPhysWeapon_->fire( SimManager::instance().currentTime(), numberInBurst );
}

void MachLogWeapon::setPhysicalWeapon( MachPhysWeapon* pPhysWeapon )
{
	pPhysWeapon_ = pPhysWeapon;
}

void perWrite( PerOstream& ostr, const MachLogWeapon& weapon )
{
	const W4dObserver& base1 = weapon;

	ostr << base1;
	ostr << weapon.lastFireTime_;
	MachPhys::Race race = weapon.pLogRace_->race();
	PER_WRITE_RAW_OBJECT( ostr, race );
	ostr << weapon.pOwner_;
	ostr << weapon.currentlyAttached_;
	ostr << weapon.pTarget_;

}

void perRead( PerIstream& istr, MachLogWeapon& weapon )
{
	W4dObserver& base1 = weapon;

	istr >> base1;
	istr >> weapon.lastFireTime_;
	MachPhys::Race race;
	uint raceInt; // TODO: mismatch? is it stored as uchar and read as uint32?
	PER_READ_RAW_OBJECT( istr, (uint&)raceInt );
	race = _STATIC_CAST(MachPhys::Race, raceInt);
	istr >> weapon.pOwner_;
	istr >> weapon.currentlyAttached_;
	istr >> weapon.pTarget_;
	weapon.pLogRace_ = &MachLogRaces::instance().race( race );
	if( weapon.currentlyAttached_ )
		weapon.pTarget_->attach( &weapon );


}

MachLogWeapon::MachLogWeapon( PerConstructor )
{
}

//virtual
void MachLogWeapon::doFire( const MexPoint3d& /*position*/ )
{
	//intentionally left blank
	ASSERT( false, "This default method should never have been called by anything." );
}

PhysAbsoluteTime MachLogWeapon::lastFireTime() const
{
	return lastFireTime_;
}

void MachLogWeapon::setChargeToZero()
{
	lastFireTime_ = SimManager::instance().currentTime();
}

void MachLogWeapon::setChargeToMaximum()
{
	lastFireTime_ = SimManager::instance().currentTime() - pPhysWeapon_->weaponData().reloadTime();
}
