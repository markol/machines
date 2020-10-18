/*
 * S C A V E N G E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/scavenge.hpp"
#include "machphys/private/otherper.hpp"
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "phys/lsclplan.hpp"
#include "phys/sclplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/scalplan.hpp"
#include "machphys/random.hpp"

PER_DEFINE_PERSISTENT( MachPhysScavenger );

//One-time ctor
MachPhysScavenger::MachPhysScavenger()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/scavenge/scavenge.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysScavenger::MachPhysScavenger( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysScavenger::MachPhysScavenger( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysScavenger::~MachPhysScavenger()
{
    TEST_INVARIANT;

}

//static
const MachPhysScavenger& MachPhysScavenger::exemplar()
{
	return MachPhysOtherPersistence::instance().scavengerExemplar();
}

void MachPhysScavenger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysScavenger& t )
{

    o << "MachPhysScavenger " << (void*)&t << " start" << std::endl;
    o << "MachPhysScavenger " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysScavenger::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysScavenger& scavenger )
{
    const W4dEntity& base = scavenger;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysScavenger& scavenger )
{
    W4dEntity& base = scavenger;

    istr >> base;
}

void MachPhysScavenger::startScavenge( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
	//visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);

	entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );

	//scale plan
	const uint nScales = 5;
	PhysRelativeTime interval = duration/( nScales - 1.0 );

	PhysLinearScalarPlan::ScalarVec times;
		times.reserve(nScales -1);

	PhysLinearScalarPlan::ScalarVec scales;
		scales.reserve(nScales);

		for( uint i = 1; i<nScales; ++i )
		{
			PhysRelativeTime time = interval*i;
			ASSERT( time <= duration, "");
			times.push_back(time);
		    scales.push_back(MachPhysRandom::randomDouble(0.8, 1.2 ));
		}

	    scales.push_back(MachPhysRandom::randomDouble(0.8, 1.2 ));

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );

	W4dScalePlanPtr scalePlanPtr = _NEW( W4dGeneralUniformScalePlan( PhysScalarPlanPtr( pPlan ) ) );

	entityPlanForEdit().scalePlan( scalePlanPtr, startTime );
}

/* End SCAVENGE.CPP *************************************************/

