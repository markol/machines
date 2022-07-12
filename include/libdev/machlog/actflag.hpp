/*
 * A C T FLAG . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogSetFlagAction

    This action class sets a scenario flag which is saved in the player database
*/

#ifndef _MACHLOG_ACTFLAG_HPP
#define _MACHLOG_ACTFLAG_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "sim/action.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogSetFlagAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogSetFlagAction();
	static MachLogSetFlagAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );
	static MachLogSetFlagAction* newDynamic( SimCondition*, bool enabled, const string& flag );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSetFlagAction );
	PER_FRIEND_READ_WRITE( MachLogSetFlagAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogSetFlagAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogSetFlagAction& t );

    MachLogSetFlagAction( const MachLogSetFlagAction& );
    MachLogSetFlagAction& operator =( const MachLogSetFlagAction& );

	string flagName_; //Name of the flag to be set
};

PER_DECLARE_PERSISTENT( MachLogSetFlagAction );

#endif

/* End ACTFLAG.HPP *************************************************/
