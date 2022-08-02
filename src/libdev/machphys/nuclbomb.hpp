/*
 * N U C L B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysNuclearBomb

    Models a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_NUCLBOMB_HPP
#define _MACHPHYS_NUCLBOMB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysNukeWave;
class MachPhysMushroomTop;
class MachPhysMushroomShaft;
class MachPhysMushroomEdge;
//class MachPhysGroundSplat;
class MachPhysFlashDisc;
class W4dUniformLight;
class MachPhysPlanetSurface;

class MachPhysNuclearBomb : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysNuclearBomb(W4dEntity* pParent, const MexTransform3d& localTransform);

    //dtor
    virtual ~MachPhysNuclearBomb();

    //Begin the explosion animation at startTime.
    //Returns the duration of the explosion.
    PhysRelativeTime startExplosion( const PhysAbsoluteTime& startTime, MachPhysPlanetSurface* pSurface );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	//returns the nuke shockwave radius at timeOffset
	static MATHEX_SCALAR shockwaveRadius( const PhysRelativeTime& timeOffset );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysNuclearBomb& t );

    PER_MEMBER_PERSISTENT( MachPhysNuclearBomb );
    PER_FRIEND_READ_WRITE( MachPhysNuclearBomb );
private:
    //Deliberately revoked
    MachPhysNuclearBomb( const MachPhysNuclearBomb& );
    MachPhysNuclearBomb& operator =( const MachPhysNuclearBomb& );
    bool operator ==( const MachPhysNuclearBomb& );

	static const PhysRelativeTime& exPlosionDuration();

	MachPhysNukeWave* pNukeWave_;
	MachPhysMushroomTop* pMushroomTop_;
	MachPhysMushroomShaft* pMushroomShaft_;
	MachPhysMushroomEdge* pMushroomEdge_;
	//MachPhysGroundSplat* pSplat_;
	MachPhysFlashDisc* pFlash_;
	W4dUniformLight* pLight_;
};

PER_DECLARE_PERSISTENT( MachPhysNuclearBomb );

#endif

/* End VORTBOMB.HPP *************************************************/
