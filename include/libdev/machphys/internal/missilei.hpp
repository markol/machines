/*
 * M I S S I L E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMissileImpl

    the implementation class of MachPhysMissile
*/

#ifndef _MACHPHYS_MISSILEI_HPP
#define _MACHPHYS_MISSILEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "mathex/vec3.hpp"

class MachPhysVapourTrail;
class W4dLink;

class MachPhysMissileImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( MachPhysMissileImpl );
    PER_FRIEND_READ_WRITE( MachPhysMissileImpl );

private:

    MachPhysMissileImpl( size_t level );
    ~MachPhysMissileImpl();

    MachPhysMissileImpl( const MachPhysMissileImpl& );
    MachPhysMissileImpl& operator =( const MachPhysMissileImpl& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMissileImpl& t );
	friend class  MachPhysMissile;

	//data members
    MachPhysVapourTrail* pVapourTrail_;
    MexVec3         vapourTrailOffset_;
    PhysAbsoluteTime destructionTime_;

	size_t level_;
	W4dLink* pFlame_;
};

PER_DECLARE_PERSISTENT( MachPhysMissileImpl );

#endif

/* End MISSILEI.HPP *************************************************/
