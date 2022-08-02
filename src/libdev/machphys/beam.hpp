/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBeam
    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_BEAM_HPP
#define _MACHPHYS_BEAM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"

class W4dLink;

class MachPhysBeam : public W4dComposite
// Canonical form revoked
{
public:
    //ctor
    MachPhysBeam( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysBeam& exemplar();

    //dtor
    ~MachPhysBeam();

     //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	PhysRelativeTime startBeam( const PhysAbsoluteTime& startTime );

	static MATHEX_SCALAR beamRadius( const PhysRelativeTime& timeOffset );
	static MATHEX_SCALAR outerRadius( const PhysRelativeTime& timeOffset );
	static MATHEX_SCALAR waveRadius( const PhysRelativeTime& timeOffset );

    void CLASS_INVARIANT;

	friend class MachPhysOtherPersistence;
    friend ostream& operator <<( ostream& o, const MachPhysBeam& t );

    PER_MEMBER_PERSISTENT( MachPhysBeam );
	PER_FRIEND_READ_WRITE( MachPhysBeam );

private:
    //Deliberately revoked
    MachPhysBeam( const MachPhysBeam& );
    MachPhysBeam& operator =( const MachPhysBeam& );
    bool operator ==( const MachPhysBeam& );

    //One-time constructor used to create the exemplar
    MachPhysBeam();

	// this yields the ion beam to not be affected by the fog
	void setMaterialFogMultipliers();

	W4dLink* pBeam_;
	W4dLink* pFlare_;
	W4dLink* pOuter_;
	W4dLink* pWave_;
};

PER_DECLARE_PERSISTENT( MachPhysBeam );

#endif

/* End VORTBOMB.HPP *************************************************/
