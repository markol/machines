/*
 * C O N S D A T I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstructionDataImpl

	Implementation class for MachPhysConstructionData.
*/

#ifndef _MACHPHYS_CONSDATI_HPP
#define _MACHPHYS_CONSDATI_HPP

#include "machphys/consdata.hpp"

class MachPhysConstructionDataImpl
{
public:

private:
    friend class MachPhysConstructionData;
	friend class MachPhysDataParser;

    MachPhysStations stations_;
    MachPhysConstructionData::Entrances localEntrances_;
};

#endif