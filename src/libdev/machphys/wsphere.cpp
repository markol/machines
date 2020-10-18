/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/wsphere.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"

#include "mathex/transf3d.hpp"

#include "system/pathname.hpp"

#include "ctl/vector.hpp"

#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysWhiteSphere );

//One-time ctor
MachPhysWhiteSphere::MachPhysWhiteSphere()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/vortex/white.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysWhiteSphere::MachPhysWhiteSphere( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysWhiteSphere::~MachPhysWhiteSphere()
{
    TEST_INVARIANT;

}

//static
const MachPhysWhiteSphere& MachPhysWhiteSphere::exemplar()
{
    return MachPhysOtherPersistence::instance().whiteSphereExemplar();
}

void MachPhysWhiteSphere::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWhiteSphere& t )
{

    o << "MachPhysWhiteSphere " << (void*)&t << " start" << std::endl;
    o << "MachPhysWhiteSphere " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysWhiteSphere::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysWhiteSphere& sphere )
{
    const W4dEntity& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysWhiteSphere& sphere )
{
    W4dEntity& base = sphere;

    istr >> base;
}

MachPhysWhiteSphere::MachPhysWhiteSphere( PerConstructor c )
:W4dEntity( c )
{
}

/* End WSPHERE.CPP *************************************************/

