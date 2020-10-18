/*
 * T I 3 . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#define _INSTANTIATE_TEMPLATE_CLASSES

#include <utility.hpp>
#include "ctl/list.hpp"
#include "ctl/stack.hpp"
#include "ctl/map.hpp"

#include "phys/cspace2.hpp"

#include "machlog/vmdata.hpp"
#include "string.hpp"

class MachOperation;
class MachLogMachine;
class MachLogConstruction;
class MachLogMineralSite;

void ti3()
{
	//static ctl_stack< ctl_list<MachOperation*> >					dummyOps;
	static ctl_map< unsigned, MachLogMachine*, less<unsigned> >		dummyMachinesRep;

    static ctl_map< PhysConfigSpace2d::PolygonId, pair< MachLogConstruction*, size_t >, less< PhysConfigSpace2d::PolygonId > > dummyMap;

    static ctl_map< string, VoiceMailID, less< string > > dummyMap2;
    static ctl_map< string, VoiceMailType, less< string > > dummyMap3;
	static ctl_map< int, MachLogMineralSite*, less< int > > dummyMap4;

}

/* End TI3.CPP *******************************************************/