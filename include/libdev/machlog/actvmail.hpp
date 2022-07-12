/*
 * A C T R E I N F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogVoiceMailAction

    This action class can play a verbal email
*/

#ifndef _MACHLOG_ACTVMAIL_HPP
#define _MACHLOG_ACTVMAIL_HPP

#include "base/base.hpp"
#include "sim/action.hpp"
#include "machlog/vmdata.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogVoiceMailAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogVoiceMailAction();
	static MachLogVoiceMailAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogVoiceMailAction );
	PER_FRIEND_READ_WRITE( MachLogVoiceMailAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogVoiceMailAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogVoiceMailAction& t );

    MachLogVoiceMailAction( const MachLogVoiceMailAction& );
    MachLogVoiceMailAction& operator =( const MachLogVoiceMailAction& );

	VoiceMailID				mailID_;
	MachPhys::Race			race_;
};

PER_DECLARE_PERSISTENT( MachLogVoiceMailAction );

#endif

/* End ACTREINF.HPP *************************************************/
