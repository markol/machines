#ifndef _SIM_MANAGER_INTERNAL
#define _SIM_MANAGER_INTERNAL

#include "ctl/vector.hpp"

#include "device/timer.hpp"

#include "sim/evdiary.hpp"
#include "sim/sim.hpp"

class SimConditionsManager;
class SimProcess;

class SimManagerImpl
{
public:
	SimManagerImpl();
	~SimManagerImpl();
    // Data members
    typedef ctl_vector<SimProcess*> SimProcesses;
    SimProcesses processes_; // Collection of all SimProcesses
    size_t nextUpdateProcess_; //Index into processes_ of next one to update;
    PhysAbsoluteTime currentTime_; //Current simulation time
    PhysAbsoluteTime devStartTime_; //Time at which simulation started
    PhysAbsoluteTime lastRenderStartTime_; //Time at which last render pass started
    SimEventDiary diary_; // Diary for storing discrete events
    bool suspended_; //True iff simulation activities suspended and time
                        //not advancing
    DevTimer& timer_; //Tracks simulation time
	bool	processesAllDeleted_;

    SimActors    		updateEveryCycleActors_;    
	SimPriority			totalAmountOfPriority_;
	SimConditionsManager*	pConditionsManager_;
    SimActors    		deadActors_;    //Actors waiting to be deleted
};

#endif //_SIM_MANAGER_INTERNAL
