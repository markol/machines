/*
 * E F F E C T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/effects.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "render/texture.hpp"
#include "world4d/matplan.hpp"
#include "world4d/matseqpl.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/entyplan.hpp"
#include "render/colour.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"
#include "render/texmgr.hpp"
#include "render/device.hpp"
#include "render/capable.hpp"
#include "ctl/nbvector.hpp"

//static
void MachPhysEffects::flashObject
(
    W4dEntity* pEntity, const RenMaterial& mat,
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,
    W4dLOD maxLOD
)
{
    //Create a material vector using mat with enough entries to cover any reasonable mesh
    const int reasonableSize = 64;
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( reasonableSize ) );
    for( int i = reasonableSize; i--; )
        pMaterialVec->push_back( mat );

    Ren::MaterialVecPtr materialVecPtr( pMaterialVec );

    //Create a single-element vector of material vectors using above material vector
    W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
    materialVecPtrs.reserve( 1 );
    materialVecPtrs.push_back( materialVecPtr );

    //Create a material plan to use
    W4dMaterialSequencePlan* pPlan =
        _NEW( W4dMaterialSequencePlan( materialVecPtrs, duration, maxLOD ) );

    for( W4dLOD lodId = 1; lodId <= maxLOD; ++lodId )
        pPlan->lodPlan( materialVecPtrs, lodId );

    W4dMaterialPlanPtr planPtr( pPlan );

    //Apply the plan to every entity from pEntity on
    for( W4dEntityIter it( pEntity ); not it.isFinished(); ++it )
        (*it).entityPlanForEdit().materialPlan( planPtr, startTime );
}

//static
const RenMaterial& MachPhysEffects::solidWhiteMaterial()
{
	static RenMaterial mat(RenColour::white());
    static bool firstTime = true;

    if( firstTime )
    {
    	mat.emissive(RenColour::white());
        firstTime = false;
    }

    return mat;
}

//static
const MachPhysEffects::Textures& MachPhysEffects::smokeTextures()
{
    static Textures theTextures;
    static bool firstTime = true;

    if( firstTime )
    {
        firstTime = false;

        //Set correct size
        theTextures.reserve( 16 );

        //Load the textures
        const char* names[] =
        {
            "smk00_bt.bmp",
            "smk01_bt.bmp",
            "smk02_bt.bmp",
            "smk03_bt.bmp",
            "smk04_bt.bmp",
            "smk05_bt.bmp",
            "smk06_bt.bmp",
            "smk07_bt.bmp",
            "smk08_bt.bmp",
            "smk09_bt.bmp",
            "smk10_bt.bmp",
            "smk11_bt.bmp",
            "smk12_bt.bmp",
            "smk13_bt.bmp",
            "smk14_bt.bmp",
            "smk15_bt.bmp"
        };

        for( int i = 0; i != 16; ++i )
            theTextures.push_back( RenTexManager::instance().createTexture( names[i] ) );
    }

    return theTextures;
}

//static
const W4dMaterialPlanPtr& MachPhysEffects::smokeMaterialPlan( SmokeColour smokeColour )
{
    switch( smokeColour )
    {
        case WHITE:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 1.0, 1.0, 1.0 ) )
            );
            return materialPlanPtr;
        }

        case PURPLE:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 183.0/255.0, 62.0/255.0, 197.0/255.0 ) )
            );
            return materialPlanPtr;
        }

        case YELLOW:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 200.0/255.0, 200.0/255.0, 0.0 ) )
            );
            return materialPlanPtr;
        }

        case RUST:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 1.0, 62.0/255.0, 0.0 ) )
            );
            return materialPlanPtr;
        }

        case BLACK:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 0.0, 0.0, 0.0 ) )
            );
            return materialPlanPtr;
        }

		case CIVILIAN_WHITE:
		{
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 220.0/255.0, 222.0/255.0, 224.0/255.0 ) )
            );
            return materialPlanPtr;
		}
		case MILITARY_RED:
		{
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 83.0/255.0, 19.0/255.0, 10.0/255.0 ) )
            );
            return materialPlanPtr;
		}
        case GREY:
        default:
        {
            static W4dMaterialPlanPtr materialPlanPtr
            (
                createSmokeMaterialPlan( RenColour( 200.0/255.0, 200.0/255.0, 200.0/255.0 ) )
            );
            return materialPlanPtr;
        }
    }
}

//static
W4dMaterialPlan* MachPhysEffects::createSmokeMaterialPlan( const RenColour& colour )
{
    //Get capabilities
    const RenCapabilities& capabilities = RenDevice::current()->capabilities();

    //Make the colour translucent if alpha not available
    RenColour myColour( colour );
    if( not capabilities.supportsTextureAlpha() and
        capabilities.supportsStippledAlpha() )
        myColour.a( 0.5 );

    //Create a collection of material vectors based on the argument colour
    const Textures& textures = smokeTextures();
    uint nTextures = textures.size();

    W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
    materialVecPtrs.reserve( nTextures );

    for( Textures::const_iterator it = textures.begin(); it != textures.end(); ++it )
    {
        //Make a single counted pointer to a RenMaterialVec, consisting of a single element
        RenMaterial mat( myColour );
        mat.texture( *it );

        RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
        pMaterialVec->push_back( mat );

        Ren::MaterialVecPtr materialVecPtr( pMaterialVec );
        materialVecPtrs.push_back( materialVecPtr );
    }

    //Construct a new material sequence plan
    PhysRelativeTime duration = 1.0;
    W4dLOD maxLOD = 2;
    W4dMaterialSequencePlan* pPlan =
        _NEW( W4dMaterialSequencePlan( materialVecPtrs, duration, maxLOD ) );

    return pPlan;
}

//static
const W4dMaterialPlanPtr& MachPhysEffects::sparksMaterialPlan()
{
    static W4dMaterialPlanPtr thePlan( createSparksMaterialPlan() );
    return thePlan;
}

//static
W4dMaterialPlan* MachPhysEffects::createSparksMaterialPlan()
{
    //Set up the 3 texture bitmap names
    const char* names[] = { "bhit0_bt.bmp", "bhit1_bt.bmp", "bhit2_bt.bmp" };
    const int nTextures = 3;

    //Create a collection of material vectors based on the textures
    W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
    materialVecPtrs.reserve( nTextures );

    for( size_t i = 0; i != nTextures; ++i )
    {
        //Load the texture and use it to create a material
        RenMaterial mat;
        mat.texture( RenTexManager::instance().createTexture( names[i] ) );

        RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
        pMaterialVec->push_back( mat );

        Ren::MaterialVecPtr materialVecPtr( pMaterialVec );
        materialVecPtrs.push_back( materialVecPtr );
    }

    //Construct a new material sequence plan
    PhysRelativeTime duration = 0.3;
    W4dLOD maxLOD = 2;
    W4dMaterialSequencePlan* pPlan =
        _NEW( W4dMaterialSequencePlan( materialVecPtrs, duration, maxLOD ) );

    return pPlan;

}

//static
const MachPhysEffects::Textures& MachPhysEffects::flameBallTextures()
{
    static Textures theTextures;
    static bool firstTime = true;

    if( firstTime )
    {
        firstTime = false;

        //Set correct size
        theTextures.reserve( 17 );

        //Load the textures
        const char* names[] =
        {
            "ex00_bt.bmp",
            "ex01_bt.bmp",
            "ex02_bt.bmp",
            "ex03_bt.bmp",
            "ex04_bt.bmp",
            "ex05_bt.bmp",
            "ex06_bt.bmp",
            "ex07_bt.bmp",
            "ex08_bt.bmp",
            "ex09_bt.bmp",
            "ex10_bt.bmp",
            "ex11_bt.bmp",
            "ex12_bt.bmp",
            "ex13_bt.bmp",
            "ex14_bt.bmp",
            "ex15_bt.bmp",
            "ex16_bt.bmp"
			/*
            "ex17_bt.bmp",
            "ex18_bt.bmp",
            "ex19_bt.bmp",
            "ex20_bt.bmp",
            "ex21_bt.bmp",
            "ex22_bt.bmp",
            "ex23_bt.bmp",
            "ex24_bt.bmp",
            "ex25_bt.bmp",
            "ex26_bt.bmp",
            "ex27_bt.bmp",
            */
        };

        for( int i = 0; i != 17; ++i )
            theTextures.push_back( RenTexManager::instance().createTexture( names[i] ) );
    }

    return theTextures;
}

