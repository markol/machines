/*
 * E N T Y P L A N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/entyplan.hpp"

#include "mathex/transf3d.hpp"
#include "world4d/entscale.hpp"
#include "world4d/manager.hpp"
#include "world4d/framereg.hpp"
#include "world4d/meshplan.hpp"
#include "world4d/world4d.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/internal/pendplan.hpp"

#include "render/material.hpp"
#include "render/fwmatmap.hpp"
#include "render/scale.hpp"

#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "phys/phys.hpp"
#include "profiler/profiler.hpp"
#include "ctl/list.hpp"

#include "world4d/internal/planfuns.hpp"
//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "world4d/internal/planfuns.ctf"
//#endif
#include "profiler/stktrace.hpp"

PER_DEFINE_PERSISTENT( W4dEntityPlan );

//////////////////////////////////////////////////////////////////////////////////////////
struct W4dEntityPlanImpl
{
	W4dEntityPlanImpl();
	W4dEntityPlanImpl(const W4dEntityPlanImpl&);
	~W4dEntityPlanImpl();

    typedef W4dPendingPlan< PhysMotionPlanPtr >		PendingMotionPlan;
    typedef W4dPendingPlan< W4dVisibilityPlanPtr >	PendingVisibilityPlan;
    typedef W4dPendingPlan< W4dMeshPlanPtr >		PendingMeshPlan;
    typedef W4dPendingPlan< W4dScalePlanPtr >	    PendingScalePlan;
    typedef W4dPendingPlan< W4dUVPlanPtr >			PendingUVPlan;
    typedef W4dPendingPlan< W4dMaterialPlanPtr >	PendingMaterialPlan;

    typedef ctl_list<PendingMotionPlan> 			PendingMotionPlans;
    typedef ctl_list<PendingMeshPlan>				PendingMeshPlans;
    typedef ctl_list<PendingVisibilityPlan>			PendingVisibilityPlans;
    typedef ctl_list<PendingScalePlan> 				PendingScalePlans;
    typedef ctl_list<PendingUVPlan>					PendingUVPlans;
    typedef ctl_list<PendingMaterialPlan>			PendingMaterialPlans;

    PendingMotionPlans*      pAbsoluteMotionPlans_;   //Queued absolute motion plans
    PendingMeshPlans*        pMeshPlans_;             //Queued mesh plans
    PendingVisibilityPlans*  pVisibilityPlans_;       //Queued visibility plans
    PendingScalePlans*       pScalePlans_;            //Queued scale plans
    PendingUVPlans*	         pUVPlans_;               //Queued texture coord plans
    PendingMaterialPlans*    pMaterialPlans_;         //Queued material plans

	//If present, forces all plans to synchronise with a frame-based animation.
    W4dFrameRegulator*		pFrameRegulator_;

    PhysAbsoluteTime    	endTime_;				// Latest finish time for all plans
	bool 					hasTransformPlan_;		// True iff there is a motion plan
	bool 					hasVisibilityPlan_;		// True iff there is a visibility plan

	ostream& writeToStream(ostream&) const;

    PER_MEMBER_PERSISTENT_DEFAULT( W4dEntityPlanImpl );
    PER_FRIEND_READ_WRITE( W4dEntityPlanImpl );
};

PER_DECLARE_PERSISTENT( W4dEntityPlanImpl );
PER_DEFINE_PERSISTENT( W4dEntityPlanImpl );

#define CB_DEPIMPL_TYPEDEF(PimpleClass, TheType)\
	[[gnu::unused]] typedef PimpleClass::TheType TheType;

// De-pimple everything in the class.
#define CB_ENTITYPLAN_DEPIMPL \
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMotionPlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMeshPlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingVisibilityPlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingScalePlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingUVPlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMaterialPlan);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMotionPlans);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMeshPlans);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingVisibilityPlans);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingScalePlans);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingUVPlans);\
	CB_DEPIMPL_TYPEDEF(W4dEntityPlanImpl, PendingMaterialPlans);\
    CB_DEPIMPL(PendingMotionPlans*, pAbsoluteMotionPlans_);\
    CB_DEPIMPL(PendingMeshPlans*, pMeshPlans_);\
    CB_DEPIMPL(PendingVisibilityPlans*, pVisibilityPlans_);\
    CB_DEPIMPL(PendingScalePlans*, pScalePlans_);\
    CB_DEPIMPL(PendingUVPlans*, pUVPlans_);\
    CB_DEPIMPL(PendingMaterialPlans*, pMaterialPlans_);\
    CB_DEPIMPL(W4dFrameRegulator*, pFrameRegulator_);\
    CB_DEPIMPL(PhysAbsoluteTime, endTime_);\
	CB_DEPIMPL(bool, hasTransformPlan_);\
	CB_DEPIMPL(bool, hasVisibilityPlan_);

//////////////////////////////////////////////////////////////////////////////////////////
W4dEntityPlanImpl::W4dEntityPlanImpl():
	hasTransformPlan_( false ),
	hasVisibilityPlan_( false ),
    pAbsoluteMotionPlans_( NULL ),
    pMeshPlans_( NULL ),
    pVisibilityPlans_( NULL ),
    pScalePlans_( NULL ),
    pUVPlans_( NULL ),
    pMaterialPlans_( NULL ),
    pFrameRegulator_( NULL ),
    endTime_( 0.0 )
{
}

W4dEntityPlanImpl::W4dEntityPlanImpl(const W4dEntityPlanImpl& rhs)
:   hasTransformPlan_( rhs.hasTransformPlan_ ),
    hasVisibilityPlan_( rhs.hasVisibilityPlan_ ),
    pAbsoluteMotionPlans_( NULL ),
    pMeshPlans_( NULL ),
    pVisibilityPlans_( NULL ),
    pScalePlans_( NULL ),
    pUVPlans_( NULL ),
    pMaterialPlans_( NULL ),
    pFrameRegulator_( NULL ),
    endTime_( rhs.endTime_ )
{
    if( rhs.pAbsoluteMotionPlans_ )
        pAbsoluteMotionPlans_ = _NEW( PendingMotionPlans( *rhs.pAbsoluteMotionPlans_ ) );

    if( rhs.pMeshPlans_ )
        pMeshPlans_ = _NEW( PendingMeshPlans( *rhs.pMeshPlans_ ) );

    if( rhs.pVisibilityPlans_ )
        pVisibilityPlans_ = _NEW( PendingVisibilityPlans( *rhs.pVisibilityPlans_ ) );

    if( rhs.pScalePlans_ )
        pScalePlans_ = _NEW( PendingScalePlans( *rhs.pScalePlans_ ) );

    if( rhs.pUVPlans_ )
        pUVPlans_ = _NEW( PendingUVPlans( *rhs.pUVPlans_ ) );

    if( rhs.pMaterialPlans_ )
        pMaterialPlans_ = _NEW( PendingMaterialPlans( *rhs.pMaterialPlans_ ) );
}

W4dEntityPlanImpl::~W4dEntityPlanImpl()
{
    _DELETE( pFrameRegulator_ );
    _DELETE( pAbsoluteMotionPlans_ );
    _DELETE( pMeshPlans_ );
    _DELETE( pVisibilityPlans_ );
    _DELETE( pScalePlans_ );
    _DELETE( pUVPlans_ );
    _DELETE( pMaterialPlans_ );
}

void perWrite( PerOstream& ostr, const W4dEntityPlanImpl& pImpl )
{
    //  TBD: Persist the rest of the entity plans
    ostr << pImpl.pAbsoluteMotionPlans_;
    ostr << pImpl.endTime_;
    PER_WRITE_RAW_OBJECT( ostr, pImpl.hasTransformPlan_ );
    PER_WRITE_RAW_OBJECT( ostr, pImpl.hasVisibilityPlan_ );
}

void perRead( PerIstream& istr, W4dEntityPlanImpl& pImpl )
{
    istr >> pImpl.pAbsoluteMotionPlans_;
    istr >> pImpl.endTime_;
    PER_READ_RAW_OBJECT( istr, pImpl.hasTransformPlan_ );
    PER_READ_RAW_OBJECT( istr, pImpl.hasVisibilityPlan_ );
}

//////////////////////////////////////////////////////////////////////////////////////////
W4dEntityPlan::W4dEntityPlan():
	pImpl_(_NEW(W4dEntityPlanImpl))
{
}

W4dEntityPlan::W4dEntityPlan(const W4dEntityPlan& rhs):
	pImpl_(_NEW(W4dEntityPlanImpl(*(rhs.pImpl_))))
{
	PRE(rhs.pImpl_);
}

W4dEntityPlan::~W4dEntityPlan()
{
	_DELETE(pImpl_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void W4dEntityPlan::absoluteMotion( const PhysMotionPlanPtr& motionPlan,
                                    const PhysAbsoluteTime& startTime, uint repetitions,
                                    uint animId  )
{
	CB_ENTITYPLAN_DEPIMPL;

    //Construct the motion plans collection if needed
    if( pAbsoluteMotionPlans_ == NULL )
        pAbsoluteMotionPlans_ = _NEW( PendingMotionPlans );

    //Add the new plan to the list
    pAbsoluteMotionPlans_->push_back( PendingMotionPlan( motionPlan, startTime,
                                                       repetitions, animId ) );

    //Set flag
    hasTransformPlan_ = true;

    endTime_ = std::max( endTime_, startTime + motionPlan->duration() * ( repetitions + 1 ) );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dEntityPlan::PlanState W4dEntityPlan::transform
(
    const PhysAbsoluteTime& actualTime, MexTransform3d* pResult, uint* pNObsolete
) const
{
	CB_ENTITYPLAN_DEPIMPL;
	PRE( hasMotionPlan() )

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    //Get the state of the plans, and the actual plan and time offset to use
    PhysMotionPlanPtr motionPlanPtr;
    PhysRelativeTime timeOffset;
    PlanState state = FindPlan( *pAbsoluteMotionPlans_, time,
                                &motionPlanPtr, &timeOffset, pNObsolete );

    //Get the result
    if( state == DEFINED )
        motionPlanPtr->transform( timeOffset, pResult );

    TEST_INVARIANT;

    return state;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool W4dEntityPlan::isDone( const PhysAbsoluteTime& actualTime ) const
{
	CB_ENTITYPLAN_DEPIMPL;
    TEST_INVARIANT;

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    return time >= endTime();
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::clearAtTime( const PhysAbsoluteTime& time )
{
	CB_ENTITYPLAN_DEPIMPL;
    TEST_INVARIANT;

    //use template function to clear each type of plan
    if( pAbsoluteMotionPlans_ )
        ClearObsoletePlansAtTime( pAbsoluteMotionPlans_, time );

    if( pMeshPlans_)
        ClearObsoletePlansAtTime( pMeshPlans_, time );

    if( pVisibilityPlans_ )
        ClearObsoletePlansAtTime( pVisibilityPlans_, time );

    if( pScalePlans_ )
        ClearObsoletePlansAtTime( pScalePlans_, time );

    if( pUVPlans_ )
        ClearObsoletePlansAtTime( pUVPlans_, time );

    if( pMaterialPlans_)
        ClearObsoletePlansAtTime( pMaterialPlans_, time );

    //Update the flag
    hasTransformPlan_ = pAbsoluteMotionPlans_  and  pAbsoluteMotionPlans_->size() != 0;
    hasVisibilityPlan_ = pVisibilityPlans_  and  pVisibilityPlans_->size() != 0;

    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::clearMotionPlans()
{
	CB_ENTITYPLAN_DEPIMPL;

    //Clear all absolute plans
    if( pAbsoluteMotionPlans_ )
    {
        while( pAbsoluteMotionPlans_->size() != 0 )
            pAbsoluteMotionPlans_->pop_front();
    }

    //Clear flag
    hasTransformPlan_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::clearAbsoluteMotionPlans()
{
    clearMotionPlans();
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::plan( const W4dEntityPlan& rhs, const PhysAbsoluteTime& startTime,
                          uint nRepetitions, uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;
	PRE(rhs.pImpl_);

    //Add each absolute plan, adding the extra time offset
    if( rhs.pImpl_->pAbsoluteMotionPlans_ )
    {
        for( PendingMotionPlans::const_iterator i = rhs.pImpl_->pAbsoluteMotionPlans_->begin();
             i != rhs.pImpl_->pAbsoluteMotionPlans_->end(); ++i )
        {
            const PendingMotionPlan& ri = *i;
            absoluteMotion( ri.planPtr(), ri.startTime() + startTime,
                            (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId );
        }
    }

    //Add each mesh plan, adding the extra time offset
    if( rhs.pImpl_->pMeshPlans_ )
    {
        for( PendingMeshPlans::const_iterator i = rhs.pImpl_->pMeshPlans_->begin();
             i != rhs.pImpl_->pMeshPlans_->end(); ++i )
        {
            const PendingMeshPlan& ri = *i;
            meshPlan( ri.planPtr(), ri.startTime() + startTime,
                            (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId  );
        }
    }

    //Add each material plan, adding the extra time offset
    if( rhs.pImpl_->pMaterialPlans_ )
    {
        for( PendingMaterialPlans::const_iterator i = rhs.pImpl_->pMaterialPlans_->begin();
             i != rhs.pImpl_->pMaterialPlans_->end(); ++i )
        {
            const PendingMaterialPlan& ri = *i;
            materialPlan( ri.planPtr(), ri.startTime() + startTime,
                            (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId  );
        }
    }

    //Add each visibility plan, adding the extra time offset
    if( rhs.pImpl_->pVisibilityPlans_ )
    {
        for( PendingVisibilityPlans::const_iterator i = rhs.pImpl_->pVisibilityPlans_->begin();
             i != rhs.pImpl_->pVisibilityPlans_->end(); ++i )
        {
            const PendingVisibilityPlan& ri = *i;
            visibilityPlan( ri.planPtr(), ri.startTime() + startTime,
                            (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId  );
        }
    }

    //Add each scale plan, adding the extra time offset
    if( rhs.pImpl_->pScalePlans_ )
    {
        for( PendingScalePlans::const_iterator i = rhs.pImpl_->pScalePlans_->begin();
             i != rhs.pImpl_->pScalePlans_->end(); ++i )
        {
            const PendingScalePlan& ri = *i;
            scalePlan( ri.planPtr(), ri.startTime() + startTime,
                            (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId  );
        }
    }

    //Add each texture co-ordinate plan, adding the extra time offset.
    if( rhs.pImpl_->pUVPlans_ )
    {
        for( PendingUVPlans::const_iterator i = rhs.pImpl_->pUVPlans_->begin(); i != rhs.pImpl_->pUVPlans_->end(); ++i)
        {
            const PendingUVPlan& ri = *i;
            uvPlan(ri.planPtr(), ri.startTime() + startTime,
    		       (ri.nRepetitions() + 1) * (nRepetitions + 1) - 1, animId );
        }
    }

    //Update the flags
    hasTransformPlan_ = pAbsoluteMotionPlans_  and  pAbsoluteMotionPlans_->size() != 0;
    hasVisibilityPlan_ = pVisibilityPlans_  and  pVisibilityPlans_->size() != 0;

    //Copy in any frame regulator
    if( pFrameRegulator_ != NULL )
    {
        _DELETE( pFrameRegulator_ );
        pFrameRegulator_ = NULL;
    }

    if( rhs.pImpl_->pFrameRegulator_ != NULL )
    {
        pFrameRegulator_ = _NEW( W4dFrameRegulator( *rhs.pImpl_->pFrameRegulator_ ) );
        pFrameRegulator_->startTime( pFrameRegulator_->startTime() + startTime );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysAbsoluteTime W4dEntityPlan::endTime() const
{
    return pImpl_->endTime_;
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::meshPlan( const W4dMeshPlanPtr& meshPlanPtr, const PhysAbsoluteTime& startTime,
                              uint nRepetitions, uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    //Create the collection if necessary
    if( pMeshPlans_ == NULL )
        pMeshPlans_ = _NEW( PendingMeshPlans );

    //Add the new plan to the list
    pMeshPlans_->push_back( PendingMeshPlan( meshPlanPtr, startTime, nRepetitions, animId ) );

    endTime_ = std::max( endTime_, startTime + meshPlanPtr->duration() * ( nRepetitions + 1 ) );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dEntityPlan::renderMesh( const W4dCamera& camera, MATHEX_SCALAR range, W4dEntity* pEntity,
                                const PhysAbsoluteTime& actualTime,
                                uint* pNObsoletePlans, const W4dComplexity& complexity ) const
{
	CB_ENTITYPLAN_DEPIMPL;

    //Deal with any mesh plans
    if( pMeshPlans_ and pMeshPlans_->size() != 0 )
    {
        //Convert the time to effective time if regulated
        PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                    actualTime );

        //Get the state of the plans, and the actual plan and time offset to use
        W4dMeshPlanPtr planPtr;
        PhysRelativeTime timeOffset;
        PlanState state = FindPlan( *pMeshPlans_, time, &planPtr, &timeOffset, pNObsoletePlans );

        //Render the mesh if defined
        if( state == DEFINED )
            planPtr->renderAtRange( camera, range, pEntity, timeOffset, complexity );
    }

    TEST_INVARIANT;
}

void W4dEntityPlan::materialPlan(
    const W4dMaterialPlanPtr& planPtr,
    const PhysAbsoluteTime& startTime,
    const uint nRepetitions, uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pMaterialPlans_ == NULL )
        pMaterialPlans_ = _NEW( PendingMaterialPlans );

    //Add the new plan to the list
    pMaterialPlans_->push_back( PendingMaterialPlan( planPtr, startTime, nRepetitions, animId ) );

    endTime_ = std::max( endTime_, startTime + planPtr->duration() * ( nRepetitions + 1 ) );

    TEST_INVARIANT;
}

bool W4dEntityPlan::hasMaterialPlan() const
{
    return pImpl_->pMaterialPlans_  and  pImpl_->pMaterialPlans_->size() != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool    W4dEntityPlan::hasVisibilityPlan() const
{
    return pImpl_->hasVisibilityPlan_;
}


void    W4dEntityPlan::visibilityPlan(
    const W4dVisibilityPlanPtr& planPtr,
    const PhysAbsoluteTime& startTime,
    uint repetitions, uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pVisibilityPlans_ == NULL )
        pVisibilityPlans_ = _NEW( PendingVisibilityPlans );

    pVisibilityPlans_->push_back( PendingVisibilityPlan(
        planPtr, startTime, repetitions, animId ) );

    endTime_ = std::max( endTime_, startTime + planPtr->duration() * ( repetitions + 1 ) );

    hasVisibilityPlan_ = true;
}

W4dEntityPlan::PlanState W4dEntityPlan::visible
(
    const PhysAbsoluteTime& actualTime, bool* pVisible, uint* pNObsolete
) const
{
	CB_ENTITYPLAN_DEPIMPL;
    PRE( hasVisibilityPlan() );

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    //Get the state of the plans, and the actual plan and time offset to use
    W4dVisibilityPlanPtr planPtr;
    PhysRelativeTime timeOffset;
    PlanState state = FindPlan( *pVisibilityPlans_, time, &planPtr, &timeOffset, pNObsolete );

    //Get the result
    if( state == DEFINED )
        *pVisible = planPtr->visible( timeOffset );

    TEST_INVARIANT;
    return state;
}

bool    W4dEntityPlan::hasScalePlan() const
{
    return pImpl_->pScalePlans_  and  pImpl_->pScalePlans_->size() > 0;
}

//  Attach a scale plan
void W4dEntityPlan::scalePlan(
    const W4dScalePlanPtr& planPtr,
    const PhysAbsoluteTime& startTime,
    uint repetitions, uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pScalePlans_ == NULL )
        pScalePlans_ = _NEW( PendingScalePlans );

    pScalePlans_->push_back( PendingScalePlan(
        planPtr, startTime, repetitions, animId ) );

    endTime_ = std::max( endTime_, startTime + planPtr->duration() * ( repetitions + 1 ) );
}

W4dEntityPlan::PlanState W4dEntityPlan::scale
(
    const PhysAbsoluteTime& actualTime, W4dEntityScale* pEntityScale, uint* pNObsolete
) const
{
	CB_ENTITYPLAN_DEPIMPL;
    PRE( hasScalePlan() );
    PRE( pEntityScale != NULL );

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    //Get the state of the plans, and the actual plan and time offset to use
    W4dScalePlanPtr planPtr;
    PhysRelativeTime timeOffset;
    PlanState state = FindPlan( *pScalePlans_, time, &planPtr, &timeOffset, pNObsolete );

    //Get the result
    if( state == DEFINED )
    {
        //Have to determine whether uniform or non-uniform scale and use appropriate functions
        W4dScalePlan& scalePlan = *planPtr;
        if( scalePlan.isNonUniform() )
        {
            RenNonUniformScale tempScale(1, 1, 1);
            scalePlan.scale( timeOffset, &tempScale );
            pEntityScale->meshScale( tempScale );
        }
        else
        {
            RenUniformScale tempScale(1);
            scalePlan.scale( timeOffset, &tempScale );
            pEntityScale->meshScale( tempScale );
        }
    }

    return state;
}

//////////////////////////////////////////////////////////////////////////////////////////
// UV plan methods.
bool W4dEntityPlan::hasUVPlan() const
{
	return pImpl_->pUVPlans_  and  pImpl_->pUVPlans_->size() > 0;
}

void W4dEntityPlan::uvPlan
(
	const W4dUVPlanPtr& planPtr,
	const PhysAbsoluteTime& startTime,
	uint repetitions, uint animId
)
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pUVPlans_ == NULL )
        pUVPlans_ = _NEW( PendingUVPlans );

	pUVPlans_->push_back(PendingUVPlan(planPtr, startTime, repetitions, animId ));
    endTime_ = std::max( endTime_, startTime + planPtr->duration() * ( repetitions + 1 ) );
}

W4dEntityPlan::PlanState W4dEntityPlan::uvTransform
(
    const PhysAbsoluteTime& actualTime, W4dLOD lodId,
    Ren::UVTransformPtr* pUVTransformPtr, W4dLOD* pMaxLOD, uint* pNObsolete
) const
{
	CB_ENTITYPLAN_DEPIMPL;
	PRE(hasUVPlan());

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    //Get the state of the plans, and the actual plan and time offset to use
    W4dUVPlanPtr planPtr;
    PhysRelativeTime timeOffset;
    PlanState state = FindPlan( *pUVPlans_, time, &planPtr, &timeOffset, pNObsolete );

    //Get the result
    if( state == DEFINED )
    {
        //Check that the transform is to be applied at the required level of detail
        W4dLOD maxLOD = planPtr->maxLOD();
        *pMaxLOD = maxLOD;
        if( lodId <= maxLOD )
            *pUVTransformPtr = planPtr->transform(timeOffset);
        else
            state = NO_PLANS;
    }

    return state;
}

void W4dEntityPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void W4dEntityPlan::clearMotionPlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pAbsoluteMotionPlans_ )
    {
        while( n--  and  pAbsoluteMotionPlans_->size() != 0 )
            pAbsoluteMotionPlans_->pop_front();

        hasTransformPlan_ = pAbsoluteMotionPlans_->size() != 0;
    }
    else
        hasTransformPlan_ = false;
}

void W4dEntityPlan::clearMeshPlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pMeshPlans_ )
    {
        while( n--  and  pMeshPlans_->size() != 0 )
            pMeshPlans_->pop_front();
    }
}

void W4dEntityPlan::clearVisibilityPlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pVisibilityPlans_ )
    {
        while( n--  and  pVisibilityPlans_->size() != 0 )
            pVisibilityPlans_->pop_front();

        hasVisibilityPlan_ = pVisibilityPlans_->size() != 0;
    }
    else
        hasVisibilityPlan_ = false;
}

void W4dEntityPlan::clearScalePlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pScalePlans_ )
    {
        while( n--  and  pScalePlans_->size() != 0 )
            pScalePlans_->pop_front();
    }
}

void W4dEntityPlan::clearUVPlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pUVPlans_ )
    {
        while( n--  and  pUVPlans_->size() != 0 )
            pUVPlans_->pop_front();
    }
}

void W4dEntityPlan::clearMaterialPlans( uint n )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pMaterialPlans_ )
    {
        while( n--  and  pMaterialPlans_->size() != 0 )
            pMaterialPlans_->pop_front();
    }
}

W4dEntityPlan::PlanState W4dEntityPlan::materialVec
(
    const PhysAbsoluteTime& actualTime, W4dLOD lodId,
    Ren::MaterialVecPtr* pMaterialVecPtr, uint* pNObsolete
) const
{
	CB_ENTITYPLAN_DEPIMPL;
    PRE( hasMaterialPlan() );

    //Convert the time to effective time if regulated
    PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                actualTime );

    //Get the state of the plans, and the actual plan and time offset to use
    W4dMaterialPlanPtr planPtr;
    PhysRelativeTime timeOffset;
    PlanState state = FindPlan( *pMaterialPlans_, time,
                                &planPtr, &timeOffset, pNObsolete );

    //Get the result. Plan may be undefined for requested lod.
    if( state == DEFINED )
    {
        if( planPtr->isLODDefined( lodId ) )
            *pMaterialVecPtr = planPtr->materialVec( timeOffset, lodId );
        else
            state = NO_PLANS;
    }

    TEST_INVARIANT;
    return state;
}

void W4dEntityPlan::clearMeshPlansAtTime( const PhysAbsoluteTime& actualTime )
{
	CB_ENTITYPLAN_DEPIMPL;

    //Check for any mesh plans
    if( hasMeshPlan() )
    {
        //Convert the time to effective time if regulated
        PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                    actualTime );

        //Clear any that are past
        ClearObsoletePlansAtTime( pMeshPlans_, time );
    }
}

void W4dEntityPlan::frameRegulator( const W4dFrameRegulator& frameRegulator )
{
	CB_ENTITYPLAN_DEPIMPL;
    _DELETE( pFrameRegulator_ );
    pFrameRegulator_ = _NEW( W4dFrameRegulator( frameRegulator ) );
}

PhysAbsoluteTime W4dEntityPlan::finishAnimation( uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    //Do the work for each plan type
    PhysAbsoluteTime latestFinishTime = 0.0;
    PhysAbsoluteTime planFinishTime;

    if( hasMotionPlan() )
    {
        planFinishTime = FinishAnimation( pAbsoluteMotionPlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );

        //Update the flag
        hasTransformPlan_ = pAbsoluteMotionPlans_->size() != 0;
    }

    if( hasMeshPlan() )
    {
        planFinishTime = FinishAnimation( pMeshPlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );
    }

    if( hasVisibilityPlan() )
    {
        planFinishTime = FinishAnimation( pVisibilityPlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );
        //Update the flag
	    hasVisibilityPlan_ = pVisibilityPlans_->size() != 0;
    }

    if( hasScalePlan() )
    {
        planFinishTime = FinishAnimation( pScalePlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );
    }

    if( hasUVPlan() )
    {
        planFinishTime = FinishAnimation( pUVPlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );
    }

    if( hasMaterialPlan() )
    {
        planFinishTime = FinishAnimation( pMaterialPlans_, animId );
        latestFinishTime = std::max( latestFinishTime, planFinishTime );
    }

    return latestFinishTime;
}

void W4dEntityPlan::clearAnimation( uint animId )
{
	CB_ENTITYPLAN_DEPIMPL;

    if( pAbsoluteMotionPlans_ )
    {
        ClearAnimation( pAbsoluteMotionPlans_, animId );
        hasTransformPlan_ = pAbsoluteMotionPlans_->size() != 0;
    }
    else
        hasTransformPlan_ = false;

    if( pMeshPlans_)
        ClearAnimation( pMeshPlans_, animId );

    if( pVisibilityPlans_ )
    {
        ClearAnimation( pVisibilityPlans_, animId );
        hasVisibilityPlan_ = pVisibilityPlans_->size() != 0;
    }
    else
        hasVisibilityPlan_ = false;

    if( pScalePlans_ )
        ClearAnimation( pScalePlans_, animId );

    if( pUVPlans_ )
        ClearAnimation( pUVPlans_, animId );

    if( pMaterialPlans_ )
        ClearAnimation( pMaterialPlans_, animId );
}

void W4dEntityPlan::clearMaterialPlansAtTime( const PhysAbsoluteTime& actualTime )
{
	CB_ENTITYPLAN_DEPIMPL;

    //Check for any mesh plans
    if( hasMaterialPlan() )
    {
        //Convert the time to effective time if regulated
        PhysAbsoluteTime time = (pFrameRegulator_ ? pFrameRegulator_->effectiveTime() :
                                                    actualTime );

        //Clear any that are past
        ClearObsoletePlansAtTime( pMaterialPlans_, time );
    }
}

bool W4dEntityPlan::hasMotionPlan() const
{
	return pImpl_->hasTransformPlan_;
}

bool W4dEntityPlan::hasMeshPlan() const
{
	return pImpl_->pMeshPlans_  and  pImpl_->pMeshPlans_->size() != 0;
}

uint W4dEntityPlan::nMaterialPlans() const
{
	return (pImpl_->pMaterialPlans_  ? pImpl_->pMaterialPlans_->size() : 0);
}

ostream& operator <<( ostream& o, const W4dEntityPlan& t )
{
	PRE(t.pImpl_);
	return t.pImpl_->writeToStream(o);
}

ostream& W4dEntityPlanImpl::writeToStream(ostream& o) const
{
    if( pAbsoluteMotionPlans_ )
    {
        o << "Absolute motion plans" << std::endl;
        for( PendingMotionPlans::const_iterator i = pAbsoluteMotionPlans_->begin();
             i != pAbsoluteMotionPlans_->end(); ++i )
        {
            o << "  " << *i << std::endl;
        }
    }

    if( pMeshPlans_ )
    {
        o << "Mesh plans" << std::endl;
        for( PendingMeshPlans::const_iterator i = pMeshPlans_->begin();
             i != pMeshPlans_->end(); ++i )
        {
            o << "  " << *i << std::endl;
        }
    }

    if( pMaterialPlans_ )
    {
        o << "Material plans" << std::endl;
        for( PendingMaterialPlans::const_iterator i = pMaterialPlans_->begin();
             i != pMaterialPlans_->end(); ++i )
        {
            o << "  " << *i << std::endl;
        }
    }

    if( pVisibilityPlans_ )
    {
        o << "Visibility plans" << std::endl;
        for( PendingVisibilityPlans::const_iterator i = pVisibilityPlans_->begin();
             i != pVisibilityPlans_->end(); ++i )
        {
            o << "  " << *i << std::endl;
        }
    }

    if( pScalePlans_ )
    {
        o << "Scale plans" << std::endl;
        for( PendingScalePlans::const_iterator i = pScalePlans_->begin();
             i != pScalePlans_->end(); ++i )
        {
            o << "  " << *i << std::endl;
        }
    }

    if( pUVPlans_ )
    {
        o << "Texture coordinate plans" << std::endl;
        for( PendingUVPlans::const_iterator i = pUVPlans_->begin(); i != pUVPlans_->end(); ++i)
        {
            o << "  " << *i << std::endl;
        }
    }

    return o;
}

void perWrite( PerOstream& ostr, const W4dEntityPlan& plan )
{
    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, W4dEntityPlan& plan )
{
    _DELETE( plan.pImpl_ );
    istr >> plan.pImpl_;
}

/* End ENTYPLAN.CPP *****************************************************/
