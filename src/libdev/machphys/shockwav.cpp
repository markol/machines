/*
 * S H O C K W A V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/shockwav.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"
#include "machphys/machphys.hpp"

#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"

#include "system/pathname.hpp"

#include "world4d/root.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"

#include "mathex/transf3d.hpp"

#include "phys/lsclplan.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

PER_DEFINE_PERSISTENT( MachPhysShockWave );

//One-time ctor
MachPhysShockWave::MachPhysShockWave()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
    readLODFile( SysPathName( "models/destroy/mach/shockwav.lod" ) );

    TEST_INVARIANT;
}

//public ctor
MachPhysShockWave::MachPhysShockWave( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( exemplar(), pParent, localTransform )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysShockWave::~MachPhysShockWave()
{
    TEST_INVARIANT;

}

//static
const MachPhysShockWave& MachPhysShockWave::exemplar()
{
	return MachPhysOtherPersistence::instance().shockWaveExemplar();
}

void MachPhysShockWave::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysShockWave& t )
{

    o << "MachPhysShockWave " << (void*)&t << " start" << std::endl;
    o << "MachPhysShockWave " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysShockWave::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//local defied in
void MachPhysShockWave::addMaterial( MachPhysShockWave::Materials* pMaterials, const RenTexture& texture )
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
const MachPhysShockWave::MaterialsPtr& MachPhysShockWave::materialsPtr( void )
{
    static MachPhysShockWave::MaterialsPtr materialsPtr;
    static bool once = true;

    if( once )
    {
        once = false;

        //Construct a vector of materials
        MachPhysShockWave::Materials* pMaterials = _NEW( MachPhysShockWave::Materials );
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


void MachPhysShockWave::startShockWave( const PhysAbsoluteTime& startTime,
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
	const MATHEX_SCALAR iniScale = fromRadius/shockwaveDefaultSize();
	const MATHEX_SCALAR endScale = toRadius/shockwaveDefaultSize();

    RenNonUniformScale a(iniScale, iniScale, zScale);
    RenNonUniformScale b(endScale, endScale, zScale);
    W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

    //Propogate thru the current model
    propogateScalePlan( planPtr, startTime, 1);


    //fade out in the last quater of duration
	//RenMaterialVec* pFlashMaterialVec =_CONST_CAST(const W4dEntity*, this)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pFlashMaterialVec =_CONST_CAST(const W4dEntity*, _STATIC_CAST(W4dEntity*, this))
        ->mesh().mesh()->materialVec().release();
	RenMaterial& matFlash = (*pFlashMaterialVec)[0];

    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(4);
		times.push_back(duration*4.0/16.0);
		times.push_back(duration*7.0/16.0);
		times.push_back(duration*10.0/16.0);
		times.push_back(duration*13.0/16.0);
		times.push_back(duration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(5);
        scales.push_back(1.0);
        scales.push_back(1.0);
        scales.push_back(0.5625);
        scales.push_back(0.25);
        scales.push_back(0.0625);
        scales.push_back(0.0); 			// 1 - 2x + x^2

    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

    W4dMaterialPlanPtr alphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matFlash, pFlashMaterialVec->size(), lineScalarPlanPtr, 1 ) );
    entityPlanForEdit().materialPlan( alphaPlanPtr, startTime );

    //Delete the temporary material vector obtained from the mesh
    _DELETE( pFlashMaterialVec );
}

//static
const double& MachPhysShockWave::shockwaveDefaultSize()
{
	static const double SIZE = 2.0; //defined in its .x file
	return SIZE;
}

//static
bool MachPhysShockWave::reachDistance(const PhysRelativeTime& duration,
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

void perWrite( PerOstream& ostr, const MachPhysShockWave& wave )
{
    const W4dEntity& base = wave;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysShockWave& wave )
{
    W4dEntity& base = wave;

    istr >> base;
}

MachPhysShockWave::MachPhysShockWave( PerConstructor c )
:W4dEntity( c )
{
}

/* End SHOCKWAV.CPP *************************************************/

