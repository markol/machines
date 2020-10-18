/*
 * PCH ML . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */


#define PRODUCTION

//#include "stdlib/tipch.hpp"

#include "mathex/point2d.hpp"
#include "phys/cspace2.hpp"

#include "machlog/artefact.hpp"
#include "machlog/internal/artsdata.hpp"

//class MexPoint2d;
//class MachLogArtefactSubType;
//class MachLogArtefactLinkData;

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
class W4dEntity;
class PhysConfigSpace2d;
class MachLogEntrance;
class MachLogProductionUnit;

#pragma off(unreferenced);

static void tiPch()
{
//	static StdPrecompiledHeader< MexPoint2d > pch_ml_dummy1;


	static StdPrecompiledHeader< MexPoint2d >	dummyPath;
    static StdPrecompiledHeader< MachLogArtefactSubType > dummySubTypes;
    static StdPrecompiledHeader< MachLogArtefactLinkData > dummyLinkDatas;

	static StdPrecompiledHeader< MachActor * > 				dummyObjects;
	static StdPrecompiledHeader< MachLogMachine * > 		dummyMachines;
	static StdPrecompiledHeader< MachLogAggressor * > 		dummyAggressors;
	static StdPrecompiledHeader< MachLogAdministrator * >	dummyAdministrators;
	static StdPrecompiledHeader< MachLogTechnician * > 		dummyTechnicians;
	static StdPrecompiledHeader< MachLogConstructor * >		dummyConstructors;
	static StdPrecompiledHeader< MachLogGeoLocator * > 		dummyGeoLocators;
	static StdPrecompiledHeader< MachLogResourceCarrier * >	dummyResourceCarriers;
	static StdPrecompiledHeader< MachLogAPC * >				dummyAPCs;
	static StdPrecompiledHeader< MachLogSpyLocator * >				dummySpyLocators;

	///////////////////////////////

	static StdPrecompiledHeader< MachLogGarrison * >			dummyGarrisons;
	static StdPrecompiledHeader< MachLogHardwareLab * >			dummyHardwareLabs;
	static StdPrecompiledHeader< MachLogSoftwareLab * >			dummySoftwareLabs;
	static StdPrecompiledHeader< MachLogFactory * >				dummyFactories;
	static StdPrecompiledHeader< MachLogSmelter * >				dummySmelters;
	static StdPrecompiledHeader< MachLogMine * >				dummyMines;
	static StdPrecompiledHeader< MachLogMissileEmplacement * >	dummyMissiles;
	static StdPrecompiledHeader< MachLogOreHolograph * >		dummyHolographs;
	static StdPrecompiledHeader< MachLogPod * >					dummyPods;
	static StdPrecompiledHeader< MachLogBeacon * >				dummyBeacons;

	static StdPrecompiledHeader< int >	dummyProductionLine;
    static StdPrecompiledHeader< PhysConfigSpace2d::PolygonId > dummyPolygonId;
	static Stdprecompiledheader< std::pair< MachLogConstruction*, size_t > > dummyPairConstructionInt;
	static Stdprecompiledheader< ctl_vector< std::pair< W4dEntity*, PhysConfigSpace2d* > > > dummyVectorPairEntityConfigSpace;
	static StdPrecompiledHeader< W4dEntity* >		dummyW4dEntites;
	static StdPrecompiledHeader< PhysConfigSpace2d* >					dummyConfigSpaces;
	static StdPrecompiledHeader< MachLogEntrance* >				dummyEntrances;
	static StdPrecompiledHeader< MachLogProductionUnit* >				dummyProductionUnits;

}

#pragma on(unreferenced);
