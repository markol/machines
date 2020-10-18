/*
 * C O L L I N F O . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/collinfo.hpp"
#include "machphys/mcmovinf.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

MachLogCollisionInfo::MachLogCollisionInfo()
: valid_( false ),
  collisionTimeValid_( false )
{

    TEST_INVARIANT;

    POST( not valid() );
}

MachLogCollisionInfo::MachLogCollisionInfo(
    const MachPhysMachineMoveInfo& info,
    const MexTransform3d& parentTransform,
    ObjectId            collisionObjectId,
    PhysAbsoluteTime    collideTime )
: valid_( true ),
  collisionTimeValid_( true ),
  collisionObjectId_( collisionObjectId )
{
    collisionPoint_ = info.transform( collideTime ).position();

    parentTransform.transform( &collisionPoint_ );

    POST( valid() );
    POST( collisionTimeValid() );
}

MachLogCollisionInfo::MachLogCollisionInfo(
    const MexPoint2d&   collisionPoint,
    ObjectId            collisionObjectId )
: valid_( true ),
  collisionTimeValid_( false ),
  collisionObjectId_( collisionObjectId ),
  collisionPoint_( collisionPoint )
{
    POST( valid() );
    POST( not collisionTimeValid() );
}

MachLogCollisionInfo::~MachLogCollisionInfo()
{
    TEST_INVARIANT;

}

const MexPoint2d& MachLogCollisionInfo::collisionPoint() const
{
    PRE( valid() );

    return collisionPoint_;
}

MachLogCollisionInfo::ObjectId MachLogCollisionInfo::collisionObjectId() const
{
    PRE( valid() );

    return collisionObjectId_;
}

PhysAbsoluteTime MachLogCollisionInfo::collisionTime() const
{
    PRE( valid() );
    PRE( collisionTimeValid() );

    return collisionTime_;
}

bool MachLogCollisionInfo::valid() const
{
    return valid_;
}

bool MachLogCollisionInfo::collisionTimeValid() const
{
    return collisionTimeValid_;
}


void MachLogCollisionInfo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogCollisionInfo& t )
{
    if( t.valid_ )
    {
        o << "Collision with object " << t.collisionObjectId_;
        o << " at point " << t.collisionPoint_;
        if( t.collisionTimeValid_ )
            o << " time " << t.collisionTime_;
    }
    else
    {
        o << "Collision info invalid";
    }

    return o;
}

/* End COLLINFO.CPP *************************************************/
