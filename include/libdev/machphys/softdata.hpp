/*
 * S O F T D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSofwareLabData

    Contains specific data for software labs.
*/

#ifndef _SOFTDATA_HPP
#define _SOFTDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysSoftwareLabData : public MachPhysConstructionData
{
public:
    MachPhysSoftwareLabData( void );
    MachPhysSoftwareLabData( const MachPhysSoftwareLabData&, const MexTransform3d& );
    ~MachPhysSoftwareLabData( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSoftwareLabData& t );

private:
	friend class MachPhysData;
    // Operation deliberately revoked
    MachPhysSoftwareLabData( const MachPhysSoftwareLabData& );

    // Operation deliberately revoked
    MachPhysSoftwareLabData& operator =( const MachPhysSoftwareLabData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSoftwareLabData& );

};


#endif

/* End SOFTDATA.HPP *************************************************/
