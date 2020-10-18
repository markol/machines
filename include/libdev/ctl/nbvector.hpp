/*
 * N B V E C T O R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
    ctl_nb_vector

    A non-bloated ctl_vector. This achieves the lack of bloat by defining an
    underlying rep_type which is simply a vector of raw memory storage, each
    element of which is the same size as the data type we actually want to
    store. Vectors of various different base sizes are pre instantiated in
    the ctl library so that it should not be necessary to instantiate this
    vector at all.

	Note that this will only work with bitwise canonical data types i.e. those
	data types whose copy constructors and assignment operators simply copy
	raw memory around.
*/

#ifndef _CTL_NB_VECTOR_HPP
#define _CTL_NB_VECTOR_HPP

//////////////////////////////////////////////////////////////////////

#include "base/persist.hpp"
#include "ctl/vector.hpp"
//#include "ctl/private/nbhelper.hpp"

//////////////////////////////////////////////////////////////////////

template < class T >
class ctl_nb_vector: public std::vector< T >
{
public:

	//typedef ctl_vector< ctl_nb_helper< sizeof( T ) > >	 rep_type;

private:

    //rep_type repVector_;

public:

	///////////////////////////////

	using std::vector< T >::vector;

	//ctl_nb_vector< T >& operator =( const ctl_nb_vector< T >& copyMe )
	//  { repVector_ = copyMe.repVector_; return *this; }


	///////////////////////////////


    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( ctl_nb_vector );

private:
    //template < T > friend bool operator ==( const ctl_nb_vector< T >& a, const ctl_nb_vector< T >& b );
    //template < T > friend bool operator <( const ctl_nb_vector< T >& a, const ctl_nb_vector< T >& b );
};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_nb_vector );

//////////////////////////////////////////////////////////////////////
/*
template < class T >
bool operator ==( const ctl_nb_vector< T >& a, const ctl_nb_vector< T >& b )
{
    return a.repVector_ == b.repVector_;
}

template < class T >
bool operator <( const ctl_nb_vector< T >& a, const ctl_nb_vector< T >& b )
{
    return a.repVector_ < b.repVector_;
}
*/
template< class T >
ostream& operator <<( ostream&, const ctl_nb_vector< T >& );

//////////////////////////////////////////////////////////////////////

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/nbvector.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _CTL_NB_VECTOR_HPP	*/



