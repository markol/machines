/*
 * C O N D F I R E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogFiredAtCondition

    This conditon code checks for one race having fired at another.

*/

#ifndef _MACHLOG_CONDFIRE_HPP
#define _MACHLOG_CONDFIRE_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogFiredAtCondition : public SimCondition
// Canonical form revoked
{
public:
	static MachLogFiredAtCondition* newFromParser( UtlLineTokeniser* );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogFiredAtCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogFiredAtCondition );
	PER_FRIEND_READ_WRITE( MachLogFiredAtCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogFiredAtCondition( const string& keyName, MachPhys::Race firingRace, MachPhys::Race firedAtRace );

    friend ostream& operator <<( ostream& o, const MachLogFiredAtCondition& t );

    MachLogFiredAtCondition( const MachLogFiredAtCondition& );
    MachLogFiredAtCondition& operator =( const MachLogFiredAtCondition& );

	MachPhys::Race								firingRace_;
	MachPhys::Race								firedAtRace_;
};

PER_DECLARE_PERSISTENT( MachLogFiredAtCondition );

#endif

/* End CONDTIME.HPP *************************************************/
