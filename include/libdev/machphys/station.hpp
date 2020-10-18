/*
 * S T A T I O N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysStation

    A station within a building. This is a point at which a machine
    can carry out a particular task - research, parking, pickup etc.
*/

#ifndef _MACHPHYS_STATION_HPP
#define _MACHPHYS_STATION_HPP

#include "base/base.hpp"

#include "mathex/point2d.hpp"

class MexTransform3d;

class MachPhysStation
{
public:
    enum Type { PARKING_BAY, RESEARCH_BAY };
    
    //  This constructor is here purely to satisfy ctl_vector
    MachPhysStation() { ASSERT_FAIL( "Illegal constructor" ); }
    
    MachPhysStation( const MexPoint2d& position, Type type );
    MachPhysStation( const MachPhysStation& );
    //  Copy and transform the station
    MachPhysStation( const MachPhysStation&, const MexTransform3d& );
    ~MachPhysStation( void );

    MachPhysStation& operator =( const MachPhysStation& );

    const MexPoint2d& position() const;
    
    Type    type() const;
    
    //  Is this station currently locked?
    bool    locked() const;
    
    //  Set the lock status
    void    lock( bool isLocked );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysStation& t );

private:

    // Operation deliberately revoked
    bool operator ==( const MachPhysStation& );

    MexPoint2d  position_;
    Type        type_;
    bool        locked_;
};


#endif

/* End STATION.HPP **************************************************/
