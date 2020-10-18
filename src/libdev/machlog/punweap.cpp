/*
 * P U N W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "world4d/garbcoll.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/punweap.hpp"
#include "machphys/punch.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/planet.hpp"
#include "machlog/punweap.hpp"
#include "machlog/punch.hpp"


PER_DEFINE_PERSISTENT( MachLogPunchWeapon );

MachLogPunchWeapon::MachLogPunchWeapon( MachLogRace* pRace, MachPhysPunchWeapon* pPhysPunchWeapon, MachActor* pOwner )
:	MachLogWeapon( pRace, pPhysPunchWeapon, pOwner ),
	pPhysPunchWeapon_( pPhysPunchWeapon )
{
    TEST_INVARIANT;
}

MachLogPunchWeapon::~MachLogPunchWeapon()
{
    TEST_INVARIANT;
}

void MachLogPunchWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void MachLogPunchWeapon::doFire( const MexPoint3d& /*position*/ )
{
 	MachPhysPunchBlast* pPhysPunchBlast = pPhysPunchWeapon_->createPunchBlast( SimManager::instance().currentTime(), 
																				*( MachLogPlanet::instance().surface() ) );
	
	MachActor* pOwner = &owner();
	pOwner->setBusyPeriod( 2.5 );	// temporary guess....should really get exact length of animation from machphys object

	_NEW( MachLogPunchBlast( &logRace() , pPhysPunchBlast, physWeapon().weaponData(), pOwner ) );
}

//virtual
void MachLogPunchWeapon::doFire( MachActor* pTarget, const MachLogFireData& )
{
	MachLogPunchWeapon::doFire( pTarget->position() );
}

ostream& operator <<( ostream& o, const MachLogPunchWeapon& t )
{

    o << "MachLogPunchWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogPunchWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPunchWeapon& weapon )
{
	const MachLogWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogPunchWeapon& weapon )
{
	MachLogWeapon& base1 = weapon;

	istr >> base1;
}

MachLogPunchWeapon::MachLogPunchWeapon( PerConstructor con )
:	MachLogWeapon( con )
{
}

void MachLogPunchWeapon::doEchoPunch()
{
 	MachPhysPunchBlast* pPhysPunchBlast = pPhysPunchWeapon_->createPunchBlast( SimManager::instance().currentTime(), 
																				*( MachLogPlanet::instance().surface() ) );
	W4dGarbageCollector::instance().add ( pPhysPunchBlast, pPhysPunchBlast->blastEndTime() );
}

void MachLogPunchWeapon::setPhysicalPunchWeapon( MachPhysPunchWeapon* pPunchWeapon )
{
    pPhysPunchWeapon_ = pPunchWeapon;
}
/* End PUNWEAP.CPP *************************************************/
