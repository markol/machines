/*
 * S T I N G L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLightSting

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_STINGL_HPP
#define _MACHPHYS_STINGL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machphys/lineproj.hpp"

class MachPhysLightSting : public MachPhysLinearProjectile
// Canonical form revoked
{
public:
	//public constructor
    MachPhysLightSting( W4dEntity* pParent, const MexTransform3d& localTransform );

    virtual ~MachPhysLightSting();

	static const MachPhysLightSting& exemplar();

    void CLASS_INVARIANT;

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearProjectile

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );
    //////////////////////////////////////////////////////////

    PER_MEMBER_PERSISTENT( MachPhysLightSting );
    PER_FRIEND_READ_WRITE( MachPhysLightSting );

private:
    friend ostream& operator <<( ostream& o, const MachPhysLightSting& t );

    MachPhysLightSting( const MachPhysLightSting& );
    MachPhysLightSting& operator =( const MachPhysLightSting& );


    //Create an impact flash attached tp pParent at localPosition offset at startTime.
    //Returns duration of the flash.
    PhysRelativeTime createImpactSplat( W4dEntity* pParent,
                     const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime );

	friend class MachPhysOtherPersistence;

	//one time constructor
    MachPhysLightSting();
};

PER_DECLARE_PERSISTENT( MachPhysLightSting );

#endif

/* End STINGL.HPP ***************************************************/
