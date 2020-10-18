/*
 * M A S T E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Master

    The master class for testing persistence. This class owns
    copies of all of the other classes that are being tested
    for persistence.
*/

#ifndef _PERSIST_MASTER_HPP
#define _PERSIST_MASTER_HPP

#include "base/base.hpp"

#include "pertest/class1.hpp"
#include "pertest/classpa.hpp"

#include "pertest/tclass1.hpp"
#include "pertest/contain.hpp"

// #include "ctl/pvector.hpp"

template <class T> class ctl_pvector;

class Master
{
public:
    Master( void );
    Master( PerIstream& istr );
    ~Master( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Master& t );
    friend bool operator ==( const Master&, const Master& );

    PER_MEMBER_PERSISTENT_DEFAULT( Master );
    PER_FRIEND_READ_WRITE( Master );

    typedef TClass1< int, double >  TC1a;    
private:
    // Operation deliberately revoked
    Master( const Master& );

    // Operation deliberately revoked
    Master& operator =( const Master& );

	Class1	c1_;
    Class1* pC1_;

    TC1a    tc1_;
    TC1a*   pTc1_;

    ClassPair   p1_;

    Containers cContain_;
};

PER_DECLARE_PERSISTENT( Master );

#endif

/* End MASTER.HPP ***************************************************/
