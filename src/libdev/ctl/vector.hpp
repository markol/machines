/*
 * V E C T O R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
    ctl_vector

    This is a version of the STL vector class that can be derived from (it
    has a virtual destructor). It also performs error checking on the
    debug version.
*/

#ifndef _CTL_VECTOR_HPP
#define _CTL_VECTOR_HPP

//////////////////////////////////////////////////////////////////////

#include "base/persist.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////



template < class T >
class ctl_vector
: public std::vector< T >
{
public:
    using std::vector< T >::vector;
    //ctl_vector( size_type n );
    virtual ~ctl_vector(){};

private:


    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_vector );


};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_vector );

//////////////////////////////////////////////////////////////////////

template < class T >
bool operator ==( const ctl_vector< T >&, const ctl_vector< T >& );

template < class T >
bool operator <( const ctl_vector< T >&, const ctl_vector< T >& );

template< class T >
ostream& operator <<( ostream&, const ctl_vector< T >& );

//////////////////////////////////////////////////////////////////////

template < >
class ctl_vector< bool >
: public std::vector< int >
{
}; // Changed specialization for STL bool vector

#ifdef  _INLINE
    #include "ctl/vector.itp"
    #ifndef NDEBUG
        #include "ctl/private/vecbit.itp"
        #include "ctl/private/vecit.itp"
        #include "ctl/private/veccit.itp"
    #endif
#endif

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
//    #include "ctl/vector.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/vector.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _CTL_VECTOR_HPP	*/

