/*
 * T I 2 . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

//#define _INSTANTIATE_TEMPLATE_CLASSES
// #define _INSTANTIATE_TEMPLATE_FUNCTIONS

#include "ctl/list.hpp"
#include "ctl/list.ctp"
#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "machlog/creatdat.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachActor;
class MachLogMachine;

class MachLogAggressor;
class MachLogAdministrator;
class MachLogTechnician;
class MachLogConstructor;
class MachLogGeoLocator;
class MachLogResourceCarrier;
class MachLogAPC;
class MachLogSpyLocator;
class MachLogGarrison;
class MachLogHardwareLab;
class MachLogSoftwareLab;
class MachLogFactory;
class MachLogSmelter;
class MachLogMine;
class MachLogMissileEmplacement;
class MachLogOreHolograph;
class MachLogBeacon;
class MachLogPod;
class MachLogRace;
class MachLogController;

void ti2()
{
	static ctl_list< MachActor * > 				dummyObjects;
	static ctl_list< MachLogMachine * > 		dummyMachines;
	static ctl_list< MachLogAggressor * > 		dummyAggressors;
	static ctl_list< MachLogAdministrator * >	dummyAdministrators;
	static ctl_list< MachLogTechnician * > 		dummyTechnicians;
	static ctl_list< MachLogConstructor * >		dummyConstructors;
	static ctl_list< MachLogGeoLocator * > 		dummyGeoLocators;
	static ctl_list< MachLogResourceCarrier * >	dummyResourceCarriers;
	static ctl_list< MachLogAPC * >				dummyAPCs;
	static ctl_list< MachLogSpyLocator * >				dummySpyLocators;
	
	///////////////////////////////

	static ctl_list< MachLogGarrison * >			dummyGarrisons;
	static ctl_list< MachLogHardwareLab * >			dummyHardwareLabs;
	static ctl_list< MachLogSoftwareLab * >			dummySoftwareLabs;
	static ctl_list< MachLogFactory * >				dummyFactories;
	static ctl_list< MachLogSmelter * >				dummySmelters;
	static ctl_list< MachLogMine * >				dummyMines;
	static ctl_list< MachLogMissileEmplacement * >	dummyMissiles;
	static ctl_list< MachLogOreHolograph * >		dummyHolographs;
	static ctl_list< MachLogPod * >					dummyPods;
	static ctl_list< MachLogBeacon * >				dummyBeacons;

	static ctl_list< int >	dummyProductionLine;
	static MachLogGameCreationData::PlayersCreationData							dummy2_;

}

/* //////////////////////////////////////////////////////////////// */

/* End TI.CPP *******************************************************/