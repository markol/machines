/*
 * S T I N G M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMetalSting

    the projectile which the WASP fires
*/

#ifndef _MACHPHYS_STINGM_HPP
#define _MACHPHYS_STINGM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machphys/trailprj.hpp"

class MachPhysMetalSting : public MachPhysTrailedProjectile
// Canonical form revoked
{
public:
    MachPhysMetalSting( W4dEntity* pParent, const MexTransform3d& localTransform );
    virtual ~MachPhysMetalSting( void );

	static const MachPhysMetalSting& exemplar();

    void CLASS_INVARIANT;

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearProjectile

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );
    //////////////////////////////////////////////////////////

    PER_MEMBER_PERSISTENT( MachPhysMetalSting );
    PER_FRIEND_READ_WRITE( MachPhysMetalSting );

private:
    friend ostream& operator <<( ostream& o, const MachPhysMetalSting& t );

    MachPhysMetalSting( const MachPhysMetalSting& );
    MachPhysMetalSting& operator =( const MachPhysMetalSting& );

	friend class MachPhysOtherPersistence;

	//one time constructor
    MachPhysMetalSting( void );
};

PER_DECLARE_PERSISTENT( MachPhysMetalSting );

#endif

/* End STINGM.HPP ***************************************************/
