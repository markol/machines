/*
 * V E C T O R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
    ctl_min_memory_vector

    This behaves in exactly the same way as a vector except
    for its use of memory. ctl_min_memory_vector will always
    use the minimum amount of memory it possibly can. This
    makes it very slow for doing repeated insertions or push_backs.
*/

#ifndef _CTL_MMVECTOR_HPP
#define _CTL_MMVECTOR_HPP

//////////////////////////////////////////////////////////////////////

#include "base/persist.hpp"

//#include "stdlib/private/ospace.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////

template < class T >
class ctl_min_memory_vector: public std::vector< T >
{
public:

	typedef T 									                value_type;

public:

	ctl_min_memory_vector(): std::vector<T>(), persistAsRaw_(false){};
	ctl_min_memory_vector( typename std::vector< T >::size_type n ):
	    std::vector<T>( n ), persistAsRaw_(false) {};
	ctl_min_memory_vector( typename std::vector< T >::size_type n, const T& value ):
	    std::vector<T>(n, value), persistAsRaw_(false) {};
	ctl_min_memory_vector( const ctl_min_memory_vector < T >& value ):
	    std::vector<T>(value), persistAsRaw_(value.persistAsRaw()) {};

    ctl_min_memory_vector( typename std::vector<T>::const_iterator first, typename std::vector<T>::const_iterator last ):
        std::vector<T>(first, last), persistAsRaw_(false) {};
	//using std::vector< T >::vector;


	~ctl_min_memory_vector() {};

	//ctl_min_memory_vector< T >& operator =( const ctl_min_memory_vector< T >& x );



    //  Set this to true to get more efficient persistence. The class T must be
    //  bitwise canonical, there must also be no pointers to any member of the
    //  vector.
    void persistAsRaw( bool flag );
    bool persistAsRaw() const;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_min_memory_vector );
private:

    bool    persistAsRaw_;

};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_min_memory_vector );

//////////////////////////////////////////////////////////////////////

//template < class T >
//bool operator ==( const ctl_min_memory_vector< T >&, const ctl_min_memory_vector< T >& );

//template < class T >
//bool operator <( const ctl_min_memory_vector< T >&, const ctl_min_memory_vector< T >& );

template< class T >
ostream& operator <<( ostream&, const ctl_min_memory_vector< T >& );

//////////////////////////////////////////////////////////////////////


//#ifdef _INLINE
//    #include "ctl/mmvector.itp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "ctl/mmvector.ctp"
    #ifndef NDEBUG
    //    #include "ctl/private/baseit.ctp"
    //    #include "ctl/private/mmvecit.ctp"
    //   #include "ctl/private/mmveccit.ctp"
    #endif
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/mmvector.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _STD_MMVECTOR	*/
