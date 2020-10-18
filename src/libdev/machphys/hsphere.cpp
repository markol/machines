/*
 * H S P H E R E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/hsphere.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/framereg.hpp"

#include "render/colour.hpp"

#include "system/pathname.hpp"
PER_DEFINE_PERSISTENT( MachPhysHemiSphere );

//One-time ctor
MachPhysHemiSphere::MachPhysHemiSphere()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/construc/hemisphe/glow.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysHemiSphere::MachPhysHemiSphere( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysHemiSphere::~MachPhysHemiSphere()
{
    TEST_INVARIANT;

}

//static
const MachPhysHemiSphere& MachPhysHemiSphere::exemplar()
{
    return MachPhysOtherPersistence::instance().hemiSphereExemplar();
}

W4dVisibilityPlanPtr MachPhysHemiSphere::startGlow( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration, const uint& frameOffset )
{
    W4dVisibilityPlanPtr hVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );

	//each frame lasts 1/15 second
	uint frames = duration*15.0;

	uint i=1;

	//the hemi sphere turns on and off every other frame and stays off every fifth frame
	while(i< frames) // 16777216 =  just a large number
	{
		PhysRelativeTime  currentTime = i/15.0;
		uint trueFrame = i + frameOffset;

		if( trueFrame%2 == 0 or trueFrame%5 == 0)
		{
			hVisibilityPlanPtr->add(false, currentTime);
		}

		else
		{
			hVisibilityPlanPtr->add(true, currentTime);
		}

		++i;
	}

    entityPlanForEdit().visibilityPlan( hVisibilityPlanPtr, startTime, 10000, MachPhys::CONSTRUCTION_CONSTRUCTING );

	W4dFrameRegulator regulator( startTime, duration, duration*2.0, frames*2);
	entityPlanForEdit().frameRegulator( regulator );

	return hVisibilityPlanPtr;
}

void MachPhysHemiSphere::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHemiSphere& t )
{

    o << "MachPhysHemiSphere " << (void*)&t << " start" << std::endl;
    o << "MachPhysHemiSphere " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysHemiSphere::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysHemiSphere& sphere )
{
    const W4dEntity& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysHemiSphere& sphere )
{
    W4dEntity& base = sphere;

    istr >> base;
}

MachPhysHemiSphere::MachPhysHemiSphere( PerConstructor c )
:W4dEntity( c )
{
}

/* End HSPHERE.CPP *************************************************/

