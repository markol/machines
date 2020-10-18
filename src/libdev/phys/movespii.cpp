/*
 * M O V E S P I I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/movespii.hpp"

PER_DEFINE_PERSISTENT( PhysMoveSpinPlanImpl );

PhysMoveSpinPlanImpl::PhysMoveSpinPlanImpl(const MexTransform3d& startLocation,
                          const MexVec3& speedVector,
						  const MexVec3& rotationAxis,
						  MATHEX_SCALAR rotationSpeed,
                          const PhysRelativeTime& endTime )
: startLocation_( startLocation ),
  speedVector_( speedVector ),
  rotationAxis_( rotationAxis ),
  rotationSpeed_(rotationSpeed ),
  endTime_( endTime )
{

    TEST_INVARIANT;
}

PhysMoveSpinPlanImpl::~ PhysMoveSpinPlanImpl()
{
    TEST_INVARIANT;

}

void  PhysMoveSpinPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const  PhysMoveSpinPlanImpl& t )
{

    o << " PhysMoveSpinPlanImpl " << (void*)&t << " start" << std::endl;
    o << " PhysMoveSpinPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysMoveSpinPlanImpl:: PhysMoveSpinPlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const  PhysMoveSpinPlanImpl& plan )
{
    ostr << plan.startLocation_;
    ostr << plan.speedVector_;
    ostr << plan.rotationAxis_;
    ostr << plan.rotationSpeed_;
    ostr << plan.endTime_;

}

void perRead( PerIstream& istr,  PhysMoveSpinPlanImpl& plan )
{
    istr >> plan.startLocation_;
    istr >> plan.speedVector_;
    istr >> plan.rotationAxis_;
    istr >> plan.rotationSpeed_;
    istr >> plan.endTime_;
}

/* End MOVESPII.CPP *************************************************/
