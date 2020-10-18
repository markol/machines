/*
 * L I G H T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/light.hpp"
#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"


PER_DEFINE_PERSISTENT( MachPhysLight );

//One-time ctor
MachPhysLight::MachPhysLight()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/light/light.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysLight::MachPhysLight( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysLight::~MachPhysLight()
{
    TEST_INVARIANT;

}

//static
const MachPhysLight& MachPhysLight::exemplar()
{
	return MachPhysOtherPersistence::instance().lightShadowExemplar();
}

void MachPhysLight::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLight& t )
{

    o << "MachPhysLight " << (void*)&t << " start" << std::endl;
    o << "MachPhysLight " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLight::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysLight& light )
{
    const W4dEntity& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysLight& light )
{
    W4dEntity& base = light;

    istr >> base;
}

void MachPhysLight::startLight( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, duration);

	entityPlanForEdit().visibilityPlan(lightVisibilityPlanPtr, startTime );
}

MachPhysLight::MachPhysLight( PerConstructor c )
:W4dEntity( c )
{
}

/* End LIGHT.CPP *************************************************/

