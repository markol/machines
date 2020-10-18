/*
 * L O A D U N D E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/loadunde.hpp"
#include "machphys/private/otherper.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysLoadingUnder );

//One-time ctor
MachPhysLoadingUnder::MachPhysLoadingUnder()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/load/under.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysLoadingUnder::MachPhysLoadingUnder( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysLoadingUnder::MachPhysLoadingUnder( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysLoadingUnder::~MachPhysLoadingUnder()
{
    TEST_INVARIANT;

}

//static
const MachPhysLoadingUnder& MachPhysLoadingUnder::exemplar()
{
	return MachPhysOtherPersistence::instance().underLoadingExemplar();
}

void MachPhysLoadingUnder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLoadingUnder& t )
{

    o << "MachPhysLoadingUnder " << (void*)&t << " start" << std::endl;
    o << "MachPhysLoadingUnder " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysLoadingUnder::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysLoadingUnder& under )
{
    const W4dEntity& base = under;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysLoadingUnder& under )
{
    W4dEntity& base = under;

    istr >> base;
}

/* End LOADUNDE.CPP *************************************************/

