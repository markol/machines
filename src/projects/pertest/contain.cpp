/*
 * C O N T A I N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/persist.hpp"
#include "pertest/contain.hpp"
#include "pertest/per.hpp"

PER_DEFINE_PERSISTENT( Containers );

Containers::Containers()
: fvi1_( randomInt( 200 ) )
{
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        vi1_.push_back( randomInt() );
    pVi1_ = _NEW( VectorInt( randomInt( 20 ) ) );
    pVi2_ = pVi1_;
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        pVi1_->push_back( randomInt() );

    for( size_t i = 0; i < randomInt( 20 ); ++i )
        mmvi1_.push_back( randomInt() );
    pMmvi1_ = _NEW( MinMemVectorInt( randomInt( 20 ) ) );
    pMmvi2_ = pMmvi1_;
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        pMmvi1_->push_back( randomInt() );

    for( size_t i = 0; i < fvi1_.size(); ++i )
        fvi1_[ i ] = randomInt();
    pFvi1_ = _NEW( FixedVectorInt( randomInt( 20 ) ) );
    pFvi2_ = pFvi1_;
    for( size_t i = 0; i < pFvi1_->size(); ++i )
        (*pFvi1_)[ i ] = randomInt();

    for( size_t i = 0; i < randomInt( 20 ); ++i )
        li1_.push_back( randomInt() );
    pLi1_ = _NEW( ListInt( randomInt( 20 ) ) );
    pLi2_ = pLi1_;
    for( size_t i = 0; i < randomInt( 20 ); ++i )
        pLi1_->push_back( randomInt() );

//    for( size_t i = 0; i < randomInt( 20 ); ++i )
//        pvi1_.push_back( _NEW( int ) );


    TEST_INVARIANT;
}

Containers::~Containers()
{
    TEST_INVARIANT;

}

void perWrite( PerOstream& ostr, const Containers& con )
{
    ostr << con.vi1_;
    ostr << con.pVi1_;
    ostr << con.pVi2_;

    ostr << con.mmvi1_;
    ostr << con.pMmvi1_;
    ostr << con.pMmvi2_;

    ostr << con.fvi1_;
    ostr << con.pFvi1_;
    ostr << con.pFvi2_;

    ostr << con.li1_;
    ostr << con.pLi1_;
    ostr << con.pLi2_;

//     ostr << con.pvi1_;
//     ostr << con.pPVi1_;
//     ostr << con.pPVi2_;
}

void perRead( PerIstream& istr, Containers& con )
{
    istr >> con.vi1_;
    istr >> con.pVi1_;
    istr >> con.pVi2_;

    istr >> con.mmvi1_;
    istr >> con.pMmvi1_;
    istr >> con.pMmvi2_;

    istr >> con.fvi1_;
    istr >> con.pFvi1_;
    istr >> con.pFvi2_;

    istr >> con.li1_;
    istr >> con.pLi1_;
    istr >> con.pLi2_;

//     istr >> con.pvi1_;
//     istr >> con.pPVi1_;
//     istr >> con.pPVi2_;
}

bool operator ==( const Containers& a, const Containers& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.vi1_, b.vi1_ );
    COMPARE_POINTERS( a.pVi1_, b.pVi1_ );
    COMPARE_POINTERS( a.pVi2_, b.pVi2_ );

    COMPARE_OBJECTS( a.mmvi1_, b.mmvi1_ );
    COMPARE_POINTERS( a.pMmvi1_, b.pMmvi1_ );
    COMPARE_POINTERS( a.pMmvi2_, b.pMmvi2_ );

    COMPARE_OBJECTS( a.fvi1_, b.fvi1_ );
    COMPARE_POINTERS( a.pFvi1_, b.pFvi1_ );
    COMPARE_POINTERS( a.pFvi2_, b.pFvi2_ );

    COMPARE_OBJECTS( a.li1_, b.li1_ );
    COMPARE_POINTERS( a.pLi1_, b.pLi1_ );
    COMPARE_POINTERS( a.pLi2_, b.pLi2_ );

    return result;
}

void Containers::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Containers& t )
{

    o << "Containers " << (void*)&t << " start" << std::endl;
    o << "Containers " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CONTAIN.CPP **************************************************/
