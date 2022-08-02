/*
 * T C L A S S 1 . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    TClass1

    Template
    Public data members
    Default constructor
    Not part of inheritance hierarchy
    Not inline
*/

#ifndef _PERTEST_TCLASS1_HPP
#define _PERTEST_TCLASS1_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

template< class T1, class T2 >
class TClass1
// Canonical form revoked
{
public:
    TClass1();
    ~TClass1();

    T1  d1_;
    T2  d2_;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TClass1< T1, T2 >& t );

    PER_MEMBER_PERSISTENT_DEFAULT( TClass1 );

private:
    TClass1( const TClass1< T1, T2 >& );
    TClass1< T1, T2 >& operator =( const TClass1< T1, T2 >& );
    bool operator ==( const TClass1< T1, T2 >& );

};

PER_READ_WRITE_T2( TClass1 );
PER_DECLARE_PERSISTENT_T2( TClass1 );

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "pertest/tclass1.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "pertest/tclass1.ctf"
//#endif


#endif

/* End TCLASS1.HPP **************************************************/
