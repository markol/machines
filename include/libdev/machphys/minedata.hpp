/*
 * M I N E D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMineData

    Contains specific data for Mines.
*/

#ifndef _MINEDATA_HPP
#define _MINEDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysMineData : public MachPhysConstructionData
{
public:
    MachPhysMineData( void );
    MachPhysMineData(const MachPhysMineData&, const MexTransform3d& );
    ~MachPhysMineData( void );
	
	//Get methods public
	MachPhys::SizeUnits capacity( void ) const;
	MachPhys::BuildingMaterialUnits extractionRate( void ) const;
	MATHEX_SCALAR sqrMineralDistance( void ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMineData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysMineData( const MachPhysMineData& );

    // Operation deliberately revoked
    MachPhysMineData& operator =( const MachPhysMineData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMineData& );

	//set methods private
	void capacity( const MachPhys::SizeUnits& );
	void extractionRate( const MachPhys::BuildingMaterialUnits&  );
	void sqrMineralDistance( const MATHEX_SCALAR& );

	MachPhys::SizeUnits				capacity_;
	MachPhys::BuildingMaterialUnits extractionRate_;
	MATHEX_SCALAR					mineralDistance_;	//Maximum Euclidian distance to mineral deposit to be valid site

};


#endif

/* End MINEDATA.HPP *************************************************/
