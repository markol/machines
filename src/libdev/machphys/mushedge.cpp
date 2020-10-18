/*
 * M U S H E D G E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/mushedge.hpp"
#include "machphys/private/otherper.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysMushroomEdge );

//One-time ctor
MachPhysMushroomEdge::MachPhysMushroomEdge()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/nuke/nuke_cov.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysMushroomEdge::MachPhysMushroomEdge( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysMushroomEdge::~MachPhysMushroomEdge()
{
    TEST_INVARIANT;

}

//static
const MachPhysMushroomEdge& MachPhysMushroomEdge::exemplar()
{
    return MachPhysOtherPersistence::instance().mushroomEdgeExemplar();
}

void MachPhysMushroomEdge::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMushroomEdge& t )
{

    o << "MachPhysMushroomEdge " << (void*)&t << " start" << std::endl;
    o << "MachPhysMushroomEdge " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysMushroomEdge::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysMushroomEdge& mush )
{
    const W4dEntity& base = mush;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMushroomEdge& mush )
{
    W4dEntity& base = mush;

    istr >> base;
}

MachPhysMushroomEdge::MachPhysMushroomEdge( PerConstructor c )
:W4dEntity( c )
{
}

/* End MUSHEDGE.CPP *************************************************/

