/*
 * B O L T E R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/abox3d.hpp"

#include "world4d/domain.hpp"

#include "machphys/plansurf.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"

#include "machlog/actor.hpp"
#include "machlog/bolter.hpp"
#include "machlog/canattac.hpp"
#include "machlog/lineproj.hpp"
#include "machlog/machine.hpp"
#include "machlog/planet.hpp"

#include "machlog/internal/firedata.hpp"

PER_DEFINE_PERSISTENT( MachLogBolterWeapon );

MachLogBolterWeapon::MachLogBolterWeapon( MachLogRace* pRace, MachPhysWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogBolterWeapon::~MachLogBolterWeapon()
{
    TEST_INVARIANT;

}

void MachLogBolterWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void MachLogBolterWeapon::doFire( MachActor* pTarget, const MachLogFireData& fireData )
{
	ASSERT( fireData.burstSeed != MachLogWeapon::DUMMY_SEED, "MachLogBolterWeapon::doFire has received a dummy fireData struct as parameter." );

	//create Bolter blobs (the logical entites here).


	MexPoint3d targetPosition = pTarget->physObject().isComposite() ? 
		pTarget->physObject().asComposite().compositeBoundingVolume().centroid() : 
		pTarget->physObject().boundingVolume().centroid();

	pTarget->globalTransform().transform( &targetPosition );

	MexLine3d line( owner().position(), targetPosition );
	
	MexPoint3d burstPoint = line.end2();
	
	// is this a completely accurate shot?
		
	if ( fireData.missed )
	{
		// using firstDrift as y-drift and secondDrift as z-drift
		
		MexVec3 relativeDrift( (fireData.firstDrift/10.0), (fireData.secondDrift/10.0), 0 );
		
		burstPoint += relativeDrift;	
		
		// finally, push the burstPoint 5m away in the direction of the target
		// (more likely to hit something behind the target than in front of it )
		
		MexVec3 pushVector ( owner().position(), targetPosition );
		pushVector.makeUnitVector();
		pushVector *= 5;
		burstPoint += pushVector;
		
	}

	// ensure point drifted to is safely within world's boundaries
	burstPoint = MachLogPlanet::instance().safeWorldCoordinate( burstPoint );
	
	int numberInBurst;
		
	if( physWeapon().weaponData().fixedBurst() )
		numberInBurst = physWeapon().weaponData().nRoundsPerBurst();				 	
	else
		numberInBurst = (int)( fireData.burstSeed *(float)( physWeapon().weaponData().nRoundsPerBurst() ) ) + 1;

	
	for( int i = 0; i < numberInBurst; ++i )
	{
		MachLogLinearProjectile::genericCheckForDamage( burstPoint, 1,
														MachLogLinearProjectile::CONSTANT_DAMAGE, 
														physWeapon().weaponData().damagePoints(), 
														physWeapon().type(),
														&line,
														&owner() );
	}	
}

ostream& operator <<( ostream& o, const MachLogBolterWeapon& t )
{

    o << "MachLogBolterWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogBolterWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogBolterWeapon& weapon )
{
	const MachLogWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogBolterWeapon& weapon )
{
	MachLogWeapon& base1 = weapon;

	istr >> base1;
}

MachLogBolterWeapon::MachLogBolterWeapon( PerConstructor con )
:	MachLogWeapon( con )
{
}

//virtual
void MachLogBolterWeapon::doFire( const MexPoint3d& targetPoint )
{
	MexLine3d line( owner().position(), targetPoint );

	for( int i = 0; i < physWeapon().weaponData().nRoundsPerBurst(); ++i )
	{
		MachLogLinearProjectile::genericCheckForDamage( line.end2(), 1,
														MachLogLinearProjectile::CONSTANT_DAMAGE, 
														physWeapon().weaponData().damagePoints(), 
														physWeapon().type(),
														&line );
	}
}


/* End BolterWEP.CPP *************************************************/
