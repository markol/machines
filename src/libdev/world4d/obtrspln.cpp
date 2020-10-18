/*
 * O B T R S P L N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/obtrspln.hpp"
#include "render/scale.hpp"

PER_DEFINE_PERSISTENT( W4dObjectTrackerScalePlan );

W4dObjectTrackerScalePlan::W4dObjectTrackerScalePlan
(
    W4dObjectTrackerPtr objectTrackerPtr, const PhysRelativeTime& duration
)
:   W4dScalePlan( duration ),
    objectTrackerPtr_( objectTrackerPtr )
{
    TEST_INVARIANT;
}

W4dObjectTrackerScalePlan::W4dObjectTrackerScalePlan( PerConstructor con )
: W4dScalePlan( con )
{
}

W4dObjectTrackerScalePlan::~W4dObjectTrackerScalePlan()
{
    TEST_INVARIANT;

}

void W4dObjectTrackerScalePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dObjectTrackerScalePlan& t )
{

    o << "W4dObjectTrackerScalePlan " << (void*)&t << " start" << std::endl;
    o << "W4dObjectTrackerScalePlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool W4dObjectTrackerScalePlan::isNonUniform() const
{
    return true;
}

//virtual
void W4dObjectTrackerScalePlan::doScale( const PhysRelativeTime&, RenUniformScale* ) const
{
    ASSERT( false, "" );
}

//virtual
void W4dObjectTrackerScalePlan::doScale
(
    const PhysRelativeTime&, RenNonUniformScale* pScale
) const
{
    //Get the current length from the object tracker
    MexTransform3d temp;
    MATHEX_SCALAR length = objectTrackerPtr_->track( &temp );

    //Scale is determined by current length related to default length
    MATHEX_SCALAR xScale = length / objectTrackerPtr_->defaultLength();

    //Set the resultant non-uniform scale
    pScale->factors( xScale, 1.0, 1.0 );

}

//virtual
W4dScalePlan* W4dObjectTrackerScalePlan::doTransformClone
(
    const MexTransform3d&
) const
{
    //Just clone this plan. NB only works if x axis not moved
    return _NEW( W4dObjectTrackerScalePlan( objectTrackerPtr_, duration() ) );
}

void perWrite( PerOstream& ostr, const W4dObjectTrackerScalePlan& plan )
{
    const W4dScalePlan& base = plan;

    ostr << base;
    ostr << plan.objectTrackerPtr_;
}

void perRead( PerIstream& istr, W4dObjectTrackerScalePlan& plan )
{
    W4dScalePlan& base = plan;

    istr >> base;
    istr >> plan.objectTrackerPtr_;
}

/* End OBTRSPLN.CPP *************************************************/
