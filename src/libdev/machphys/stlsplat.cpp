/*
 * S T I N G S P L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/stlsplat.hpp"
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
#include "world4d/entity.hpp"

PER_DEFINE_PERSISTENT( MachPhysLightStingSplat );

//One-time ctor
MachPhysLightStingSplat::MachPhysLightStingSplat()
:W4dComposite( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    //readCompositeFile( SysPathName( "models/weapons/wasp/light/splat.cdf" ) );
    readLODFile( SysPathName( "models/weapons/wasp/light/splat.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysLightStingSplat::MachPhysLightStingSplat( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysLightStingSplat::MachPhysLightStingSplat( PerConstructor con )
: W4dComposite( con )
{
}

MachPhysLightStingSplat::~MachPhysLightStingSplat()
{
    TEST_INVARIANT;

}

//static
const MachPhysLightStingSplat& MachPhysLightStingSplat::exemplar()
{
	return MachPhysOtherPersistence::instance().lightStingSplatExemplar();
}

void MachPhysLightStingSplat::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLightStingSplat& t )
{

    o << "MachPhysLightStingSplat " << (void*)&t << " start" << std::endl;
    o << "MachPhysLightStingSplat " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLightStingSplat::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysLightStingSplat& scorch )
{
    const W4dComposite& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysLightStingSplat& scorch )
{
    W4dComposite& base = scorch;

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

		textures.push_back( RenTexManager::instance().createTexture("bhit0_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("bhit1_bt.bmp") );
		textures.push_back( RenTexManager::instance().createTexture("bhit2_bt.bmp") );
    }

	return textures;
}

//static
void MachPhysLightStingSplat::preloadTextures()
{
	splatTextures();
}

void MachPhysLightStingSplat::startLightStingSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
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
            _NEW( W4dSimpleUniformScalePlan( 1.0, 4.0, duration ) );
        scalePlanPtr = pScalePlan;

    }

    //Set its visibility plan
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set the scaling plan
    entityPlan.scalePlan( scalePlanPtr, startTime );

	W4dMaterialSequencePlan::makePlan(this, splatTextures(), startTime, duration );

	//regulate the frame so that he 4 texutures will be geranteed to show up
	W4dFrameRegulator regulator( startTime, duration, duration*2.0, 3);
	entityPlan.frameRegulator( regulator );
}

/* End STINGSPL.CPP *************************************************/

