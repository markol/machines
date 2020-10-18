/*
 * M O V E S P I N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/quatern.hpp"
#include "mathex/radians.hpp"
#include "phys/movespin.hpp"
#include "phys/internal/movespii.hpp"

PER_DEFINE_PERSISTENT( PhysMoveSpinPlan );

PhysMoveSpinPlan::PhysMoveSpinPlan(const MexTransform3d& startPosition,
                          const MexVec3& speedVector,
						  const MexVec3& rotationAxis,
						  MATHEX_SCALAR rotationSpeed,
                          const PhysRelativeTime& endTime )
: PhysMotionPlan( endTime ),
  pImpl_( _NEW( PhysMoveSpinPlanImpl( startPosition, speedVector, rotationAxis, rotationSpeed, endTime ) ) )
{

    TEST_INVARIANT;
}

PhysMoveSpinPlan::~PhysMoveSpinPlan()
{
    TEST_INVARIANT;
	_DELETE(pImpl_);
}

void PhysMoveSpinPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void PhysMoveSpinPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{

    CB_DEPIMPL( MexTransform3d, startLocation_ )
    CB_DEPIMPL( MexVec3, speedVector_ )
    CB_DEPIMPL( MexVec3, rotationAxis_ )
    CB_DEPIMPL( MATHEX_SCALAR, rotationSpeed_ )

    PRE( pResult != NULL );
    PRE( timeOffset >= 0.0 );

	// initialise
	*pResult = startLocation_;

	// translate
    MexVec3 displacementVector( speedVector_ );
    displacementVector*=timeOffset;
	pResult->translate( MexPoint3d( displacementVector ) );

	// rotate
	MATHEX_SCALAR rotationAngle = rotationSpeed_ * timeOffset;
	pResult->rotate( MexQuaternion( rotationAxis_, MexRadians( rotationAngle ) ) );

}

ostream& operator <<( ostream& o, const PhysMoveSpinPlan& t )
{

    o << "PhysMoveSpinPlan " << (void*)&t << " start" << std::endl;
    o << "PhysMoveSpinPlan " << (void*)&t << " end" << std::endl;

    return o;
}

PhysMoveSpinPlan::PhysMoveSpinPlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const PhysMoveSpinPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysMoveSpinPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}
/* End MOVESPIN.CPP *************************************************/
