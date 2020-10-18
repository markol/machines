/*
 * D B L F L A M E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/private/otherper.hpp"

#include "stdlib/string.hpp"


#include "render/scale.hpp"
#include "render/texture.hpp"
#include "render/matvec.hpp"
#include "render/colour.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/soundman.hpp"

#include "system/pathname.hpp"

#include "machphys/dblflame.hpp"
#include "machphys/effects.hpp"
#include "machphys/snddata.hpp"

#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysDoublesidedFlame );

//One-time ctor
MachPhysDoublesidedFlame::MachPhysDoublesidedFlame()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	//Load the mesh data
    readLODFile( SysPathName( "models/fire/fire.lod" ) );

    //Ensure textures loaded
    materialsPtr();

	// Make sure flames do not get affected by construction animation
	clientData( (int)MachPhys::ECD_DONTAPPLYANIMATION );

    TEST_INVARIANT;
}

//public
MachPhysDoublesidedFlame::MachPhysDoublesidedFlame( W4dEntity* pParent,
                              const MexTransform3d& localTransform ):
W4dEntity(exemplar(), pParent, localTransform)
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	// Make sure flames do not get affected by construction animation
	clientData( (int)MachPhys::ECD_DONTAPPLYANIMATION );

    TEST_INVARIANT;
}


MachPhysDoublesidedFlame::~MachPhysDoublesidedFlame()
{
    TEST_INVARIANT;
}

//static
const MachPhysDoublesidedFlame& MachPhysDoublesidedFlame::exemplar()
{
	return MachPhysOtherPersistence::instance().doubleSidedFlameExemplar();
}

void MachPhysDoublesidedFlame::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysDoublesidedFlame& t )
{

    o << "MachPhysDoublesidedFlame " << (void*)&t << " start" << std::endl;
    o << "MachPhysDoublesidedFlame " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysDoublesidedFlame::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysDoublesidedFlame::startFlame(
    					      const MATHEX_SCALAR& width, const MATHEX_SCALAR& height,
    					      const PhysAbsoluteTime& startTime,
    					      const PhysRelativeTime& duration)
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

	static uint frameNumber = 0;
    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( materialsPtr(),
                                    frameNumber,
                                    duration ) );
	frameNumber+= 5;
	frameNumber %= 16;

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

 	//the flame model has a default size of 1 meter X 1 meter
	//scale it to the defined size
	temporaryScale( RenNonUniformScale(1.0, width, height), NOT_PROPOGATE );

   //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration );

    //Play burning sound
	W4dSoundManager::instance().playForDuration( pParent(), SID_BURNING,
                                  startTime, duration );

    if( startTime < endTime_ )
    {
	        entityPlanForEdit().clearVisibilityPlans( 10 );
			W4dSoundManager::instance().stop( pParent() );
 	}

    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Store the time at which burning ceases
    endTime_ = std::max( endTime_, startTime + duration );
}

// static
const MachPhysDoublesidedFlame::MaterialsPtr&   MachPhysDoublesidedFlame::materialsPtr( void )
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

void MachPhysDoublesidedFlame::addMaterial( Materials* pMaterials, const RenTexture& texture )
{
    //Create the material that uses it
    RenMaterial material;
				material.diffuse(RenColour::black());
				material.emissive(RenColour(1.0, 1.0, 1.0));
    material.texture( texture );

    //Create a material vector consisting of the single material
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( 1 ) );
    pMaterialVec->push_back( material );

    //Add a counted pointer to the material vector to the argument
    pMaterials->push_back( Ren::MaterialVecPtr( pMaterialVec ) );
}

MachPhysDoublesidedFlame::MachPhysDoublesidedFlame( PerConstructor con)
:W4dEntity( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysDoublesidedFlame& flame )
{
    const W4dEntity& base = flame;

    ostr << base;
    ostr << flame.endTime_;
}

void perRead( PerIstream& istr, MachPhysDoublesidedFlame& flame )
{
    W4dEntity& base = flame;

    istr >> base;
    istr >> flame.endTime_;
}

/* End DBLFLAME.CPP *************************************************/
