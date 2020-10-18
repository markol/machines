/*
 * S I M P L S C A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/simplsca.hpp"

PER_DEFINE_PERSISTENT( W4dSimpleUniformScalePlan );

//////////////////////////////////////////////////////////////////////////////

W4dSimpleUniformScalePlan::W4dSimpleUniformScalePlan
(
    MATHEX_SCALAR startScale, MATHEX_SCALAR endScale, const PhysRelativeTime& duration
)
:   W4dScalePlan( duration),
    startScale_( startScale ),
    endScale_( endScale )
{
    PRE( duration >= 0.0 )

    TEST_INVARIANT;
}

W4dSimpleUniformScalePlan::W4dSimpleUniformScalePlan( PerConstructor con )
: W4dScalePlan( con )
{
}

W4dSimpleUniformScalePlan::~W4dSimpleUniformScalePlan()
{
    TEST_INVARIANT;

}

//virtual
bool W4dSimpleUniformScalePlan::isNonUniform() const
{
    return false;
}

//virtual
void W4dSimpleUniformScalePlan::doScale
(
    const PhysRelativeTime& timeOffset, RenUniformScale* pScale
) const
{
    PRE( timeOffset >= 0.0 );

    //Check for past end time
    PhysRelativeTime myDuration = duration();
    if( timeOffset >= myDuration )
        //*pScale = endScale_;
        *pScale = RenUniformScale(endScale_);
    else
    {
        //Get proportion of way through
        MATHEX_SCALAR f = timeOffset / myDuration;

        //Hence compute the new scale and set it
        MATHEX_SCALAR s = startScale_ * (1.0 - f) + endScale_ * f;
        pScale->factor( s );
    }
}

//virtual
void W4dSimpleUniformScalePlan::doScale( const PhysRelativeTime&, RenNonUniformScale* ) const
{
    ASSERT( false, "Invalid scale type" );
}

//virtual
W4dScalePlan* W4dSimpleUniformScalePlan::doTransformClone
(
    const MexTransform3d&
) const
{
    ASSERT( false, "Invalid scale type" );
    return NULL;
}

void W4dSimpleUniformScalePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dSimpleUniformScalePlan& t )
{

    o << "W4dSimpleUniformScalePlan " << (void*)&t << " start" << std::endl;
    o << "W4dSimpleUniformScalePlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dSimpleUniformScalePlan& plan )
{
    const W4dScalePlan& base = plan;

    ostr << base;
    ostr << plan.startScale_;
    ostr << plan.endScale_;
}

void perRead( PerIstream& istr, W4dSimpleUniformScalePlan& plan )
{
    W4dScalePlan& base = plan;

    istr >> base;
    istr >> plan.startScale_;
    istr >> plan.endScale_;
}

PER_DEFINE_PERSISTENT( W4dSimpleNonUniformScalePlan );

//////////////////////////////////////////////////////////////////////////////

W4dSimpleNonUniformScalePlan::W4dSimpleNonUniformScalePlan
(
    const RenNonUniformScale& startScale, const RenNonUniformScale& endScale,
    const PhysRelativeTime& duration
)
:   W4dScalePlan( duration ),
    startScale_( startScale ),
    endScale_( endScale )
{
    PRE( duration >= 0.0 )

    TEST_INVARIANT;
}

W4dSimpleNonUniformScalePlan::W4dSimpleNonUniformScalePlan( PerConstructor con )
: W4dScalePlan( con ),
  startScale_( con ),
  endScale_( con )
{
}

W4dSimpleNonUniformScalePlan::~W4dSimpleNonUniformScalePlan()
{
    TEST_INVARIANT;
}

//virtual
bool W4dSimpleNonUniformScalePlan::isNonUniform() const
{
    return true;
}

//virtual
void W4dSimpleNonUniformScalePlan::doScale
(
    const PhysRelativeTime& timeOffset, RenNonUniformScale* pScale
) const
{
    PRE( timeOffset >= 0.0 );

    //Check for past end time
    PhysRelativeTime myDuration = duration();
    if( timeOffset >= myDuration )
        *pScale = endScale_;
    else
    {
        //Get proportion of way through
        MATHEX_SCALAR f = timeOffset / myDuration;
        MATHEX_SCALAR g = 1.0 - f;

        //Hence compute the new scale and set it
        MATHEX_SCALAR sx = startScale_.x() * g + endScale_.x() * f;
        MATHEX_SCALAR sy = startScale_.y() * g + endScale_.y() * f;
        MATHEX_SCALAR sz = startScale_.z() * g + endScale_.z() * f;
        pScale->factors( sx, sy, sz );
    }
}

//virtual
void W4dSimpleNonUniformScalePlan::doScale( const PhysRelativeTime&, RenUniformScale* ) const
{
    ASSERT( false, "Invalid scale type" );
}

//virtual
W4dScalePlan* W4dSimpleNonUniformScalePlan::doTransformClone
(
    const MexTransform3d& offsetTransform
) const
{
    return _NEW( W4dSimpleNonUniformScalePlan
                 (
                    RenNonUniformScale( startScale_, offsetTransform ),
                    RenNonUniformScale( endScale_, offsetTransform ),
                    duration()
                 )
               );
}

void W4dSimpleNonUniformScalePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dSimpleNonUniformScalePlan& t )
{

    o << "W4dSimpleNonUniformScalePlan " << (void*)&t << " start" << std::endl;
    o << "W4dSimpleNonUniformScalePlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dSimpleNonUniformScalePlan& plan )
{
    const W4dScalePlan& base = plan;

    ostr << base;
    ostr << plan.startScale_;
    ostr << plan.endScale_;
}

void perRead( PerIstream& istr, W4dSimpleNonUniformScalePlan& plan )
{
    W4dScalePlan& base = plan;

    istr >> base;
    istr >> plan.startScale_;
    istr >> plan.endScale_;
}

//////////////////////////////////////////////////////////////////////////////
/* End SIMPLSCA.CPP *************************************************/
