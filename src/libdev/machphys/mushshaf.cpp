/*
 * M U S H S H A F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/mushshaf.hpp"
#include "machphys/private/otherper.hpp"
#include "mathex/transf3d.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"

#include "system/pathname.hpp"
#include "ctl/list.hpp"

PER_DEFINE_PERSISTENT( MachPhysMushroomShaft );

//One-time ctor
MachPhysMushroomShaft::MachPhysMushroomShaft()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/nuke/nuke_col.lod" ) );

	setMaterialFogMultipliers();

    TEST_INVARIANT;
}

//public ctor
MachPhysMushroomShaft::MachPhysMushroomShaft( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysMushroomShaft::~MachPhysMushroomShaft()
{
    TEST_INVARIANT;

}

//static
const MachPhysMushroomShaft& MachPhysMushroomShaft::exemplar()
{
    return MachPhysOtherPersistence::instance().mushroomShaftExemplar();
}

void MachPhysMushroomShaft::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMushroomShaft& t )
{

    o << "MachPhysMushroomShaft " << (void*)&t << " start" << std::endl;
    o << "MachPhysMushroomShaft " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysMushroomShaft::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysMushroomShaft& mush )
{
    const W4dEntity& base = mush;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMushroomShaft& mush )
{
    W4dEntity& base = mush;

    istr >> base;
}

MachPhysMushroomShaft::MachPhysMushroomShaft( PerConstructor c )
:W4dEntity( c )
{
}

static void setFogMultipliers( const W4dEntity* entity )
{
	PRE( entity );

	if( entity->hasMesh() )
	{
		RenMesh* meshPtr( _CONST_CAST( RenMesh*, &( *entity->mesh().mesh() ) ) );

		std::auto_ptr<RenMaterialVec> materialASet = meshPtr->materialVec();
		RenMaterialVec& materialSet = *materialASet;
	    for( RenMaterialVec::iterator i = materialSet.begin(); i != materialSet.end(); ++i )
	    {
	        if( (*i).hasFogMultiplier() )
	        {
				(*i).fogMultiplier( 2.0 );
			}
	    }

		meshPtr->materialVec( &materialSet );
	}
}

void MachPhysMushroomShaft::setMaterialFogMultipliers()
{
	setFogMultipliers( this );
	for( W4dEntity::W4dEntities::const_iterator it = children().begin(); it != children().end(); ++it )
	{
		setFogMultipliers( (*it) );
	}
}
/* End MUSHSHAF.CPP *************************************************/

