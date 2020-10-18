#include "network/internal/mapping.hpp"

#include "base/base.hpp"


//static
NetMappings::GUIDVector& NetMappings::currentGUIDS()
{
	static GUIDVector currentGUIDS;
	return currentGUIDS;
}

//static
NetMappings::DPIDVector& NetMappings::currentDPIDS()
{
	static DPIDVector currentDPIDS;
	return currentDPIDS;
}

//static
unsigned NetMappings::mapDPIDtoUnsigned( DPID mapDPID )
{
	DPIDVector& currentDPIDS = NetMappings::currentDPIDS();

	DPIDVector::iterator i = currentDPIDS.begin();
	DPIDVector::iterator j = currentDPIDS.end();

	unsigned index = 0;
	bool found = false;

	while(i!=j && !found)
	{
		if(*i == mapDPID)
			found = true;
		else
		{
			++i;
			++index;
		}
	}

	if(!found)
	{
		currentDPIDS.push_back(mapDPID);
		index = currentDPIDS.size() - 1;
	}

	return index;
}

//static
DPID NetMappings::mapUnsignedtoDPID( unsigned mapUnsigned )
{
	PRE(mapUnsigned < currentDPIDS().size());

	DPIDVector& currentDPIDS = NetMappings::currentDPIDS();

	if( mapUnsigned >= currentDPIDS.size() )
		return 0;

	return currentDPIDS[mapUnsigned];
}

//static
void NetMappings::updateDPID( unsigned mapUnsigned, DPID newMapID )
{
	PRE(mapUnsigned < currentDPIDS().size());

	DPIDVector& currentDPIDS = NetMappings::currentDPIDS();

	currentDPIDS[mapUnsigned] = newMapID;
}

//static
unsigned NetMappings::mapGUIDtoUnsigned( GUID mapGUID )
{
	GUIDVector& currentGUIDS = NetMappings::currentGUIDS();

	GUIDVector::iterator i = currentGUIDS.begin();
	GUIDVector::iterator j = currentGUIDS.end();

	unsigned index = 0;
	bool found = false;

	while(i!=j && !found)
	{
		if(*i == mapGUID)
			found = true;
		else
		{
			++i;
			++index;
		}
	}

	if(!found)
	{
		currentGUIDS.push_back(mapGUID);
		index = currentGUIDS.size() - 1;
	}

	return index;
}

//static
GUID NetMappings::mapUnsignedtoGUID( unsigned mapUnsigned )
{
	PRE(mapUnsigned < currentGUIDS().size());

	GUIDVector& currentGUIDS = NetMappings::currentGUIDS();

	return currentGUIDS[mapUnsigned];
}
