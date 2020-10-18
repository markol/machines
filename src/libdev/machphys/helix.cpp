/*
 * H E L I X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/helix.hpp"
#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysHelix );

MachPhysHelix::MachPhysHelix( W4dEntity* pParent, const MexTransform3d& localTransform, Twist twist )
:   W4dEntity( exemplar( twist ), pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysHelix::MachPhysHelix( Twist twist )
:   W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
    //Load the mesh data
    switch( twist )
    {
        case LEFT_TWIST:
            readLODFile( SysPathName( "models/weapons/helix/helix1.lod" ) );
            break;
        case RIGHT_TWIST:
            readLODFile( SysPathName( "models/weapons/helix/helix2.lod" ) );
            break;
    }

    //TBD: Disable the back fuce cull via cast here.
    //These needs to be changed so the property is defined in the .x file.
    //Ren::ConstMeshPtr myMesh = _CONST_CAST( const W4dEntity&, *this).mesh().mesh();
    Ren::ConstMeshPtr myMesh = _CONST_CAST( const W4dEntity&, _STATIC_CAST(W4dEntity&, *this)).mesh().mesh();
    _CONST_CAST( RenMesh&, *myMesh ).backFaceAll( false );

    TEST_INVARIANT;
}

MachPhysHelix::MachPhysHelix( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysHelix::~MachPhysHelix()
{
    TEST_INVARIANT;

}

//static
const MachPhysHelix& MachPhysHelix::exemplar( Twist twist )
{
	/*
    //Use the one time constructor
    static MachPhysHelix& leftHelix = *_NEW( MachPhysHelix( LEFT_TWIST ) );
    static MachPhysHelix& rightHelix = *_NEW( MachPhysHelix( RIGHT_TWIST ) );

    return (twist == LEFT_TWIST ? leftHelix : rightHelix);
	*/
	return MachPhysOtherPersistence::instance().helixExemplar(twist);
}

//virtual
bool MachPhysHelix::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysHelix::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHelix& t )
{

    o << "MachPhysHelix " << (void*)&t << " start" << std::endl;
    o << "MachPhysHelix " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysHelix& helix )
{
    const W4dEntity& base = helix;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysHelix& helix )
{
    W4dEntity& base = helix;

    istr >> base;
}

/* End HELIX.CPP ****************************************************/
