/*
 * O R I E N T P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/orientpl.hpp"
#include "world4d/entity.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysOrientationAndPositionPlan );

MachPhysOrientationAndPositionPlan::MachPhysOrientationAndPositionPlan( W4dEntity* pEntity, const PhysRelativeTime& duration )
:   PhysMotionPlan( duration ),
	pEntity_( pEntity ),
	localPosition_( pEntity->localTransform().position() ),
	zGlobal_(pEntity->globalTransform().position().z() )
{

    TEST_INVARIANT;
}

MachPhysOrientationAndPositionPlan::~MachPhysOrientationAndPositionPlan()
{
    TEST_INVARIANT;

}

void MachPhysOrientationAndPositionPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysOrientationAndPositionPlan& t )
{

    o << "MachPhysOrientationAndPositionPlan " << (void*)&t << " start" << std::endl;
    o << "MachPhysOrientationAndPositionPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachPhysOrientationAndPositionPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{
	PRE( pResult != NULL );

	//obtain the current parentTransform
	MexTransform3d parentTransform = pEntity_->pParent()->globalTransform();

	//set the global position of the entity	to be just above ground level
	MexPoint3d globalPosition = localPosition_;
	parentTransform.transform( &globalPosition );
	globalPosition.z( zGlobal_ );

	MexTransform3d globalXform = MexTransform3d();
	               globalXform.position( globalPosition );

	//the new local transform:  *pResult = inV(parentTransform) * globalXform
	parentTransform.transformInverse( globalXform, pResult );
}

MachPhysOrientationAndPositionPlan::MachPhysOrientationAndPositionPlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysOrientationAndPositionPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pEntity_;
	ostr << plan.localPosition_;
	ostr << plan.zGlobal_;
}

void perRead( PerIstream& istr, MachPhysOrientationAndPositionPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pEntity_;
	istr >> plan.localPosition_;
	istr >> plan.zGlobal_;
}

/* End ORIENTPL.CPP **************************************************/
