/*
 * D E B R I S U N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    MachPhysDebris

    A brief description of the class should go in here
*/

#ifndef _DEBRISUN_HPP
#define _DEBRISUN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/machphys.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/smokpuff.hpp"
#include "machphys/flames.hpp"

#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"

#include "mathex/abox2d.hpp"

class MachPhysDebrisUnit : public W4dEntity
{
public:
    MachPhysDebrisUnit( W4dEntity* pParent,
    				const W4dTransform3d& localTransform,
    				const MexAlignedBox2d& boundary,
    				const PhysRelativeTime& overallDuration
    				);

    virtual ~MachPhysDebrisUnit( void );

	void startBurning();

    static const MachPhysDebrisUnit& factory();
	static void preloadTextures();

    void CLASS_INVARIANT;

	bool intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const;

    PER_MEMBER_PERSISTENT( MachPhysDebrisUnit );
    PER_FRIEND_READ_WRITE( MachPhysDebrisUnit );
private:
    // Operations deliberately revoked
    MachPhysDebrisUnit( const MachPhysDebrisUnit& );
    MachPhysDebrisUnit& operator =( const MachPhysDebrisUnit& );
    bool operator ==( const MachPhysDebrisUnit& );

	friend class MachPhysOtherPersistence;
    MachPhysDebrisUnit( W4dEntity* pParent, const MexTransform3d& transform );

	MachPhysSmokePlume* pPlume1_;
	MachPhysSmokePlume* pPlume2_;
	MachPhysSmokePlume* pPlume3_;
	MachPhysFlame* pFlame1_;
	MachPhysFlame* pFlame2_;
	MachPhysFlame* pFlame3_;

	PhysRelativeTime overallDuration_;

	MexAlignedBox2d boundary_;
};

PER_DECLARE_PERSISTENT( MachPhysDebrisUnit );

#endif

/* End DEBRISUN.HPP ***************************************************/