//static
const W4dMaterialPlanPtr& MachPhysEffects::flameBallMaterialPlan()
{
	static W4dMaterialPlanPtr materialPlanPtr(createFlameBallMaterialPlan());
	return materialPlanPtr;
}

//static
W4dMaterialPlan* MachPhysEffects::createFlameBallMaterialPlan()
{
    //Get capabilities
    const RenCapabilities& capabilities = RenDevice::current()->capabilities();

    //Make the colour translucent if alpha not available
/*    RenColour myColour( colour );
    if( not capabilities.supportsTextureAlpha() and
        capabilities.supportsStippledAlpha() )
        myColour.a( 0.5 );
*/
    //Create a collection of material vectors based on the argument colour
    const Textures& textures = flameBallTextures();
    uint nTextures = textures.size();

    W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
    materialVecPtrs.reserve( nTextures );

    RenMaterial mat( RenColour(1.0, 1.0, 1.0) );
    for(int i = 0; i<nTextures; ++i )
    {
        //Make a single counted pointer to a RenMaterialVec, consisting of a single element
        mat.texture( textures[i] );

        RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
        pMaterialVec->push_back( mat );

        Ren::MaterialVecPtr materialVecPtr( pMaterialVec );
        materialVecPtrs.push_back( materialVecPtr );
    }

    //Construct a new material sequence plan
    PhysRelativeTime duration = 1.4;
    W4dLOD maxLOD = 2;
    W4dMaterialSequencePlan* pPlan =
        _NEW( W4dMaterialSequencePlan( materialVecPtrs, duration, maxLOD ) );

    return pPlan;
}

