/*
 * L I N V P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dLinearFloatValuePlan

    A derived class of W4dFloatValuePlan.
    Implement a linear mapping between time and value.
*/

#ifndef _WORLD4D_LINVPLAN_HPP
#define _WORLD4D_LINVPLAN_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"

#include "world4d/valplan.hpp"

class W4dLinearFloatValuePlan : public W4dFloatValuePlan
{
public:
    W4dLinearFloatValuePlan(
        MATHEX_SCALAR initialValue,
        MATHEX_SCALAR finalValue,
        const PhysRelativeTime& time );
     // PRE( time > 0 );
        
    ~W4dLinearFloatValuePlan( void );

    void add( MATHEX_SCALAR value, const PhysRelativeTime& time );
    // PRE( time > duration() );
    
    virtual MATHEX_SCALAR value( const PhysRelativeTime& timeOffset ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dLinearFloatValuePlan& t );

private:
    // Operation deliberately revoked
    W4dLinearFloatValuePlan( const W4dLinearFloatValuePlan& );

    // Operation deliberately revoked
    W4dLinearFloatValuePlan& operator =( const W4dLinearFloatValuePlan& );

    // Operation deliberately revoked
    bool operator ==( const W4dLinearFloatValuePlan& );

    struct Entry
    {
        PhysRelativeTime        time_;  // Time at which the value is defined
        MATHEX_SCALAR   value_; // The value

        //  This constructor here purely to keep ctl_vector happy
        Entry( void )
        {
        }
        
        Entry( MATHEX_SCALAR value, const PhysRelativeTime& time )
        : value_( value ), time_( time )
        {
        };
    };

    typedef ctl_vector< Entry > Entries;

    Entries entries_;
};


#endif

/* End LINVPLAN.HPP *************************************************/
