/*
 * G U S P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/gusplan.hpp"

PER_DEFINE_PERSISTENT( W4dGeneralUniformScalePlan );

W4dGeneralUniformScalePlan::W4dGeneralUniformScalePlan( const PhysScalarPlanPtr&  scalarPlanPtr )
:W4dScalePlan(scalarPlanPtr->duration()),
 physScalarPlanPtr_(scalarPlanPtr)
{
    TEST_INVARIANT;
}

W4dGeneralUniformScalePlan::W4dGeneralUniformScalePlan( PerConstructor con )
: W4dScalePlan( con )
{
}

W4dGeneralUniformScalePlan::~W4dGeneralUniformScalePlan()
{
    TEST_INVARIANT;

}

void W4dGeneralUniformScalePlan::physScalarPlan(const PhysScalarPlanPtr& plan)
{
	physScalarPlanPtr_ = plan;
}

const W4dGeneralUniformScalePlan::PhysScalarPlanPtr& W4dGeneralUniformScalePlan::physScalarPlan() const
{
	return physScalarPlanPtr_;
}

void W4dGeneralUniformScalePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dGeneralUniformScalePlan& t )
{

    o << "W4dGeneralUniformScalePlan " << (void*)&t << " start" << std::endl;
    o << "W4dGeneralUniformScalePlan " << (void*)&t << " end" << std::endl;

    return o;
}

    //return the defined uniform scale at timeOffset in pScale
//virtual
void W4dGeneralUniformScalePlan::doScale( const PhysRelativeTime& timeOffset, RenUniformScale* pScale ) const
{
    PRE( timeOffset >= 0.0 );

    //Check for past end time
        MATHEX_SCALAR s = physScalarPlanPtr_->scalar(timeOffset);
        pScale->factor( s );
}

//virtual
void W4dGeneralUniformScalePlan::doScale( const PhysRelativeTime&, RenNonUniformScale* ) const
{
	ASSERT(false, "	Invalid scale type. ");
}

//virtual
W4dScalePlan* W4dGeneralUniformScalePlan::doTransformClone( const MexTransform3d& ) const
{
	ASSERT(false, "	Invalid scale type. ");
	return NULL;
}

bool W4dGeneralUniformScalePlan::isNonUniform() const
{
	return false;
}

void perWrite( PerOstream& ostr, const W4dGeneralUniformScalePlan& plan )
{
    const W4dScalePlan& base = plan;

    ostr << base;
    ostr << plan.physScalarPlanPtr_;
}

void perRead( PerIstream& istr, W4dGeneralUniformScalePlan& plan )
{
    W4dScalePlan& base = plan;

    istr >> base;
    istr >> plan.physScalarPlanPtr_;
}

/* End GUSPLAN.CPP **************************************************/
