#include "machines/sdlapp.hpp"
#include "sim/manager.hpp"

void debugTiming( const char*, bool );

void SDLApp::checkSuspendSimManager()
{
	// For profiling/timing purposes, we can start with time supsended so that
	// we get repeatable positions for every entity in the world.
	if (getenv("SUSPEND_TIME"))
		SimManager::instance().suspend();
}


void SDLApp::performSimCycle()
{
	//Perform a simulation cycle
	debugTiming("SDLApp:: call SimManger", true );
    SimManager::instance().cycle();
	debugTiming("SDLApp:: call SimManger complete", false );
}

void SDLApp::simResetTime()
{
    //Ensure the simulation time starts at zero, rather than the time taken to load all
    //the models
    SimManager::instance().resetTime();
}

void SDLApp::simSuspend()
{
	SimManager::instance().suspend();
}

void SDLApp::simResume()
{
	SimManager::instance().resume();
}
