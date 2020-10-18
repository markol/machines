/*
 * P U L S P L A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/pulsplat.hpp"
#include "machphys/private/otherper.hpp"
#include "ctl/vector.hpp"
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "render/texmgr.hpp"

#include "world4d/simplsca.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/framereg.hpp"
#include "world4d/matseqpl.hpp"

PER_DEFINE_PERSISTENT( MachPhysPulseSplat );

//One-time ctor
MachPhysPulseSplat::MachPhysPulseSplat()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/splat/pulse/splat.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysPulseSplat::MachPhysPulseSplat( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysPulseSplat::MachPhysPulseSplat( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysPulseSplat::~MachPhysPulseSplat()
{
    TEST_INVARIANT;

}

//static
const MachPhysPulseSplat& MachPhysPulseSplat::exemplar()
{
	return MachPhysOtherPersistence::instance().pulseSplatExemplar();
}

void MachPhysPulseSplat::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPulseSplat& t )
{

    o << "MachPhysPulseSplat " << (void*)&t << " start" << std::endl;
    o << "MachPhysPulseSplat " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysPulseSplat::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysPulseSplat& scorch )
{
    const W4dEntity& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysPulseSplat& scorch )
{
    W4dEntity& base = scorch;

    istr >> base;
}

//local
static 	const ctl_vector<RenTexture>& textures()
{
	static ctl_vector<RenTexture> textures;
	static bool first = true;

	if( first )
	{
		first = false;

		textures.reserve(4);

		textures.push_back( RenTexManager::instance().createTexture("pls00_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("pls01_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("pls02_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("pls03_bt.bmp") );
	}
	return textures;
}

//static
void MachPhysPulseSplat::preloadTextures()
{
	textures();
}

void MachPhysPulseSplat::startPulseSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
    //Counted pointers etc for standard plans
    static W4dVisibilityPlanPtr visibilityPlanPtr;
    static W4dScalePlanPtr scalePlanPtr;

    //First time set up standard plans
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

        //Create the visibility plan
        W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
        pVisibilityPlan->add( false, duration );
        visibilityPlanPtr = pVisibilityPlan;

        //Create the scale plan
        W4dSimpleUniformScalePlan* pScalePlan =
            _NEW( W4dSimpleUniformScalePlan( 1.0, 2.0, duration ) );
        scalePlanPtr = pScalePlan;

    }

    //Set its visibility plan
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set the scaling plan
    entityPlan.scalePlan( scalePlanPtr, startTime );

	W4dMaterialSequencePlan::makePlan(this, textures(), startTime, duration);

	//regulate the frame so that he 4 texutures will be geranteed to show up
	W4dFrameRegulator regulator( startTime, duration, duration*2.0, 4);
	entityPlan.frameRegulator( regulator );

}
/* End PULSPLAT.CPP *************************************************/

