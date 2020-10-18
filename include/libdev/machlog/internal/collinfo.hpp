/*
 * C O L L I N F O . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachLogCollisionInfo

    Store the collision information when we attempt
    to move a machine through an obstacle
*/

#ifndef _MACHLOG_COLLINFO_HPP
#define _MACHLOG_COLLINFO_HPP

#include "base/base.hpp"
#include "phys/cspace2.hpp"
#include "mathex/point2d.hpp"

class MachPhysMachineMoveInfo;
class MexTransform3d;

class MachLogCollisionInfo
// Memberwise canonical
{
public:
    typedef PhysConfigSpace2d::ObjectId ObjectId;

    MachLogCollisionInfo( void );
    //  POST( not valid() );
    
    MachLogCollisionInfo(
        const MachPhysMachineMoveInfo& info,
        const MexTransform3d& parentTransform,
        ObjectId            collisionObjectId,
        PhysAbsoluteTime    collideTime );
    //  POST( valid() );
    //  POST( collisionTimeValid() );

    MachLogCollisionInfo(
        const MexPoint2d&   collisionPoint,
        ObjectId            collisionObjectId );
    //  POST( valid() );
    //  POST( not collisionTimeValid() );

    ~MachLogCollisionInfo( void );

    const MexPoint2d& collisionPoint() const;
    //  PRE( valid() );

    ObjectId collisionObjectId() const;
    //  PRE( valid() );

    PhysAbsoluteTime collisionTime() const;
    //  PRE( valid() );
    //  PRE( collisionTimeValid() );

    //  true iff one of the non-default constructors has been called
    bool valid() const;

    //  true iff a constructor that sets the collision time has been called
    bool collisionTimeValid() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogCollisionInfo& t );

    // MachLogCollisionInfo( const MachLogCollisionInfo& );
    // MachLogCollisionInfo& operator =( const MachLogCollisionInfo& );

    bool                    valid_;
    bool                    collisionTimeValid_;
    PhysAbsoluteTime        collisionTime_;
    ObjectId                collisionObjectId_;
    MexPoint2d              collisionPoint_;
};


#endif

/* End COLLINFO.HPP *************************************************/
