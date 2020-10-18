/*

	SimCondition
	This class is the abstract base class for any condition/action mechanisms
	That might be used in higher order libraries.

*/



#ifndef _SIM_CONDITIONS
#define _SIM_CONDITIONS

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "phys/phys.hpp"
#include "stdlib/string.hpp"


class SimAction;

class SimCondition
{
public:
	typedef ctl_pvector< SimCondition >		Conditions;
	enum BooleanOperator { ANY, ALL };

	SimCondition( const string& keyName );
	SimCondition( const string& keyName, const ctl_pvector< SimCondition >& subConditions, BooleanOperator );

	virtual ~SimCondition();

	const string& keyName() const;

    //Current number of actions depending on this condition
    uint nLinkedActions() const;

    //Adjust the count of linked actions. Normally only called by the SimAction class.
	//The interface is public to allow other things to manipulate the counts directly.
    void incrementLinkedActionCount();
    void decrementLinkedActionCount();

	//returns true false dependant on doHasConditionBeenMet (virtual)
	//TBD: non-const due to crappy implementation detail of conditions vector (no const_iterators).
	bool hasConditionBeenMet();
	//the action is the thing with the call back time but the condition knows how long its callbacks should be
	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const = 0;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimCondition& t );

	PER_MEMBER_PERSISTENT_ABSTRACT( SimCondition );
	PER_FRIEND_READ_WRITE( SimCondition );

protected:
	//if the user streams a SimConditon then we need a virtual mechanism to output the most derived class.
	virtual void doOutputOperator( ostream& ) const = 0;

	virtual bool 	doHasConditionBeenMet() const = 0;
	//derived classes need to be able to see the sub conditions if they are there - potentially in a non-const version
	Conditions&			conditions();
	const Conditions&	conditions() const;

	//A base class implementation is provided and will do nothing. Called from the associated Action when it becomes
	//enabled. (i.e enable is specifically called).
	virtual void doBecomeEnabled();


private:
    friend class SimAction;

	string			keyName_;
	//conditions can have sub conditions which must be met before the parent condition
	//is met.
	Conditions		conditions_;
	//the boolean operator is only important if there are sub conditions (the operator describes how they are combined).
	BooleanOperator	booleanOperator_;

    uint nLinkedActions_; //Number of actions referencing this condition
    PhysAbsoluteTime timeConditionLastEvaluated_; //When we last checked the result
    bool lastResult_; //The answer at that time
};

PER_DECLARE_PERSISTENT( SimCondition );
PER_ENUM_PERSISTENT( SimCondition::BooleanOperator );


#endif
