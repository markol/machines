/*
 * STINGL . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLightSting

*/

#ifndef _MACHLOG_STINGL_HPP
#define _MACHLOG_STINGL_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysLightSting;
class MachPhysLinearProjectile;
class MachActor;

class MachLogLightSting : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogLightSting(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogLightSting( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogLightSting& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLightSting );
	PER_FRIEND_READ_WRITE( MachLogLightSting );

private:
	void doBeDestroyed();

    MachLogLightSting( const MachLogLightSting& );
    MachLogLightSting& operator =( const MachLogLightSting& );
    bool operator ==( const MachLogLightSting& );

};

PER_DECLARE_PERSISTENT( MachLogLightSting );

#endif

/* End STINGL.HPP *************************************************/
