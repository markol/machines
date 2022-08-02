/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysVortexBomb

    Models a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_VORTBOMB_HPP
#define _MACHPHYS_VORTBOMB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysBlackSphere;
class MachPhysWhiteSphere;
class MachPhysVortexSphere;

class MachPhysVortexBomb : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysVortexBomb(W4dEntity* pParent, const MexTransform3d& localTransform);

    //dtor
    virtual ~MachPhysVortexBomb();

    //Begin the explosion animation at startTime.
    //Returns the duration of the explosion.
    PhysRelativeTime startExplosion( const PhysAbsoluteTime& startTime );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

	//returns the time offset when black sphere expandes to distance
	static bool radiusTime(const  MATHEX_SCALAR& distance, PhysRelativeTime* pTime);
	static MATHEX_SCALAR radius(const  PhysRelativeTime& timeOffset );

    friend ostream& operator <<( ostream& o, const MachPhysVortexBomb& t );

    PER_MEMBER_PERSISTENT( MachPhysVortexBomb );
    PER_FRIEND_READ_WRITE( MachPhysVortexBomb );
private:
    //Deliberately revoked
    MachPhysVortexBomb( const MachPhysVortexBomb& );
    MachPhysVortexBomb& operator =( const MachPhysVortexBomb& );
    bool operator ==( const MachPhysVortexBomb& );

	MachPhysBlackSphere* pBlackSphere_;
	MachPhysWhiteSphere* pWhiteSphere_;
	MachPhysVortexSphere* pVortexSphere_;
};

PER_DECLARE_PERSISTENT( MachPhysVortexBomb );

#endif

/* End VORTBOMB.HPP *************************************************/
