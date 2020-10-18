/*
 * S P L A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGroundSplat

    Models part of construction damage animation
*/

#ifndef _MACHPHYS_SPLAT_HPP
#define _MACHPHYS_SPLAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysGroundSplat : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysGroundSplat( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysGroundSplat& exemplar();

    //dtor
    ~MachPhysGroundSplat();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysGroundSplat& t );

    PER_MEMBER_PERSISTENT( MachPhysGroundSplat );

private:
    //Deliberately revoked
    MachPhysGroundSplat( const MachPhysGroundSplat& );
    MachPhysGroundSplat& operator =( const MachPhysGroundSplat& );
    bool operator ==( const MachPhysGroundSplat& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysGroundSplat();

};

PER_DECLARE_PERSISTENT( MachPhysGroundSplat );
PER_READ_WRITE( MachPhysGroundSplat );

#endif

/* End SPLAT.HPP *************************************************/
