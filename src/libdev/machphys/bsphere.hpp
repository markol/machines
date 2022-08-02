/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBlackSphere

    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_BSPHERE_HPP
#define _MACHPHYS_BSPHERE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysBlackSphere : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysBlackSphere( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysBlackSphere& exemplar();

    //dtor
    ~MachPhysBlackSphere();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysBlackSphere& t );

    PER_MEMBER_PERSISTENT( MachPhysBlackSphere );

private:
    //Deliberately revoked
    MachPhysBlackSphere( const MachPhysBlackSphere& );
    MachPhysBlackSphere& operator =( const MachPhysBlackSphere& );
    bool operator ==( const MachPhysBlackSphere& );

	friend class MachPhysOtherPersistence;

    //One-time constructor used to create the exemplar
    MachPhysBlackSphere();

};

PER_READ_WRITE( MachPhysBlackSphere );
PER_DECLARE_PERSISTENT( MachPhysBlackSphere );


#endif

/* End BSPHERE.HPP *************************************************/
