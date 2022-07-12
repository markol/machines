/*
 * P L A N E N T R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCompositePlanEntry

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_PLANENTR_HPP
#define _WORLD4D_PLANENTR_HPP

#include "base/base.hpp"

#include "world4d/world4d.hpp"
#include "world4d/entyplan.hpp"

class W4dCompositePlanEntry
// Canonical form revoked
{
public:
    W4dCompositePlanEntry( W4dLinkId id, W4dEntityPlan plan )
    : id_( id ), plan_( plan )
    {}
    
    ~W4dCompositePlanEntry();

    const W4dLinkId&        id() const
    {
        return id_;
    }
    
    const W4dEntityPlan&    plan() const
    {
        return plan_;
    }

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCompositePlanEntry& t );

    PER_MEMBER_PERSISTENT( W4dCompositePlanEntry );
    PER_FRIEND_READ_WRITE( W4dCompositePlanEntry );

private:
    W4dCompositePlanEntry( const W4dCompositePlanEntry& );
    W4dCompositePlanEntry& operator =( const W4dCompositePlanEntry& );
    bool operator ==( const W4dCompositePlanEntry& );

    W4dLinkId       id_;
    W4dEntityPlan   plan_;
};

PER_DECLARE_PERSISTENT( W4dCompositePlanEntry );

#endif

/* End PLANENTR.HPP *************************************************/
