/*
 * V S P H E R E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysVortexSphere
    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_VSPHERE_HPP
#define _MACHPHYS_VSPHERE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysVortexSphere : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysVortexSphere( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysVortexSphere& exemplar();

    //dtor
    ~MachPhysVortexSphere();

     //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysVortexSphere& t );

    PER_MEMBER_PERSISTENT( MachPhysVortexSphere );

private:
    //Deliberately revoked
    MachPhysVortexSphere( const MachPhysVortexSphere& );
    MachPhysVortexSphere& operator =( const MachPhysVortexSphere& );
    bool operator ==( const MachPhysVortexSphere& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysVortexSphere();

};

PER_DECLARE_PERSISTENT( MachPhysVortexSphere );
PER_READ_WRITE( MachPhysVortexSphere );

#endif

/* End VSPHERE.HPP *************************************************/
