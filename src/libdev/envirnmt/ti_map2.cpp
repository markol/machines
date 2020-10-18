#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/map.hpp"
#include "mathex/angle.hpp"

class EnvElevationColourTable;

void envDummyFunction3()
{
	ctl_map<MexDegrees, EnvElevationColourTable*, less<MexDegrees> > cluts1;
}
