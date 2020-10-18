/*
 * P L A S P L A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlasmaSplat

    Models part of construction damage animation
*/

#ifndef _MACHPHYS_PLASPLAT_HPP
#define _MACHPHYS_PLASPLAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysPlasmaSplat : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysPlasmaSplat( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysPlasmaSplat& exemplar();

    //dtor
    ~MachPhysPlasmaSplat();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startPlasmaSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

	static void preloadTextures();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPlasmaSplat& t );

    PER_MEMBER_PERSISTENT( MachPhysPlasmaSplat );

private:
    //Deliberately revoked
    MachPhysPlasmaSplat( const MachPhysPlasmaSplat& );
    MachPhysPlasmaSplat& operator =( const MachPhysPlasmaSplat& );
    bool operator ==( const MachPhysPlasmaSplat& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPlasmaSplat();

};

PER_DECLARE_PERSISTENT( MachPhysPlasmaSplat );
PER_READ_WRITE( MachPhysPlasmaSplat );

#endif

/* End PLASPLAT.HPP *************************************************/
