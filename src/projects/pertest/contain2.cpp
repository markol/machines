/*
 * C O N T A I N 2 . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/contain2.hpp"
#include "pertest/persist.hpp"

PER_DEFINE_PERSISTENT( Containers2 );

Containers2::Containers2()
{
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        pvi1_.push_back( _NEW( int( randomInt() ) ) );
    pPVi1_ = _NEW( PVectorInt( randomInt( 20 ) ) );
    pPVi2_ = pPVi1_;
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        pPVi1_->push_back( _NEW( int( randomInt() ) ) );


    TEST_INVARIANT;
}

Containers2::~Containers2()
{
    TEST_INVARIANT;

}

void perWrite( PerOstream& ostr, const Containers2& con )
{
    ostr << con.pvi1_;
    ostr << con.pPVi1_;
    ostr << con.pPVi2_;
}

void perRead( PerIstream& istr, Containers2& con )
{
    istr >> con.pvi1_;
    istr >> con.pPVi1_;
    istr >> con.pPVi2_;
}

bool operator ==( const Containers2& a, const Containers2& b )
{
    bool    result = true;
    
    COMPARE_OBJECTS( a.pvi1_, b.pvi1_ );
    COMPARE_POINTERS( a.pPVi1_, b.pPVi1_ );
    COMPARE_POINTERS( a.pPVi2_, b.pPVi2_ );

    return result;
}

void Containers2::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Containers2& t )
{

    o << "Containers2 " << (void*)&t << " start" << endl;
    o << "Containers2 " << (void*)&t << " end" << endl;

    return o;
}

/* End CONTAIN2.CPP **************************************************/
