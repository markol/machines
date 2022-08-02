/*
 * P U L S A U R A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPulseAura

    Models part of construction damage animation
*/

#ifndef _MACHPHYS_PULSAURA_HPP
#define _MACHPHYS_PULSAURA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysPulseAura : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysPulseAura( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysPulseAura& exemplar();

    //dtor
    ~MachPhysPulseAura();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startPulseAura( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseAura& t );

    PER_MEMBER_PERSISTENT( MachPhysPulseAura );

private:
    //Deliberately revoked
    MachPhysPulseAura( const MachPhysPulseAura& );
    MachPhysPulseAura& operator =( const MachPhysPulseAura& );
    bool operator ==( const MachPhysPulseAura& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPulseAura();

};

PER_DECLARE_PERSISTENT( MachPhysPulseAura );
PER_READ_WRITE( MachPhysPulseAura );

#endif

/* End ULSAURA.HPP *************************************************/
