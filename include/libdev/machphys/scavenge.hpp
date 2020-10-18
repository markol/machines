/*
 * S C A V E N G E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysScavenger

    Models the resource carrier 3 scavenging animation
*/

#ifndef _MACHPHYS_SCAVENGE_HPP
#define _MACHPHYS_SCAVENGE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysScavenger: public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysScavenger( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysScavenger();

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysScavenger& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startScavenge( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysScavenger& t );

    PER_MEMBER_PERSISTENT( MachPhysScavenger );

private:
    //Deliberately revoked
    MachPhysScavenger( const MachPhysScavenger& );
    MachPhysScavenger& operator =( const MachPhysScavenger& );
    bool operator ==( const MachPhysScavenger& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysScavenger();

};

PER_DECLARE_PERSISTENT( MachPhysScavenger );
PER_READ_WRITE( MachPhysScavenger );

#endif

/* End SCAVENGE.HPP *************************************************/
