/*
 * E N T Y P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dEntityPlan

    Collates various types of time-based plans such as PhysMotionPlan,
    W4dMeshPlan for a single W4dEntity.
    
    Responsibilities:
    Optionally references a PhysMotionPlan defining the object's absolute local
    transform, with a start time for the plan.
    Optionally references a PhysMotionPlan defining a relative transform for the object,
    to be applied after it's absolute transform, with a start time for the plan.
    Optionally references a W4dMeshPlan defining extra/substitute mesh display for
    the object, with a start time for the plan.
    Optionally references a W4dEntity to which the plans are to be applied during
    a simulation.
*/

#ifndef _WORLD4D_ENTYPLAN_HPP
#define _WORLD4D_ENTYPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "render/render.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "world4d/fwrdplan.hpp"

//Forward refs
class MexTransform3d;
class W4dFrameRegulator;
class W4dEntityScale;
class W4dEntityPlanImpl;
class W4dCamera;
class W4dEntity;
class W4dComplexity;

//orthodox canonical (revoked)
class W4dEntityPlan
{
public:
    //Defines state of plans for various attributes at a given time
    enum PlanState
    {
        NO_PLANS, //No plans defined for the specified attribute
        NOT_STARTED, //Time specified is before first plan begins
        DEFINED, //There is a plan defining the attribute at specified time
    };

    //Constructor initialises to no plans, and no W4dEntity
    W4dEntityPlan( void );

    //Copy ctor
    W4dEntityPlan( const W4dEntityPlan& );

    //Destructor
    ~W4dEntityPlan( void );

    //Modify this plan to use any motion or other plans defined in rhs, applying
    //an additional time offset startTime to each plan.
    //The incoming plan is to be repeated nRepetitions times
    void plan( const W4dEntityPlan& rhs, const PhysAbsoluteTime& startTime,
               uint nRepetitions = 0, uint animId = 0 );

    //  Return the latest finish time for all plans
    PhysRelativeTime    endTime( void ) const;

    //True iff the plan is complete at time time
    bool isDone( const PhysAbsoluteTime& time ) const;

    //Delete any plans completed by time
    void clearAtTime( const PhysAbsoluteTime& time );

    //Clear all plans associated with animation id animId
    void clearAnimation( uint animId );

    //Finish the plans with animation id animId. This sets the repetition count
    //so that each plan with this id will terminate after the current repetition.
    //Returns the time at which all animations will complete.
    PhysAbsoluteTime finishAnimation( uint animId );

    ////////////////////////////////////////////////////////
    // Transform plans

    //Sets the absolute transform plan starting at time startTime
    //The number of additional repetitions of the motion to perform is repetitions.
    void absoluteMotion( const PhysMotionPlanPtr& motionPlan, const PhysAbsoluteTime& startTime,
                         uint repetitions = 0, uint animId = 0 );
    
    //true if an absolute motion plan is defined
    bool hasMotionPlan() const;

    //Returns the state of any plans defining current transform.    
    //If state is DEFINED updates pResult with the value defined at time.
    //pNObsolete returns the count of obsolete plans at time.
    PlanState transform( const PhysAbsoluteTime& time, MexTransform3d* pResult,
                         uint* pNObsolete ) const;
    //PRE( hasMotionPlan() )

    //Clear the first n motion plans
    void clearMotionPlans( uint n );

    //Delete any motion plans
    void clearMotionPlans();

    //Obsolete - use clearMotionPlans()
    void clearAbsoluteMotionPlans();

    ////////////////////////////////////////////////////////
    // Visibility plans

    //  Attach a visibility plan    
    void visibilityPlan(
        const W4dVisibilityPlanPtr& planPtr,
        const PhysAbsoluteTime& startTime,
        uint repetitions = 0, uint animId = 0 );

    //True iff a visibility plan exists
    bool hasVisibilityPlan() const;

    //Returns the state of any plans defining visibility.
    //If state is DEFINED updates pVisible with the value defined at time.
    //pNObsolete returns the count of obsolete plans at time.
    PlanState visible( const PhysAbsoluteTime& time, bool* pVisible,
                       uint* pNObsolete ) const;
    //PRE( hasVisibilityPlan() )

    //Clear the first n visibility plans
    void clearVisibilityPlans( uint n );
        
    ////////////////////////////////////////////////////////
    // Material plans

    bool hasMaterialPlan() const;

