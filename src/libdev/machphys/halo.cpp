/*
 * H A L O . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/halo.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "phys/timespin.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "phys/rampacce.hpp"
#include "render/texture.hpp"
#include "render/scale.hpp"
#include "world4d/root.hpp"
#include "world4d/composit.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/matplan.hpp"

#include "machphys/halompln.hpp"


#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysLightningHalo );

//One-time ctor
MachPhysLightningHalo::MachPhysLightningHalo(HaloType type)
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
	switch(type)
	{
		case ELECTRIC:
    		readLODFile( SysPathName( "models/weapons/electro/lightbox.lod" ) );
			break;

		case VIRUS:
    		readLODFile( SysPathName( "models/weapons/virus/virusbox.lod" ) );
			break;
	}
    TEST_INVARIANT;
}

//public ctor
MachPhysLightningHalo::MachPhysLightningHalo( W4dEntity* pParent, const MexTransform3d& localTransform, HaloType type )
:W4dEntity( exemplar(type), pParent, localTransform )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysLightningHalo::MachPhysLightningHalo( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysLightningHalo::~MachPhysLightningHalo()
{
    TEST_INVARIANT;

}

//static
const MachPhysLightningHalo& MachPhysLightningHalo::exemplar(HaloType type)
{
    return MachPhysOtherPersistence::instance().haloExemplar(type);
/*
	MachPhysLightningHalo* halo = NULL;

	switch(type)
	{
		case ELECTRIC:
		{
		    static MachPhysLightningHalo* electric = _NEW( MachPhysLightningHalo( ELECTRIC ) );
			halo = electric;
			break;
		}
		case VIRUS:
		{
		    static MachPhysLightningHalo* virus = _NEW( MachPhysLightningHalo( VIRUS ) );
			halo = virus;
			break;
		}
	}
    return *halo;
*/
}

void MachPhysLightningHalo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLightningHalo& t )
{

    o << "MachPhysLightningHalo " << (void*)&t << " start" << std::endl;
    o << "MachPhysLightningHalo " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLightningHalo::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}


void MachPhysLightningHalo::startLightning
(
	W4dEntity* pVictim,
	const PhysAbsoluteTime& startTime,
	const PhysRelativeTime& duration,
	const int& sign,
	const HaloType type
)
{

	const MexAlignedBox3d& victimBox3D =
	    (pVictim->isComposite() ? pVictim->asComposite().compositeBoundingVolume()
                                : pVictim->boundingVolume());

    const MexPoint3d& minCorner = victimBox3D.minCorner();
    const MexPoint3d& maxCorner = victimBox3D.maxCorner();
	MexPoint3d centre( 0.5*(minCorner.x() + maxCorner.x()),
	                   0.5*(minCorner.y() + maxCorner.y()),
	                   0.5*(minCorner.z() + maxCorner.z()) );

	MexTransform3d localXform = MexTransform3d(centre);


	//scale them to the size of the pComposit
	MATHEX_SCALAR height = maxCorner.z() - minCorner.z();
	MATHEX_SCALAR scale = height/3.0;
	RenNonUniformScale aScale( maxCorner.x() - minCorner.x(),
	                           maxCorner.y() - minCorner.y(),
	                           scale);

	temporaryScale(aScale, NOT_PROPOGATE );

	//motion Plan
	MexPoint3d endPosition;

	switch(sign)
	{
		case 1:
			endPosition = MexPoint3d( centre.x(), centre.y(), maxCorner.z() - 0.5*scale );
			break;

		case -1:
		default:
			endPosition = MexPoint3d( centre.x(), centre.y(), minCorner.z() + 0.5*scale );
			break;
	}

	//motion plan
	PhysLinearMotionPlan  *pMotionPlan;
	const PhysRelativeTime cycleTime = 0.5;	  //hard code the cycle time

	switch(type)
	{
		case VIRUS:
		{
    		pMotionPlan = _NEW( PhysLinearMotionPlan( localXform,
                          							  MexTransform3d(endPosition),
                          							  cycleTime ) );
			pMotionPlan->add(localXform, cycleTime*2);
			break;
		}
		case ELECTRIC:
		{
   			pMotionPlan = _NEW( PhysLinearMotionPlan( localXform,
                          							  MexTransform3d(endPosition),
                          							  duration ) );
            break;
		}
	}

   	PhysMotionPlanPtr motionPlanPtr = pMotionPlan;
	entityPlanForEdit().absoluteMotion( motionPlanPtr,  startTime, (duration/cycleTime + 1));

 	//material plan
	MachPhysLightningHaloMaterialPlan* pHaloMatPlan = _NEW(MachPhysLightningHaloMaterialPlan(duration, W4dLOD(1), type));
	W4dMaterialPlanPtr matPlanPtr = pHaloMatPlan;
	entityPlanForEdit().materialPlan(matPlanPtr, startTime);

	//visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);
	entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );
}

void perWrite( PerOstream& ostr, const MachPhysLightningHalo& halo )
{
    const W4dEntity& base = halo;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysLightningHalo& halo )
{
    W4dEntity& base = halo;

    istr >> base;
}

/* End HALO.CPP *************************************************/

