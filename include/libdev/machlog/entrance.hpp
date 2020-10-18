/*
 * E N T R A N C E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogEntrance

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_ENTRANCE_HPP
#define _MACHLOG_ENTRANCE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/consdata.hpp"

#include "phys/cspace2.hpp"

class MachLogConstruction;

class MachLogEntrance
{
public:
    MachLogEntrance(
        const MachPhysConstructionData::EntranceData& physEntrance,
        size_t  entranceNum,
        MachLogConstruction* pConstruction,
        PhysConfigSpace2d* pInteriorConfigSpace );

    ~MachLogEntrance( void );

    //  Try and lock this entrance, return true if successful
    bool    lock();
    //  POST( locked() );
    
    //  Unlock the entrance
    void    unlock();
    // PRE( locked() );
    // POST( not locked() );
    
    //  Return true if the entrance is locked
    bool    locked() const;

    PhysConfigSpace2d::PolygonId    interiorPolygonId() const;
    PhysConfigSpace2d::PolygonId    exteriorPolygonId() const;
    
    //The owning construction
    MachLogConstruction& construction() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogEntrance& t );

	PER_MEMBER_PERSISTENT( MachLogEntrance );
	PER_FRIEND_READ_WRITE( MachLogEntrance );

private:
    // Operation deliberately revoked
    MachLogEntrance( const MachLogEntrance& );

    // Operation deliberately revoked
    MachLogEntrance& operator =( const MachLogEntrance& );

    // Operation deliberately revoked
    bool operator ==( const MachLogEntrance& );

    bool    locked_;

    PhysConfigSpace2d::PolygonId    interiorPolygonId_;
    PhysConfigSpace2d::PolygonId    exteriorPolygonId_;
    MachLogConstruction* pConstruction_; //Back pointer to owning construction
};

PER_DECLARE_PERSISTENT( MachLogEntrance );

#endif

/* End ENTRANCE.HPP *************************************************/
