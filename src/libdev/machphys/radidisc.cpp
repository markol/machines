/*
 * R A D I D I S C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/radidisc.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "mathex/transf3d.hpp"
#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysRadialDisc );

MachPhysRadialDisc::MachPhysRadialDisc( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    TEST_INVARIANT;
}

//One-time ctor
MachPhysRadialDisc::MachPhysRadialDisc()
:   W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
    //Load the mesh data
    readLODFile( SysPathName( "models/weapons/disc/disc.lod" ) );

    //TBD: Disable the bcak fuce cull via static cast here.
    //These needs to be changed so the property is defined in the .x file.
    //Ren::ConstMeshPtr myMesh = _CONST_CAST( const W4dEntity&, *this).mesh().mesh();
    //_CONST_CAST( RenMesh&, *myMesh ).backFaceAll( false );
	//requested Steve to work on the model already. 14.12.98. Yueai

    TEST_INVARIANT;
}

MachPhysRadialDisc::MachPhysRadialDisc( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysRadialDisc::~MachPhysRadialDisc()
{
    TEST_INVARIANT;

}

void MachPhysRadialDisc::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysRadialDisc& t )
{

    o << "MachPhysRadialDisc " << (void*)&t << " start" << std::endl;
    o << "MachPhysRadialDisc " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysRadialDisc& MachPhysRadialDisc::exemplar()
{
    return MachPhysOtherPersistence::instance().radialDiscExemplar();
}

//virtual
bool MachPhysRadialDisc::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
const RenTexture& MachPhysRadialDisc::circleTexture()
{
    static RenTexture theTex = RenTexManager::instance().createTexture("ring_bt.bmp");
    return theTex;
}

void perWrite( PerOstream& ostr, const MachPhysRadialDisc& disc )
{
    const W4dEntity& base = disc;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysRadialDisc& disc )
{
    W4dEntity& base = disc;

    istr >> base;
}

/* End RADIDISC.CPP *************************************************/

