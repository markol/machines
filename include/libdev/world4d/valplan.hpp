/*
 * V A L P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dFloatValuePlan

    This class is an abstract base class for functions which map a time value in the
    range [0, duration] to a single floating point value.
*/

#ifndef _WORLD4D_VALPLAN_HPP
#define _WORLD4D_VALPLAN_HPP

#include "base/base.hpp"

#include "ctl/countptr.hpp"
#include "phys/phys.hpp"

class W4dFloatValuePlan
{
public:
    W4dFloatValuePlan( const PhysRelativeTime& duration );
    virtual ~W4dFloatValuePlan( void );

    //Return the plan's duration
    const PhysRelativeTime& duration( void ) const;

    //Override to define result as a function of timeOffset.
    //If time is greater than duration, the value at time duration is to be returned.
    virtual MATHEX_SCALAR value( const PhysRelativeTime& timeOffset ) const = 0;

    //True iff the duration of the plan is <= timeOffset
    bool isDone( const PhysRelativeTime& timeOffset ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dFloatValuePlan& t );

protected:
    //set the duration of the plan to newDuration
    void duration( const PhysRelativeTime& newDuration);

private:
    // Operation deliberately revoked
    W4dFloatValuePlan( const W4dFloatValuePlan& );

    // Operation deliberately revoked
    W4dFloatValuePlan& operator =( const W4dFloatValuePlan& );

    // Operation deliberately revoked
    bool operator ==( const W4dFloatValuePlan& );

    // Data members
    PhysRelativeTime duration_; // The maximum time at which the mapping is defined
};

//Provides a reference counted pointer to a W4dFloatValuePlan
typedef CtlCountedPtr< W4dFloatValuePlan > W4dFloatValuePlanPtr;


#endif

/* End VALPLAN.HPP **************************************************/
