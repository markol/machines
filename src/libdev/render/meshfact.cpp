/*
 * M E S H F A C T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/meshfact.hpp"
#include "render/mesh.hpp"

// static
RenIMeshFactory& RenIMeshFactory::instance()
{
    static RenIMeshFactory instance_;
    return instance_;
}

RenIMeshFactory::RenIMeshFactory()
{
	PRE(Ren::initialised());
    TEST_INVARIANT;
}

RenIMeshFactory::~RenIMeshFactory()
{
    TEST_INVARIANT;

}

// virtual
Ren::ConstMeshPtr RenIMeshFactory::doCreatePart( const RenIMeshID& id )
{
    ASSERT_INFO( id.pathName() );
    ASSERT_INFO( id.combinedName() );
    ASSERT_INFO( id.scale() );

	return RenMesh::createUnique(id.pathName(), id.combinedName(), id.scale());
}

void RenIMeshFactory::removeFromFactory(Ren::ConstMeshPtr mesh)
{
	// Unfortunately, this function must delete a *value* from the map,
	// rather than a key.  So, we have to do a linear search.  We traverse
	// the entire map, just in case there are two copies of mesh (although,
	// the logic of the factoy should ensure that there's only one).
	PartMap::iterator it = partMap().begin();
	while (it != partMap().end())
	{
		PartMap::iterator it2 = it;
		++it;

		if ((*it2).second == mesh)
			partMap().erase(it2);
	}
}

void RenIMeshFactory::removeAllFromFactory()
{
	partMap().erase(partMap().begin(), partMap().end());
	POST(partMap().size() == 0);
}

void RenIMeshFactory::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenIMeshFactory& )
{
    return o;
}

/* End MESHFACT.CPP *************************************************/
