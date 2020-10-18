/*
 * M C P E R S 3 C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/mcpers3c.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"

PhysThirdPerson::PhysThirdPerson()
{}

// virtual
PhysThirdPerson::~PhysThirdPerson()
{}

PhysThirdPersonCameraConstraint::PhysThirdPersonCameraConstraint( PhysThirdPerson* pThirdPerson )
:	pThirdPerson_( pThirdPerson )
{
	PRE( pThirdPerson );

    TEST_INVARIANT;
}

PhysThirdPersonCameraConstraint::~PhysThirdPersonCameraConstraint()
{
    TEST_INVARIANT;

	_DELETE( pThirdPerson_ );
}

void PhysThirdPersonCameraConstraint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void PhysThirdPersonCameraConstraint::move( MexTransform3d& transform, PhysMotion& , double )
{
	//Compute camera position using the current global position of the target entity,
	//and the relative transform.
	static MexPoint3d oldPos;
	static MexEulerAngles oldAngle;
	static PhysThirdPerson* oldThirdPerson = NULL;

    if( pThirdPerson_->thirdPersonExists() )
    {
    	pThirdPerson_->getIdealCameraLocation( &transform );

		MexPoint3d pos;
		MexEulerAngles angle;
		transform.position( &pos );
		transform.rotation( &angle );

		if ( pThirdPerson_ != oldThirdPerson )
		{
			oldPos = pos;
			oldAngle = angle;
			oldThirdPerson = pThirdPerson_;
		}
		else
		{
			// Camera moves slowly towards the ideal location.
			MexLine3d line( pos, oldPos );
			MexPoint3d newPos = line.pointAtDistance( line.length() * 0.90 );
			transform.position( newPos );


			MexEulerAngles diffAngle( 	angle.azimuth() - oldAngle.azimuth(),
										angle.elevation() - oldAngle.elevation(),
										angle.roll() - oldAngle.roll() );

			// Make sure diff angles don't go less than or greater than pi.
			if ( diffAngle.azimuth().asScalar() > Mathex::PI )
				diffAngle.azimuth( diffAngle.azimuth().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( diffAngle.azimuth().asScalar() < -Mathex::PI )
				diffAngle.azimuth( diffAngle.azimuth().asScalar() + ( 2.0 * Mathex::PI ) );
			if ( diffAngle.elevation().asScalar() > Mathex::PI )
				diffAngle.elevation( diffAngle.elevation().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( diffAngle.elevation().asScalar() < -Mathex::PI )
				diffAngle.elevation( diffAngle.elevation().asScalar() + ( 2.0 * Mathex::PI ) );
			if ( diffAngle.roll().asScalar() > Mathex::PI )
				diffAngle.roll( diffAngle.roll().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( diffAngle.roll().asScalar() < -Mathex::PI )
				diffAngle.roll( diffAngle.roll().asScalar() + ( 2.0 * Mathex::PI ) );


			MexEulerAngles newAngle( ( diffAngle.azimuth() * 0.10 )		+ oldAngle.azimuth(),
									 ( diffAngle.elevation() * 0.10 )	+ oldAngle.elevation(),
									 ( diffAngle.roll() * 0.10 ) 		+ oldAngle.roll() );

			// Make sure new angles don't go less than or greater than pi.
			if ( newAngle.azimuth().asScalar() > Mathex::PI )
			    newAngle.azimuth( newAngle.azimuth().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( newAngle.azimuth().asScalar() < -Mathex::PI )
				newAngle.azimuth( newAngle.azimuth().asScalar() + ( 2.0 * Mathex::PI ) );
		   	if ( newAngle.elevation().asScalar() > Mathex::PI )
			    newAngle.elevation( newAngle.elevation().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( newAngle.elevation().asScalar() < -Mathex::PI )
				newAngle.elevation( newAngle.elevation().asScalar() + ( 2.0 * Mathex::PI ) );
			if ( newAngle.roll().asScalar() > Mathex::PI )
			    newAngle.roll( newAngle.roll().asScalar() - ( 2.0 * Mathex::PI ) );
			else if ( newAngle.roll().asScalar() < -Mathex::PI )
				newAngle.roll( newAngle.roll().asScalar() + ( 2.0 * Mathex::PI ) );

			transform.rotation( newAngle );

			oldPos = newPos;
			oldAngle = newAngle;
		}
    }
}

// virtual
bool PhysThirdPersonCameraConstraint::snapTo( MexPoint3d* )
{
	return false;
}

// virtual
bool PhysThirdPersonCameraConstraint::snapTo( MexTransform3d* )
{
	return false;
}

ostream& operator <<( ostream& o, const PhysThirdPersonCameraConstraint& t )
{

    o << "PhysThirdPersonCameraConstraint " << (void*)&t << " start" << std::endl;
    o << "PhysThirdPersonCameraConstraint " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MCPERS3C.CPP *************************************************/
