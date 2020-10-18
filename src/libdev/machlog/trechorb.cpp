/*
 * T R E C H O R B . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "world4d/domain.hpp"
#include "sim/manager.hpp"
#include "machphys/treachry.hpp"
#include "machphys/trechorb.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/trechorb.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogTreacheryOrb );

MachLogTreacheryOrb::MachLogTreacheryOrb(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData ),
	race_( pRace->race() ),
	pLogRace_( pRace )
{
	HAL_STREAM("MLTreacheryOrb::CTOR\n" );
    TEST_INVARIANT;
}

MachLogTreacheryOrb::~MachLogTreacheryOrb()
{
    TEST_INVARIANT;

}

void MachLogTreacheryOrb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogTreacheryOrb& t )
{

    o << "MachLogTreacheryOrb " << (void*)&t << " start" << std::endl;
    o << "MachLogTreacheryOrb " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogTreacheryOrb::doBeDestroyed()
{
	ctl_pvector< MachActor >	actors;
	MachLogSpacialManipulation::genericCheckForIntersections( globalDestructionPosition(), 1, &actors );
	HAL_STREAM("MLTreacheryOrb::doBeDestroyed...num of intersections " << actors.size() << std::endl );
	PhysAbsoluteTime now = SimManager::instance().currentTime();
	for( ctl_pvector< MachActor >::iterator i = actors.begin(); i != actors.end() ; ++i )
	{
		if( (*i)->race() != race_ )
		{
			HAL_STREAM(" we have a hit id: " << (*i)->id() << std::endl );
			if( (*i)->objectIsMachine() )
			{
				MachLogMachine& mlm = (*i)->asMachine();
				const MachLogMachine& cmlm = (*i)->asMachine();
				const MachPhysMachine& mpm = cmlm.physMachine();
				MachPhysMachine& ncmpm = _CONST_CAST( MachPhysMachine&, mpm );
				MachPhys::Race oldRace = mlm.race();
				mlm.assignToDifferentRace( *pLogRace_ );
				MachPhysTreacheryWeapon::traitorAnimation( &ncmpm, now, oldRace, race_ );
				MachPhysTreacheryOrb& mpto = _STATIC_CAST( MachPhysTreacheryOrb&, physLinearProjectile() );
				mpto.suckRaceAt( now, mlm.race() );
				if( mlm.objectType() == MachLog::TECHNICIAN )
					MachLogRaces::instance().techniciansTreacheried( race_, MachLogRaces::instance().techniciansTreacheried( race_ ) + 1 );
				break;
			}
		}
	}
}

void perWrite( PerOstream& ostr, const MachLogTreacheryOrb& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogTreacheryOrb& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogTreacheryOrb::MachLogTreacheryOrb( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End TRECHORB.CPP *************************************************/
