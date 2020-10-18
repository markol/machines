/*
 * D A M A G W A V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/damagwav.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "machphys/machphys.hpp"

#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"

#include "system/pathname.hpp"

#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"

#include "mathex/transf3d.hpp"

#include "phys/lsclplan.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

PER_DEFINE_PERSISTENT( MachPhysBurstWave );

//One-time ctor
MachPhysBurstWave::MachPhysBurstWave()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
    readLODFile( SysPathName( "models/destroy/construc/shockwav.lod" ) );

    TEST_INVARIANT;
}

//public ctor
MachPhysBurstWave::MachPhysBurstWave( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysBurstWave::~MachPhysBurstWave()
{
    TEST_INVARIANT;

}

//static
const MachPhysBurstWave& MachPhysBurstWave::exemplar()
{
    return MachPhysOtherPersistence::instance().burstWaveExemplar();
}

void MachPhysBurstWave::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBurstWave& t )
{

    o << "MachPhysBurstWave " << (void*)&t << " start" << std::endl;
    o << "MachPhysBurstWave " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysBurstWave::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysBurstWave::startBurstWave( const PhysAbsoluteTime& startTime,
                                        const PhysRelativeTime& duration,
                                        const MATHEX_SCALAR& fromRadius,
                                        const MATHEX_SCALAR& toRadius,
                                        const MATHEX_SCALAR& zScale )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

    //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Make a simple scale plan
	const MATHEX_SCALAR iniScale = fromRadius/burstWaveDefaultSize();
	const MATHEX_SCALAR endScale = toRadius/burstWaveDefaultSize();

    RenNonUniformScale a(iniScale, iniScale, zScale);
    RenNonUniformScale b(endScale, endScale, zScale);
    W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

    propogateScalePlan( planPtr, startTime, 1);


    //fade out in the last quater of duration
	//RenMaterialVec* pFlashMaterialVec =_CONST_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pFlashMaterialVec =_CONST_CAST(const W4dEntity*, _STATIC_CAST(W4dEntity*, this) )->mesh().mesh()
        ->materialVec().release();
	RenMaterial& matFlash = (*pFlashMaterialVec)[0];

    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(4);
		times.push_back(duration*4.0/16.0);
		times.push_back(duration*7.0/16.0);
		times.push_back(duration*10.0/16.0);
		times.push_back(duration*13.0/16.0);
		times.push_back(duration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(5);
        scales.push_back(1.0);
        scales.push_back(1.0);
        scales.push_back(0.5625);
        scales.push_back(0.25);
        scales.push_back(0.0625);
        scales.push_back(0.0); 			// 1 - 2x + x^2

    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

    W4dMaterialPlanPtr alphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matFlash, pFlashMaterialVec->size(), lineScalarPlanPtr, 1 ) );
    entityPlanForEdit().materialPlan( alphaPlanPtr, startTime );

}

//static
const double& MachPhysBurstWave::burstWaveDefaultSize()
{
	static const double SIZE = 2.0; //defined in its .x file
	return SIZE;
}


void perWrite( PerOstream& ostr, const MachPhysBurstWave& wave )
{
    const W4dEntity& base = wave;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysBurstWave& wave )
{
    W4dEntity& base = wave;

    istr >> base;
}

MachPhysBurstWave::MachPhysBurstWave( PerConstructor c )
:W4dEntity( c )
{
}

/* End DAMAGWAV.CPP *************************************************/

