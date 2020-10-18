/*
 * C O M P P L A I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCompositePlanImpl

    Data implementatin object for W4dCompositePlan
*/

#ifndef _WORLD4D_COMPPLAI_HPP
#define _WORLD4D_COMPPLAI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "phys/phys.hpp"

#include "world4d/compplan.hpp"

class W4dCompositePlanEntry;
class W4dEntityPlan;

class W4dCompositePlanImpl
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( W4dCompositePlanImpl );
    PER_FRIEND_READ_WRITE( W4dCompositePlanImpl );

private:
    friend class W4dCompositePlan;

    W4dCompositePlanImpl();

    ~W4dCompositePlanImpl();

    friend ostream& operator <<( ostream& o, const W4dCompositePlanImpl& t );
	friend void perRead( PerIstream& istr, W4dCompositePlan& plan );

    W4dCompositePlanImpl( const W4dCompositePlanImpl& );
    W4dCompositePlanImpl& operator =( const W4dCompositePlanImpl& );

    //data members
    W4dEntityPlan* pCompositePlan_; // The one for the composite itself
    W4dCompositePlan::Entries entries_; // The collection of link id, link plan pairs
    void* pad1_;
    void* pad2_;
    string  name_;
    PhysRelativeTime cachedFinishTime_; //Expensive to compute. -1 indicates needs re-evaluation.
};

PER_DECLARE_PERSISTENT( W4dCompositePlanImpl );


#endif

/* End COMPPLAI.HPP *************************************************/
