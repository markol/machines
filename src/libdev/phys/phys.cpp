#include "phys/phys.hpp"
#include "device/time.hpp"

// static 
PhysAbsoluteTime Phys::time() 
{ 
	return PhysAbsoluteTime( DevTime::instance().time() );
};

// static 
PhysPathFindingPriority     Phys::defaultPathFindingPriority()
{
    return 0;
}