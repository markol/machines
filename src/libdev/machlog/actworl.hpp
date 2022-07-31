/*
 * A C T W I N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogWinOrLoseAction

    This action will set win or lost status according to whether or not the race in question has the highest gross
	score.
*/

#ifndef _MACHLOG_ACTWinOrLose_HPP
#define _MACHLOG_ACTWinOrLose_HPP

#include "base/base.hpp"
#include "sim/action.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogWinOrLoseAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogWinOrLoseAction();
	static MachLogWinOrLoseAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );
	static MachLogWinOrLoseAction* newDynamic( SimCondition*, bool enabled, MachPhys::Race race );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogWinOrLoseAction );
	PER_FRIEND_READ_WRITE( MachLogWinOrLoseAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogWinOrLoseAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogWinOrLoseAction& t );

    MachLogWinOrLoseAction( const MachLogWinOrLoseAction& );
    MachLogWinOrLoseAction& operator =( const MachLogWinOrLoseAction& );

	MachPhys::Race			race_;
};

PER_DECLARE_PERSISTENT( MachLogWinOrLoseAction );

#endif

/* End ACTWinOrLose.HPP *************************************************/
