/*
 * T O R C H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/torch.hpp"
#include "machphys/private/weapper.hpp"

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

PER_DEFINE_PERSISTENT( MachPhysTorch );

//One-time ctor
MachPhysTorch::MachPhysTorch()
:   W4dComposite( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID )
{
	//Load the model
    readCompositeFile( SysPathName( "models/construc/torch/torch.cdf" ) );
    TEST_INVARIANT;
}

//public ctor
MachPhysTorch::MachPhysTorch( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( exemplar(), pParent, localTransform )
{

    TEST_INVARIANT;
}

MachPhysTorch::~MachPhysTorch()
{
    TEST_INVARIANT;

}

//static
const MachPhysTorch& MachPhysTorch::exemplar()
{
    //Use the one time constructor
    static MachPhysTorch& torch = *_NEW( MachPhysTorch );
    return torch;
}

void MachPhysTorch::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTorch& t )
{

    o << "MachPhysTorch " << (void*)&t << " start" << std::endl;
    o << "MachPhysTorch " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysTorch::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysTorch& sphere )
{
    const W4dComposite& base = sphere;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysTorch& sphere )
{
    W4dComposite& base = sphere;

    istr >> base;
}

MachPhysTorch::MachPhysTorch( PerConstructor c )
: W4dComposite( c )
{
	// Intentionally Empty
}

/* End TORCH.CPP *************************************************/

