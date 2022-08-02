/*
 * L O A D O V E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLoadingOver

    Models part of resource loading animation
*/

#ifndef _MACHPHYS_LOADOVER_HPP
#define _MACHPHYS_LOADOVER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"

class MachPhysLoadingOver: public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLoadingOver( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysLoadingOver();

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLoadingOver& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLoadingOver& t );

    PER_MEMBER_PERSISTENT( MachPhysLoadingOver );

private:
    //Deliberately revoked
    MachPhysLoadingOver( const MachPhysLoadingOver& );
    MachPhysLoadingOver& operator =( const MachPhysLoadingOver& );
    bool operator ==( const MachPhysLoadingOver& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLoadingOver();

};

PER_DECLARE_PERSISTENT( MachPhysLoadingOver );
PER_READ_WRITE( MachPhysLoadingOver );

#endif

/* End LOADOVER.HPP *************************************************/
