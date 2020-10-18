/*
 * M U S H T O P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMushroomTop

    Models part of a nuclear bomb explosion animation
*/

#ifndef _MACHPHYS_MUSHTOP_HPP
#define _MACHPHYS_MUSHTOP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysMushroomTop : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysMushroomTop( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysMushroomTop& exemplar();

    //dtor
    ~MachPhysMushroomTop();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMushroomTop& t );

    PER_MEMBER_PERSISTENT( MachPhysMushroomTop );

private:
    //Deliberately revoked
    MachPhysMushroomTop( const MachPhysMushroomTop& );
    MachPhysMushroomTop& operator =( const MachPhysMushroomTop& );
    bool operator ==( const MachPhysMushroomTop& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysMushroomTop();

	void setMaterialFogMultipliers();

};

PER_READ_WRITE( MachPhysMushroomTop );
PER_DECLARE_PERSISTENT( MachPhysMushroomTop );


#endif

/* End MUSHTOP.HPP *************************************************/
