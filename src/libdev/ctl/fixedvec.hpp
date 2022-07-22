/*
 * F I X E D V E C . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 *
 * Template
 * Classe(s):	ctl_fixed_vector	similar to the stdlib\vector class except that
 *								it is of a fixed size and has a cut down public interface
 *								(Modelled after the reversible_container class ).
 *
 */

/* START FILE **********************************************************************/

#ifndef _CTL_FIXEDVEC_HPP
#define _CTL_FIXEDVEC_HPP


#ifndef ctl_fixed_vector
	#define ctl_fixed_vector ctl_fixed_vector
#endif


#include "base/base.hpp"
#include "base/persist.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////

template < class T >
class ctl_fixed_vector: public std::vector< T >
{
public:
	typedef T 									        value_type;

protected:


public:
    using std::vector< T >::vector;

	//ctl_fixed_vector();
	// POST( size() == 0 );

	//ctl_fixed_vector( size_type n, const T& value );
	// POST( size() == n );

	//ctl_fixed_vector( size_type n );
	// POST( size() == n );

	//ctl_fixed_vector( const ctl_fixed_vector < T >& rhs);
	// POST( *this == rhs );

	~ctl_fixed_vector(){};
	// POST( size() == 0 );
	// POST : T's destructor is applied to every element,
	// and all allocated raw memory is returned.


    PER_MEMBER_PERSISTENT_DEFAULT( ctl_fixed_vector );

private:


};

PER_DEFINE_PERSISTENT_INLINE_T1( ctl_fixed_vector );

//////////////////////////////////////////////////////////////////////

template < class T >
bool operator ==( const ctl_fixed_vector< T >&, const ctl_fixed_vector< T >& );
// PRE( == is an equivalence relation on T );
// POST( iff( result, a.size() == b.size()
//			and equal( a.begin(), a.end(), b.begin() ) );


template < class T >
bool operator < ( const ctl_fixed_vector< T >&, const ctl_fixed_vector< T >& );
// PRE( < is a total ordering relation on T )
// POST( iff( result,
//	lexicographical_compare( a.begin(), a.end(), b.begin(), b.end() ) );

template< class T >
ostream& operator <<( ostream&, const ctl_fixed_vector< T >& );


//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "ctl/fixedvec.itp"
    #ifndef NDEBUG
        #include "ctl/private/fvecbit.itp"
        #include "ctl/private/fvecit.itp"
        #include "ctl/private/fveccit.itp"
    #endif
#endif

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
//    #include "ctl/fixedvec.ctp"
#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/fixedvec.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _STD_VECTOR	*/

/* END FILE ************************************************************************/
