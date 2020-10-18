
#include "sim/conditim.hpp"
#include "sim/action.hpp"
#include "sim/conditio.hpp"
#include "sim/manager.hpp"

class SimConditionsManagerImpl
{
	//Note all private

	friend class SimConditionsManager;
	SimConditionsManager::Actions	actions_;
    PhysAbsoluteTime earliestEvaluateTime_;

	SimConditionsManagerImpl()
    : earliestEvaluateTime_( 0.0 )
	{
        actions_.reserve( 64 );
	}

	~SimConditionsManagerImpl()
	{
		while( actions_.size() > 0 )
		{
			_DELETE( actions_.front() );
			actions_.erase( actions_.begin() );
		}
	}

};

SimConditionsManager::SimConditionsManager()
:	pImpl_( _NEW( SimConditionsManagerImpl ) )
{
}

SimConditionsManager::~SimConditionsManager()
{
	_DELETE( pImpl_ );
}

void SimConditionsManager::loadGame()
{
	if( pImpl_ )
		_DELETE( pImpl_ );
	pImpl_ = _NEW( SimConditionsManagerImpl );
}

void SimConditionsManager::unloadGame()
{
	if( pImpl_ )
		_DELETE( pImpl_ );
	pImpl_ = _NEW( SimConditionsManagerImpl );
}

void SimConditionsManager::update()
{
    //Check to see if it is time for any of the actions to be checked
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
    if( now >= pImpl_->earliestEvaluateTime_ )
    {
        pImpl_->earliestEvaluateTime_ = now + 3600.0;

    	size_t i = 0;
    	Actions& actions = pImpl_->actions_;
    	HAL_STREAM("SimConditionsManager::update actions_.size " << actions.size() << std::endl );
    	HAL_STREAM(" time now: " << now << std::endl );
    	while( i < actions.size() )
    	{
    		SimAction* pAction = actions[ i ];

    		HAL_STREAM("action.enabled " << pAction->enabled() << " " << pAction->conditionKeyName() << " nextCallBackTime " << pAction->nextCallBackTime() << std::endl );
            bool actionDeleted = false;
    		if( pAction->enabled() )
            {
    			if( pAction->nextCallBackTime() < now  and	pAction->checkConditionAndDoAction() )
        		{
        			_DELETE( pAction );
        			actions.erase( actions.begin() + i );
                    actionDeleted = true;
        		}
                else
                {
                    PhysAbsoluteTime timeForNextCheck = pAction->nextCallBackTime();
                    if( timeForNextCheck < pImpl_->earliestEvaluateTime_ )
                        pImpl_->earliestEvaluateTime_ = timeForNextCheck;
                }
            }

            if( not actionDeleted)
    			++i;
    	}
	}
}

void SimConditionsManager::addAction( SimAction* pNewAction )
{
	HAL_STREAM("SimConditionsManager:;addAction\n" );
	pImpl_->actions_.push_back( pNewAction );
}

void SimConditionsManager::enableAction( const string& keyName )
{
	HAL_STREAM("SimConditionsManager:;enableAction " << keyName << std::endl );
	Actions& actions = pImpl_->actions_;
	for( int i = 0 ; i < actions.size(); ++i )
		if( actions[i]->conditionKeyName() == keyName )
			actions[i]->enabled( true );
}


const SimConditionsManager::Actions&	SimConditionsManager::actions() const
{
	return pImpl_->actions_;
}


void SimConditionsManager::forceCheckOnNextUpdate()
{
    pImpl_->earliestEvaluateTime_ = 0.0;
}
