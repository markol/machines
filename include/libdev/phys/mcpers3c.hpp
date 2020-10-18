/*
 * M C P E R S 3 C . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysThirdPersonCameraConstraint

    A brief description of the class should go in here
*/

#ifndef _PHYS_MCPERS3C_HPP
#define _PHYS_MCPERS3C_HPP

#include "base/base.hpp"
#include "phys/mcmotion.hpp"

class PhysThirdPerson
// Canonical form revoked
{
public:
	PhysThirdPerson();
	virtual ~PhysThirdPerson();

	// When following different things it is possible that they will come and go
	// out of existance (e.g. get destroyed ). 
	virtual bool thirdPersonExists() const = 0;

	virtual void getIdealCameraLocation( MexTransform3d* ) = 0;

private:
	// Operations deliberately revoked
    PhysThirdPerson( const PhysThirdPerson& );
    PhysThirdPerson& operator =( const PhysThirdPerson& );
    bool operator ==( const PhysThirdPerson& );
};

class PhysThirdPersonCameraConstraint : public PhysMotionConstraint
// Canonical form revoked
{
public:
    PhysThirdPersonCameraConstraint( PhysThirdPerson* );
	//PRE( pThirdPerson );

    virtual ~PhysThirdPersonCameraConstraint( void );

    virtual void move( MexTransform3d&, PhysMotion&, double elapsedTime );

	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* location );

    void CLASS_INVARIANT;

private:
	// Operations deliberately revoked
    PhysThirdPersonCameraConstraint( const PhysThirdPersonCameraConstraint& );
    PhysThirdPersonCameraConstraint& operator =( const PhysThirdPersonCameraConstraint& );
    bool operator ==( const PhysThirdPersonCameraConstraint& );

	friend ostream& operator <<( ostream& o, const PhysThirdPersonCameraConstraint& t );

    //Data members
	PhysThirdPerson* pThirdPerson_; //The thing being followed	
};


#endif

/* End MCPERS3C.HPP *************************************************/
