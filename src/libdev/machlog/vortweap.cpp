/*
 * V O R T W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/actor.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/machine.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/vortweap.hpp"
#include "machlog/vortbomb.hpp"


PER_DEFINE_PERSISTENT( MachLogVortexWeapon );

MachLogVortexWeapon::MachLogVortexWeapon( MachLogRace* pRace, MachPhysWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogWeapon( pRace, pPhysWeapon, pOwner )
{
	HAL_STREAM("MLVortexWeapon::CTOR owner.id " << pOwner->id() << std::endl );
    TEST_INVARIANT;
}

MachLogVortexWeapon::~MachLogVortexWeapon()
{
	HAL_STREAM("MLVortexWeapon::DTOR race " << std::endl );
    TEST_INVARIANT;

}

void MachLogVortexWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void MachLogVortexWeapon::doFire( const MexPoint3d& position )
{
	//create pulse blobs (the logical entites here).
	PhysRelativeTime timeOffset = 0;
	int launchOffset = 0;
	_NEW( MachLogVortexBomb( &logRace() , position, physWeapon().weaponData(), &owner() ) );
	MachLogRaces::instance().vortexHasBeenFired( owner().race(), true );

	owner().setBusyPeriod( 9.0 );
}

//virtual
void MachLogVortexWeapon::doFire( MachActor* pTarget, const MachLogFireData& )
{	 
	MexPoint3d vortexPoint = pTarget->position();

	const MachActor& ownerActor = owner();

	if( not( ownerActor.objectIsMachine() and ownerActor.asMachine().insideBuilding() ) )
	{
		MexPoint3d estimated1SecondTargetOffset = pTarget->predictedTargetOffset( 1.3 );
		MexTransform3d trans = pTarget->globalTransform();
		trans.translate( estimated1SecondTargetOffset );

		MexPoint2d idealPoint = trans.position();

		// try to ensure that the vortex detonates at least far enough away to not expand and reach us.

		MATHEX_SCALAR safeDistance = physWeapon().weaponData().extras()[0] + 6.0;

		MexPoint2d myPosition = owner().position();

		if( myPosition.sqrEuclidianDistance( idealPoint ) <= sqr( safeDistance ) )
		{
			MexVec2 meToTargetVector ( myPosition, idealPoint );
			meToTargetVector.makeUnitVector();
			meToTargetVector *= safeDistance;
			idealPoint = myPosition;
			idealPoint += meToTargetVector;
		}

		// ensure point drifted to is safely within world's boundaries
		vortexPoint = MachLogPlanet::instance().safeWorldCoordinate( idealPoint );

		// finally, place the epicentre right on the ground (or just very slightly above it).
		vortexPoint = MachLogSpacialManipulation::heightAdjustedLocation( vortexPoint );	
	}

	MachLogVortexWeapon::doFire( vortexPoint );
}

ostream& operator <<( ostream& o, const MachLogVortexWeapon& t )
{

    o << "MachLogVortexWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogVortexWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogVortexWeapon& weapon )
{
	const MachLogWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogVortexWeapon& weapon )
{
	MachLogWeapon& base1 = weapon;

	istr >> base1;
}

MachLogVortexWeapon::MachLogVortexWeapon( PerConstructor con )
:	MachLogWeapon( con )
{
}

/* End PULSEWEP.CPP *************************************************/
