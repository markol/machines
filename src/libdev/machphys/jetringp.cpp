/*
 * J E T R I N G P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/jetringp.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/entity.hpp"

PER_DEFINE_PERSISTENT( MachPhysJetRingPlan );

MachPhysJetRingPlan::MachPhysJetRingPlan( W4dEntity* pEntity, const MexPoint3d& targetOffsetGlobal, const PhysRelativeTime& duration )
:   PhysMotionPlan( duration ),
pEntity_( pEntity ),
localPosition_( pEntity->localTransform().position() ),
targetOffsetGlobal_( targetOffsetGlobal )
{

    TEST_INVARIANT;
}

MachPhysJetRingPlan::~MachPhysJetRingPlan()
{
    TEST_INVARIANT;

}

void MachPhysJetRingPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysJetRingPlan& t )
{

    o << "MachPhysJetRingPlan " << (void*)&t << " start" << std::endl;
    o << "MachPhysJetRingPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachPhysJetRingPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{
	PRE( pResult != NULL );

	//obtain the current parentTransform
	MexTransform3d parentTransform = pEntity_->pParent()->globalTransform();

	MexPoint3d originGlobal = localPosition_;
	parentTransform.transform( &originGlobal );

	MexVec3 xAxis(originGlobal, targetOffsetGlobal_ );
	xAxis.z( 0.0 );
	xAxis.makeUnitVector();

	MexVec3 zAxis( 0, 0, 1 );
	ASSERT( xAxis.isPerpendicularTo( zAxis ), "xAxis is not perpendicular to z axis" );

	MexVec3 yAxis = MexVec3::crossProduct( zAxis, xAxis );

	MexTransform3d newTransform(xAxis, yAxis, zAxis, originGlobal );
	parentTransform.transformInverse( newTransform, pResult );

/*
	parentTransform.transformInverse( &xAxis );
	MexVec3 planeDefiningVector;

	if( fabs( xAxis.x()) <= 0.00001 )
		planeDefiningVector = MexVec3(1, 0, 0);

	else if( fabs( xAxis.y()) <= 0.00001 )
		planeDefiningVector = MexVec3(0, 1, 0);

	else if( fabs( xAxis.z()) <= 0.00001 )
		planeDefiningVector = MexVec3(0, 0, 1);

	else
	{
		MATHEX_SCALAR x = 2.0*localPosition_.x();
		MATHEX_SCALAR y = 2.0*localPosition_.y();

		MATHEX_SCALAR z = localPosition_.z() -
		                  (xAxis.x()*localPosition_.x() - xAxis.y()*localPosition_.y())/xAxis.z();

		if( fabs(x)<= 0.00001 and fabs(y)<=0.00001 and fabs(z)<=0.00001 )
		{
			x = 1.0;
			y = 1.0;
			z = -( xAxis.x() + xAxis.y() )/xAxis.z();
		}
		planeDefiningVector = MexVec3(x, y, z );
	}
    *pResult = MexTransform3d( MexTransform3d::X_XZ, xAxis, planeDefiningVector, localPosition_ );
*/
}

MachPhysJetRingPlan::MachPhysJetRingPlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysJetRingPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pEntity_;
	ostr << plan.localPosition_;
	ostr << plan.targetOffsetGlobal_;
}

void perRead( PerIstream& istr, MachPhysJetRingPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pEntity_;
	istr >> plan.localPosition_;
	istr >> plan.targetOffsetGlobal_;
}

/* End JETRINGP.CPP **************************************************/
