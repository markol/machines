/*
 * P U L S P L A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPulseSplat

    Models part of construction damage animation
*/

#ifndef _MACHPHYS_PULSPLAT_HPP
#define _MACHPHYS_PULSPLAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysPulseSplat : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysPulseSplat( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysPulseSplat& exemplar();
	static void preloadTextures();

    //dtor
    ~MachPhysPulseSplat();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startPulseSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseSplat& t );

    PER_MEMBER_PERSISTENT( MachPhysPulseSplat );

private:
    //Deliberately revoked
    MachPhysPulseSplat( const MachPhysPulseSplat& );
    MachPhysPulseSplat& operator =( const MachPhysPulseSplat& );
    bool operator ==( const MachPhysPulseSplat& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPulseSplat();

};

PER_DECLARE_PERSISTENT( MachPhysPulseSplat );
PER_READ_WRITE( MachPhysPulseSplat );

#endif

/* End PULSPLAT.HPP *************************************************/
