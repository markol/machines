/*
 * H S P H E R E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysHemiSphere

    Models part of a construction constructing animation.
*/

#ifndef _MACHPHYS_HSPHERE_HPP
#define _MACHPHYS_HSPHERE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysHemiSphere : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysHemiSphere( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysHemiSphere& exemplar();

    //dtor
    ~MachPhysHemiSphere();

	W4dVisibilityPlanPtr startGlow( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, const uint& frameOffset);

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysHemiSphere& t );

    PER_MEMBER_PERSISTENT( MachPhysHemiSphere );

private:
    //Deliberately revoked
    MachPhysHemiSphere( const MachPhysHemiSphere& );
    MachPhysHemiSphere& operator =( const MachPhysHemiSphere& );
    bool operator ==( const MachPhysHemiSphere& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysHemiSphere();
};

PER_READ_WRITE( MachPhysHemiSphere );
PER_DECLARE_PERSISTENT( MachPhysHemiSphere );


#endif

/* End HSPHERE.HPP *************************************************/
