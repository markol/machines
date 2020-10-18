/*
 * P E R S I S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/persist.hpp"
#include "phys/accetumb.hpp"
#include "phys/linemoti.hpp"
#include "phys/linetrav.hpp"
#include "phys/timeangl.hpp"
#include "phys/timespin.hpp"
#include "phys/movespin.hpp"

// static
PhysPersistence& PhysPersistence::instance()
{
    static PhysPersistence instance_;
    return instance_;
}

PhysPersistence::PhysPersistence()
{
    PER_REGISTER_DERIVED_CLASS( PhysLinearTravelPlan );
    PER_REGISTER_DERIVED_CLASS( PhysLinearMotionPlan );
    PER_REGISTER_DERIVED_CLASS( PhysAccelerateTumblePlan );
    PER_REGISTER_DERIVED_CLASS( PhysTimedAnglePlan );
    PER_REGISTER_DERIVED_CLASS( PhysTimedSpinPlan );
    PER_REGISTER_DERIVED_CLASS( PhysMoveSpinPlan );

    TEST_INVARIANT;
}

PhysPersistence::~PhysPersistence()
{
    TEST_INVARIANT;

}

void PhysPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysPersistence& t )
{

    o << "PhysPersistence " << (void*)&t << " start" << std::endl;
    o << "PhysPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PERSIST.CPP **************************************************/
