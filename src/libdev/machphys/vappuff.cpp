/*
 * V A P P U F F . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"

#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/vappuff.hpp"
#include "render/device.hpp"
#include "render/capable.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "sim/manager.hpp"
#include "world4d/matplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/matseqpl.hpp"
#include "world4d/visplan.hpp"
#include "phys/linemoti.hpp"
#include "machphys/effects.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/trailprj.hpp"
#include "machphys/machphys.hpp"

PER_DEFINE_PERSISTENT( MachPhysVapourPuff );

MachPhysVapourPuff::MachPhysVapourPuff(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MATHEX_SCALAR size, size_t missile_level )
: W4dSprite3d( pParent, localTransform, size, size, initialMaterial( missile_level ) ),
  missile_level_( missile_level )
{
	PRE( missile_level >= MachPhysTrailedProjectile::MISSILE1 and
		 missile_level <  MachPhysTrailedProjectile::NMISSILES);

    visible( false );

    TEST_INVARIANT;
}

MachPhysVapourPuff::MachPhysVapourPuff( PerConstructor con )
: W4dSprite3d( con )
{
}

MachPhysVapourPuff::~MachPhysVapourPuff()
{
    TEST_INVARIANT;
}

void MachPhysVapourPuff::startPuff()
{
    //ensure we have an entityPlan, and remove all existing plans
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.clearAnimation( WISP_ANIMATION_ID );

    //Add the material plan that cycles through the smoke textures
    PhysAbsoluteTime now = SimManager::instance().currentTime();
    uint nRepetitions = 0;
    entityPlan.materialPlan( materialPlanPtr( missile_level_ ), now, nRepetitions, WISP_ANIMATION_ID );

    //Add the scale plan that makes the wisp grow
    entityPlan.scalePlan( scalePlanPtr( missile_level_ ), now, nRepetitions, WISP_ANIMATION_ID );

    //Add the visibility plan that makes it go off after the material plan has finished
    entityPlan.visibilityPlan( visibilityPlanPtr( missile_level_ ), now, nRepetitions, WISP_ANIMATION_ID );
}

//static
W4dScalePlanPtr MachPhysVapourPuff::scalePlanPtr( size_t missile_level )
{
    const MATHEX_SCALAR initialScale = 0.5;
    const MATHEX_SCALAR finalScale = 5.5;
    const MATHEX_SCALAR duration = nFrames( missile_level ) * timePerFrame() * 0.5;

    static W4dScalePlanPtr scalePlanPtr_(
      _NEW( W4dSimpleUniformScalePlan( initialScale, finalScale, duration ) ) );

    return scalePlanPtr_;
}

//static
W4dMaterialPlanPtr MachPhysVapourPuff::materialPlanPtr( size_t missile_level )
{
    static W4dMaterialPlanPtr materialPlanPtr1 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE1),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE1).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr2 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE2),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE2).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr3 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE3),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE3).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr4 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE4),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE4).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr5 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE5),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE5).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

  static W4dMaterialPlanPtr materialPlanPtr6 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE6),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE6).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr7 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE7),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE7).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr8 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::MISSILE_LARGE),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::MISSILE_LARGE).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr9 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::NUCLEAR_MISSILE),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::NUCLEAR_MISSILE).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr10 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::BEE_BOMB),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::BEE_BOMB).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    static W4dMaterialPlanPtr materialPlanPtr11 =
        _NEW(W4dMaterialSequencePlan( materials(MachPhysTrailedProjectile::METAL_STING),
                                      PhysRelativeTime ( materials(MachPhysTrailedProjectile::METAL_STING).size() * timePerFrame() ),
                                      W4dLOD( 0 ) ) );

    W4dMaterialPlanPtr materialPlanPtr;

	switch( missile_level )
	{
		case MachPhysTrailedProjectile::MISSILE1:
		{
			materialPlanPtr = materialPlanPtr1;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE2:
		{
			materialPlanPtr = materialPlanPtr2;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE3:
		{
			materialPlanPtr = materialPlanPtr3;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE4:
		{
			materialPlanPtr = materialPlanPtr4;
 			break;
		}
		case MachPhysTrailedProjectile::MISSILE5:
		{
			materialPlanPtr = materialPlanPtr5;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE6:
		{
			materialPlanPtr = materialPlanPtr6;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE7:
		{
			materialPlanPtr = materialPlanPtr7;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE_LARGE:
		{
			materialPlanPtr = materialPlanPtr8;
			break;
		}
		case MachPhysTrailedProjectile::NUCLEAR_MISSILE:
		{
			materialPlanPtr = materialPlanPtr9;
			break;
		}
		case MachPhysTrailedProjectile::BEE_BOMB:
		{
			materialPlanPtr = materialPlanPtr10;
			break;
		}
		case MachPhysTrailedProjectile::METAL_STING:
		{
			materialPlanPtr = materialPlanPtr11;
			break;
		}
        default:
            ASSERT_BAD_CASE_INFO( missile_level );
            break;
	}
    return  materialPlanPtr;
}

// static
void MachPhysVapourPuff::preload( void )
{
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE1 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE2 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE3 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE4 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE5 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE6 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE7 );
    materialPlanPtr(MachPhysTrailedProjectile::MISSILE_LARGE );
    materialPlanPtr(MachPhysTrailedProjectile::NUCLEAR_MISSILE );
    materialPlanPtr(MachPhysTrailedProjectile::BEE_BOMB );
    materialPlanPtr(MachPhysTrailedProjectile::METAL_STING );
}

//static
RenMaterial MachPhysVapourPuff::initialMaterial( size_t missile_level )
{
    const Materials& puffMaterials = materials(missile_level);
    const RenMaterialVec& materialVec = *(puffMaterials.front() );
    return materialVec.front();
}

//static
const MachPhysVapourPuff::Materials&  MachPhysVapourPuff::materials( size_t missile_level )
{

	static Materials mat1 = createMaterials( MachPhysTrailedProjectile::MISSILE1 );
	static Materials mat2 = createMaterials( MachPhysTrailedProjectile::MISSILE2 );
	static Materials mat3 = createMaterials( MachPhysTrailedProjectile::MISSILE3 );
	static Materials mat4 = createMaterials( MachPhysTrailedProjectile::MISSILE4 );
	static Materials mat5 = createMaterials( MachPhysTrailedProjectile::MISSILE5 );
	static Materials mat6 = createMaterials( MachPhysTrailedProjectile::MISSILE6 );
	static Materials mat7 = createMaterials( MachPhysTrailedProjectile::MISSILE7 );
	static Materials mat8 = createMaterials( MachPhysTrailedProjectile::MISSILE_LARGE );
	static Materials mat9 = createMaterials( MachPhysTrailedProjectile::NUCLEAR_MISSILE );
	static Materials mat10 = createMaterials( MachPhysTrailedProjectile::BEE_BOMB );
	static Materials mat11 = createMaterials( MachPhysTrailedProjectile::METAL_STING );

    Materials* materials = NULL;

	switch( missile_level )
	{
		case MachPhysTrailedProjectile::MISSILE1:
		{
			materials = &mat1;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE2:
		{
			materials = &mat2;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE3:
		{
			materials = &mat3;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE4:
		{
			materials = &mat4;
 			break;
		}
		case MachPhysTrailedProjectile::MISSILE5:
		{
			materials = &mat5;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE6:
		{
			materials = &mat6;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE7:
		{
			materials = &mat7;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE_LARGE:
		{
			materials = &mat8;
			break;
		}
		case MachPhysTrailedProjectile::NUCLEAR_MISSILE:
		{
			materials = &mat9;
			break;
		}
		case MachPhysTrailedProjectile::BEE_BOMB:
		{
			materials = &mat10;
			break;
		}
		case MachPhysTrailedProjectile::METAL_STING:
		{
			materials = &mat11;
			break;
		}
        default:
            ASSERT_BAD_CASE_INFO( missile_level );
            break;
	}
	return *materials;
}

//static
MachPhysVapourPuff::Materials MachPhysVapourPuff::createMaterials( size_t missile_level )
{
	MachPhys::WeaponType weaponType = MachPhys::MULTI_LAUNCHER3;
	switch( missile_level )
	{
		case MachPhysTrailedProjectile::MISSILE1:
		{
			weaponType = MachPhys::MULTI_LAUNCHER3;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE2:
		{
			weaponType = MachPhys::MULTI_LAUNCHER5;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE3:
		{
			weaponType = MachPhys::MULTI_LAUNCHER6;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE4:
		{
			weaponType = MachPhys::MULTI_LAUNCHER4;
 		break;
		}
		case MachPhysTrailedProjectile::MISSILE5:
		{
			weaponType = MachPhys::MULTI_LAUNCHER2;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE6:
		{
			weaponType = MachPhys::MULTI_LAUNCHER1;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE7:
		{
			weaponType = MachPhys::MULTI_LAUNCHER7;
			break;
		}
		case MachPhysTrailedProjectile::MISSILE_LARGE:
		{
			weaponType = MachPhys::LARGE_MISSILE;
			break;
		}
		case MachPhysTrailedProjectile::NUCLEAR_MISSILE:
		{
			weaponType = MachPhys::NUCLEAR_MISSILE;
			break;
		}
		case MachPhysTrailedProjectile::BEE_BOMB:
		{
			weaponType = MachPhys::BEE_BOMB;
			break;
		}
		case MachPhysTrailedProjectile::METAL_STING:
		{
			weaponType = MachPhys::WASP_METAL_STING;
			break;
		}
        default:
            ASSERT_BAD_CASE_INFO( missile_level );
            break;
	}

	const MachPhysWeaponData* data = &MachPhysData::instance().weaponData( weaponType );

    Materials materials;

    //See if textured alpha is available
    bool hasTexturedAlpha = RenDevice::current()->capabilities().supportsTextureAlpha();

    //Add all the materials to the static variable
    uint nTextures = data->trailTextures().size();
	ASSERT( nTextures>0, "" )
    materials.reserve( nTextures );
    MATHEX_SCALAR reciprocal = 1.0 / MATHEX_SCALAR( nTextures );

    for( size_t i = 0; i != nTextures; ++i )
    {
        //Use alpha to fade it if textured alpha not available
        MATHEX_SCALAR alpha =
            (hasTexturedAlpha ? 1.0 : 1.0 - MATHEX_SCALAR( i ) * reciprocal);
		RenTexture texture = data->trailTextures()[i];
        addMaterial( &materials, texture, alpha, data->trailColour());
    }

    return materials;
}

// static
void MachPhysVapourPuff::addMaterial
(
    Materials* pMaterials, const RenTexture& texture, MATHEX_SCALAR alpha, const RenColour& colour
)
{
    //Create the material that uses it
    RenMaterial material(RenColour::black());
	material.emissive(colour);
    material.texture( texture );

    if( alpha != 1.0 )
    {
        RenColour newColour = material.diffuse();
		newColour.a( alpha );
        material.diffuse( newColour );
    }

    //Create a material vector consisting of the single material
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
    pMaterialVec->push_back( material );

    //Add a counted pointer to the material vector to the argument
    pMaterials->push_back( Ren::MaterialVecPtr( pMaterialVec ) );
}

//static
size_t  MachPhysVapourPuff::nFrames( size_t missile_level )
{
    return materials(missile_level).size();
}

void MachPhysVapourPuff::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVapourPuff& t )
{

    o << "MachPhysVapourPuff " << (void*)&t << " start" << std::endl;
    o << "MachPhysVapourPuff " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const PhysRelativeTime MachPhysVapourPuff::timePerFrame()
{
    //This values represents a frame rate of 20
    return 0.05;
}


W4dVisibilityPlanPtr MachPhysVapourPuff::visibilityPlanPtr(  size_t missile_level )
{
    //set up the visibility plan to hav ethe same duration as the material plan
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );

    visibilityPlanPtr->add( false, materialPlanPtr( missile_level )->duration() );

    return visibilityPlanPtr;
}

void perWrite( PerOstream& ostr, const MachPhysVapourPuff& puff )
{
    const W4dSprite3d& base = puff;

    ostr << base;
	ostr << puff.missile_level_;
}

void perRead( PerIstream& istr, MachPhysVapourPuff& puff )
{
    W4dSprite3d& base = puff;

    istr >> base;
	istr >> puff.missile_level_;
}

/* End VAPPUFF.CPP **************************************************/
