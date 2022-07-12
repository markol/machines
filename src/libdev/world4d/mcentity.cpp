/*
 * M C E N T I T Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/mcentity.hpp"
#include "world4d/entity.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"

void W4dMotionControlledEntity::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


W4dMotionControlledEntity::W4dMotionControlledEntity( W4dEntity* pEntity )
: pMotionControlledEntity_( pEntity )
{

    TEST_INVARIANT;
}

// virtual
W4dMotionControlledEntity::~W4dMotionControlledEntity()
{
    TEST_INVARIANT;

}

// virtual
void W4dMotionControlledEntity::setPosition( const MexTransform3d& trans )
{
	if( trans.key() != pMotionControlledEntity_->globalTransform().key() )
		pMotionControlledEntity_->globalTransform( trans );
}

// virtual
void W4dMotionControlledEntity::setOrientation( const MexTransform3d& trans )
{
	MexTransform3d xform = pMotionControlledEntity_->localTransform();
	MexVec3 oldPos = xform.position();

	MexEulerAngles defaultAngles;
	trans.rotation( &defaultAngles );

	MexTransform3d newXform( defaultAngles, oldPos );
	pMotionControlledEntity_->localTransform(newXform);
}

// virtual
MexTransform3d W4dMotionControlledEntity::globalTransform() const
{
	return pMotionControlledEntity_->globalTransform();
}

// virtual
void W4dMotionControlledEntity::globalTransform( const MexTransform3d& trans )
{
	// only update if the transform has changed,
  	// We do not use the transform keys to do the comparison. The reason is the following:
	// PhysMotionControl::update  (PhysZenithFlyControl::update for instance) calls
	// W4dMotionControlledEntity::globalTransform() to get the transform, does some processing on the transform
	// and calls W4dMotionControlledEntity::globalTransform( const MexTransform3d & )  to set it
	// The processing affects the transform even if it does not need to do so. i.e. the key of the transform
	// in W4dMotionControlledEntity will be modified even though the transform is not.
	// This test allows us to doing lazy evaluation based on the camera transform keys in domainRender
	// Why doesn't the operator != compile in debug ??
	if( not (trans == pMotionControlledEntity_->globalTransform() ) )
		pMotionControlledEntity_->globalTransform( trans );
}

// virtual
MexTransform3d W4dMotionControlledEntity::localTransform() const
{
   return pMotionControlledEntity_->localTransform();
}

// virtual
void W4dMotionControlledEntity::localTransform( const MexTransform3d& trans )
{
	pMotionControlledEntity_->localTransform( trans );
}

ostream& operator <<( ostream& o, const W4dMotionControlledEntity& t )
{

    o << "W4dMotionControlledEntity " << (void*)&t << " start" << std::endl;
    o << "W4dMotionControlledEntity " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MCENTITY.CPP *************************************************/
