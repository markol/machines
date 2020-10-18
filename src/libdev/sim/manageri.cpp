#include "sim/internal/manageri.hpp"

#include "device/timer.hpp"

#include "sim/process.hpp"
#include "sim/conditim.hpp"

SimManagerImpl::SimManagerImpl()
:   suspended_( false ),
    nextUpdateProcess_( 0 ),
    lastRenderStartTime_( 0 ),
    timer_( *_NEW( DevTimer ) ),
	processesAllDeleted_( true ),
	totalAmountOfPriority_( 0 )
{
    updateEveryCycleActors_.reserve( 24 );
    deadActors_.reserve( 24 );
}

SimManagerImpl::~SimManagerImpl()
{
	//Empty
}

