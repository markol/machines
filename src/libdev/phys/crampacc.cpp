/*
 * C R A M P A C C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/crampacc.hpp"
#include "phys/rampacce.hpp"

PhysCompressedRampAcceleration::PhysCompressedRampAcceleration()
:   startSpeed_( 0 ),
    firstAcceleration_( 0 ),
    secondAcceleration_( 0 ),
    totalTime_( 0 ),
    firstAccelerationTime_( 0 ),
    secondAccelerationTime_( 0 )

{

    TEST_INVARIANT;
}

PhysCompressedRampAcceleration::PhysCompressedRampAcceleration( const PhysRampAcceleration& ramp )
:   startSpeed_( ramp.startSpeed() ),
    firstAcceleration_( ramp.firstAcceleration() ),
    secondAcceleration_( ramp.secondAcceleration() ),
    totalTime_( ramp.totalTime() ),
    firstAccelerationTime_( ramp.firstAccelerationTime() ),
    secondAccelerationTime_( ramp.secondAccelerationTime() )
{
}

PhysCompressedRampAcceleration::~PhysCompressedRampAcceleration()
{
    TEST_INVARIANT;

}

void PhysCompressedRampAcceleration::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysCompressedRampAcceleration& t )
{

    o << "PhysCompressedRampAcceleration " << (void*)&t << " start" << std::endl;
    o << "startSpeed_ " << t.startSpeed_;
    o << "firstAcceleration_ " << t.firstAcceleration_;
    o << "secondAcceleration_ " << t.secondAcceleration_;
    o << "totalTime_ " << t.totalTime_;
    o << "firstAccelerationTime_ " << t.firstAccelerationTime_;
    o << "secondAccelerationTime_ " << t.secondAccelerationTime_;
    o << "PhysCompressedRampAcceleration " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CRAMPACC.CPP *************************************************/
