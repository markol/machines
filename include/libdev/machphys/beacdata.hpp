/*
 * B E A C D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysBeaconData

    Contains specific data for Comms Beacons.
*/

#ifndef _BEACONDATA_HPP
#define _BEACONDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysBeaconData : public MachPhysConstructionData
{
public:
    MachPhysBeaconData( void );
    MachPhysBeaconData( const MachPhysBeaconData&, const MexTransform3d& );
    ~MachPhysBeaconData( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysBeaconData& t );

private:
	friend class MachPhysData;
    // Operation deliberately revoked
    MachPhysBeaconData( const MachPhysBeaconData& );

    // Operation deliberately revoked
    MachPhysBeaconData& operator =( const MachPhysBeaconData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysBeaconData& );

};


#endif

/* End BEACDATA.HPP *************************************************/
