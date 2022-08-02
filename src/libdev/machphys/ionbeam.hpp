/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysIonBeam

    Models a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_IONBEAM_HPP
#define _MACHPHYS_IONBEAM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysPlanetSurface;

class MachPhysIonBeam : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysIonBeam(W4dEntity* pParent, const MexTransform3d& localTransform);

    //dtor
    virtual ~MachPhysIonBeam();

    //Begin the explosion animation at startTime.
    //Returns the duration of the explosion.
    PhysRelativeTime startExplosion( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface );

	//returns the duration of the explosion
	static const PhysRelativeTime& duration();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	//returns the time offset when beam expandes to distance
	static bool radiusTime(const  MATHEX_SCALAR& distance, PhysRelativeTime* pTime);

	//the maximum radius of the beam
	static const MATHEX_SCALAR radius();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysIonBeam& t );

    PER_MEMBER_PERSISTENT( MachPhysIonBeam );
private:
    //Deliberately revoked
    MachPhysIonBeam( const MachPhysIonBeam& );
    MachPhysIonBeam& operator =( const MachPhysIonBeam& );
    bool operator ==( const MachPhysIonBeam& );
};

PER_READ_WRITE( MachPhysIonBeam );
PER_DECLARE_PERSISTENT( MachPhysIonBeam );

#endif

/* End VORTBOMB.HPP *************************************************/
