/*
 * S C A L P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/scalplan.hpp"

#ifndef _INLINE
    #include "world4d/scalplan.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( W4dScalePlan );

W4dScalePlan::W4dScalePlan( const PhysRelativeTime& duration )
:   duration_( duration )
{
    PRE( duration >= 0.0 )

    TEST_INVARIANT;
}

W4dScalePlan::W4dScalePlan( PerConstructor )
{
}

W4dScalePlan::~W4dScalePlan()
{
    TEST_INVARIANT;

}

void W4dScalePlan::scale( const PhysRelativeTime& timeOffset, RenUniformScale* pScale ) const
{
    PRE( not isNonUniform() );
    doScale( timeOffset, pScale );
}

void W4dScalePlan::scale( const PhysRelativeTime& timeOffset, RenNonUniformScale* pScale ) const
{
    PRE( isNonUniform() );
    doScale( timeOffset, pScale );
}

W4dScalePlan* W4dScalePlan::transformClone( const MexTransform3d& offsetTransform ) const
{
    PRE( isNonUniform() );
    return doTransformClone( offsetTransform );
}

void W4dScalePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dScalePlan& t )
{

    o << "W4dScalePlan " << (void*)&t << " start" << std::endl;
    o << "W4dScalePlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dScalePlan& plan )
{
    ostr << plan.duration_;
}

void perRead( PerIstream& istr, W4dScalePlan& plan )
{
    istr >> plan.duration_;
}

/* End SCALPLAN.CPP *************************************************/
