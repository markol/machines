/*
 * P U F F D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PuffData

    Store a pointer to a smoke puff, a pointer to an entity plan
    and a time offset at which the puff should come into existance
    and start following the plan.
*/

#ifndef _MACHPHYS_PUFFDATA_HPP
#define _MACHPHYS_PUFFDATA_HPP

#include "base/base.hpp"

class PuffData
{
public:
    //  Constructor only supplied for ctl_vector
    PuffData()
    : pSmokePuff_( NULL),
      pPlan_( NULL )
    {
    }
    
    PuffData(
    	MachPhysSmokePuff* pSmokePuff,
    	W4dEntityPlan* pPlan,
    	const PhysRelativeTime& startTimeOffset )
    : pSmokePuff_( pSmokePuff ),
	  pPlan_( pPlan ),
      startTimeOffset_( startTimeOffset )
    {
    }
    
    ~PuffData(){}

    MachPhysSmokePuff* pSmokePuff() const
    {
        PRE( pSmokePuff_ != NULL );
        
        return pSmokePuff_;
    }
    
	W4dEntityPlan*	pPlan() const
	{
        PRE( pPlan_ != NULL );

		return pPlan_;
	}

    const PhysRelativeTime& startTimeOffset() const
    {
        return startTimeOffset_;
    }
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PuffData& t );

    PER_MEMBER_PERSISTENT_INLINE( PuffData );
    PER_FRIEND_READ_WRITE( PuffData );
    
private:
    // Compiler supplied copy constructor is OK
    // PuffData( const PuffData& );

    // Compiler supplied assignment operator is OK
    // PuffData& operator =( const PuffData& );

    // Operation deliberately revoked
    bool operator ==( const PuffData& );

    MachPhysSmokePuff*   pSmokePuff_;
	W4dEntityPlan*		pPlan_;
    PhysRelativeTime            startTimeOffset_;
    
};


#endif

/* End PUFFDATA.HPP *************************************************/
