#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/map.hpp"
#include <string.hpp>

class EnvOrbit;
class EnvSatellite;
class EnvElevationColourTable;

void envDummyFunction2()
{
	ctl_map<string,		EnvElevationColourTable*, less<string> > cluts2;
	ctl_map<string,		EnvSatellite*,			  less<string> > satellites;
	ctl_map<string,		EnvOrbit*,     			  less<string> > orbits;
}
