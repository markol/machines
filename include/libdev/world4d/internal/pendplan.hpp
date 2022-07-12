/*
 * P E N D P L A N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dPendingPlan

    A convenient structure for keeping the details of
    a plan in one place
*/

#ifndef _WORLD4D_PENDPLAN_HPP
#define _WORLD4D_PENDPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

// forward decl
template< class PLAN_TYPE > class W4dPendingPlan;

template< class PLAN_TYPE >
PLAN_TYPE& w4dPendingPlanPlanPtr( W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
PhysAbsoluteTime& w4dPendingPlanStartTime( W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
uint& w4dPendingPlanNRepetitions( W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
uint& w4dPendingPlanAnimId( W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
const PLAN_TYPE& w4dPendingPlanPlanPtr( const W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
const PhysAbsoluteTime& w4dPendingPlanStartTime( const W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
const uint& w4dPendingPlanNRepetitions( const W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
const uint& w4dPendingPlanAnimId( const W4dPendingPlan< PLAN_TYPE >& plan );

template< class PLAN_TYPE >
class W4dPendingPlan
{
public:
    W4dPendingPlan( PLAN_TYPE newPlanPtr, PhysAbsoluteTime newStartTime,
                            uint newNRepetitions, uint animId )
    : planPtr_( newPlanPtr ), startTime_( newStartTime ),
      nRepetitions_( newNRepetitions ),
      animId_( animId )
    {}

    ~W4dPendingPlan()
    {}

    //  These three functions required for STL compatability
    W4dPendingPlan()
    {  }

    bool operator ==( const W4dPendingPlan< PLAN_TYPE >& rhs )
    {
        return startTime_ == rhs.startTime_ and
               nRepetitions_ == rhs.nRepetitions_ and
               planPtr_ == rhs.planPtr_ and
               animId_ == rhs.animId_;
    }

    bool operator <( const W4dPendingPlan< PLAN_TYPE >& rhs )
    {
        return startTime_ < rhs.startTime_;
    }

    //  Default assignment operator is adequate

    PLAN_TYPE   planPtr() const
    {
        return planPtr_;
    }

    PhysAbsoluteTime    startTime() const
    {
        return startTime_;
    }

    uint        nRepetitions() const
    {
        return nRepetitions_;
    }

    void nRepetitions( uint nNewRepetitions )
    {
        nRepetitions_ = nNewRepetitions;
    }

    uint animId() const
    {
        return animId_;
    }

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( W4dPendingPlan );

private:

    PLAN_TYPE           planPtr_;       //The plan
    PhysAbsoluteTime    startTime_;     //Its start time
    uint                nRepetitions_;  //Number of repetitions
    uint                animId_;        //An id associated with this animation

    //  These friend functions exist purely to allow perRead to work without
    //  making perRead a friend function. If perRead was a friend function
    //  we would only be able to instantiate W4dPendingPlan over classes that
    //  are themselves persistent.

    friend  PLAN_TYPE&          w4dPendingPlanPlanPtr<>( W4dPendingPlan< PLAN_TYPE >& );
    friend  PhysAbsoluteTime&   w4dPendingPlanStartTime<>( W4dPendingPlan< PLAN_TYPE >& );
    friend  uint&               w4dPendingPlanNRepetitions<>( W4dPendingPlan< PLAN_TYPE >& );
    friend  uint&               w4dPendingPlanAnimId<>( W4dPendingPlan< PLAN_TYPE >& );

    //  These friend functions exist purely to allow perWrite to work without
    //  making perWrite a friend function. If perWrite was a friend function
    //  we would only be able to instantiate W4dPendingPlan over classes that
    //  are themselves persistent.

    friend  const PLAN_TYPE&          w4dPendingPlanPlanPtr<>( const W4dPendingPlan< PLAN_TYPE >& );
    friend  const PhysAbsoluteTime&   w4dPendingPlanStartTime<>( const W4dPendingPlan< PLAN_TYPE >& );
    friend  const uint&               w4dPendingPlanNRepetitions<>( const W4dPendingPlan< PLAN_TYPE >& );
    friend  const uint&               w4dPendingPlanAnimId<>( const W4dPendingPlan< PLAN_TYPE >& );

    //void CLASS_INVARIANT;

};

PER_READ_WRITE_T1( W4dPendingPlan );
PER_DECLARE_PERSISTENT_T1( W4dPendingPlan );

template< class PLAN_TYPE >
W4dPendingPlan< PLAN_TYPE >::W4dPendingPlan( PerConstructor )
{
}

template< class PLAN_TYPE >
ostream& operator <<( ostream&, const W4dPendingPlan< PLAN_TYPE >& );

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "world4d/internal/pendplan.ctf"
//#endif

#endif

/* End PENDPLAN.HPP *************************************************/
