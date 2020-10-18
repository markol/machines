/*
 * V I S P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dVisibilityPlan

    Implement a mapping from time to a visibility flag
*/

#ifndef _WORLD4D_VISPLAN_HPP
#define _WORLD4D_VISPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/countptr.hpp"

#include "ctl/vector.hpp"

#include "phys/phys.hpp"

class W4dVisibilityPlan
{
public:
    //  Specify the visibility at time zero
    W4dVisibilityPlan( bool visible );
    ~W4dVisibilityPlan( void );

    //  Specify the visibility at the given time
    void add( bool visible, const PhysRelativeTime& time );
    
    //  Return the value of the visibility flag at the given time
    bool visible( const PhysRelativeTime& time ) const;

    const PhysRelativeTime& duration( void ) const;
    
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( W4dVisibilityPlan );
    PER_FRIEND_READ_WRITE( W4dVisibilityPlan );
private:
    // Operation deliberately revoked
    W4dVisibilityPlan( const W4dVisibilityPlan& );

    // Operation deliberately revoked
    W4dVisibilityPlan& operator =( const W4dVisibilityPlan& );

    // Operation deliberately revoked
    bool operator ==( const W4dVisibilityPlan& );

    typedef ctl_vector< PhysRelativeTime >  Times;
    typedef ctl_vector< bool >      Visibles;
    
    Times       times_;
    Visibles    visibleFlags_;
    
    PhysRelativeTime    duration_;
};

PER_DECLARE_PERSISTENT( W4dVisibilityPlan );

//Provides a reference counted pointer to a W4dVisibilityPlan
typedef CtlCountedPtr< W4dVisibilityPlan > W4dVisibilityPlanPtr;

#endif

/* End VISPLAN.HPP **************************************************/
