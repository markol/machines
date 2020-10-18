/*
 * A C C E T U M I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysAccelerateTumblePlanImp

    A brief description of the class should go in here
*/

#ifndef _PHYS_ACCETUMI_HPP
#define _PHYS_ACCETUMI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/vec3.hpp"
#include "mathex/eulerang.hpp"

#include "phys/eulertx.hpp"
#include "phys/motplan.hpp"
#include "phys/accetumb.hpp"

#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"

class PhysAccelerateTumblePlanImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( PhysAccelerateTumblePlanImpl );
    PER_FRIEND_READ_WRITE( PhysAccelerateTumblePlanImpl );

private:
	friend class PhysAccelerateTumblePlan;

    PhysAccelerateTumblePlanImpl( const PhysAccelerateTumblePlan::EulerTransformsPtr& transformsPtr,
							      const PhysMotionPlan::TimesPtr& segmentTimesPtr,
							      const MexVec3& acceleration );

    ~PhysAccelerateTumblePlanImpl( void );

    void CLASS_INVARIANT;

    PhysAccelerateTumblePlanImpl( const PhysAccelerateTumblePlanImpl& );
    PhysAccelerateTumblePlanImpl& operator =( const PhysAccelerateTumblePlanImpl& );

    friend ostream& operator <<( ostream& o, const PhysAccelerateTumblePlanImpl& t );

    PhysAccelerateTumblePlan::EulerTransformsPtr   transformsPtr_;
    PhysMotionPlan::TimesPtr        segmentTimesPtr_;
    MexVec3         acceleration_;

    //  Cached data - updated as we change segments
    size_t          currentSegmentIndex_;
    MexEulerAngles  deltaOrientation_;
    MexEulerAngles  initialOrientation_;
    MexVec3         initialVelocity_;
};

PER_DECLARE_PERSISTENT( PhysAccelerateTumblePlanImpl );

#endif

/* End ACCETUMI.HPP *************************************************/
