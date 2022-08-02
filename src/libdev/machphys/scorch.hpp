/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGroundScorch

    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_SCORCH_HPP
#define _MACHPHYS_SCORCH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysGroundScorch : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysGroundScorch( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysGroundScorch& exemplar();

    //dtor
    ~MachPhysGroundScorch();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysGroundScorch& t );

    PER_MEMBER_PERSISTENT( MachPhysGroundScorch );

private:
    //Deliberately revoked
    MachPhysGroundScorch( const MachPhysGroundScorch& );
    MachPhysGroundScorch& operator =( const MachPhysGroundScorch& );
    bool operator ==( const MachPhysGroundScorch& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysGroundScorch();

};

PER_DECLARE_PERSISTENT( MachPhysGroundScorch );
PER_READ_WRITE( MachPhysGroundScorch );

#endif

/* End VORTBOMB.HPP *************************************************/
