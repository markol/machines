/*
 * B E E B O M B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBeeBomb

    the model and animations of the projectile which the weapon	 bee fires.
*/

#ifndef _MACHPHYS_BEEBOMB_HPP
#define _MACHPHYS_BEEBOMB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/trailprj.hpp"

class W4dLink;
class MexTransform3d;
class MachPhysWeaponData;

class MachPhysBeeBomb: public MachPhysTrailedProjectile
// Canonical form revoked
{
public:

	//public constructor
    MachPhysBeeBomb( W4dEntity* pParent, const MexTransform3d& localTransform );
    virtual ~MachPhysBeeBomb( void );

    //Return an exemplar bomb - ensures the mesh is loaded
	static const MachPhysBeeBomb& exemplar();

    void CLASS_INVARIANT;

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
	virtual	PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time, MachPhys::StrikeType strikeType );

    PER_MEMBER_PERSISTENT( MachPhysBeeBomb );

private:
    friend ostream& operator <<( ostream& o, const MachPhysBeeBomb& t );

    MachPhysBeeBomb( const MachPhysBeeBomb& );
    MachPhysBeeBomb& operator =( const MachPhysBeeBomb& );

    //Create an impact flash attached tp pParent at localPosition offset at startTime.
    //Returns duration of the flash.
    PhysRelativeTime createImpactBlast( W4dEntity* pParent,
                     const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime );

	friend class MachPhysOtherPersistence;

	//one time constructor
    MachPhysBeeBomb( void );

    //Implement a bomb explosion at time
    void createBombExplosion( const PhysAbsoluteTime& time );
};

PER_DECLARE_PERSISTENT( MachPhysBeeBomb );
PER_READ_WRITE( MachPhysBeeBomb );

#endif

/* End BEEBOMB.HPP **************************************************/
