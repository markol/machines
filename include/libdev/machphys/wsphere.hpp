/*
 * V O R T B O M B . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWhiteSphere
    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_WSPHERE_HPP
#define _MACHPHYS_WSPHERE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysWhiteSphere : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysWhiteSphere( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysWhiteSphere& exemplar();

    //dtor
    virtual ~MachPhysWhiteSphere();

     //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWhiteSphere& t );

    PER_MEMBER_PERSISTENT( MachPhysWhiteSphere );

private:
    //Deliberately revoked
    MachPhysWhiteSphere( const MachPhysWhiteSphere& );
    MachPhysWhiteSphere& operator =( const MachPhysWhiteSphere& );
    bool operator ==( const MachPhysWhiteSphere& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysWhiteSphere();

};

PER_DECLARE_PERSISTENT( MachPhysWhiteSphere );
PER_READ_WRITE( MachPhysWhiteSphere );

#endif

/* End VORTBOMB.HPP *************************************************/
