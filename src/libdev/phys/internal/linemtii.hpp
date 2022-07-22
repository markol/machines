/*
 * L I N E M T I I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysLinearMotionPlanImpl

    A brief description of the class should go in here
*/

#ifndef _PHYS_LINEMTII_HPP
#define _PHYS_LINEMTII_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

#include "ctl/vector.hpp"

class PhysLinearMotionPlanImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( PhysLinearMotionPlanImpl );
    PER_FRIEND_READ_WRITE( PhysLinearMotionPlanImpl );

private:

    PhysLinearMotionPlanImpl( const MexTransform3d& startPosition,
                          const MexTransform3d& endPosition,
                          const PhysRelativeTime& endTime );
    ~PhysLinearMotionPlanImpl();

    PhysLinearMotionPlanImpl( const PhysLinearMotionPlanImpl& );
    PhysLinearMotionPlanImpl& operator =( const PhysLinearMotionPlanImpl& );

    //Extends the motion path to end at endPosition at time EndTime
    void add( const MexTransform3d& endPosition, const PhysRelativeTime& endTime );
    //PRE( endTime >= duration )

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const PhysLinearMotionPlanImpl& t );
	friend class PhysLinearMotionPlan;

    struct Entry
    {
        PhysRelativeTime time; // Time at which the transform defines position
        MexTransform3d transform; // The transform
        Entry( const MexTransform3d& _transform, const PhysRelativeTime _time )
        : transform( _transform ), time( _time )
        {
        };

        PER_MEMBER_PERSISTENT_INLINE( Entry );
        PER_FRIEND_READ_WRITE( Entry );
        friend void PhysDummyFunction1();


    };
    PER_FRIEND_READ_WRITE( Entry );
    friend void PhysDummyFunction1();

    typedef ctl_vector<Entry*> Entries;
    //typedef JonVec<Entry> Entries;

    Entries entries_;

    //Cached values. When calculating an interpolated position, we store the
    //translation and orientation change between the 2 bounding transforms.
    //The orientation is stored as a unit axis vector and angle.
    size_t targetIndex_; //Index of current target Entry (0 => undefined)
    MexPoint3d translation_; //Translation component of modifying transform
    MexVec3 rotationAxis_; //Unit direction vector for axis of rotation change
    MATHEX_SCALAR angle_; //Angle of full rotation between bounding positions
    bool interpolationCached_ :1; //True if above data is valid for the transition from
                                  //targetIndex-1 to targetIndex
};

PER_DECLARE_PERSISTENT( PhysLinearMotionPlanImpl );

#endif

/* End LINEMTII.HPP *************************************************/
