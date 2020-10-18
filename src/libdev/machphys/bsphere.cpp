/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/bsphere.hpp"
#include "machphys/private/otherper.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysBlackSphere );

//One-time ctor
MachPhysBlackSphere::MachPhysBlackSphere()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/vortex/black.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysBlackSphere::MachPhysBlackSphere( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysBlackSphere::~MachPhysBlackSphere()
{
    TEST_INVARIANT;

}

//static
const MachPhysBlackSphere& MachPhysBlackSphere::exemplar()
{
    return MachPhysOtherPersistence::instance().blackSphereExemplar();
}

void MachPhysBlackSphere::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBlackSphere& t )
{

    o << "MachPhysBlackSphere " << (void*)&t << " start" << std::endl;
    o << "MachPhysBlackSphere " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysBlackSphere::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysBlackSphere::MachPhysBlackSphere( PerConstructor c )
:W4dEntity( c )
{
}

void perWrite( PerOstream& ostr, const MachPhysBlackSphere& sphere )
{
    const W4dEntity& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysBlackSphere& sphere )
{
    W4dEntity& base = sphere;

    istr >> base;
}

/* End VORTBOMB.CPP *************************************************/

