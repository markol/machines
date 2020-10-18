/*
 * W E A P D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysWeaponsLabData

    Contains specific data for weapons labs.
*/

#ifndef _MACHPHYS_WEAPDATA_HPP
#define _MACHPHYS_WEAPDATA_HPP

#include "machphys/consdata.hpp"

class MexTransform3d;

class MachPhysWeaponsLabData : public MachPhysConstructionData
{
public:
    MachPhysWeaponsLabData( void );
    MachPhysWeaponsLabData( const MachPhysWeaponsLabData&, const MexTransform3d& );
    ~MachPhysWeaponsLabData( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWeaponsLabData& t );

private:
	friend class MachPhysData;
    // Operation deliberately revoked
    MachPhysWeaponsLabData( const MachPhysWeaponsLabData& );

    // Operation deliberately revoked
    MachPhysWeaponsLabData& operator =( const MachPhysWeaponsLabData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysWeaponsLabData& );

};


#endif

/* End WEAPDATA.HPP *************************************************/
