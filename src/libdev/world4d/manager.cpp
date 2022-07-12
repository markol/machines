/*
 * M A N A G E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/domain.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/root.hpp"

#ifndef _INLINE
    #include "world4d/manager.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// static
W4dManager& W4dManager::instance()
{
    static W4dManager instance_;
    return instance_;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dManager::W4dManager()
:   currentTime_( 0 ),
    pSceneManager_( NULL ),
    frameNumber_( 0 ),
	currentPassId_( 0 ),
    generalPositionId_( 0 ),
    hasArtificialTime_( false )
{
    TEST_INVARIANT;

    //Constrain order of construction of hidden root and garbage collector, so destruction
    //works. We need the garbage collector to be destructed first.
    //This assumes garbage collector hasn't been invoked yet.
    W4dRoot::hiddenRoot();
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dManager::~W4dManager()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dManager::render()
{
    TEST_INVARIANT;

    //Just render the scene if we have one
    if( pSceneManager_ != NULL )
        pSceneManager_->render();

    ++frameNumber_;
}

size_t W4dManager::frameNumber() const
{
    return frameNumber_;
}

//////////////////////////////////////////////////////////////////////////////////////////
ulong W4dManager::generateRenderPassId()
{
	return ++currentPassId_;
}

void W4dManager::emergencyShutdown()
{
	std::cout << "W4dManager::emergencyShutdown trying to restore screen mode." << std::endl;
}

void W4dManager::renderingSuspended()
{
	std::cout << "W4dManager::renderingSuspended rendering is unavailable." << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const W4dManager& t )
{

    o << "W4dManager " << (void*)&t << " start" << std::endl;
    o << "W4dManager " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dManager::update()
{
    W4dDomain::initialiseNewSubjects();

    //Do any outstanding garbage collection
    W4dGarbageCollector::instance().collect();
}
/* End MANAGER.CPP **************************************************/
