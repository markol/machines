/*
 * M A T P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/matplan.hpp"

#ifndef _INLINE
    #include "world4d/matplan.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( W4dMaterialPlan );

W4dMaterialPlan::W4dMaterialPlan( const PhysRelativeTime& duration, W4dLOD maxLOD )
:   duration_( duration ),
    maxLOD_( maxLOD )
{
    TEST_INVARIANT;
}

W4dMaterialPlan::W4dMaterialPlan( PerConstructor )
{
}

W4dMaterialPlan::~W4dMaterialPlan()
{
    TEST_INVARIANT;

}

void W4dMaterialPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( duration_ >= 0.0 );
    INVARIANT( maxLOD_ >= 0 );
}

ostream& operator <<( ostream& o, const W4dMaterialPlan& t )
{

    o << "W4dMaterialPlan " << (void*)&t << " start" << std::endl;
    o << "  duration " << t.duration_ << std::endl;
    o << "  maxLod " << t.maxLOD_ << std::endl;
    o << "W4dMaterialPlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dMaterialPlan& plan )
{
    ostr << plan.duration_;
    ostr << plan.maxLOD_;
}

void perRead( PerIstream& istr, W4dMaterialPlan& plan )
{
    istr >> plan.duration_;
    istr >> plan.maxLOD_;
}

/* End MATPLAN.CPP **************************************************/
