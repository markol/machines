/*
 * F L A M E S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/stflames.hpp"

//#include "stdlib/string.hpp"
//#include "system/pathname.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/soundman.hpp"

#include "machphys/random.hpp"
#include "machphys/snddata.hpp"
#include "machphys/machphys.hpp"

PER_DEFINE_PERSISTENT( MachPhysSTFFlame );

MachPhysSTFFlame::MachPhysSTFFlame(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFlameType flameType,
    MATHEX_SCALAR width,
    MATHEX_SCALAR height,
    const MexVec3& spinAxis,
    const PhysRelativeTime& duration )
: W4dSpinSprite( pParent, localTransform, width, height, MachPhysFlame::initialMaterial( flameType ), spinAxis ),
  flameType_( flameType ),
  duration_(duration),
  startedBurning_( false )
{
	// The current model is all black with emissive flames and things.  Hence, 
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);
    visible( false );

	// Make sure flames do not get affected by construction animation
	clientData( (int)MachPhys::ECD_DONTAPPLYANIMATION );
}

MachPhysSTFFlame::MachPhysSTFFlame( PerConstructor con )
: W4dSpinSprite( con ),
  startedBurning_( false )
{
}

MachPhysSTFFlame::~MachPhysSTFFlame()
{
    TEST_INVARIANT;
}

void MachPhysSTFFlame::startFlame( const PhysRelativeTime& startTime )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

	static uint frameNumber = 0;
    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( MachPhysFlame::materialsPtr( flameType_ ),
                                    frameNumber, //MachPhysRandom::randomInt(0, 15),
                                    duration_ ) );
	frameNumber+= 5;
	frameNumber %= 16;

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

    //Add the visibility plan
    W4dVisibilityPlanPtr visibilityPlanPtr = _NEW( W4dVisibilityPlan( true ) );
    visibilityPlanPtr->add( false, duration_ );
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );
	
	//If this flame is just becoming visible then play the associated sound
	if(!startedBurning_)
	{
		startedBurning_ = true;
		W4dSoundManager::instance().playForDuration(this, SID_BURNING, 0, duration_);
	}
}


void MachPhysSTFFlame::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


void MachPhysSTFFlame::startBurning( const PhysAbsoluteTime& startTime )
{
    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

    //Construct a frame based material plan with a random frame offset
    W4dMaterialFramePlan* pMaterialPlan =
        _NEW( W4dMaterialFramePlan( MachPhysFlame::materialsPtr( flameType_ ),
                                    MachPhysRandom::randomInt(0, 15),
                                    duration_ ) );

    W4dMaterialPlanPtr materialPlanPtr( pMaterialPlan );
    entityPlan.materialPlan( materialPlanPtr, startTime );

    //Make it visible immediately
    visible( true );
}

void perWrite( PerOstream& ostr, const MachPhysSTFFlame& flame )
{
    const W4dSpinSprite& base = flame;
    
    ostr << base;

    ostr << flame.duration_;
    ostr << flame.flameType_;
}

void perRead( PerIstream& istr, MachPhysSTFFlame& flame )
{
    W4dSpinSprite& base = flame;
    
    istr >> base;

    istr >> flame.duration_;
    istr >> flame.flameType_;
}

/* End FLAMES.CPP ***************************************************/
