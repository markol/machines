/*
 * G A R R D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysGarrisonData

    Contains specific data for Garrisons.
*/

#ifndef _GARRISONDATA_HPP
#define _GARRISONDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysGarrisonData : public MachPhysConstructionData
{
public:
    MachPhysGarrisonData( void );
    MachPhysGarrisonData( const MachPhysGarrisonData&, const MexTransform3d& );
    ~MachPhysGarrisonData( void );
	
	MachPhys::HitPointUnits repairRate( void ) const;

	void repairRate( MachPhys::HitPointUnits newRepairRate );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysGarrisonData& t );

private:
	friend class MachPhysData;
    // Operation deliberately revoked
    MachPhysGarrisonData( const MachPhysGarrisonData& );

    // Operation deliberately revoked
    MachPhysGarrisonData& operator =( const MachPhysGarrisonData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysGarrisonData& );
	
	MachPhys::HitPointUnits repairRate_;

};


#endif

/* End GARRDATA.HPP *************************************************/
