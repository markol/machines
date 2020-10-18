/*
 * B E E B O M B . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/beebomb.hpp"


#include "stdlib/string.hpp"

#include "world4d/domain.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"

#include "machlog/beebomex.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"

//PER_DEFINE_PERSISTENT( MachLogBeeBomb );

MachLogBeeBomb::MachLogBeeBomb(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData ),
	pLogRace_( pRace ),
	weaponData_( weaponData )
{
	HAL_STREAM("MLBeeBomb::CTOR\n" );
    TEST_INVARIANT;
}

MachLogBeeBomb::~MachLogBeeBomb()
{
    TEST_INVARIANT;

}

void MachLogBeeBomb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogBeeBomb& t )
{

    o << "MachLogBeeBomb " << (void*)&t << " start" << std::endl;
    o << "MachLogBeeBomb " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogBeeBomb::doBeDestroyed()
{
	// network stuff....

	if( MachLogNetwork::instance().isNetworkGame() )
	{		
		MachLogNetwork::instance().messageBroker().sendCreateSpecialWeaponEffectMessage( position(), MachPhys::BEE_BOMB );
	}	
	
	// and create the physical entity on my own node.

	_NEW( MachLogBeeBombExplosion( pLogRace_, globalDestructionPosition(), weaponData_, pOwner() ) );
}


/*
void perWrite( PerOstream& ostr, const MachLogBeeBomb& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogBeeBomb& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogBeeBomb::MachLogBeeBomb( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}
*/

//forced recompile 3/2/99 CPS

/* End BEEBOMB.CPP *************************************************/
