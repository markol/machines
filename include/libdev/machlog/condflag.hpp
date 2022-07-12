/*
 * C O N D FLAG . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogScenarioFlagCondition

    Condition is true if a named scenario flag has been set at the time the action is parsed.
    ie the condition need only be determined once, since the validity is determined by events in
    a previous scenario.
*/

#ifndef _MACHLOG_CONDFLAG_HPP
#define _MACHLOG_CONDFLAG_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"

class UtlLineTokeniser;

class MachLogScenarioFlagCondition : public SimCondition
// Canonical form revoked
{
public:

	static MachLogScenarioFlagCondition* newFromParser( UtlLineTokeniser* );

    MachLogScenarioFlagCondition( const string& keyName, bool isSet );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogScenarioFlagCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogScenarioFlagCondition );
	PER_FRIEND_READ_WRITE( MachLogScenarioFlagCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    friend ostream& operator <<( ostream& o, const MachLogScenarioFlagCondition& t );

    MachLogScenarioFlagCondition( const MachLogScenarioFlagCondition& );
    MachLogScenarioFlagCondition& operator =( const MachLogScenarioFlagCondition& );

	bool flagSet_;

};

PER_DECLARE_PERSISTENT( MachLogScenarioFlagCondition );

#endif

/* End CONDFLAG.HPP *************************************************/
