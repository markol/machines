/*
 * S T A T I O N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysStations

    A collection of MachPhysStation
*/

#ifndef _MACHPHYS_STATIONS_HPP
#define _MACHPHYS_STATIONS_HPP

#include "base/base.hpp"

#include "machphys/station.hpp"
#include "ctl/vector.hpp"

class MexPoint2d;
class MexTransform3d;

class MachPhysStations
{
public:
    MachPhysStations( void );
    MachPhysStations( const MachPhysStations& );
    //  Copy and transform the statiions
    MachPhysStations( const MachPhysStations&, const MexTransform3d& );
    ~MachPhysStations( void );

    MachPhysStations& operator =( const MachPhysStations& );

    size_t  nStations() const;
    
    //  See if a free station of a given type exists. Not that this function does
    //  not lock the station it finds
    bool   freeStation( MachPhysStation::Type type, MachPhysStation** ppStation );
    //  POST( implies( result, not (*ppStation)->locked() ) );
    //  POST( implies( result, (*ppStation)->type() == type ) );

    //true if there is a station within epsilon of position, and if so returns its
    //pointer in ppStation
    bool nearStation( const MexPoint2d& position, MachPhysStation** ppStation );

    void    addStation( const MexPoint2d& position, MachPhysStation::Type type );
    
    const MachPhysStation& station( size_t i ) const;
    // PRE( i < nStations() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysStations& t );

private:

    // Operation deliberately revoked
    bool operator ==( const MachPhysStations& );

    typedef ctl_vector< MachPhysStation >   Stations;
    
    Stations    stations_;
};


#endif

/* End STATIONS.HPP *************************************************/
