#ifndef _MAPPING_HPP
#define	_MAPPING_HPP
/*******************************************************************
 DESCRIPTION: This class contains the static mapping functions
 required to map between Windows specific types (GUID and DPID), and
 their non-Windows equivalents.
 *******************************************************************/


//#include "dplay.h"

#include "ctl/vector.hpp"
#define DPID int
#define GUID int

class NetMappings
{
public:
	static unsigned mapDPIDtoUnsigned( DPID );
	static DPID mapUnsignedtoDPID( unsigned mapUnsigned );
	//PRE(mapUnsigned < currentDPIDS().size());

	//Update the given map index with the new DPID
	static void updateDPID( unsigned mapUnsigned, DPID newMapID );
	//PRE(mapUnsigned < currentDPIDS().size());

	static unsigned mapGUIDtoUnsigned( GUID );
	static GUID mapUnsignedtoGUID( unsigned mapUnsigned );
	//PRE(mapUnsigned < currentGUIDS().size());

private:
	typedef ctl_vector<GUID> GUIDVector;
	static GUIDVector& currentGUIDS();

	typedef ctl_vector<DPID> DPIDVector;
	static DPIDVector& currentDPIDS();
};

#endif //_MAPPING_HPP
