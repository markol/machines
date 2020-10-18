
#include "sim/action.hpp"
#include "sim/conditio.hpp"
#include "sim/manager.hpp"
#include "sim/conditim.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( SimAction );


SimAction::SimAction( SimCondition* pCondition, bool enabled )
:	pCondition_( pCondition ),
	conditionKeyName_( pCondition->keyName() ),
	enabled_( enabled )
{
	nextCallBackTime( 0 );
    pCondition_->incrementLinkedActionCount();
}

//virtual
SimAction::~SimAction()
{
    //Decerement the reference count in the associated condition, deleting it if no longer
    //used.
	if( pCondition_ )
    {
        pCondition_->decrementLinkedActionCount();
        if( pCondition_->nLinkedActions() == 0 )
    		_DELETE( pCondition_ );
    }
}

bool SimAction::checkConditionAndDoAction()
{
	if( not enabled() )
		return false;

	if( pCondition_->hasConditionBeenMet() )
	{
		doAction();
		return true;
	}

	nextCallBackTime( pCondition_->recommendedCallBackTimeGap() );

	return false;
}

bool SimAction::enabled() const
{
	return enabled_;
}

void SimAction::enabled( bool newEnabled )
{
	enabled_ = newEnabled;

    if( newEnabled )
    {
        //Ensure the actions get check on next update
		becomeEnabled();
        SimManager::instance().conditionsManager().forceCheckOnNextUpdate();
    }
}

PhysAbsoluteTime SimAction::nextCallBackTime() const
{
	return nextCallBackTime_;
}

void SimAction::nextCallBackTime( const PhysRelativeTime& timeOffset )
{
	nextCallBackTime_ = SimManager::instance().currentTime() + timeOffset;
}

//friend
ostream& operator <<( ostream& o, const SimAction& t )
{
	t.doOutputOperator( o );
	return o;
}

const string& SimAction::conditionKeyName() const
{
	return conditionKeyName_;
}

//virtual
void SimAction::doOutputOperator( ostream& o ) const
{
	o << "SimAction " << (void*)this << std::endl;
	o << "Enabled " << enabled_ << std::endl;
	o << "NextCallBackTime " << nextCallBackTime_ << std::endl;
	o << "Condition " << *pCondition_ << std::endl;
}

void perWrite( PerOstream& ostr, const SimAction& action )
{
	ostr << action.conditionKeyName_;
	ostr << action.pCondition_;
	ostr << action.nextCallBackTime_;
	ostr << action.enabled_;

}

void perRead( PerIstream& istr, SimAction& action )
{
	istr >> action.conditionKeyName_;
	istr >> action.pCondition_;
	istr >> action.nextCallBackTime_;
	istr >> action.enabled_;
}

SimAction::SimAction( PerConstructor )
{
}

void SimAction::becomeEnabled()
{
	pCondition_->doBecomeEnabled();
	doBecomeEnabled();
}

//virtual
void SimAction::doBecomeEnabled()
{
	//default implementation does nothing.
}
