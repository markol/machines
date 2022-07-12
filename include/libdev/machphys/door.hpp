/*
 * D O O R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysDoor

    Represents a single door in an entrance
*/

#ifndef _MACHPHYS_DOOR_HPP
#define _MACHPHYS_DOOR_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "phys/phys.hpp"

//forward decls
class W4dEntity;
class MexVec3;

//Orthodox canonical( revoked )
class MachPhysDoor
{
public:
    //ctor. The door is entity pDoor. The displacement in mvoing from closed
    //to open is dispalcement, and the speed/acceleration to use are specified.
    //Door must be in the closed position.
    MachPhysDoor( W4dEntity* pDoor, const MexVec3& displacement,
                  MATHEX_SCALAR speed, MATHEX_SCALAR acceleration );

    //dtor
    ~MachPhysDoor();

    //The physical door entity
    const W4dEntity& doorEntity() const
    {
        return *pDoor_;
    };

    //Initiate the door open/close animation. Return duration.
    PhysRelativeTime changeState( bool doOpen );

    void CLASS_INVARIANT;

private:
    // Operations deliberately revoked
    MachPhysDoor( const MachPhysDoor& );
    MachPhysDoor& operator =( const MachPhysDoor& );
    bool operator ==( const MachPhysDoor& );

    //data members
    W4dEntity *pDoor_;//The door to move
    MexPoint3d openLocation_; //The location when open
    MexPoint3d closedLocation_; //The location when closed
    MATHEX_SCALAR speed_; //Rate to use when moving
    MATHEX_SCALAR acceleration_;//The rate of change of the rate
};


#endif

/* End DOOR.HPP *****************************************************/
