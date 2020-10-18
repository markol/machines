/*
 * C L A S S P A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "pertest/persist.hpp"
#include "pertest/classpa.hpp"
#include "pertest/per.hpp"

PER_DEFINE_PERSISTENT( ClassPair );

ClassPair::ClassPair()
: p1_( randomInt(), randomDouble() )
{
    pP1_ = _NEW( Pair( randomInt(), randomDouble() ) );

    TEST_INVARIANT;
}

ClassPair::~ClassPair()
{
    TEST_INVARIANT;

    _DELETE( pP1_ );
}

void ClassPair::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const ClassPair& t )
{
    o << "Pair ( " << t.p1_.first << t.p1_.second << " )" << std::endl;

    return o;
}

bool operator ==( const ClassPair& a, const ClassPair& b )
{
    bool    result = true;

    COMPARE_OBJECTS( a.p1_, b.p1_ );

    return result;
}

bool operator !=( const ClassPair& a, const ClassPair& b )
{
    return !(a == b);
}

void perWrite( PerOstream& ostr, const ClassPair& p )
{
    ostr << p.p1_;
}

void perRead( PerIstream& istr, ClassPair& p )
{
    istr >> p.p1_;
}

/* End CLASSPA.CPP **************************************************/
