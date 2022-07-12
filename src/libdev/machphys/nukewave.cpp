/*
 * N U K E W A V E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/nukewave.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/machphys.hpp"

#include "stdlib/string.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"

#include "system/pathname.hpp"

#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/simplsca.hpp"

#include "mathex/transf3d.hpp"

#include "phys/lsclplan.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "ctl/list.hpp"

PER_DEFINE_PERSISTENT( MachPhysNukeWave );

//One-time ctor
MachPhysNukeWave::MachPhysNukeWave()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	//Load the mesh data
    readLODFile( SysPathName( "models/weapons/nuke/shockwav.lod" ) );

	setMaterialFogMultipliers();

    TEST_INVARIANT;
}

//public ctor
MachPhysNukeWave::MachPhysNukeWave( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysNukeWave::~MachPhysNukeWave()
{
    TEST_INVARIANT;

}

//static
const MachPhysNukeWave& MachPhysNukeWave::exemplar()
{
    return MachPhysOtherPersistence::instance().nukeWaveExemplar();
}

void MachPhysNukeWave::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysNukeWave& t )
{

    o << "MachPhysNukeWave " << (void*)&t << " start" << std::endl;
    o << "MachPhysNukeWave " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysNukeWave::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//local static
 void MachPhysNukeWave::addMaterial( MachPhysNukeWave::Materials* pMaterials, const RenTexture& texture )
{
    //Create the material that uses it
    RenMaterial material;
				//material.emissive(RenColour(1.0, 0.5, 0.5, 1.0));
				//material.diffuse(RenColour(1.0, 0.5, 0.5, 1.0));
    material.texture( texture );

    //Create a material vector consisting of the single material
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
    pMaterialVec->push_back( material );

    //Add a counted pointer to the material vector to the argument
    pMaterials->push_back( Ren::MaterialVecPtr( pMaterialVec ) );
}

// static
const MachPhysNukeWave::MaterialsPtr& MachPhysNukeWave::materialsPtr()
{
    static MachPhysNukeWave::MaterialsPtr materialsPtr;
    static bool once = true;

    if( once )
    {
        once = false;

        //Construct a vector of materials
        MachPhysNukeWave::Materials* pMaterials = _NEW( MachPhysNukeWave::Materials );
        pMaterials->reserve( 6 );

		RenTexture tex = RenTexManager::instance().createTexture( "wall0_bt.BMP" );
        addMaterial( pMaterials, tex );

		tex = RenTexManager::instance().createTexture( "wall1_bt.BMP" );
        addMaterial( pMaterials, tex );

		tex = RenTexManager::instance().createTexture( "wall2_bt.BMP" );
        addMaterial( pMaterials, tex );

		tex = RenTexManager::instance().createTexture( "wall3_bt.BMP" );
        addMaterial( pMaterials, tex );

		tex = RenTexManager::instance().createTexture( "wall4_bt.BMP" );
        addMaterial( pMaterials, tex );

		tex = RenTexManager::instance().createTexture( "wall5_bt.BMP" );
        addMaterial( pMaterials, tex );

        //Store in counted pointer
        materialsPtr = pMaterials;
    }

    return materialsPtr;
}


void MachPhysNukeWave::startNukeWave( const PhysAbsoluteTime& startTime,
                                        const PhysRelativeTime& duration,
                                        const MATHEX_SCALAR& fromRadius,
                                        const MATHEX_SCALAR& toRadius,
                                        const MATHEX_SCALAR& zScale )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();
/*
	//static
	uint frameNumber = 0;
    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( materialsPtr(), frameNumber,  duration ) );

	//++frameNumber;
	//frameNumber %= 6;

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

*/
    //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Make a simple scale plan
	const MATHEX_SCALAR iniScale = fromRadius/waveDefaultSize();
	const MATHEX_SCALAR endScale = toRadius/waveDefaultSize();

    RenNonUniformScale a(iniScale, iniScale, zScale);
    RenNonUniformScale b(endScale, endScale, zScale);
    W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

    //Propogate thru the current model
    propogateScalePlan( planPtr, startTime, 1);
}

//static
const double& MachPhysNukeWave::waveDefaultSize()
{
	static const double SIZE = 7.35535; //defined in its .x file
	return SIZE;
}

//static
bool MachPhysNukeWave::reachDistance(const PhysRelativeTime& duration,
                        	  		  const MATHEX_SCALAR& fromRadius,
                        	  	      const MATHEX_SCALAR& toRadius,
                        	  		  const  MATHEX_SCALAR& distance,
                        	  		  PhysRelativeTime* pTime)
{
	bool reach;
	*pTime = (distance - fromRadius)*duration/(toRadius - fromRadius);

	if( *pTime < 0 )
	{
		*pTime = 0;
		reach = true;
	}
	else if( *pTime > duration )
	{
		reach = false;
	}
	else
	{
		reach = true;
	}
	return reach;
}

void perWrite( PerOstream& ostr, const MachPhysNukeWave& wave )
{
    const W4dEntity& base = wave;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysNukeWave& wave )
{
    W4dEntity& base = wave;

    istr >> base;
}

MachPhysNukeWave::MachPhysNukeWave( PerConstructor c )
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

void MachPhysNukeWave::setMaterialFogMultipliers()
{
	setFogMultipliers( this );
	for( W4dEntity::W4dEntities::const_iterator it = children().begin(); it != children().end(); ++it )
	{
		setFogMultipliers( (*it) );
	}
}
/* End NUKEWAVE.CPP *************************************************/

