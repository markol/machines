/*
 * E N T R A N C E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysEntrance

    An entrance to a physical building.
*/

#ifndef _MACHPHYS_ENTRANCE_HPP
#define _MACHPHYS_ENTRANCE_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"

//forward decls
class W4dEntity;
class MachPhysDoor;
class MexVec3;

//orthodox canonical( revoked )
class MachPhysEntrance
{
public:
    //ctor.
    MachPhysEntrance();

    //dtor
    ~MachPhysEntrance( void );

    //Create a door, which has physical rep pDoor. To move from closed to open position
    //traverses through displacement using speed/acceleration specified.
    void addDoor( W4dEntity* pDoor, const MexVec3& displacement,
                  MATHEX_SCALAR speed, MATHEX_SCALAR acceleration );
    //PRE( nDoors() < 2 );

    //Number of doors added
    uint nDoors() const;

    //Initiate the door open/close animation. Return duration. This is counted -
    //each open request increments the counter, each close request decrements it.
    //The door will only be closed if the count is zero
    PhysRelativeTime isOpen( bool doOpen );

    //Return current door state (at conclusion of any current animation)
    bool isOpen() const;

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysEntrance( const MachPhysEntrance& );
    MachPhysEntrance& operator =( const MachPhysEntrance& );
    bool operator ==( const MachPhysEntrance& );

    //data members
    MachPhysDoor* pDoor1_; //First door
    MachPhysDoor* pDoor2_; //Other door

    size_t  openCount_; //  Zero if the door is closed, greater than zero if open
};


#endif

/* End ENTRANCE.HPP *************************************************/
