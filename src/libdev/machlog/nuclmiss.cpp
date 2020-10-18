/*
 * N U C L M I S S . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/nuclmiss.hpp"

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"

#include "machlog/cntrl_pc.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/nuclbomb.hpp"
#include "machlog/vmman.hpp"


#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/stats.hpp"
#include "machlog/actor.hpp"
//#include "machlog/actor.hpp"
//#include "machlog/spacial.hpp"

#include "sim/manager.hpp"

#include"world4d/garbcoll.hpp"

MachLogNuclearMissile::MachLogNuclearMissile(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData, DO_NOT_DETECT_INTERSECTIONS ),
	weaponData_( weaponData ),
	pLogRace_( pRace )
{
	// send voicemail if it wasn't me initiating this launch
	if( MachLogRaces::instance().pcController().race() != pOwner->race() )
		MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_NUKE_LAUNCH, MachLogRaces::instance().pcController().race() );
	
    TEST_INVARIANT;
}

MachLogNuclearMissile::~MachLogNuclearMissile()
{
    TEST_INVARIANT;

}

void MachLogNuclearMissile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNuclearMissile& t )
{

    o << "MachLogNuclearMissile " << (void*)&t << " start" << std::endl;
    o << "MachLogNuclearMissile " << (void*)&t << " end" << std::endl;

    return o;
}



void MachLogNuclearMissile::doBeDestroyed()
{

	// network stuff....

	if( MachLogNetwork::instance().isNetworkGame() )
	{		
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( position(), MachPhys::NUCLEAR_MISSILE );
	}	
	
	// and create the physical entity on my own node.

	_NEW( MachLogNuclearBomb( pLogRace_, globalDestructionPosition(), weaponData_, pOwner() ) );	

/*		
	MachLogNuclearBomb* pPhysNuclearBomb = NULL;
	MachLogNuclearMissile::pNewPhysNuclearBomb( globalDestructionPosition(), &pPhysNuclearBomb );
	
	ASSERT( pPhysNuclearBomb, "We have a NULL pointer to the pPhysNuclearBomb." );
	
	PhysRelativeTime relTime = 12; // a figure I've chosen arbitrarily that seems to be about long enough.
	pPhysNuclearBomb->startExplosion( SimManager::instance().currentTime(), MachLogPlanet::instance().surface() );
	W4dGarbageCollector::instance().add( pPhysNuclearBomb, relTime + SimManager::instance().currentTime() );
		
	checkForDamage( MachLogRaces::instance().stats().nuclearMissileBlastRange(), MachLogLinearProjectile::LINEAR_DAMAGE, MachPhys::NUCLEAR_MISSILE );
	*/
	
}

//forced recompile 3/2/99 CPS

/* End NUCLMISS.CPP *************************************************/
