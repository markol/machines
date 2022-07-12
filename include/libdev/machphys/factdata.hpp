/*
 * F A C T D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysFactoryData

    Contains specific data for smelters.
*/

#ifndef _MACHPHYS_FACTDATA_HPP
#define _MACHPHYS_FACTDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysFactoryData : public MachPhysConstructionData
{
public:
    MachPhysFactoryData();
    MachPhysFactoryData( const MachPhysFactoryData&, const MexTransform3d& );
    ~MachPhysFactoryData();

	//Get mthods public
	MachPhys::BuildingMaterialUnits buildRate() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysFactoryData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysFactoryData( const MachPhysFactoryData& );

    // Operation deliberately revoked
    MachPhysFactoryData& operator =( const MachPhysFactoryData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysFactoryData& );

	//Set mthods public
	void buildRate( const MachPhys::BuildingMaterialUnits& );

	MachPhys::BuildingMaterialUnits 	buildRate_;

};


#endif

/* End FACTDATA.HPP *************************************************/
