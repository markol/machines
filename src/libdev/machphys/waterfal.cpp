/*
 * W A T E R F A L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/waterfal.hpp"

#include "stdlib/string.hpp"

#include "machphys/snddata.hpp"
#include "machphys/private/otherper.hpp"

#include "mathex/transf3d.hpp"
#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/uvtrans.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/soundman.hpp"

#include "render/uvanim.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "system/pathname.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysWaterfall );

//One-time ctor
MachPhysWaterfall::MachPhysWaterfall()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/planet/dales/lod/waterfal.lod" ) );
    TEST_INVARIANT;
}


//public ctor
MachPhysWaterfall::MachPhysWaterfall( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
	//make it visible
    visible( true );

	//make an uv plan
    const PhysAbsoluteTime now = SimManager::instance().currentTime();
	const PhysRelativeTime oneYear = 31536000;

	W4dUVTranslation* pUVTranslation = _NEW( W4dUVTranslation(oneYear, W4dLOD(1), MexVec2(0, 1.6), MexVec2(0,0)) );

	//static const RenTexture watFallTex = RenTexManager::instance().createTexture( "SEMITR_B.BMP" );
	//pUVTranslation->setFilterTexture(watFallTex);

	W4dUVPlanPtr bUVPlanPtr = pUVTranslation;
	entityPlanForEdit().uvPlan(bUVPlanPtr, now, (uint)oneYear,  1);

    //Register its sound
	W4dSoundManager::instance().play( this, SID_WATERFALL, PhysAbsoluteTime( 0 ), 0 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/waterf.wav" ),
//                                      PhysAbsoluteTime( 0 ), 100.0, 75.0,
//                                      W4dSoundManager::LOOP_CONTINUOUSLY );

    TEST_INVARIANT;
}

MachPhysWaterfall::~MachPhysWaterfall()
{
    TEST_INVARIANT;
}

//static
const MachPhysWaterfall& MachPhysWaterfall::exemplar()
{
    return MachPhysOtherPersistence::instance().waterfallExemplar();
}

void MachPhysWaterfall::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWaterfall& t )
{

    o << "MachPhysWaterfall " << (void*)&t << " start" << std::endl;
    o << "MachPhysWaterfall " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysWaterfall::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysWaterfall& sphere )
{
    const W4dEntity& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysWaterfall& sphere )
{
    W4dEntity& base = sphere;

    istr >> base;
}
/* End WATERFAL.CPP *************************************************/