    // Attach a material plan starting at time startTime, with specified repetitions
    void materialPlan(
        const W4dMaterialPlanPtr& planPtr,
        const PhysAbsoluteTime& startTime,
        const uint repetitions = 0, uint animId = 0 );

    //Returns the state of any plans defining material override.    
    //If state is DEFINED updates pMaterialVecPtr with the material vector override
    //defined at time, for level of detail lodId.
    //pNObsolete returns the count of obsolete plans at time.
    PlanState materialVec( const PhysAbsoluteTime& time, W4dLOD lodId,
                           Ren::MaterialVecPtr* pMaterialVecPtr, uint* pNObsolete ) const;
    //PRE( hasMaterialPlan() )

    //Clear the first n material plans
    void clearMaterialPlans( uint n );

    //Clear any obsolete material plans at time
    void clearMaterialPlansAtTime( const PhysAbsoluteTime& time );

    //The number of material plans currently defined
    uint nMaterialPlans() const;

    ////////////////////////////////////////////////////////
    // Mesh plans

    //True iff a mesh plan is defined
    bool hasMeshPlan() const;

    //Add meshPlanPtr starting at time startTime to the planned activities.
    //The mesh plan is to be repeated nRepetitions times
    void meshPlan( const W4dMeshPlanPtr& meshPlanPtr, const PhysAbsoluteTime& startTime,
                   uint nRepetitions = 0, uint animId = 0 );

    //Render any mesh plan at time time.
    //pNObsolete returns the count of obsolete plans at time.
    void renderMesh( const W4dCamera& camera, MATHEX_SCALAR range, W4dEntity* pEntity,
                                const PhysAbsoluteTime& time,
                                uint* pNObsolete, const W4dComplexity& complexity ) const;

    //Clear the first n mesh plans
    void clearMeshPlans( uint n );

    //Clear any obsolete mesh plans at time
    void clearMeshPlansAtTime( const PhysAbsoluteTime& time );

    ////////////////////////////////////////////////////////
    //Scale plans

    //True if has a scale plan
    bool hasScalePlan() const;

    //Attach a scale plan    
    void scalePlan( const W4dScalePlanPtr& planPtr, const PhysAbsoluteTime& startTime,
                    uint repetitions = 0, uint animId = 0 );

    //Returns the state of any plans defining scale.
    //If state is DEFINED updates pEntityScale with the value defined at time.
    //pNObsolete returns the count of obsolete plans at time.
    PlanState scale( const PhysAbsoluteTime& time, W4dEntityScale* pEntityScale,
                     uint* pNObsolete ) const;
    //  PRE( hasScalePlan() );

    //Clear the first n scale plans
    void clearScalePlans( uint n );

    ////////////////////////////////////////////////////////
    //Texture coordinate (UV) plans
    
    bool hasUVPlan() const;

    //  Attach a texture co-ordinate animation plan.
    void uvPlan( const W4dUVPlanPtr& planPtr, const PhysAbsoluteTime& startTime,
                 uint repetitions = 0, uint animId = 0 );


    //Returns the state of any plans defining UV transform.
    //The state is for level of detail lodId.
    //If state is DEFINED updates pUVTransformPtr with the value defined at time.
    //Also updates pMaxLOD with the maximum level of detail to which the transform
    //is to be applied.
    //pNObsolete returns the count of obsolete plans at time.
    PlanState uvTransform( const PhysAbsoluteTime& time, W4dLOD lodId,
                           Ren::UVTransformPtr* pUVTransformPtr, W4dLOD* pMaxLOD, uint* pNObsolete ) const;
    //  PRE( hasUVPlan() );

    //Clear the first n UV plans
    void clearUVPlans( uint n );

    ////////////////////////////////////////////////////////
    // Frame regulator

    //Attach a frame regulator to the plan
    void frameRegulator( const W4dFrameRegulator& frameRegulator );

    ////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dEntityPlan& t );

    PER_MEMBER_PERSISTENT_DEFAULT( W4dEntityPlan );
    PER_FRIEND_READ_WRITE( W4dEntityPlan );

private:
    // Operations deliberately revoked
    W4dEntityPlan& operator =( const W4dEntityPlan& );
    bool operator ==( const W4dEntityPlan& );

	W4dEntityPlanImpl*	pImpl_;
};

PER_DECLARE_PERSISTENT( W4dEntityPlan );

#endif

/* End ENTYPLAN.HPP *****************************************************/
