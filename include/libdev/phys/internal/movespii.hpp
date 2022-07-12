/*
 * M O V E S P I I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysMoveSpinPlanImpl

    A brief description of the class should go in here
*/

#ifndef _MOVESPII_HPP
#define _MOVESPII_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

class PhysMoveSpinPlanImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( PhysMoveSpinPlanImpl );
    PER_FRIEND_READ_WRITE( PhysMoveSpinPlanImpl );

    void CLASS_INVARIANT;

private:
	friend class PhysMoveSpinPlan;
    friend ostream& operator <<( ostream& o, const PhysMoveSpinPlanImpl& t );

    PhysMoveSpinPlanImpl( const MexTransform3d& startLocation,
                          const MexVec3& speedVector,
						  const MexVec3& rotationAxis,
						  MATHEX_SCALAR rotationSpeed,
                          const PhysRelativeTime& endTime);
    ~PhysMoveSpinPlanImpl();
    PhysMoveSpinPlanImpl( const PhysMoveSpinPlanImpl& );
    PhysMoveSpinPlanImpl& operator =( const PhysMoveSpinPlanImpl& );

	MexTransform3d startLocation_;
	MexVec3 speedVector_;
	MexVec3 rotationAxis_;
	MATHEX_SCALAR  rotationSpeed_;
    PhysRelativeTime endTime_; 
};

PER_DECLARE_PERSISTENT( PhysMoveSpinPlanImpl );

#endif

/* End MOVESPII.HPP *************************************************/
