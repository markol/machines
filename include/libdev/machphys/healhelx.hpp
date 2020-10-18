/*
 * H E A L H E L X . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysHealHelix

    Models the supercharger healing helix.
*/

#ifndef _MACHPHYS_HEALHELX_HPP
#define _MACHPHYS_HEALHELX_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"

class MachPhysHealHelix : public W4dEntity
// Canonical form revoked
{
public:
    //ctor. Attaches to pParent at position localTransform
    MachPhysHealHelix( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysHealHelix();

    //returns a material used to apply to the helix
    static const RenMaterial& helixMaterial();

    //returns a material sequence plan suitable for the healing animations
    //static const W4dMaterialPlanPtr& MachPhysHealHelix::healCyclingTexturePlan();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysHealHelix& t );

    PER_MEMBER_PERSISTENT( MachPhysHealHelix );

private:
    MachPhysHealHelix( const MachPhysHealHelix& );
    MachPhysHealHelix& operator =( const MachPhysHealHelix& );
    bool operator ==( const MachPhysHealHelix& );

};

PER_DECLARE_PERSISTENT( MachPhysHealHelix );
PER_READ_WRITE( MachPhysHealHelix );

#endif

/* End HEALHELX.HPP *************************************************/
