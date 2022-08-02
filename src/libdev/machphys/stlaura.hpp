/*
 * S T L A U R A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLightStingAura

    Models part of a wasp light sting effect
*/

#ifndef _MACHPHYS_STLAURA_HPP
#define _MACHPHYS_STLAURA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysLightStingAura : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLightStingAura( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLightStingAura& exemplar();

    //dtor
    virtual ~MachPhysLightStingAura();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startLightStingAura( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLightStingAura& t );

    PER_MEMBER_PERSISTENT( MachPhysLightStingAura );

private:
    //Deliberately revoked
    MachPhysLightStingAura( const MachPhysLightStingAura& );
    MachPhysLightStingAura& operator =( const MachPhysLightStingAura& );
    bool operator ==( const MachPhysLightStingAura& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLightStingAura();

};

PER_DECLARE_PERSISTENT( MachPhysLightStingAura );
PER_READ_WRITE( MachPhysLightStingAura );

#endif

/* End STLAURA.HPP *************************************************/
