/*
 * L O A D I N G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/loading.hpp"

#include "stdlib/string.hpp"
#include "ctl/countptr.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "phys/lsclplan.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "world4d/entity.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/light.hpp"

#include "machphys/loadligh.hpp"
#include "machphys/loadover.hpp"
#include "machphys/loadunde.hpp"

PER_DEFINE_PERSISTENT( MachPhysResourceLoading );

//public constructor
MachPhysResourceLoading::MachPhysResourceLoading(  W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID )
{
    TEST_INVARIANT;
}

MachPhysResourceLoading::~MachPhysResourceLoading()
{
    TEST_INVARIANT;

}

void MachPhysResourceLoading::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysResourceLoading& t )
{

    o << "MachPhysResourceLoading " << (void*)&t << " start" << std::endl;
    o << "MachPhysResourceLoading " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysResourceLoading::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

static const MATHEX_SCALAR frameRate()
{
	static const MATHEX_SCALAR rate = 22.0;
	return rate;
}

static const MATHEX_SCALAR& defaultSize()
{
	static const MATHEX_SCALAR SIZE_OVER = 3.563743*2.0; //read from .x file
	return SIZE_OVER;
}

static PhysScalarPlanPtr scalarPlanPtr( const PhysRelativeTime& time1, const PhysRelativeTime& time2 )
{
	PRE( time2 > time1 );

	PhysLinearScalarPlan::ScalarVec times;
		times.reserve(2);
		times.push_back(time1);
		times.push_back(time2);

	PhysLinearScalarPlan::ScalarVec scales;
		scales.reserve(3);
	    scales.push_back(1.0);
	    scales.push_back(1.0);
	    scales.push_back(0);

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );

	return PhysScalarPlanPtr( pPlan );
}

const PhysRelativeTime MachPhysResourceLoading::startLoading( const PhysAbsoluteTime& startTime, const MATHEX_SCALAR& size )
{
	MachPhysLoadingUnder*  pUnder = _NEW( MachPhysLoadingUnder( this, MexTransform3d() ) );

	const PhysRelativeTime duration = 22.0/frameRate();

	//visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);

	pUnder->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );

	//scale plan
    const MATHEX_SCALAR a = size/defaultSize();
    const MATHEX_SCALAR c = 2.0*a;
    W4dScalePlanPtr scalePlanPtr( _NEW( W4dSimpleUniformScalePlan( 1, c, duration ) ) );

	pUnder->propogateScalePlan( scalePlanPtr, startTime );

	//alpha plan
	PhysScalarPlanPtr alphaPlanPtr = scalarPlanPtr(	2.0/frameRate(), duration );

	W4dSimpleAlphaPlan::makePlan( pUnder, startTime, alphaPlanPtr, 1 );

	MachPhysLoadingOver*   pOver = _NEW( MachPhysLoadingOver(  this, MexTransform3d() ) );
	pOver->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );
	pOver->propogateScalePlan( scalePlanPtr, startTime );
	W4dSimpleAlphaPlan::makePlan( pOver, startTime, alphaPlanPtr, 1 );


	MachPhysLoadingLight* pLightLoading = _NEW( MachPhysLoadingLight( this, MexTransform3d() ) );
	const PhysRelativeTime lightDuration = 12.0/frameRate();

	//visibility plan
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, lightDuration);

	pLightLoading->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	//scale plan
    const MATHEX_SCALAR b = 0.5;
    W4dScalePlanPtr lightScalePlanPtr( _NEW( W4dSimpleUniformScalePlan( a, b, lightDuration ) ) );

	pLightLoading->propogateScalePlan( lightScalePlanPtr, startTime );

	//alpha plan
	static PhysScalarPlanPtr lightAlphaPlanPtr = scalarPlanPtr(	2.0/frameRate(), lightDuration );
	W4dSimpleAlphaPlan::makePlan( pLightLoading, startTime, lightAlphaPlanPtr, 1 );

	//create a light

	W4dPointLight* pLight = _NEW( W4dPointLight(this, MexVec3(0, 0, 1), 20.0));

	pLight->colour(RenColour(2.0, 2.24, 2.0) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->visible(false);

	//visibility plan
	const PhysRelativeTime pointLightDuration = 10.0/frameRate();

    W4dVisibilityPlanPtr visPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visPlanPtr->add(false, pointLightDuration);

	pLight->entityPlanForEdit().visibilityPlan( visPlanPtr, startTime );

	//intensity plan
	PhysScalarPlanPtr intensityPlanPtr;
	bool justOnce = true;

	if( justOnce )
	{
		justOnce = false;

		PhysLinearScalarPlan::ScalarVec times;
			times.reserve(1);
			times.push_back(pointLightDuration);

		PhysLinearScalarPlan::ScalarVec scales;
			scales.reserve(2);
		    scales.push_back(4.0);
		    scales.push_back(0);

		PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );

		intensityPlanPtr = pPlan;
	}

	pLight->intensityPlan( intensityPlanPtr, startTime);

	return duration;
}


MachPhysResourceLoading::MachPhysResourceLoading( PerConstructor con )
: W4dEntity( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysResourceLoading& scorch )
{
    const W4dEntity& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysResourceLoading& scorch )
{
    W4dEntity& base = scorch;

    istr >> base;
}

/* End LOADING.CPP *************************************************/
