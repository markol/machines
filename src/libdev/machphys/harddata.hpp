/*
 * H A R D D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysHardwareLabData

    Contains specific data for hardware labs.
*/

#ifndef _HARDDATA_HPP
#define _HARDDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysHardwareLabData : public MachPhysConstructionData
{
public:
    MachPhysHardwareLabData();
    MachPhysHardwareLabData( const MachPhysHardwareLabData&, const MexTransform3d& );
    ~MachPhysHardwareLabData();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysHardwareLabData& t );

private:
	friend class MachPhysData;
    // Operation deliberately revoked
    MachPhysHardwareLabData( const MachPhysHardwareLabData& );

    // Operation deliberately revoked
    MachPhysHardwareLabData& operator =( const MachPhysHardwareLabData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysHardwareLabData& );

};


#endif

/* End HARDDATA.HPP *************************************************/
