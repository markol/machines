/*
 * L I G H T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLight

    Models the light on the ground when a "metal" weapon fires
*/

#ifndef _MACHPHYS_LIGHT_HPP
#define _MACHPHYS_LIGHT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysLight : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLight( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLight& exemplar();

    //dtor
    ~MachPhysLight();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

	void startLight( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

    friend ostream& operator <<( ostream& o, const MachPhysLight& t );

    PER_MEMBER_PERSISTENT( MachPhysLight );
	PER_FRIEND_READ_WRITE( MachPhysLight );

private:
    //Deliberately revoked
    MachPhysLight( const MachPhysLight& );
    MachPhysLight& operator =( const MachPhysLight& );
    bool operator ==( const MachPhysLight& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLight();

};

PER_DECLARE_PERSISTENT( MachPhysLight );

#endif

/* End LIGHT.HPP *************************************************/
