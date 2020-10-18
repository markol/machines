// All the SDLApp members which depend upon PhysMotion... classes.
#include "machines/sdlapp.hpp"
#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/point3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "phys/motplan.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

void SDLApp::moveObject( W4dEntity* pObject )
{
    //Construct start and endposition
    MexPoint3d pos( 0.0, 0.0, 0.0 );

    MexQuaternion rot;
    MexVec3 axis( 0, 0, 1 );
    rot.set( axis, MexDegrees(90) );
    MexTransform3d startPosition;
    MexTransform3d endPosition( pos );
    endPosition.rotation( rot );
    PhysRelativeTime endTime( 5 );

    PhysMotionPlanPtr motionPlanPtr( _NEW( PhysLinearMotionPlan( startPosition,
                                                                 endPosition,
                                                                 endTime ) ) );

    pObject->entityPlanForEdit().absoluteMotion( motionPlanPtr, 10 );
}


