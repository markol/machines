/*
 * E L E C T R O . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/electro.hpp"
#include "machphys/private/otherper.hpp"
#include "ctl/vector.hpp"
#include "phys/rampacce.hpp"

#include "world4d/root.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/scale.hpp"

#include "phys/timespin.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"

#include "mathex/line3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysElectro );

//public ctor
MachPhysElectro::MachPhysElectro(W4dEntity* pParent, const MexTransform3d& localTransform, FlashType type)
:MachPhysLinearProjectile( exemplar(type), pParent, localTransform )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//only because LOD file was read
	updateCompositeBoundingVolume();

	visible(true);
    TEST_INVARIANT;
}

//One-time ctor
MachPhysElectro::MachPhysElectro(FlashType type)
:MachPhysLinearProjectile(MachPhysOtherPersistence::instance().pRoot(), MexTransform3d())
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	switch(type)
	{
		case MachPhysElectro::VIRUS:
    		readLODFile( SysPathName( "models/weapons/virus/virusblt.lod" ) );
			break;
		case MachPhysElectro::ELECTRIC:
    		readLODFile( SysPathName( "models/weapons/electro/lightnin.lod" ) );
			break;
	}
    TEST_INVARIANT;
}

MachPhysElectro::MachPhysElectro( PerConstructor con )
: MachPhysLinearProjectile( con )
{
}

MachPhysElectro::~MachPhysElectro()
{
    TEST_INVARIANT;

}

//static
const MachPhysElectro& MachPhysElectro::exemplar(FlashType type)
{
	const MachPhysElectro& electro = MachPhysOtherPersistence::instance().electroExemplar( type );

    //TBD: Disable the back face cull via cast here.
    //These needs to be changed so the property is defined in the .x file.
    Ren::ConstMeshPtr myMesh = electro.mesh().mesh();
    _CONST_CAST( RenMesh&, *myMesh ).backFaceAll( false );

    return electro;
}


void MachPhysElectro::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysElectro& t )
{

    o << "MachPhysElectro " << (void*)&t << " start" << std::endl;
    o << "MachPhysElectro " << (void*)&t << " end" << std::endl;

    return o;
}


void MachPhysElectro::scaleAndSpin(const PhysAbsoluteTime& burstTime,
							  const PhysRelativeTime& duration,
							  const MATHEX_SCALAR scale,
							  const MexTransform3d xForm )
{
	temporaryScale(RenNonUniformScale(scale, 2, 2), NOT_PROPOGATE );
	updateCompositeBoundingVolume();

    //Set up visibility plan
	visible(false);
    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
    pVisibilityPlan->add( false, duration );

    W4dVisibilityPlanPtr visibilityPlanPtr( pVisibilityPlan );
    entityPlanForEdit().visibilityPlan( visibilityPlanPtr, burstTime );

	//motion plan
	PhysTimedSpinPlan* pSpinPlan = _NEW( PhysTimedSpinPlan(MexVec3(1, 0, 0),
															  xForm,
															  MexDegrees(90),
															  MexDegrees(2400), 1) );
	pSpinPlan->addSegment(PhysRelativeTime(duration), MexDegrees(0));

	PhysMotionPlanPtr electroMotionPlanPtr = pSpinPlan;

	entityPlanForEdit().absoluteMotion( electroMotionPlanPtr,  burstTime, 1000);

    //Store default flight path data
    MexPoint3d startPosition = xForm.position();
    MexPoint3d endPosition(scale*5, 0, 0);
    	       xForm.transform(&endPosition);

    MexPoint3d globalStartPosition( startPosition );
    MexPoint3d globalEndPosition( endPosition );
    const MexTransform3d& parentTransform = pParent()->globalTransform();
    parentTransform.transform( &globalStartPosition );
    parentTransform.transform( &globalEndPosition );

    flightStartTime( burstTime );
    flightDuration( duration );
    flightPath( MexLine3d( globalStartPosition, globalEndPosition ) );

}

void perWrite( PerOstream& ostr, const MachPhysElectro& electro )
{
    const MachPhysLinearProjectile& base = electro;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysElectro& electro )
{
    MachPhysLinearProjectile& base = electro;

    istr >> base;
}

/* End ELECTRO.CPP ****************************************************/
