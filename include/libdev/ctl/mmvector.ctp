/*
 * M M V E C T O R . C T P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//////////////////////////////////////////////////////////////////////

#ifndef _CTL_MMVECTOR_CTP
#define _CTL_MMVECTOR_CTP

#ifndef _INLINE
    #include "ctl/mmvector.itp"
#endif

#ifdef NDEBUG
    #define REP_ITERATOR( x )   x
#else
    #define REP_ITERATOR( x )   x.rep_iterator()
#endif

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/*
template < class T >
ctl_min_memory_vector< T >::ctl_min_memory_vector( ctl_min_memory_vector< T >::size_type n,
						const T& value )
: persistAsRaw_( false )
{
	start = static_allocator.allocate( n );
	uninitialized_fill_n( start, n, value );
	finish = start + n;
	end_of_storage = finish;
}

template < class T >
ctl_min_memory_vector< T >::ctl_min_memory_vector( ctl_min_memory_vector< T >::size_type n )
: persistAsRaw_( false )
{
	start = static_allocator.allocate( n );
	uninitialized_fill_n( start, n, T() );
	finish = start + n;
	end_of_storage = finish;
}

//////////////////////////////////////////////////////////////////////

template < class T >
ctl_min_memory_vector< T >::ctl_min_memory_vector( const ctl_min_memory_vector< T >& x )
: persistAsRaw_( false )
{
	start = static_allocator.allocate( x.end() - x.begin() );
	finish = uninitialized_copy( x.begin(), x.end(), start );
	end_of_storage = finish;
}

template < class T >
ctl_min_memory_vector< T >::ctl_min_memory_vector( ctl_min_memory_vector< T >::const_iterator first,
						ctl_min_memory_vector< T >::const_iterator last )
: persistAsRaw_( false )
{
	size_type n = 0;
	distance( REP_ITERATOR( first ), REP_ITERATOR( last ), n );
	start = static_allocator.allocate( n );
	finish = uninitialized_copy( first, last, start );
	end_of_storage = finish;
}
*/
#ifndef NDEBUG
    // #include "ctl/private/baseit.ctp"
   // #include "ctl/private/mmvecit.ctp"
   // #include "ctl/private/mmveccit.ctp"
#endif

#endif

//////////////////////////////////////////////////////////////////////

/* End MMVECTOR.CTP ***************************************************/
