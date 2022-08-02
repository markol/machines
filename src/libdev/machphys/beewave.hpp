/*
 * B E E W A V E. H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBeeBombWave

    Models part of a bee bomb explosion animation
*/

#ifndef _BEEWAVE_HPP
#define _BEEWAVE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/entity.hpp"

class MachPhysBeeBombWave : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysBeeBombWave( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar  - ensures the bomb meshes and textures are loaded
    static const MachPhysBeeBombWave& exemplar();

	//returns the radius of the wave at timeOffset (count from startTime of startBeeWave
	static MATHEX_SCALAR radius( const PhysRelativeTime& timeOffset );

	//start the shock wave as planned,  return the duration
	const PhysRelativeTime startBeeWave( const PhysAbsoluteTime& startTime );
    //dtor
    virtual ~MachPhysBeeBombWave();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysBeeBombWave& t );

    PER_MEMBER_PERSISTENT( MachPhysBeeBombWave );

private:
    //Deliberately revoked
    MachPhysBeeBombWave( const MachPhysBeeBombWave& );
    MachPhysBeeBombWave& operator =( const MachPhysBeeBombWave& );
    bool operator ==( const MachPhysBeeBombWave& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysBeeBombWave();

};

PER_READ_WRITE( MachPhysBeeBombWave );
PER_DECLARE_PERSISTENT( MachPhysBeeBombWave );

#endif

/* End BEEWAVE.HPP *************************************************/
