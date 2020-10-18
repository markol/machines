/*
 * L O A D O V E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/loadover.hpp"
#include "machphys/private/otherper.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysLoadingOver );

//One-time ctor
MachPhysLoadingOver::MachPhysLoadingOver()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/load/over.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysLoadingOver::MachPhysLoadingOver( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysLoadingOver::MachPhysLoadingOver( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysLoadingOver::~MachPhysLoadingOver()
{
    TEST_INVARIANT;

}

//static
const MachPhysLoadingOver& MachPhysLoadingOver::exemplar()
{
	return MachPhysOtherPersistence::instance().overLoadingExemplar();
}

void MachPhysLoadingOver::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLoadingOver& t )
{

    o << "MachPhysLoadingOver " << (void*)&t << " start" << std::endl;
    o << "MachPhysLoadingOver " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLoadingOver::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysLoadingOver& over )
{
    const W4dEntity& base = over;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysLoadingOver& over )
{
    W4dEntity& base = over;

    istr >> base;
}

/* End LOADOVER.CPP *************************************************/

