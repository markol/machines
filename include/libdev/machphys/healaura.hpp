/*
 * H E A L A U R A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysHealAura

    Models the be-healed aura of expanding crosses to be attached to a machine
    being healed by the supercharger weapon.
*/

#ifndef _MACHPHYS_HEALAURA_HPP
#define _MACHPHYS_HEALAURA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"
#include "phys/phys.hpp"

//forward refs
class MachPhysMachine;
class W4dGeneric;

class MachPhysHealAura : public W4dEntity
// Canonical form revoked
{
public:
    //ctor. Attaches itself to pMachine, the machine being healed.
    //Becomes active at startTime, and remains so until destructed.
    MachPhysHealAura( MachPhysMachine* pMachine, const PhysAbsoluteTime& startTime );

    //dtor.
    virtual ~MachPhysHealAura();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    //Return exemplars for the two crosses
    static const W4dGeneric& centralCrossExemplar();
    static const W4dGeneric& radialCrossExemplar();

    //returns a material used to apply to the radial cross
    static const RenMaterial& radialCrossMaterial();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysHealAura& t );

    PER_MEMBER_PERSISTENT( MachPhysHealAura );

private:
    MachPhysHealAura( const MachPhysHealAura& );
    MachPhysHealAura& operator =( const MachPhysHealAura& );
    bool operator ==( const MachPhysHealAura& );

	friend class MachPhysOtherPersistence;

	static W4dGeneric& newRadialCross();
	static W4dGeneric& newCentralCross();

};

PER_DECLARE_PERSISTENT( MachPhysHealAura );
PER_READ_WRITE( MachPhysHealAura );

#endif

/* End HEALAURA.HPP *************************************************/
