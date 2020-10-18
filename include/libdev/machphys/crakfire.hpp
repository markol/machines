/*
 * C R A K F I R E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysCrackFire

    part of the gorrila punch animation
*/

#ifndef _MACHPHYS_CRAKFIRE_HPP
#define _MACHPHYS_CRAKFIRE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"

class MachPhysPlanetSurface;
class MachPhysCrackFireImpl;
class MexTransform3d;
class MexPoint2d;
class MexPoint3d;

template < class T > class ctl_vector;

class MachPhysCrackFire : public W4dComposite
// Canonical form revoked
{
public:

	enum ChasmType : unsigned char { GARILLA_PUNCH_CHASM, BEE_BOMB_CHASM };
    //ctor
    MachPhysCrackFire( W4dEntity* pParent, const MexTransform3d& startTransform,
                       const MATHEX_SCALAR& length, const ChasmType& type = GARILLA_PUNCH_CHASM );

    MachPhysCrackFire( W4dEntity* pParent, const MexPoint3d& startPoint,
	                                   const MexPoint3d& endPoint, const ChasmType& type = GARILLA_PUNCH_CHASM );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysCrackFire& exemplar( const ChasmType& type );

    //dtor
    virtual ~MachPhysCrackFire();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	enum DisplayDebris : unsigned char { DEBRIS_DISPLAY, DEBRIS_NOT_DISPLAY };
	void startCrackAndFire( const PhysAbsoluteTime& startTime,
	                        const PhysRelativeTime& duration,
	                        const MATHEX_SCALAR& crackYScale,
	                        const MATHEX_SCALAR& fireZScale,
	                        const ChasmType& type = GARILLA_PUNCH_CHASM,
	                        DisplayDebris  debris = DEBRIS_DISPLAY );

	typedef ctl_vector < MexPoint2d >  Points;

	//create crack and fires on a segmented line defined by points, start them from startTime
	static void createCrackFires
	(
		W4dEntity* pParent,
		const MexTransform3d& position,
		const Points& points,
		const PhysAbsoluteTime& startTime,
		const PhysRelativeTime& duration,
		const MachPhysPlanetSurface& surface,
		const MATHEX_SCALAR& crackScale,
		const MATHEX_SCALAR& fireScale,
		DisplayDebris  debris = DEBRIS_DISPLAY,
		MachPhysCrackFire::ChasmType type = MachPhysCrackFire::BEE_BOMB_CHASM
	);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysCrackFire& t );

    PER_MEMBER_PERSISTENT( MachPhysCrackFire );
    PER_FRIEND_READ_WRITE( MachPhysCrackFire );

private:
    //Deliberately revoked
    MachPhysCrackFire( const MachPhysCrackFire& );
    MachPhysCrackFire& operator =( const MachPhysCrackFire& );
    bool operator ==( const MachPhysCrackFire& );

	friend class MachPhysOtherPersistence;

    //One-time constructor used to create the exemplar
    MachPhysCrackFire(const ChasmType& type);

	void setMaterialFogMultipliers();

	MachPhysCrackFireImpl* pImpl_;

};

PER_DECLARE_PERSISTENT( MachPhysCrackFire );
PER_ENUM_PERSISTENT( MachPhysCrackFire::ChasmType );
PER_ENUM_PERSISTENT( MachPhysCrackFire::DisplayDebris );

#endif

/* End CRAKFIRE.HPP *************************************************/
