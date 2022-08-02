/*

	SimAction
	ABC for any action code used in higher order libraries.

*/

#ifndef _SIM_ACTION
#define _SIM_ACTION

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "stdlib/string.hpp"

class SimCondition;


class SimAction
{
public:
	SimAction( SimCondition*, bool enabled );
	virtual ~SimAction();
	//returns if it should be kept "alive" - sets the next call back time internally.
	bool checkConditionAndDoAction();

	bool enabled() const;
	void enabled( bool );

	PhysAbsoluteTime nextCallBackTime() const;
	const string& conditionKeyName() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimAction& t );

	//Non virtual mechanism which will call both into the condition::doBecomeEnabled and the virtual doBecomeEnabled
	void becomeEnabled();

	PER_MEMBER_PERSISTENT_ABSTRACT( SimAction );
	PER_FRIEND_READ_WRITE( SimAction );

protected:
	virtual void doOutputOperator( ostream& ) const = 0;

	virtual void doAction() = 0;
	void nextCallBackTime( const PhysRelativeTime& );
	//A base class implementation is provided
	virtual void doBecomeEnabled();

private:

	string 				conditionKeyName_;
	SimCondition*		pCondition_;
	PhysAbsoluteTime	nextCallBackTime_;
	bool				enabled_;

};

PER_DECLARE_PERSISTENT( SimAction );

#endif
