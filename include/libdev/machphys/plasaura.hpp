/*
 * P L A S A U R A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlasmaAura

    Models part of construction damage animation
*/

#ifndef _MACHPHYS_PLASAURA_HPP
#define _MACHPHYS_PLASAURA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysPlasmaAura : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysPlasmaAura( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysPlasmaAura& exemplar();

    //dtor
    ~MachPhysPlasmaAura();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startPlasmaAura( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPlasmaAura& t );

    PER_MEMBER_PERSISTENT( MachPhysPlasmaAura );

private:
    //Deliberately revoked
    MachPhysPlasmaAura( const MachPhysPlasmaAura& );
    MachPhysPlasmaAura& operator =( const MachPhysPlasmaAura& );
    bool operator ==( const MachPhysPlasmaAura& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPlasmaAura();

};

PER_DECLARE_PERSISTENT( MachPhysPlasmaAura );
PER_READ_WRITE( MachPhysPlasmaAura );

#endif

/* End PLASAURA.HPP *************************************************/
