/*
 * B E E B L A S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "machphys/beeblast.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "phys/lsclplan.hpp"
#include "render/matvec.hpp"
#include "render/scale.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/simplsca.hpp"

PER_DEFINE_PERSISTENT( MachPhysBeeBlast );

//public constructor
MachPhysBeeBlast::MachPhysBeeBlast(  W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( exemplar(), pParent, localTransform )
{
	visible ( false );

    TEST_INVARIANT;
}

//one time constructor
MachPhysBeeBlast::MachPhysBeeBlast()
:   W4dComposite( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID )
{
	//Load the mesh data
    readCompositeFile( SysPathName( "models/weapons/bee/chasm.cdf" ) );

    TEST_INVARIANT;
}

//static
const MachPhysBeeBlast& MachPhysBeeBlast::exemplar()
{
    return MachPhysOtherPersistence::instance().beeBlastExemplar();
}

MachPhysBeeBlast::~MachPhysBeeBlast()
{
    TEST_INVARIANT;

}

void MachPhysBeeBlast::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBeeBlast& t )
{

    o << "MachPhysBeeBlast " << (void*)&t << " start" << endl;
    o << "MachPhysBeeBlast " << (void*)&t << " end" << endl;

    return o;
}

//virtual
bool MachPhysBeeBlast::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysBeeBlast::startBeeBlast( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, const MATHEX_SCALAR& startScale )
{
	//visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);

	entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );

	//scale plan
    RenNonUniformScale a(startScale, startScale, startScale);
    RenNonUniformScale b(10*startScale, 10*startScale, 4*startScale);
    W4dScalePlanPtr scalePlanPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

	propogateScalePlan( scalePlanPtr, startTime );

/*
	//alpha plan
	RenMaterialVec* pMaterialVec =_CONST_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();

	uint nMaterials = pMaterialVec->size();
	ASSERT( nMaterials>0, "not enought Materials" );

	if( nMaterials>0 )
	{
		RenMaterial& mat = (*pMaterialVec)[0];

		PhysLinearScalarPlan::ScalarVec times;
			times.reserve(1);
			times.push_back(duration);

		PhysLinearScalarPlan::ScalarVec scales;
			scales.reserve(2);
		    scales.push_back(1.0);
		    scales.push_back(0);

		PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );

		PhysScalarPlanPtr alphaPlanPtr = pPlan;

	    W4dMaterialPlanPtr materialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( mat, nMaterials, alphaPlanPtr, 1 ) );
	    entityPlanForEdit().materialPlan( materialAlphaPlanPtr, startTime );
	}
*/
}


MachPhysBeeBlast::MachPhysBeeBlast( PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysBeeBlast& scorch )
{
    const W4dComposite& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysBeeBlast& scorch )
{
    W4dComposite& base = scorch;

    istr >> base;
}

/* End BEEBLAST.CPP *************************************************/
