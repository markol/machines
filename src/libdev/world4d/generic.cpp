/*
 * G E N E R I C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/generic.hpp"

#include "system/pathname.hpp"
#include "render/meshinst.hpp"
#include "mathex/double.hpp"
#include "world4d/internal/nolight.hpp"

#include "ctl/vector.hpp"
#include "stdlib/string.hpp"

PER_DEFINE_PERSISTENT( W4dGeneric );

W4dGeneric::W4dGeneric(W4dEntity* pParent, const W4dTransform3d& localXform, Solidity solidity)
: W4dEntity(pParent, localXform, solidity)
{
    TEST_INVARIANT;
}

W4dGeneric::W4dGeneric(const W4dGeneric& copyMe, W4dEntity* pParent, const W4dTransform3d& localXform)
: W4dEntity( copyMe, pParent, localXform )
{
}

W4dGeneric::~W4dGeneric()
{
    TEST_INVARIANT;
}

void W4dGeneric::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool W4dGeneric::loadSingleMesh(const SysPathName& fileName, const string& meshName, double scale)
{
	RenMeshInstance* pMesh = RenMeshInstance::createShared(fileName, meshName, scale);
    ASSERT_INFO( fileName );
    ASSERT_INFO( meshName );
	ASSERT(pMesh, runtime_error("Failed to load mesh from file."));

	// Since this is the only mesh, give it a near infinite transition range.
	add( pMesh, HUGE_VAL, 0 );

	name(meshName);

	// Check to see if this entity is entirely emissive.
	if (!w4dNeedsLighting(this))
	{
		doNotLight(true);
		IAIN_STREAM("Disabled lighting for " << fileName << ", " << meshName << "\n");
	}

	return true;
}

bool W4dGeneric::loadLODFile(const SysPathName& filename)
{
	readLODFile(filename);

    //  Use the last part of the name without any extension

    SysPathName withoutExt( filename );
    withoutExt.extension( "" );

	name(withoutExt.components().back());
	return true;
}

bool W4dGeneric::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const
{
    //Just check against bounding volume
    return defaultIntersectsLine( line, pDistance, accuracy );
}

void perWrite( PerOstream& ostr, const W4dGeneric& generic )
{
    const W4dEntity& base = generic;

    ostr << base;
}

void perRead( PerIstream& istr, W4dGeneric& generic )
{
    W4dEntity& base = generic;

    istr >> base;
}

W4dGeneric::W4dGeneric( PerConstructor con )
: W4dEntity( con )
{
}

W4dGeneric::W4dGeneric(const W4dEntity& copyMe, W4dEntity* pParent, const W4dTransform3d& localXform)
: W4dEntity( copyMe, pParent, localXform )
{
}

/* End GENERIC.CPP **************************************************/
