/*
 * F L A M E S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/point3d.hpp"

#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"

#include "sim/manager.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/manager.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matfrmpl.hpp"

#include "machphys/flames.hpp"
#include "machphys/random.hpp"
#include "machphys/effects.hpp"

PER_DEFINE_PERSISTENT( MachPhysFlame );

MachPhysFlame::MachPhysFlame(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFlameType flameType,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    MATHEX_SCALAR dOffset,
    const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration )
: W4dSprite3d( pParent, localTransform, width, height, initialMaterial( flameType ) ),
  flameType_( flameType ),
  duration_(duration)
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

    depthOffset( dOffset );
    visible( false );

    startFlame( startTime );
}

MachPhysFlame::MachPhysFlame(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFlameType flameType,
    MATHEX_SCALAR size,
    MATHEX_SCALAR dOffset,
    const PhysRelativeTime& duration )
: W4dSprite3d( pParent, localTransform, size, size, initialMaterial( flameType ) ),
  flameType_( flameType ),
  duration_(duration)
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

    depthOffset( dOffset );
    visible( false );
}

MachPhysFlame::MachPhysFlame(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFlameType flameType,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    MATHEX_SCALAR dOffset,
    const PhysRelativeTime& duration )
: W4dSprite3d( pParent, localTransform, width, height, initialMaterial( flameType ) ),
  flameType_( flameType ),
  duration_(duration)
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

    depthOffset( dOffset );
    visible( false );
	YUEAI_STREAM( " MachPhysFlame ctor " << std::endl );
}

MachPhysFlame::MachPhysFlame( PerConstructor con )
: W4dSprite3d( con )
{
}

MachPhysFlame::~MachPhysFlame()
{
    TEST_INVARIANT;
}

void MachPhysFlame::startFlame( const PhysRelativeTime& startTime )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();


	//static uint frameNumber = 0;
    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( materialsPtr( flameType_ ),
                                    /*frameNumber, */MachPhysRandom::randomInt(0, 15),
                                    duration_ ) );
	//frameNumber+= 5;
	//frameNumber %= 16;

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

    //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration_ );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );
}

// static
MachPhysFlameType MachPhysFlame::randomFlame()
{
    MachPhysFlameType    result = FLAME_1;
    return result;
}

// static
void MachPhysFlame::preload()
{
    materials1Ptr();
}

// static
const MachPhysFlame::MaterialsPtr&   MachPhysFlame::materials1Ptr()
{
    static MaterialsPtr materialsPtr;
    static bool once = true;

    if( once )
    {
        once = false;

        //Get the textures
        const MachPhysEffects::Textures& textures = MachPhysEffects::flameTextures();
        uint nTextures = textures.size();

        //Construct a vector of materials
        Materials* pMaterials = _NEW( Materials );
        pMaterials->reserve( nTextures );

        for( size_t i = 0; i != nTextures; ++i )
            addMaterial( pMaterials, textures[i] );

        //Store in counted pointer
        materialsPtr = pMaterials;
    }

    return materialsPtr;
}

void MachPhysFlame::addMaterial( Materials* pMaterials, const RenTexture& texture )
{
    //Create the material that uses it
    RenMaterial material(RenColour::black());
	material.emissive(RenColour::white());
    material.texture( texture );

    //Create a material vector consisting of the single material
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
    pMaterialVec->push_back( material );

    //Add a counted pointer to the material vector to the argument
    pMaterials->push_back( Ren::MaterialVecPtr( pMaterialVec ) );
}

RenMaterial MachPhysFlame::initialMaterial( MachPhysFlameType flameType )
{
    const Materials& flameMaterials = *materialsPtr( flameType );
    const RenMaterialVec& materialVec = *(flameMaterials.front() );
    return materialVec.front();
}

void MachPhysFlame::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//static
const MachPhysFlame::MaterialsPtr&   MachPhysFlame::materialsPtr
(
    MachPhysFlameType flameType
)
{
    switch( flameType )
    {
        case FLAME_1:
            return materials1Ptr();
        default:
            ASSERT_BAD_CASE_INFO( flameType );
    }

    return materials1Ptr();
}

void MachPhysFlame::startBurning( const PhysAbsoluteTime& startTime )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( materialsPtr( flameType_ ),
                                    MachPhysRandom::randomInt(0, 15),
                                    duration_ ) );

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

    //Make it visible immediately
    visible( true );
}

void perWrite( PerOstream& ostr, const MachPhysFlame& flame )
{
    const W4dSprite3d& base = flame;

    ostr << base;

    ostr << flame.duration_;
    ostr << flame.flameType_;
}

void perRead( PerIstream& istr, MachPhysFlame& flame )
{
    W4dSprite3d& base = flame;

    istr >> base;

    istr >> flame.duration_;
    istr >> flame.flameType_;
}

ostream& operator <<( ostream& ostr, MachPhysFlameType type )
{
    switch( type )
    {
        case FLAME_1:
            ostr << "flame 1";
            break;
        default:
            ASSERT_BAD_CASE_INFO( (int)type );
    }

    return ostr;
}

/* End FLAMES.CPP ***************************************************/