//static
const MachPhysEffects::Textures& MachPhysEffects::flameTextures()
{
    static Textures theTextures;
    static bool firstTime = true;

    if( firstTime )
    {
        firstTime = false;

        //Set correct size
        theTextures.reserve( 16 );

        //Load the textures
        const char* names[] =
        {
            "fl00_bt.bmp",
            "fl01_bt.bmp",
            "fl02_bt.bmp",
            "fl03_bt.bmp",
            "fl04_bt.bmp",
            "fl05_bt.bmp",
            "fl06_bt.bmp",
            "fl07_bt.bmp",
            "fl08_bt.bmp",
            "fl09_bt.bmp",
            "fl10_bt.bmp",
            "fl11_bt.bmp",
            "fl12_bt.bmp",
            "fl13_bt.bmp",
            "fl14_bt.bmp",
            "fl15_bt.bmp"
        };

        for( int i = 0; i != 16; ++i )
            theTextures.push_back( RenTexManager::instance().createTexture( names[i] ) );
    }

    return theTextures;
}

//static
W4dMaterialSequencePlan* MachPhysEffects::createMaterialSequencePlan
(
    const RenMaterial& baseMaterial, const Textures& textures,
    uint nMaterialsPerVector,
    const PhysAbsoluteTime& duration, W4dLOD maxLOD
)
{
    //Construct the vector of material vectors, each element vector having
    //one element consisting of the base material but using the texture from textures.
    uint nTextures = textures.size();
    W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
    materialVecPtrs.reserve( nTextures );

    RenMaterial workingMaterial( baseMaterial );

    for( size_t i = 0; i != nTextures; ++i )
    {
        workingMaterial.texture( textures[i] );

        RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( nMaterialsPerVector ) );
        for( uint j = 0; j != nMaterialsPerVector; ++j )
            pMaterialVec->push_back( workingMaterial );

        Ren::MaterialVecPtr materialVecPtr( pMaterialVec );
        materialVecPtrs.push_back( materialVecPtr );
    }

    //Hence construct the plan
    return _NEW( W4dMaterialSequencePlan( materialVecPtrs, duration, maxLOD ) );
}

/* End EFFECTS.CPP **************************************************/
