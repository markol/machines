/*
 * A C T W I N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogEnableActionAction

    This action class will enable another action (which must have been initially disabled).
*/

#ifndef _MACHLOG_ACTEnableAction_HPP
#define _MACHLOG_ACTEnableAction_HPP

#include "base/base.hpp"
#include "sim/action.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogEnableActionAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogEnableActionAction();
	static MachLogEnableActionAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );
	static MachLogEnableActionAction* newDynamic( SimCondition*, bool enabled, const string& enableToken );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogEnableActionAction );
	PER_FRIEND_READ_WRITE( MachLogEnableActionAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogEnableActionAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogEnableActionAction& t );

    MachLogEnableActionAction( const MachLogEnableActionAction& );
    MachLogEnableActionAction& operator =( const MachLogEnableActionAction& );

	string		actionConditionKeyName_;
};

PER_DECLARE_PERSISTENT( MachLogEnableActionAction );

#endif

/* End ACTEnableAction.HPP *************************************************/
