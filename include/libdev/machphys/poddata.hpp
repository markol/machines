/*
 * P O D D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysPodData

    Contains specific data for Missile Pods.
*/

#ifndef _PODDATA_HPP
#define _PODDATA_HPP

#include "machphys/consdata.hpp"
#include "mathex/angle.hpp"

class MexTransform3d;

class MachPhysPodData : public MachPhysConstructionData
{
public:
    MachPhysPodData( void );
    MachPhysPodData( const MachPhysPodData&, const MexTransform3d& );
    ~MachPhysPodData( void );
	
	//Get methods public							Added 6/3/98 CPS							
	MachPhys::SizeUnits capacity( void ) const;
	MachPhys::BuildingMaterialUnits extractionRate( void ) const;

    void CLASS_INVARIANT;

    friend ostream& operator << ( ostream& o, const MachPhysPodData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysPodData( const MachPhysPodData& );

    // Operation deliberately revoked
    MachPhysPodData& operator =( const MachPhysPodData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysPodData& );
	
	//set methods private							Added 6/3/98 CPS
	void capacity( const MachPhys::SizeUnits& );
	void extractionRate( const MachPhys::BuildingMaterialUnits&  );

	MachPhys::SizeUnits				capacity_;
	MachPhys::BuildingMaterialUnits extractionRate_;

};


#endif

/* End PODDATA.HPP *************************************************/
