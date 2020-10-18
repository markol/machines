/*
 * T I G U I 4. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */



#include "ctl/vector.hpp"
#include "ctl/vector.ctp"


class MachGuiDbElement;
class MachGuiDbPlayer;
class MachGuiDbPlayerScenario;
class MachGuiDbScenario;
class MachGuiDbPlanet;
class MachGuiDbSystem;
class MachGuiDbSavedGame;

static void tiGui4()
{
	static ctl_vector< MachGuiDbElement* > t1;
	static ctl_vector< MachGuiDbPlayer* > t2;
	static ctl_vector< MachGuiDbPlayerScenario* > t3;
	static ctl_vector< MachGuiDbScenario* > t4;
	static ctl_vector< MachGuiDbPlanet* > t5;
	static ctl_vector< MachGuiDbSystem* > t6;
	static ctl_vector< MachGuiDbSavedGame* > t7;
}

/* End TIGUI4.CPP ****************************************************/
