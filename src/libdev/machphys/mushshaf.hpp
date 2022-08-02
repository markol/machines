/*
 * M U S H S H A F . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMushroomShaft

    Models part of a nuclear bomb explosion animation
*/

#ifndef _MACHPHYS_MUSHSHAF_HPP
#define _MACHPHYS_MUSHSHAF_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysMushroomShaft : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysMushroomShaft( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysMushroomShaft& exemplar();

    //dtor
    ~MachPhysMushroomShaft();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMushroomShaft& t );

    PER_MEMBER_PERSISTENT( MachPhysMushroomShaft );

private:
    //Deliberately revoked
    MachPhysMushroomShaft( const MachPhysMushroomShaft& );
    MachPhysMushroomShaft& operator =( const MachPhysMushroomShaft& );
    bool operator ==( const MachPhysMushroomShaft& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysMushroomShaft();

	void setMaterialFogMultipliers();

};

PER_READ_WRITE( MachPhysMushroomShaft );
PER_DECLARE_PERSISTENT( MachPhysMushroomShaft );


#endif

/* End MUSHSHAF.HPP *************************************************/
