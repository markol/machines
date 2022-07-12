/*
 * S M E L D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSmelterData

    Contains specific data for smelters.
*/

#ifndef _SMELTERDATA_HPP
#define _SMELTERDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysSmelterData : public MachPhysConstructionData
{
public:
    MachPhysSmelterData();
    MachPhysSmelterData( const MachPhysSmelterData&, const MexTransform3d& );
    ~MachPhysSmelterData();

	// Get methods public
	MachPhys::SizeUnits capacity() const;
	MATHEX_SCALAR sqrMineDistance() const;
	
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSmelterData& t );

private:
	friend class MachPhysDataParser;

    // Operation deliberately revoked
    MachPhysSmelterData( const MachPhysSmelterData& );

    // Operation deliberately revoked
    MachPhysSmelterData& operator =( const MachPhysSmelterData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSmelterData& );

	// Set methods private
    void capacity( const MachPhys::SizeUnits& newCapacity );
	void sqrMineDistance( MATHEX_SCALAR );


	MachPhys::SizeUnits				capacity_;
   	MATHEX_SCALAR					mineDistance_;	//Minimum Euclidian distance to closest mine to be valid site
	

};


#endif

/* End SMELDATA.HPP *************************************************/
