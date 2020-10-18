/*
 * M U S H T O P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "machphys/mushtop.hpp"
#include "machphys/private/otherper.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "ctl/list.hpp"

PER_DEFINE_PERSISTENT( MachPhysMushroomTop );

//One-time ctor
MachPhysMushroomTop::MachPhysMushroomTop()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/nuke/nuke_msh.lod" ) );

	setMaterialFogMultipliers();

    TEST_INVARIANT;
}

//public ctor
MachPhysMushroomTop::MachPhysMushroomTop( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysMushroomTop::~MachPhysMushroomTop()
{
    TEST_INVARIANT;

}

//static
const MachPhysMushroomTop& MachPhysMushroomTop::exemplar()
{
    return MachPhysOtherPersistence::instance().mushroomTopExemplar();
}

void MachPhysMushroomTop::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMushroomTop& t )
{

    o << "MachPhysMushroomTop " << (void*)&t << " start" << std::endl;
    o << "MachPhysMushroomTop " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysMushroomTop::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysMushroomTop& mush )
{
    const W4dEntity& base = mush;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMushroomTop& mush )
{
    W4dEntity& base = mush;

    istr >> base;
}

MachPhysMushroomTop::MachPhysMushroomTop( PerConstructor c )
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

void MachPhysMushroomTop::setMaterialFogMultipliers()
{
	setFogMultipliers( this );
	for( W4dEntity::W4dEntities::const_iterator it = children().begin(); it != children().end(); ++it )
	{
		setFogMultipliers( (*it) );
	}
}

/* End MUSHTOP.CPP *************************************************/

