/*
 * L O A D U N D E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLoadingUnder

    Models part of resource loading animation
*/

#ifndef _MACHPHYS_LOADUNDE_HPP
#define _MACHPHYS_LOADUNDE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"

class MachPhysLoadingUnder: public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLoadingUnder( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysLoadingUnder();

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLoadingUnder& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLoadingUnder& t );

    PER_MEMBER_PERSISTENT( MachPhysLoadingUnder );

private:
    //Deliberately revoked
    MachPhysLoadingUnder( const MachPhysLoadingUnder& );
    MachPhysLoadingUnder& operator =( const MachPhysLoadingUnder& );
    bool operator ==( const MachPhysLoadingUnder& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLoadingUnder();

};

PER_DECLARE_PERSISTENT( MachPhysLoadingUnder );
PER_READ_WRITE( MachPhysLoadingUnder );

#endif

/* End LOADUNDE.HPP *************************************************/
