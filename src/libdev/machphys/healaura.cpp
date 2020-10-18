/*
 * H E A L A U R A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/healaura.hpp"
#include "machphys/healhelx.hpp"
#include "machphys/machine.hpp"
#include "machphys/private/otherper.hpp"

#include "world4d/generic.hpp"
#include "world4d/root.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "render/scale.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "phys/linemoti.hpp"
#include "phys/lsclplan.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/degrees.hpp"
#include "ctl/vector.hpp"

PER_DEFINE_PERSISTENT( MachPhysHealAura );

MachPhysHealAura::MachPhysHealAura( MachPhysMachine* pMachine, const PhysAbsoluteTime& startTime )
:   W4dEntity( pMachine, MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //Set hardcoded constants
    const MATHEX_SCALAR clearance = 0.35; //Initial distance of cross out from bounding volume
    const PhysRelativeTime cycleTime = 1.2; //Duration of one cycle of the healing animation
    const uint nRepetitions = 100000; //Lots of repetitions should keep going long enough

    //Get the machine dimensions, and set the cross size accordingly
    const MexAlignedBox3d& boundary = pMachine->compositeBoundingVolume();
    const MexPoint3d& minCorner = boundary.minCorner();
    const MexPoint3d& maxCorner = boundary.maxCorner();

    MATHEX_SCALAR width  = boundary.xLength();
    MATHEX_SCALAR length = boundary.yLength();
    MATHEX_SCALAR height = boundary.zLength();

	//reposition the cross
	localTransform( MexPoint3d(0, 0, minCorner.z() ) );

    MATHEX_SCALAR size = height * 0.75;
    if( width < size )
        size = width;
    if( length < size )
        size = length;

    //Set up a scale plan for the radial crosses
    MATHEX_SCALAR startScale = size;
    MATHEX_SCALAR endScale = size * 1.3;
    W4dSimpleUniformScalePlan* pScalePlan =
        _NEW( W4dSimpleUniformScalePlan( startScale, endScale, cycleTime ) );

    W4dScalePlanPtr scalePlanPtr( pScalePlan );

    //First time construct the scalar plans for the alpha plans
    static bool firstTime = true;
    static PhysScalarPlanPtr centralAlphaValuePlanPtr;
    static PhysScalarPlanPtr radialAlphaValuePlanPtr;

    if( firstTime )
    {
        firstTime = false;

        //Construct the alpha value plans
        PhysLinearScalarPlan::ScalarVec times;
        PhysLinearScalarPlan::ScalarVec centralValues;
        PhysLinearScalarPlan::ScalarVec radialValues;

        times.reserve( 2 );
        times.push_back( cycleTime - 0.2 );
        times.push_back( cycleTime );

        centralValues.reserve( 3 );
        centralValues.push_back( 0.6 );
        centralValues.push_back( 0.0 );
        centralValues.push_back( 0.0 );

        radialValues.reserve( 3 );
        radialValues.push_back( 1.0 );
        radialValues.push_back( 0.0 );
        radialValues.push_back( 0.0 );

        centralAlphaValuePlanPtr = _NEW( PhysLinearScalarPlan( times, centralValues ) );
        radialAlphaValuePlanPtr = _NEW( PhysLinearScalarPlan( times, radialValues ) );
    }

    //Construct the two alpha plans for the 2 cross types
    const RenMaterial& centralMaterial = MachPhysHealHelix::helixMaterial();
    const uint nMaterialsInVector = 1;
    const W4dLOD maxLOD = 0;

    W4dMaterialPlanPtr centralMaterialPlanPtr =
        _NEW( W4dSimpleAlphaPlan( centralMaterial, nMaterialsInVector,
                                  centralAlphaValuePlanPtr, maxLOD ) );

    W4dMaterialPlanPtr radialMaterialPlanPtr =
        _NEW( W4dSimpleAlphaPlan( radialCrossMaterial(), nMaterialsInVector,
                                  radialAlphaValuePlanPtr, maxLOD ) );

    //Construct each of the four pairs of crosses
    MATHEX_SCALAR aXAngle[] = {0.0,-90.0,0.0,90.0};
    MATHEX_SCALAR aYAngle[] = {90.0,0.0,-90.0,0.0};
    MATHEX_SCALAR aXShift[] = { maxCorner.x() + clearance, 0.0, minCorner.x() - clearance, 0.0 };
    MATHEX_SCALAR aYShift[] = { 0.0, maxCorner.y() + clearance, 0.0, minCorner.y() - clearance };
    MATHEX_SCALAR zShift = height * 0.45;

    for( size_t i = 0; i != 4; ++i )
    {
        //Compute the orientation for the crosses
        MexEulerAngles orientation( 0.0, MexDegrees( aYAngle[i] ), MexDegrees( aXAngle[i] ) );

        //Compute the inner and outer positions
        MATHEX_SCALAR x = aXShift[i];
        if( x > 0.0 )
            x += clearance * 3.0;
        else if( x < 0.0 )
            x -= clearance * 3.0;

        MATHEX_SCALAR y = aYShift[i];
        if( y > 0.0 )
            y += clearance * 3.0;
        else if( y < 0.0 )
            y -= clearance * 3.0;

        MexPoint3d innerPos( aXShift[i], aYShift[i], zShift );
        MexPoint3d outerPos( x, y, zShift );

        //Construct the inner and outer transforms
        MexTransform3d innerTransform( orientation, innerPos );
        MexTransform3d outerTransform( orientation, outerPos );

        //Construct the central and radial crosses
        W4dGeneric* pCentralCross =
            _NEW( W4dGeneric( centralCrossExemplar(), this, innerTransform ) );

        W4dGeneric* pRadialCross =
            _NEW( W4dGeneric( radialCrossExemplar(), this, innerTransform ) );

        //get the entity plan objects for the crosses
        W4dEntityPlan& centralEntityPlan = pCentralCross->entityPlanForEdit();
        W4dEntityPlan& radialEntityPlan = pRadialCross->entityPlanForEdit();

        //Set up a transform plan for the radial cross
        PhysLinearMotionPlan* pTransformPlan =
            _NEW( PhysLinearMotionPlan( innerTransform, outerTransform, cycleTime ) );

        PhysMotionPlanPtr transformPlanPtr( pTransformPlan );
        radialEntityPlan.absoluteMotion( transformPlanPtr, startTime, nRepetitions );

        //Set scale for central cross, and scale plan for radial cross
        pCentralCross->temporaryScale( RenUniformScale( startScale ), W4dEntity::NOT_PROPOGATE );
        radialEntityPlan.scalePlan( scalePlanPtr, startTime, nRepetitions );

        //Apply the material plans
        centralEntityPlan.materialPlan( centralMaterialPlanPtr, startTime, nRepetitions);
        radialEntityPlan.materialPlan( radialMaterialPlanPtr, startTime, nRepetitions);
    }

    TEST_INVARIANT;
}

MachPhysHealAura::MachPhysHealAura( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysHealAura::~MachPhysHealAura()
{
    TEST_INVARIANT;

}

void MachPhysHealAura::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHealAura& t )
{

    o << "MachPhysHealAura " << (void*)&t << " start" << std::endl;
    o << "MachPhysHealAura " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const W4dGeneric& MachPhysHealAura::centralCrossExemplar()
{
	return MachPhysOtherPersistence::instance().healAuraCentralCrossExemplar();
}

//static
const W4dGeneric& MachPhysHealAura::radialCrossExemplar()
{
	return 	MachPhysOtherPersistence::instance().healAuraRadialCrossExemplar();
}

//static
W4dGeneric& MachPhysHealAura::newRadialCross()
{
    W4dGeneric* pRadialCross = _NEW( W4dGeneric( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID ) );

    //Read the file
    pRadialCross->loadLODFile( SysPathName( "models/weapons/heal/healcros.lod" ) );

        //TBD: Disable the back fuce cull via cast here.
        //These needs to be changed so the property is defined in the .x file.
        //Ren::ConstMeshPtr myMesh = _CONST_CAST( const W4dEntity&, theCross).mesh().mesh();
       // _CONST_CAST( RenMesh&, *myMesh ).backFaceAll( false );

    return *pRadialCross;
}

//static
W4dGeneric& MachPhysHealAura::newCentralCross()
{
    W4dGeneric* pCentralCross = _NEW(W4dGeneric(MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID ) );

    //Read the file
    pCentralCross->loadLODFile( SysPathName( "models/weapons/heal/healaura.lod" ) );

        //TBD: Disable the back fuce cull via cast here.
        //These needs to be changed so the property is defined in the .x file.
        //Ren::ConstMeshPtr myMesh = _CONST_CAST( const W4dEntity&, theCross).mesh().mesh();
        //_CONST_CAST( RenMesh&, *myMesh ).backFaceAll( false );

    return *pCentralCross;
}

//virtual
bool MachPhysHealAura::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
const RenMaterial& MachPhysHealAura::radialCrossMaterial()
{
    static RenMaterial mat( RenColour::black() );
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

        //Set colour to emissive white
        mat.emissive( RenColour::white() );

        //Load the special purpose texture
        mat.texture( RenTexManager::instance().createTexture( "heal_bt.bmp" ) );
    }

    return mat;
}

void perWrite( PerOstream& ostr, const MachPhysHealAura& aura )
{
    const W4dEntity& base = aura;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysHealAura& aura )
{
    W4dEntity& base = aura;

    istr >> base;
}

/* End HEALAURA.CPP *************************************************/
