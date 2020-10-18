/*
 * D O O R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "phys/linetrav.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "sim/manager.hpp"

#include "machphys/door.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysDoor::MachPhysDoor( W4dEntity* pDoor, const MexVec3& displacement,
                  MATHEX_SCALAR speed, MATHEX_SCALAR acceleration )
:   pDoor_( pDoor ),
    speed_( speed ),
    acceleration_( acceleration ),
    closedLocation_( pDoor->localTransform().position() )
{
    PRE( pDoor != NULL );
    PRE( speed > 0 );
    PRE( acceleration > 0 );

    //Compute open location
    openLocation_ = closedLocation_;
    openLocation_ += displacement;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysDoor::~MachPhysDoor()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysDoor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachPhysDoor::changeState( bool doOpen )
{
    //Create a motion plan for the move. First create a collection of the 2 transforms.
    PhysMotionPlan::Transforms* pTransforms = _NEW( PhysMotionPlan::Transforms );
    pTransforms->reserve( 2 );

    MexTransform3d doorTransform( pDoor_->localTransform() );

    //doorTransform.position( doOpen ? closedLocation_ : openLocation_ );
    pTransforms->push_back( doorTransform );

    doorTransform.position( doOpen ? openLocation_ : closedLocation_ );
    pTransforms->push_back( doorTransform );

    //Now create the plan
    PhysLinearTravelPlan* pPlan =
        _NEW( PhysLinearTravelPlan( PhysMotionPlan::TransformsPtr( pTransforms ),
                                    0, speed_, acceleration_, acceleration_,
                                    0, 1, 1, 1, true ) );

    //Apply it to the entity
    pDoor_->entityPlanForEdit().absoluteMotion( PhysMotionPlanPtr( pPlan ),
                                                SimManager::instance().currentTime() );

    return pPlan->duration();
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End DOOR.CPP *****************************************************/
