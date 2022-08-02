
#ifndef _SIM_CONDITIONS_MANAGER
#define _SIM_CONDITIONS_MANAGER

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"

class SimManager;
class SimCondition;
class SimAction;
class SimConditionsManagerImpl;

class SimConditionsManager
{
public:
	friend class SimManager;
	friend class Simcondition;

	~SimConditionsManager();
	typedef ctl_pvector< SimAction >	Actions;
	const Actions&	actions() const;

    //Ensure the action conditions are checked on next update call, as opposed to the
    //cached earliest required time. Called automatically when an action is enabled.
    void forceCheckOnNextUpdate();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimConditionsManager& t );

private:
	SimConditionsManager();

	//add action.
	void addAction( SimAction* );

	//enable action.
	void enableAction( const string& keyName );

	//remove condition.
	void remove( const string& keyName );

	void update();

	void loadGame();
	void unloadGame();

	SimConditionsManagerImpl*	pImpl_;
};


#endif

