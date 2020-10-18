/*
 * S P L A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/splat.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
//#include "world4d/simplsca.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"

#include "mathex/transf3d.hpp"

#include "system/pathname.hpp"

#include "ctl/vector.hpp"

#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysGroundSplat );

//One-time ctor
MachPhysGroundSplat::MachPhysGroundSplat()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/destroy/splat/splat.lod" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysGroundSplat::MachPhysGroundSplat( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );
    TEST_INVARIANT;
}

MachPhysGroundSplat::MachPhysGroundSplat( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysGroundSplat::~MachPhysGroundSplat()
{
    TEST_INVARIANT;

}

//static
const MachPhysGroundSplat& MachPhysGroundSplat::exemplar()
{
    return MachPhysOtherPersistence::instance().groundSplatExemplar();
}

void MachPhysGroundSplat::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGroundSplat& t )
{

    o << "MachPhysGroundSplat " << (void*)&t << " start" << std::endl;
    o << "MachPhysGroundSplat " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysGroundSplat::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysGroundSplat& scorch )
{
    const W4dEntity& base = scorch;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysGroundSplat& scorch )
{
    W4dEntity& base = scorch;

    istr >> base;
}

/* End SPLAT.CPP *************************************************/

