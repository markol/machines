/*
 * M A S T E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/persist.hpp"

#include "pertest/per.hpp"

#include "pertest/master.hpp"

// #include "pertest/pvector.hpp"

PER_DEFINE_PERSISTENT( Master );

Master::Master()
{
    pC1_ = _NEW( Class1() );

    pTc1_ = _NEW( TC1a );

    TEST_INVARIANT;
}

Master::~Master()
{
    TEST_INVARIANT;

    _DELETE( pC1_ );
}

void Master::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const Master& t )
{
	ostr << t.c1_;
    ostr << t.pC1_;

	ostr << t.tc1_;
    ostr << t.pTc1_;

    ostr << t.p1_;
}

void perRead( PerIstream& istr, Master& t )
{
	istr >> t.c1_;
    istr >> t.pC1_;

	istr >> t.tc1_;
    istr >> t.pTc1_;

    istr >> t.p1_;
}

ostream& operator <<( ostream& o, const Master& t )
{
    o << "Master " << (void*)&t << " start" << std::endl;

	o << t.c1_;
    if( t.pC1_ )
        o << *t.pC1_;

    o << t.p1_;

    o << "Master " << (void*)&t << " end" << std::endl;

    return o;
}

bool operator ==( const Master& a, const Master& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.c1_, b.c1_ );
    COMPARE_POINTERS( a.pC1_, b.pC1_ );
    COMPARE_OBJECTS( a.p1_, b.p1_ );

    return result;
}

/* End MASTER.CPP ***************************************************/
