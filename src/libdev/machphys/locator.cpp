/*
 * L O C A T O R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "phys/rampacce.hpp"
#include "machphys/locator.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"

#include "phys/linemoti.hpp"
#include "phys/motplan.hpp"
#include "phys/lsclplan.hpp"
#include "phys/sclplan.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/scalplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysLocator );

//One-time ctor
MachPhysLocator::MachPhysLocator()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/locate/radial.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysLocator::MachPhysLocator( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysLocator::MachPhysLocator( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysLocator::~MachPhysLocator()
{
    TEST_INVARIANT;

}

//static
const MachPhysLocator& MachPhysLocator::exemplar()
{
	return MachPhysOtherPersistence::instance().locatorExemplar();
}

void MachPhysLocator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLocator& t )
{

    o << "MachPhysLocator " << (void*)&t << " start" << std::endl;
    o << "MachPhysLocator " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLocator::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysLocator& locator )
{
    const W4dEntity& base = locator;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysLocator& locator )
{
    W4dEntity& base = locator;

    istr >> base;
}

void MachPhysLocator::startLocate( const PhysAbsoluteTime& startTime,
                                   const PhysRelativeTime& duration,
                                   const MATHEX_SCALAR& maxSize,
				                   uint nRepetations,
				                   uint animId )
{
	//visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);

	entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime, nRepetations, animId );

	//appear at 0.5*maxSize then scale to maxSize over 0.5 second and repeat
	PhysLinearScalarPlan::ScalarVec times;
		times.reserve(1);
		times.push_back(0.5);

	PhysLinearScalarPlan::ScalarVec scales;
		scales.reserve(2);
	    scales.push_back(maxSize*0.5);
	    scales.push_back(maxSize);

	PhysLinearScalarPlan* pScalarPlan = _NEW( PhysLinearScalarPlan(times, scales) );
	W4dScalePlanPtr scalePlanPtr = _NEW( W4dGeneralUniformScalePlan( PhysScalarPlanPtr( pScalarPlan ) ) );

	const size_t nReps = duration/0.5 + 1;
	entityPlanForEdit().scalePlan( scalePlanPtr, startTime, nReps*nRepetations, animId );

	//spin around the z axis at 2 Hz
    MexTransform3d spinTransform( MexEulerAngles( MexDegrees(180), 0.0, 0.0 ) );

    //Set start and first position transforms
    const MexTransform3d& startPosition = localTransform();
    MexTransform3d endPosition( startPosition );
    endPosition.transform( spinTransform );

    PhysLinearMotionPlan* pSpinPlan =
        _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.5 ) );

    //move back to start position
    pSpinPlan->add( startPosition, 1.0 );

    //Add the plan
    PhysMotionPlanPtr spinPlanPtr( pSpinPlan );
   	entityPlanForEdit().absoluteMotion(spinPlanPtr, startTime, nReps*nRepetations, animId );
}

/* End LOCATOR.CPP *************************************************/

