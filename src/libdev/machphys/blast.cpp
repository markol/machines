/*
 * B L A S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"
#include "machphys/blast.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "phys/rampacce.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/punch.hpp"

#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"
#include "phys/timeangl.hpp"
#include "phys/lsclplan.hpp"

#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/simplsca.hpp"

#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"

PER_DEFINE_PERSISTENT( MachPhysBlast );

//One-time ctor
MachPhysBlast::MachPhysBlast()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/punch/blast.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysBlast::MachPhysBlast( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysBlast::~MachPhysBlast()
{
    TEST_INVARIANT;

}

//static
const MachPhysBlast& MachPhysBlast::exemplar()
{
    return MachPhysOtherPersistence::instance().blastExemplar();
}

void MachPhysBlast::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBlast& t )
{

    o << "MachPhysBlast " << (void*)&t << " start" << std::endl;
    o << "MachPhysBlast " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysBlast::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysBlast& blast )
{
    const W4dEntity& base = blast;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysBlast& blast )
{
    W4dEntity& base = blast;

    istr >> base;
}

void MachPhysBlast::startBlast( const PhysAbsoluteTime& startTime )
{
	MATHEX_SCALAR frameRate = MachPhysPunchBlast::punchFrameRate();
	PhysRelativeTime blastDuration = 13.0/frameRate;
	PhysRelativeTime blastAlphaDelay = 9.0/frameRate;
    W4dVisibilityPlanPtr blastVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	blastVisibilityPlanPtr->add(false, blastDuration);

	entityPlanForEdit().visibilityPlan(blastVisibilityPlanPtr, startTime );

	// blast scale plan
    W4dScalePlanPtr planPtr( _NEW( W4dSimpleUniformScalePlan( 1, 5, blastDuration ) ) );

	propogateScalePlan( planPtr, startTime, 1);

/*	redefined in the .x file
	//blast motion plan
   	PhysMotionPlan::Times* pTimesList = _NEW( PhysMotionPlan::Times );
   	pTimesList->push_back(blastDuration);

  	PhysMotionPlan::AnglesPtr anglesListPtr = PhysMotionPlan::AnglesPtr( _NEW( PhysMotionPlan::Angles ) );
   	anglesListPtr->push_back(MexDegrees(0));
  	anglesListPtr->push_back(MexDegrees(90));

    MexVec3 newvec(0, 0, 1);
    MexVec3 position( 0,0,0 );

    position = localTransform().position();
  	PhysTimedAnglePlan* pAnglePlan = _NEW(PhysTimedAnglePlan(anglesListPtr,
						 					 PhysMotionPlan::TimesPtr( pTimesList ),
  						 				 newvec,
  						 				 position));

	PhysMotionPlanPtr anglePlanPtr( pAnglePlan );

   	entityPlanForEdit().absoluteMotion(anglePlanPtr, startTime );
*/
    // blast alpha plan
	//RenMaterialVec* pBlastMaterialVec =_CONST_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pBlastMaterialVec =_STATIC_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();
	RenMaterial& matBlast = (*pBlastMaterialVec)[0];
	uint nMaterials = pBlastMaterialVec->size();

	PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(2);
		times.push_back(blastAlphaDelay);
		times.push_back(blastDuration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(3);
		scales.push_back(1.0);
		scales.push_back(1.0);
		scales.push_back(0.0);

    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

    W4dMaterialPlanPtr blastMaterialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matBlast, nMaterials, lineScalarPlanPtr, 1 ) );
    entityPlanForEdit().materialPlan( blastMaterialAlphaPlanPtr, startTime );
}

MachPhysBlast::MachPhysBlast( PerConstructor c )
:W4dEntity( c )
{
}

/* End BLAST.CPP *************************************************/

