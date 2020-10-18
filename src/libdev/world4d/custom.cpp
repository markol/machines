/*
 * C U S T O M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/custom.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "mathex/double.hpp"

#ifndef _INLINE
    #include "world4d/custom.ipp"
#endif

PER_DEFINE_PERSISTENT( W4dCustom );

W4dCustom::W4dCustom
(
    W4dEntity* pParent, const W4dTransform3d& localTransform, Solidity solid,
    W4dDistance firstLODDistance
)
:   W4dEntity( pParent, localTransform, solid )
{
    //Set sensible allocation size
    meshes_.reserve( 2 );

    //Construct a new empty mesh and cache it
    addMesh( firstLODDistance, W4dLOD( 0 ) );

    TEST_INVARIANT;
}

W4dCustom::W4dCustom
(
    const W4dCustom& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform
)
:   W4dEntity( copyMe, pParent, localTransform ),
    meshes_( copyMe.meshes_ )
{
}

W4dCustom::~W4dCustom()
{
    TEST_INVARIANT;
}

void W4dCustom::addMesh( W4dDistance distance, W4dLOD LODid )
{
    //Construct a new empty mesh and cache it
    Ren::MeshPtr meshPtr = RenMesh::createEmpty();
    meshes_.push_back( &(*meshPtr) );

    //Construct a mesh instance and add to the entity
    add( _NEW( RenMeshInstance( meshPtr ) ), distance, LODid );
}

void W4dCustom::emptyMesh( W4dDistance distance, W4dLOD LODid )
{
    PRE( LODid < meshes_.size() );

	Ren::MeshPtr meshPtr = RenMesh::createEmpty();
	meshes_[ LODid ] = &(*meshPtr);

	//Construct a mesh instance and add to the entity
    add( _NEW( RenMeshInstance( meshPtr ) ), distance, LODid );
}

bool W4dCustom::loadUniqueMesh
(
	const SysPathName& fileName,
	const string& meshName,
	W4dDistance distance,
	W4dLOD LODid
)
{
	Ren::MeshPtr meshPtr = RenMesh::createUnique(fileName, meshName);
    ASSERT_INFO( fileName );
    ASSERT_INFO( meshName );
	ASSERT(meshPtr.isDefined(), runtime_error("Failed to load mesh from file."));

	meshes_[ LODid ] = &(*meshPtr);

	//Construct a mesh instance and add to the entity
    add( _NEW( RenMeshInstance( meshPtr ) ), distance, LODid );

	return true;
}

RenMesh& W4dCustom::mesh( W4dLOD LODid )
{
    PRE( LODid < meshes_.size() );
    return *meshes_[ LODid ];
}

//virtual
bool W4dCustom::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const W4dCustom& custom )
{
    const W4dEntity& base = custom;

    ostr << base;
}

void perRead( PerIstream& istr, W4dCustom& custom )
{
    W4dEntity& base = custom;

    istr >> base;
}

W4dCustom::W4dCustom( PerConstructor con )
: W4dEntity( con )
{
}

void W4dCustom::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dCustom& t )
{
    o << "W4dCustom " << (void*)&t << " start" << std::endl;
    o << "W4dCustom " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CUSTOM.CPP ***************************************************/
