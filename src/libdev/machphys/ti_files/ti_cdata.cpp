/*
 * T I _ C D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
#define _INSTANTIATE_TEMPLATE_CLASSES

#include "machphys/consdata.hpp"

void MachDummyFunctionConstructionData()
{
	static	MachPhysConstructionData::Entrances		dummyEntrances_;
//	static	ctl_vector< MachPhysConstructionData::EntranceData >	dummyEntrances2_;
	static  MachPhysConstructionData::Locations		dummyLocations_;
}

/* End TI_CDATA.CPP *****************************************************/
