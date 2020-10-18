/*
 * B E E W A V E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/beewave.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"

#include "system/pathname.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"

#include "mathex/transf3d.hpp"

#include "phys/lsclplan.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

PER_DEFINE_PERSISTENT( MachPhysBeeBombWave );

//One-time ctor
MachPhysBeeBombWave::MachPhysBeeBombWave()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/bee/shockwav.lod" ) );

    TEST_INVARIANT;
}

//public ctor
MachPhysBeeBombWave::MachPhysBeeBombWave( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysBeeBombWave::~MachPhysBeeBombWave()
{
    TEST_INVARIANT;

}

//static
const MachPhysBeeBombWave& MachPhysBeeBombWave::exemplar()
{
    return MachPhysOtherPersistence::instance().beeBombWaveExemplar();
}

void MachPhysBeeBombWave::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBeeBombWave& t )
{

    o << "MachPhysBeeBombWave " << (void*)&t << " start" << std::endl;
    o << "MachPhysBeeBombWave " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysBeeBombWave::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

static const MATHEX_SCALAR defaultRadius()
{
	static const MATHEX_SCALAR RADIUS = 15.5137;
	return RADIUS;
}

static PhysRelativeTime waveExpansionTimeOffset()
{
	static const PhysRelativeTime OFFSET = 1.5;

	return OFFSET;
}

static const  RenNonUniformScale& waveStartScale()
{
    static const RenNonUniformScale startScale(0.1, 0.1, 1);

	return startScale;
}

static const  RenNonUniformScale& waveEndScale()
{
    static const RenNonUniformScale endScale(2.2, 2.2, 1);

	return endScale;
}

static PhysRelativeTime waveDuration()
{
	static const PhysRelativeTime DURATION = 2.5;

	return DURATION;
}

//static
MATHEX_SCALAR MachPhysBeeBombWave::radius( const PhysRelativeTime& timeOffset )
{
	PRE_INFO( timeOffset );
	PRE( timeOffset >= 0 );
	PRE( timeOffset <= 6.0 );

	MATHEX_SCALAR waveRadius;

	if( timeOffset <= waveExpansionTimeOffset() )
		waveRadius = defaultRadius()*waveStartScale().x();
	else
	{
		static const MATHEX_SCALAR rate = defaultRadius()*(waveEndScale().x()-waveStartScale().x())/waveDuration();
		waveRadius = rate*(timeOffset-waveExpansionTimeOffset());
	}

	return waveRadius;
}

const PhysRelativeTime  MachPhysBeeBombWave::startBeeWave( const PhysAbsoluteTime& startTime )
{
    const PhysRelativeTime duration = 4;

    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

    //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Make a simple scale plan
    //RenNonUniformScale a(0.2, 0.2, 1);
    //RenNonUniformScale b(2.2, 2.2, 1);
    //W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, 2.5 ) ) );

    //Propogate thru the current model
    //propogateScalePlan( planPtr, startTime + 1.5);
    static const W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( waveStartScale(), waveEndScale(), waveDuration() ) ) );
    propogateScalePlan( planPtr, startTime + waveExpansionTimeOffset());

	//alpha plan
	//RenMaterialVec* pMaterialVec =_CONST_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pMaterialVec =_STATIC_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();

	uint nMaterials = pMaterialVec->size();
	ASSERT( nMaterials>0, "not enought Materials" );

	if( nMaterials>0 )
	{
		RenMaterial& mat = (*pMaterialVec)[0];

	    PhysLinearScalarPlan::ScalarVec times;
	    	times.reserve(4);
			times.push_back(1.5);
			times.push_back(2);
			times.push_back(3.5);
			times.push_back(4);

	    PhysLinearScalarPlan::ScalarVec scales;
	    	scales.reserve(5);
	        scales.push_back(0);
	        scales.push_back(0);
	        scales.push_back(1);
	        scales.push_back(1);
	        scales.push_back(0.0);

	    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

	    W4dMaterialPlanPtr materialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( mat, nMaterials, lineScalarPlanPtr, 1 ) );
	    entityPlan.materialPlan( materialAlphaPlanPtr, startTime );
	}

	return duration;
}

void perWrite( PerOstream& ostr, const MachPhysBeeBombWave& wave )
{
    const W4dEntity& base = wave;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysBeeBombWave& wave )
{
    W4dEntity& base = wave;

    istr >> base;
}

MachPhysBeeBombWave::MachPhysBeeBombWave( PerConstructor c )
: W4dEntity( c )
{
	// Intentionally Empty
}

/* End BEEWAVE.CPP *************************************************/

