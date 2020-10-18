/*
 * F I R E B A L L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/firebali.hpp"
#include "machphys/fireball.hpp"
#include "machphys/random.hpp"
#include "machphys/snddata.hpp"

#include "world4d/entyplan.hpp"
#include "world4d/soundman.hpp"


PER_DEFINE_PERSISTENT( MachPhysFireball );


/////////////////////////////////////////////////////////////

MachPhysFireball::MachPhysFireball(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFireballType fireballType,
    MATHEX_SCALAR size,
    MATHEX_SCALAR dOffset,
    const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration )
: W4dSprite3d( pParent, localTransform, size, size, MachPhysFireballImpl::initialMaterial( fireballType ) )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	pImpl_ = _NEW(MachPhysFireballImpl( fireballType, size, duration ));

    //  Move the fireball forward so that it is in front of the object that is exploding
    depthOffset( dOffset );

    //  Make sure the fireball is only visibile while it is exploding
    visible( false );

    startFireball( startTime );
}

MachPhysFireball::MachPhysFireball(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    MachPhysFireballType fireballType,
    MATHEX_SCALAR size,
    MATHEX_SCALAR dOffset,
    const PhysRelativeTime& duration )
: W4dSprite3d( pParent, localTransform, size, size, MachPhysFireballImpl::initialMaterial( fireballType ) )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	pImpl_ = _NEW(MachPhysFireballImpl( fireballType, size, duration ));

    //  Move the fireball forward so that it is in front of the object that is exploding
    depthOffset( dOffset );

    //  Make sure the fireball is only visibile while it is exploding
    visible( false );
}

MachPhysFireball::MachPhysFireball( PerConstructor con )
: W4dSprite3d( con )
{
	pImpl_ = _NEW(MachPhysFireballImpl());
}

MachPhysFireball::~MachPhysFireball()
{
    TEST_INVARIANT;

    //  Stop any explosion sound associated with this fireball
    W4dSoundManager::instance().stop( this );

    _DELETE( pImpl_ );
}

void MachPhysFireball::startFireball( const PhysRelativeTime& startTime )
{
	CB_DEPIMPL(MachPhysFireballType, fireballType_);
	CB_DEPIMPL(MATHEX_SCALAR, size_);

    //Get the entity plan for this entity
    W4dEntityPlan& entityPlan = entityPlanForEdit();

    //Add the material plan
    const W4dMaterialPlanPtr& matPlanPtr = MachPhysFireballImpl::materialPlanPtr( fireballType_ );
    entityPlan.materialPlan( matPlanPtr, startTime );

    //Add the visibility plan
    entityPlan.visibilityPlan( MachPhysFireballImpl::visibilityPlanPtr( fireballType_ ), startTime );

//    //get the number of frames
//    uint nFrames = MachPhysFireballImpl::materials( fireballType_ ).size();
}

// static
MachPhysFireballType MachPhysFireball::randomFireball( void )
{
    MachPhysFireballType    result = FIREBALL_1;

    size_t fireball = MachPhysRandom::randomInt( N_FIREBALLS );

    switch( fireball )
    {
        case 0:
            result = FIREBALL_1;
            break;

        case 1:
            result = FIREBALL_2;
            break;

        case 2:
            result = FIREBALL_3;
            break;

        case 3:
            result = FIREBALL_4;
            break;

        default:
            ASSERT_BAD_CASE_INFO( fireball );
            break;
    }

    return result;
}

// static
void MachPhysFireball::preload( void )
{
    MachPhysFireballImpl::materialPlanPtr1();
    //materialPlanPtr2();
    //materialPlanPtr3();
    //materialPlanPtr4();
}

// static
void MachPhysFireball::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


//static

void perWrite( PerOstream& ostr, const MachPhysFireball& fireball )
{
    const W4dSprite3d& base = fireball;

    ostr << base;
    ostr << fireball.pImpl_->size_;
    ostr << fireball.pImpl_->maxDuration_;
    ostr << fireball.pImpl_->fireballType_;
}

void perRead( PerIstream& istr, MachPhysFireball& fireball )
{
    W4dSprite3d& base = fireball;

    istr >> base;
    istr >> fireball.pImpl_->size_;
    istr >> fireball.pImpl_->maxDuration_;
    istr >> fireball.pImpl_->fireballType_;
}

/* End FIREBALL.CPP *************************************************/
