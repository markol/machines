/*
 * S P A R K S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/sparks.hpp"

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

PER_DEFINE_PERSISTENT( MachPhysSparks );

//One-time ctor
MachPhysSparks::MachPhysSparks()
:   W4dComposite( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID )
{
	//Load the model
    readCompositeFile( SysPathName( "models/construc/sparks/sparks.cdf" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysSparks::MachPhysSparks( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( exemplar(), pParent, localTransform )
{
	visible( false );
    TEST_INVARIANT;
}

MachPhysSparks::~MachPhysSparks()
{
    TEST_INVARIANT;

}

//static
const MachPhysSparks& MachPhysSparks::exemplar()
{
    return MachPhysOtherPersistence::instance().sparksExemplar();
}

void MachPhysSparks::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSparks& t )
{

    o << "MachPhysSparks " << (void*)&t << " start" << std::endl;
    o << "MachPhysSparks " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysSparks::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysSparks::MachPhysSparks(PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysSparks& sparks )
{
    const W4dComposite& base = sparks;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysSparks& sparks )
{
    W4dComposite& base = sparks;

    istr >> base;
}

/* End SPARKS.CPP *************************************************/

