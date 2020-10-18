/*
 * D A M A G W A V . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBurstWave

    Models part of a construction damage animation
*/

#ifndef _DAMAGWAV_HPP
#define _DAMAGWAV_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/entity.hpp"
#include "machphys/flames.hpp"

class MachPhysBurstWave : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysBurstWave( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar  - ensures the bomb meshes and textures are loaded
    static const MachPhysBurstWave& exemplar();

	//start the shock wave as planned
	void startBurstWave( const PhysAbsoluteTime& startTime,
                        const PhysRelativeTime& duration,
                        const MATHEX_SCALAR& fromRadius,
                        const MATHEX_SCALAR& toRadius,
                        const MATHEX_SCALAR& zScale);
    //dtor
    ~MachPhysBurstWave();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysBurstWave& t );

    PER_MEMBER_PERSISTENT( MachPhysBurstWave );

private:
    //Deliberately revoked
    MachPhysBurstWave( const MachPhysBurstWave& );
    MachPhysBurstWave& operator =( const MachPhysBurstWave& );
    bool operator ==( const MachPhysBurstWave& );
	static const double& burstWaveDefaultSize();

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysBurstWave();
};

PER_READ_WRITE( MachPhysBurstWave );
PER_DECLARE_PERSISTENT( MachPhysBurstWave );

#endif

/* End DAMAGWAV.HPP *************************************************/
