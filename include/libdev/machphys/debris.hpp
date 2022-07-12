/*
 * D E B R I S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    MachPhysDebris

    A brief description of the class should go in here
*/

#ifndef _DEBRIS_HPP
#define _DEBRIS_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/pvector.hpp"

#include "machphys/machphys.hpp"
#include "machphys/debrisun.hpp"

#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"

#include "mathex/abox2d.hpp"

class MachPhysDebris : public W4dEntity
{
public:
	enum MachPhysDebrisType : unsigned int  { LARGE, LARGISH, MEDIUM, SMALLISH, SMALL };

	typedef ctl_pvector< MachPhysDebrisUnit >	DebrisUnits;

    MachPhysDebris( W4dEntity* pParent,
    				const W4dTransform3d& localTransform,
    				const MexAlignedBox2d& boundary,
				    const PhysRelativeTime& burningDuration,
    				const MachPhysDebrisType& debrisType);

    ~MachPhysDebris();

	void startBurning();

    void CLASS_INVARIANT;

	bool intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const;

	DebrisUnits& debrisUnits();

    PER_MEMBER_PERSISTENT( MachPhysDebris );
    PER_FRIEND_READ_WRITE( MachPhysDebris );

private:
    // Operations deliberately revoked
    MachPhysDebris( const MachPhysDebris& );
    MachPhysDebris& operator =( const MachPhysDebris& );
    bool operator ==( const MachPhysDebris& );
	////

    MachPhysDebris( W4dEntity* pParent, const MexTransform3d& transform );

    static const MachPhysDebris& factory();

	MachPhysDebrisType debrisType_;

    PhysRelativeTime burningDuration_;

	DebrisUnits debrisUnits_;

	/*
	MachPhysDebrisUnit* unit1_;
	MachPhysDebrisUnit* unit2_;
	MachPhysDebrisUnit* unit3_;
	MachPhysDebrisUnit* unit4_;
	MachPhysDebrisUnit* unit5_;
	*/

};

PER_ENUM_PERSISTENT( MachPhysDebris::MachPhysDebrisType );
PER_DECLARE_PERSISTENT( MachPhysDebris );

#endif

/* End DEBRIS.HPP ***************************************************/
