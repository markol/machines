#include "machphys/internal/firebali.hpp"

#include "system/pathname.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/matvec.hpp"
#include "render/colour.hpp"
#include "render/render.hpp"

#include "world4d/matseqpl.hpp"

MachPhysFireballImpl::MachPhysFireballImpl(
   	MachPhysFireballType fireballType,
   	MATHEX_SCALAR size,
   	const PhysRelativeTime& duration )
: size_( size ),
  maxDuration_( duration ),
  fireballType_( fireballType )
{
}

MachPhysFireballImpl::MachPhysFireballImpl()
: size_( 0 ),
  maxDuration_( 0 ),
  fireballType_(  )
{
	//Empty
}

MachPhysFireballImpl::~MachPhysFireballImpl()
{
	//Empty
}

void MachPhysFireballImpl::addMaterial( Materials* pMaterials, const string& textureFileName )
{
    //Create the texture
    RenTexture texture = RenTexManager::instance().createTexture( textureFileName );

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

RenMaterial MachPhysFireballImpl::initialMaterial( MachPhysFireballType fireballType )
{
    const Materials* pMaterials = NULL;

    switch( fireballType )
    {
        //All fireballs are the same at the moment
        case FIREBALL_1:
        case FIREBALL_2:
        case FIREBALL_3:
        case FIREBALL_4:
            pMaterials = &materials1();
            break;

        default:
            ASSERT_BAD_CASE_INFO( fireballType );
            break;
    }

    const RenMaterialVec& materialVec = *(pMaterials->front() );
    return materialVec.front();
}

const MachPhysFireballImpl::Materials&   MachPhysFireballImpl::materials1()
{
    static Materials materials;
    static bool once = true;

    if( once )
    {
        once = false;
        materials.reserve( 10 );

		addMaterial( &materials, "bng00_bt.bmp" );
		addMaterial( &materials, "bng01_bt.bmp" );
		addMaterial( &materials, "bng02_bt.bmp" );
		addMaterial( &materials, "bng03_bt.bmp" );
		addMaterial( &materials, "bng04_bt.bmp" );
		addMaterial( &materials, "bng05_bt.bmp" );
		addMaterial( &materials, "bng06_bt.bmp" );
		addMaterial( &materials, "bng07_bt.bmp" );
		addMaterial( &materials, "bng08_bt.bmp" );
		addMaterial( &materials, "bng09_bt.bmp" );

		/*
        addMaterial( &materials, "ex00_bt.bmp" );
        addMaterial( &materials, "ex01_bt.bmp" );
        addMaterial( &materials, "ex02_bt.bmp" );
        addMaterial( &materials, "ex03_bt.bmp" );
        addMaterial( &materials, "ex04_bt.bmp" );
        addMaterial( &materials, "ex05_bt.bmp" );
        addMaterial( &materials, "ex06_bt.bmp" );
        addMaterial( &materials, "ex07_bt.bmp" );
        addMaterial( &materials, "ex08_bt.bmp" );
        addMaterial( &materials, "ex09_bt.bmp" );
        addMaterial( &materials, "ex10_bt.bmp" );
        addMaterial( &materials, "ex11_bt.bmp" );
        addMaterial( &materials, "ex12_bt.bmp" );
        addMaterial( &materials, "ex13_bt.bmp" );
        addMaterial( &materials, "ex14_bt.bmp" );
        addMaterial( &materials, "ex15_bt.bmp" );
        addMaterial( &materials, "ex16_bt.bmp" );
        addMaterial( &materials, "ex17_bt.bmp" );
        addMaterial( &materials, "ex18_bt.bmp" );
        addMaterial( &materials, "ex19_bt.bmp" );
        addMaterial( &materials, "ex20_bt.bmp" );
        addMaterial( &materials, "ex21_bt.bmp" );
        addMaterial( &materials, "ex22_bt.bmp" );
        addMaterial( &materials, "ex23_bt.bmp" );
        addMaterial( &materials, "ex24_bt.bmp" );
        addMaterial( &materials, "ex25_bt.bmp" );
        addMaterial( &materials, "ex26_bt.bmp" );
        addMaterial( &materials, "ex27_bt.bmp" );
		*/
    }

    return materials;
}

// static
const MachPhysFireballImpl::Materials&   MachPhysFireballImpl::materials2()
{
    static Materials materials;
    static bool once = true;

    if( once )
    {
        once = false;
        materials.reserve( 20 );

        //     addMaterial( &materials, "fb200_bt.bmp" );
        //     addMaterial( &materials, "fb201_bt.bmp" );
        //     addMaterial( &materials, "fb202_bt.bmp" );
        //     addMaterial( &materials, "fb203_bt.bmp" );
        //     addMaterial( &materials, "fb204_bt.bmp" );
        //     addMaterial( &materials, "fb205_bt.bmp" );
        //     addMaterial( &materials, "fb206_bt.bmp" );
        //     addMaterial( &materials, "fb207_bt.bmp" );
        //     addMaterial( &materials, "fb208_bt.bmp" );
        //     addMaterial( &materials, "fb209_bt.bmp" );
        //     addMaterial( &materials, "fb210_bt.bmp" );
        //     addMaterial( &materials, "fb211_bt.bmp" );
        addMaterial( &materials, "fb212_bt.bmp" );
        addMaterial( &materials, "fb213_bt.bmp" );
        addMaterial( &materials, "fb214_bt.bmp" );
        addMaterial( &materials, "fb215_bt.bmp" );
        addMaterial( &materials, "fb216_bt.bmp" );
        addMaterial( &materials, "fb217_bt.bmp" );
        addMaterial( &materials, "fb218_bt.bmp" );
        addMaterial( &materials, "fb219_bt.bmp" );
        addMaterial( &materials, "fb220_bt.bmp" );
        addMaterial( &materials, "fb221_bt.bmp" );
        addMaterial( &materials, "fb222_bt.bmp" );
        addMaterial( &materials, "fb223_bt.bmp" );
        addMaterial( &materials, "fb224_bt.bmp" );
        addMaterial( &materials, "fb225_bt.bmp" );
        addMaterial( &materials, "fb226_bt.bmp" );
        addMaterial( &materials, "fb227_bt.bmp" );
        addMaterial( &materials, "fb228_bt.bmp" );
    }

    return materials;
}

// static
const MachPhysFireballImpl::Materials&   MachPhysFireballImpl::materials3()
{
    static Materials materials;
    static bool once = true;

    if( once )
    {
        once = false;
        materials.reserve( 11 );

        addMaterial( &materials, "fb300_bt.bmp" );
        addMaterial( &materials, "fb301_bt.bmp" );
        addMaterial( &materials, "fb302_bt.bmp" );
        addMaterial( &materials, "fb303_bt.bmp" );
        addMaterial( &materials, "fb304_bt.bmp" );
        addMaterial( &materials, "fb305_bt.bmp" );
        addMaterial( &materials, "fb306_bt.bmp" );
        addMaterial( &materials, "fb307_bt.bmp" );
        addMaterial( &materials, "fb308_bt.bmp" );
        addMaterial( &materials, "fb309_bt.bmp" );
        addMaterial( &materials, "fb310_bt.bmp" );
    }

    return materials;
}

// static
const MachPhysFireballImpl::Materials&   MachPhysFireballImpl::materials4()
{
    static Materials materials;
    static bool once = true;

    if( once )
    {
        once = false;
        materials.reserve( 20 );

        //     addMaterial( &materials, "fb400_bt.bmp" );
        //     addMaterial( &materials, "fb401_bt.bmp" );
        addMaterial( &materials, "fb402_bt.bmp" );
        addMaterial( &materials, "fb403_bt.bmp" );
        addMaterial( &materials, "fb404_bt.bmp" );
        addMaterial( &materials, "fb405_bt.bmp" );
        addMaterial( &materials, "fb406_bt.bmp" );
        addMaterial( &materials, "fb407_bt.bmp" );
        addMaterial( &materials, "fb408_bt.bmp" );
        addMaterial( &materials, "fb409_bt.bmp" );
        addMaterial( &materials, "fb410_bt.bmp" );
        addMaterial( &materials, "fb411_bt.bmp" );
        addMaterial( &materials, "fb412_bt.bmp" );
        addMaterial( &materials, "fb413_bt.bmp" );
        addMaterial( &materials, "fb414_bt.bmp" );
        addMaterial( &materials, "fb415_bt.bmp" );
        addMaterial( &materials, "fb416_bt.bmp" );
    }

    return materials;
}

// static
W4dMaterialPlanPtr MachPhysFireballImpl::materialPlanPtr1()
{
    static  W4dMaterialPlanPtr materialPlanPtr =
        _NEW(W4dMaterialSequencePlan( materials1(),
                                      PhysRelativeTime ( materials1().size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    return  materialPlanPtr;
}

// static
W4dMaterialPlanPtr MachPhysFireballImpl::materialPlanPtr2()
{
    static  W4dMaterialPlanPtr materialPlanPtr =
        _NEW(W4dMaterialSequencePlan( materials2(),
                                      PhysRelativeTime ( materials2().size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    return  materialPlanPtr;
}

// static
W4dMaterialPlanPtr MachPhysFireballImpl::materialPlanPtr3()
{
    static  W4dMaterialPlanPtr materialPlanPtr =
        _NEW(W4dMaterialSequencePlan( materials3(),
                                      PhysRelativeTime ( materials3().size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    return  materialPlanPtr;
}

// static
W4dMaterialPlanPtr MachPhysFireballImpl::materialPlanPtr4()
{
    static  W4dMaterialPlanPtr materialPlanPtr =
        _NEW(W4dMaterialSequencePlan( materials4(),
                                      PhysRelativeTime ( materials4().size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    return  materialPlanPtr;
}

// why segfault if return is reference type?
const PhysRelativeTime MachPhysFireballImpl::timePerFrame()
{
    //This values represents a frame rate of 20
    return 0.05;
}

//static
const W4dVisibilityPlanPtr& MachPhysFireballImpl::visibilityPlanPtr1()
{
    static W4dVisibilityPlanPtr planPtr =
        _NEW( W4dVisibilityPlan( true ) );

    static bool once = true;
    if( once )
    {
        once = false;
        planPtr->add( false, materialPlanPtr1()->duration() );
    }

    return planPtr;
}

//static
const W4dVisibilityPlanPtr& MachPhysFireballImpl::visibilityPlanPtr2()
{
    static W4dVisibilityPlanPtr planPtr =
        _NEW( W4dVisibilityPlan( true ) );

    static bool once = true;
    if( once )
    {
        once = false;
        planPtr->add( false, materialPlanPtr2()->duration() );
    }

    return planPtr;
}

//static
const W4dVisibilityPlanPtr& MachPhysFireballImpl::visibilityPlanPtr3()
{
    static W4dVisibilityPlanPtr planPtr =
        _NEW( W4dVisibilityPlan( true ) );

    static bool once = true;
    if( once )
    {
        once = false;
        planPtr->add( false, materialPlanPtr3()->duration() );
    }

    return planPtr;
}

//static
const W4dVisibilityPlanPtr& MachPhysFireballImpl::visibilityPlanPtr4()
{
    static W4dVisibilityPlanPtr planPtr =
        _NEW( W4dVisibilityPlan( true ) );

    static bool once = true;
    if( once )
    {
        once = false;
        planPtr->add( false, materialPlanPtr4()->duration() );
    }

    return planPtr;
}

//static
W4dMaterialPlanPtr MachPhysFireballImpl::materialPlanPtr( MachPhysFireballType fireballType )
{
    switch( fireballType )
    {
        //All fireballs the same at the moment
        case FIREBALL_1:
        case FIREBALL_2:
        case FIREBALL_3:
        case FIREBALL_4:
            return materialPlanPtr1();

        default:
            ASSERT_BAD_CASE_INFO( fireballType );
    }

    return materialPlanPtr1();
}

//static
const W4dVisibilityPlanPtr& MachPhysFireballImpl::visibilityPlanPtr( MachPhysFireballType fireballType )
{
    switch( fireballType )
    {
        //All fireballs are the same for now
        case FIREBALL_1:
        case FIREBALL_2:
        case FIREBALL_3:
        case FIREBALL_4:
            return visibilityPlanPtr1();

        default:
            ASSERT_BAD_CASE_INFO( fireballType );
    }

    return visibilityPlanPtr1();
}

const MachPhysFireballImpl::Materials&   MachPhysFireballImpl::materials
(  MachPhysFireballType fireballType )
{
    switch( fireballType )
    {
        //All fireballs the same for now
        case FIREBALL_1:
        case FIREBALL_2:
        case FIREBALL_3:
        case FIREBALL_4:
            return materials1();

        default:
            ASSERT_BAD_CASE_INFO( fireballType );
    }

    return materials1();
}
