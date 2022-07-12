/*
 * M O V E S P I N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysMoveSpinPlan

	Implements a linear motion with a constant spinning over a fixed axis
*/

#ifndef _MOVESPIN_HPP
#define _MOVESPIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/motplan.hpp"

class PhysMoveSpinPlanImpl;

class PhysMoveSpinPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    PhysMoveSpinPlan( const MexTransform3d& startPosition,
                          const MexVec3& speedVector,
						  const MexVec3& rotationAxis,
						  MATHEX_SCALAR rotationSpeed,
                          const PhysRelativeTime& endTime);

    ~PhysMoveSpinPlan();

    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( PhysMoveSpinPlan );
    PER_FRIEND_READ_WRITE( PhysMoveSpinPlan );

private:
    friend ostream& operator <<( ostream& o, const PhysMoveSpinPlan& t );

    PhysMoveSpinPlan();
    PhysMoveSpinPlan( const PhysMoveSpinPlan& );
    PhysMoveSpinPlan& operator =( const PhysMoveSpinPlan& );

	PhysMoveSpinPlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysMoveSpinPlan );

#endif

/* End MOVESPIN.HPP *************************************************/
