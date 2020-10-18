/*
 * S P L A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/plasplat.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "render/texmgr.hpp"

#include "world4d/simplsca.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/framereg.hpp"
#include "world4d/matseqpl.hpp"

PER_DEFINE_PERSISTENT( MachPhysPlasmaSplat );

//One-time ctor
MachPhysPlasmaSplat::MachPhysPlasmaSplat()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/splat/splat.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysPlasmaSplat::MachPhysPlasmaSplat( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysPlasmaSplat::MachPhysPlasmaSplat( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysPlasmaSplat::~MachPhysPlasmaSplat()
{
    TEST_INVARIANT;

}

//static
const MachPhysPlasmaSplat& MachPhysPlasmaSplat::exemplar()
{
	return MachPhysOtherPersistence::instance().plasmaSplatExemplar();
}

void MachPhysPlasmaSplat::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPlasmaSplat& t )
{

    o << "MachPhysPlasmaSplat " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlasmaSplat " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysPlasmaSplat::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysPlasmaSplat& scorch )
{
    const W4dEntity& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysPlasmaSplat& scorch )
{
    W4dEntity& base = scorch;

    istr >> base;
}


//local
static 	ctl_vector<RenTexture>& splatTextures()
{
	static ctl_vector<RenTexture> textures;
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

		textures.reserve(4);

		textures.push_back( RenTexManager::instance().createTexture("spl00_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("spl01_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("spl02_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("spl03_bt.bmp") );
    }

	return textures;
}

void MachPhysPlasmaSplat::startPlasmaSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
    //Counted pointers etc for standard plans
    static W4dVisibilityPlanPtr visibilityPlanPtr;
    static W4dScalePlanPtr scalePlanPtr;

    typedef ctl_vector< Ren::MaterialVecPtr > MaterialVecPtrs;
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

	W4dMaterialSequencePlan::makePlan(this, splatTextures(), startTime, duration );

	//regulate the frame so that he 4 texutures will be geranteed to show up
	W4dFrameRegulator regulator( startTime, duration, duration, 4);
	entityPlan.frameRegulator( regulator );

}


//static
void MachPhysPlasmaSplat::preloadTextures()
{
	splatTextures();
}

/* End PLASPLAT.CPP *************************************************/

