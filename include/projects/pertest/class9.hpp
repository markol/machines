/*
 * C L A S S 9 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class9

    Abstract template class with no default constructor
*/

#ifndef _PERSIST_CLASS9_HPP
#define _PERSIST_CLASS9_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

template< class X >
class Class9
// Canonical form revoked
{
public:
    Class9( double d );
    virtual ~Class9( void );

    X       x1_;
    double  d1_;

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_ABSTRACT_VIRTUAL( Class9 );

    friend ostream& operator <<( ostream& o, const Class9< X >& t );

    virtual void overrideMe() const = 0;

private:
    Class9( void );
    Class9( const Class9< X >& );
    Class9< X >& operator =( const Class9< X >& );

};


template< class X >
bool operator ==( const Class9< X >&, const Class9< X >& );

PER_READ_WRITE_T1( Class9 );
PER_DECLARE_PERSISTENT_T1( Class9 );

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "pertest/class9.ctp"
//#endif


#endif

/* End CLASS9.HPP ***************************************************/
