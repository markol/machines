/*
 * C O M P P L A N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dCompositePlan

    Collates a number of W4dEntityPlans for a W4dComposite, thus defining
    some animation. Optionally has one W4dEntityPlan each for the composite itself,
    and each of its W4dLinks.
*/

#ifndef _COMPPLAN_HPP
#define _COMPPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
//#include <utility.hpp>
#include "stdlib/string.hpp"
#include "phys/phys.hpp"

#include "world4d/world4d.hpp"


//Forward declarations
template <class T> class ctl_list;
class W4dCompositePlanEntry;
class W4dEntityPlan;
class W4dCompositePlanImpl;

//orthodox canonical (revoked)
class W4dCompositePlan
{
public:
    //Collection of entries
    typedef ctl_list<W4dCompositePlanEntry*> Entries;

    //ctor,dtor
    W4dCompositePlan( const string& name );
    ~W4dCompositePlan( void );

    const   string& name( void ) const;

    //Set the plan for the composite itself
    void compositePlan( const W4dEntityPlan& plan );
    //PRE( pCompositePlan == NULL )

    //Add plan for the link with id id
    void linkPlan( W4dLinkId id, const W4dEntityPlan& plan );

    //Export the list of pairs of link id/link plan
    const Entries& entries( void ) const;

    //True iff the plan has a plan for the composite entity
    bool hasCompositePlan( void ) const;

    //Export the plan for the composite entity
    const W4dEntityPlan& compositePlan( void ) const;
    //PRE( hasCompositePlan() )

    //The finishing time of the plan
    PhysRelativeTime finishTime( void ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCompositePlan& t );

    PER_MEMBER_PERSISTENT( W4dCompositePlan );
    PER_FRIEND_READ_WRITE( W4dCompositePlan );

private:
    // Operations deliberately revoked
    W4dCompositePlan( const W4dCompositePlan& );
    W4dCompositePlan& operator =( const W4dCompositePlan& );
    bool operator ==( const W4dCompositePlan& );

    // Data members
    W4dCompositePlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( W4dCompositePlan );

typedef CtlCountedPtr< W4dCompositePlan > W4dCompositePlanPtr;

#endif

/* End COMPPLAN.HPP *************************************************/
