/*
 * V S P H E R E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/vsphere.hpp"

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "machphys/private/otherper.hpp"

#include "mathex/transf3d.hpp"
#include "system/pathname.hpp"
#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"

#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysVortexSphere );

//One-time ctor
MachPhysVortexSphere::MachPhysVortexSphere()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/vortex/sphere.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysVortexSphere::MachPhysVortexSphere( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysVortexSphere::~MachPhysVortexSphere()
{
    TEST_INVARIANT;

}

//static
const MachPhysVortexSphere& MachPhysVortexSphere::exemplar()
{
    return MachPhysOtherPersistence::instance().vortexSphereExemplar();
}

void MachPhysVortexSphere::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVortexSphere& t )
{

    o << "MachPhysVortexSphere " << (void*)&t << " start" << std::endl;
    o << "MachPhysVortexSphere " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysVortexSphere::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysVortexSphere& sphere )
{
    const W4dEntity& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysVortexSphere& sphere )
{
    W4dEntity& base = sphere;

    istr >> base;
}

MachPhysVortexSphere::MachPhysVortexSphere( PerConstructor c )
:W4dEntity( c )
{
}

/* End VSPHERE.CPP *************************************************/

