/*
 * C O N D T I M E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogTimerCondition

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CONDTIME_HPP
#define _MACHLOG_CONDTIME_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"

class UtlLineTokeniser;

class MachLogTimerCondition : public SimCondition
// Canonical form revoked
{
public:

	static MachLogTimerCondition* newFromParser( UtlLineTokeniser* );

    MachLogTimerCondition( const string& keyName, const PhysAbsoluteTime& );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogTimerCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTimerCondition );
	PER_FRIEND_READ_WRITE( MachLogTimerCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;
	virtual void doBecomeEnabled();

private:

    friend ostream& operator <<( ostream& o, const MachLogTimerCondition& t );

    MachLogTimerCondition( const MachLogTimerCondition& );
    MachLogTimerCondition& operator =( const MachLogTimerCondition& );

	PhysAbsoluteTime		fireAtTime_;
	PhysAbsoluteTime		originalFireAtTime_;

};

PER_DECLARE_PERSISTENT( MachLogTimerCondition );

#endif

/* End CONDTIME.HPP *************************************************/
