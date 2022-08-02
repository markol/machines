/*
 * L I N E P R J I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLinearProjectileImpl

    the implememtation class of  MachPhysLinearProjectile
*/

#ifndef _MACHPHYS_LINEPRJI_HPP
#define _MACHPHYS_LINEPRJI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/line3d.hpp"
#include "phys/phys.hpp"

#include "machphys/lineproj.hpp"

class MachPhysPlanetSurface;

class MachPhysLinearProjectileImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( MachPhysLinearProjectileImpl );
    PER_FRIEND_READ_WRITE( MachPhysLinearProjectileImpl );

private:
    MachPhysLinearProjectileImpl();
    ~MachPhysLinearProjectileImpl();

    MachPhysLinearProjectileImpl( const MachPhysLinearProjectileImpl& );
    MachPhysLinearProjectileImpl& operator =( const MachPhysLinearProjectileImpl& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLinearProjectileImpl& t );
	friend class MachPhysLinearProjectile;

    PhysAbsoluteTime flightStartTime_; //Time starts flying
    PhysRelativeTime flightDuration_; //Duration of complete path
    MexLine3d flightPath_; //start to end point of original flight path
    MachPhysPlanetSurface* pPlanetSurface_; //Surface if set
	bool	lightsSuppressed_;
    MachPhysMachine::ControlType launchControlType_;

};

PER_DECLARE_PERSISTENT( MachPhysLinearProjectileImpl );

#endif

/* End LINEPRJI.HPP *************************************************/
