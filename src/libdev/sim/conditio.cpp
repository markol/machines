#include "sim/conditio.hpp"
#include "sim/action.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( SimCondition );

SimCondition::SimCondition( const string& keyName )
:	keyName_( keyName ),
    nLinkedActions_( 0 ),
    timeConditionLastEvaluated_( 0 ),
    lastResult_( false )
{
}

SimCondition::SimCondition( const string& keyName, const ctl_pvector< SimCondition >& subConditions, SimCondition::BooleanOperator booleanOperator )
:	keyName_( keyName ),
	conditions_( subConditions ),
	booleanOperator_( booleanOperator ),
    nLinkedActions_( 0 ),
    timeConditionLastEvaluated_( 0 ),
    lastResult_( false )
{
    for( ctl_pvector< SimCondition >::iterator it = conditions_.begin(); it != conditions_.end(); ++it )
        (*it)->incrementLinkedActionCount();
}

//virtual
SimCondition::~SimCondition()
{
	while( conditions_.size() )
	{
        SimCondition* pCondition = conditions_.front();
        pCondition->decrementLinkedActionCount();
        if( pCondition->nLinkedActions() == 0 )
    		_DELETE( pCondition );

		conditions_.erase( conditions_.begin() );
	}
}

bool SimCondition::hasConditionBeenMet()
{
	bool result = true;

    //Get the time and see if we just already checked the condition
    PhysAbsoluteTime now = SimManager::instance().currentTime();

    if( timeConditionLastEvaluated_ == now )
        result = lastResult_;
    else
    {
    	if( conditions_.size() and booleanOperator_ == ALL )
    	{
    		for( Conditions::iterator i = conditions_.begin(); i != conditions_.end(); ++i )
    			if( not (*i)->hasConditionBeenMet() )
    				result = false;
    	}
    	else if( conditions_.size() and booleanOperator_ == ANY )
    	{
    		result = false;
    		for( Conditions::iterator i = conditions_.begin(); i != conditions_.end(); ++i )
    			if( (*i)->hasConditionBeenMet() )
    				result = true;
    	}
    	else
    		result = doHasConditionBeenMet();

        //Cache the result at this time
        timeConditionLastEvaluated_ = now;
        lastResult_ = result;
    }

	return result;
}

const string& SimCondition::keyName() const
{
	return keyName_;
}

SimCondition::Conditions&			SimCondition::conditions()
{
	return conditions_;
}

const SimCondition::Conditions&	SimCondition::conditions() const
{
	return conditions_;
}

//friend
ostream& operator <<( ostream& o, const SimCondition& t )
{
	t.doOutputOperator( o );
	return o;
}

//virtual
void SimCondition::doOutputOperator( ostream& o ) const
{
	o << "SimCondition " << (void*)this << std::endl;
}



void perWrite( PerOstream& ostr, const SimCondition& condition )
{
	ostr << condition.keyName_;
	ostr << condition.conditions_;
	ostr << condition.booleanOperator_;
    ostr << condition.nLinkedActions_;
}

void perRead( PerIstream& istr, SimCondition& condition )
{
	istr >> condition.keyName_;
	istr >> condition.conditions_;
	istr >> condition.booleanOperator_;
    istr >> condition.nLinkedActions_;
}

SimCondition::SimCondition( PerConstructor )
{
}

uint SimCondition::nLinkedActions() const
{
    return nLinkedActions_;
}

void SimCondition::incrementLinkedActionCount()
{
    ++nLinkedActions_;
}

void SimCondition::decrementLinkedActionCount()
{
    PRE( nLinkedActions_ != 0 );
    --nLinkedActions_;
}


//virtual
void SimCondition::doBecomeEnabled()
{
	//defualt implementation does nothing
}
