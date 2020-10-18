/*
 * L O A D L I G H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLoadingLight

    Models part of resource loading animation
*/

#ifndef _MACHPHYS_LOADLIGH_HPP
#define _MACHPHYS_LOADLIGH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"


class MachPhysLoadingLight: public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLoadingLight( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysLoadingLight();

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLoadingLight& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLoadingLight& t );

    PER_MEMBER_PERSISTENT( MachPhysLoadingLight );

private:
    //Deliberately revoked
    MachPhysLoadingLight( const MachPhysLoadingLight& );
    MachPhysLoadingLight& operator =( const MachPhysLoadingLight& );
    bool operator ==( const MachPhysLoadingLight& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLoadingLight();

};

PER_DECLARE_PERSISTENT( MachPhysLoadingLight );
PER_READ_WRITE( MachPhysLoadingLight );

#endif

/* End LOADLIGH.HPP *************************************************/
