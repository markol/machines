/*
 * B E E B O M B E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBeeBombExplosion

    the elements of a bee bomb explosion
*/

#ifndef _MACHPHYS_BEEBOMBE_HPP
#define _MACHPHYS_BEEBOMBE_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"

#include "world4d/entity.hpp"

class MachPhysPlanetSurface;

class MachPhysBeeBombExplosion : public W4dEntity

// Canonical form revoked
{
public:
    MachPhysBeeBombExplosion( W4dEntity* pParent, const MexTransform3d& localTransform );
    virtual ~MachPhysBeeBombExplosion( void );

	const PhysRelativeTime startExplosion( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface);

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysBeeBombExplosion& t );

    MachPhysBeeBombExplosion( const MachPhysBeeBombExplosion& );
    MachPhysBeeBombExplosion& operator =( const MachPhysBeeBombExplosion& );

	//creat concentric rings (6) of fire and crack
	static const PhysRelativeTime createFireRings
	(
		W4dEntity* pParent,
		const MexTransform3d& position,
		const PhysAbsoluteTime& startTime,
		const MachPhysPlanetSurface& surface
	);
};

#endif

/* End BEEBOMBE.HPP ***************************************************/
