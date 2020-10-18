/*
 * M U S H E D G E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMushroomEdge

    Models part of a nuclear bomb explosion animation
*/

#ifndef _MACHPHYS_MUSHEDGE_HPP
#define _MACHPHYS_MUSHEDGE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysMushroomEdge : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysMushroomEdge( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysMushroomEdge& exemplar();

    //dtor
    ~MachPhysMushroomEdge();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMushroomEdge& t );

    PER_MEMBER_PERSISTENT( MachPhysMushroomEdge );

private:
    //Deliberately revoked
    MachPhysMushroomEdge( const MachPhysMushroomEdge& );
    MachPhysMushroomEdge& operator =( const MachPhysMushroomEdge& );
    bool operator ==( const MachPhysMushroomEdge& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysMushroomEdge();

};

PER_READ_WRITE( MachPhysMushroomEdge );
PER_DECLARE_PERSISTENT( MachPhysMushroomEdge );


#endif

/* End MUSHEDGE.HPP *************************************************/
