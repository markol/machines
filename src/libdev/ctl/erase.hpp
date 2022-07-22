/*
 * E R A S E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

// Helper functions for safely erasing objects from standard containers.

#ifndef _CTL_ERASE_HPP
#define _CTL_ERASE_HPP


//#include "ctl/algorith.hpp"
#include <algorithm>
#include "ctl/function.hpp"

//////////////////////////////////////////////////////////////////////

#include "ctl/vector.hpp"
#include "ctl/list.hpp"
#include "ctl/set.hpp"
#include "ctl/map.hpp"

//////////////////////////////////////////////////////////////////////

template < class InIt, class Dist >
void
distance( InIt first, InIt last, Dist& n );

//////////////////////////////////////////////////////////////////////

template < class T >
bool ctl_erase_first_instance( ctl_vector< T > * pC, const T& key )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_vector< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_vector< T >::size_type old_nvalues = ctl_count( *pC, key ) );

	typename ctl_vector< T >::iterator i = find( pC->begin(), pC->end(), key );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count( *pC, key ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	return result;
}

template < class T, class UNARY_PREDICATE >
bool ctl_erase_first_instance_if( ctl_vector< T > * pC, UNARY_PREDICATE p )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_vector< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_vector< T >::size_type old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_vector< T >::iterator i = find_if( pC->begin(), pC->end(), p );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count_if( *pC, p ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	return result;
}

///////////////////////////////////

template < class T >
void ctl_erase_all_instances( ctl_vector< T > * pC, const T& key )
// erase all instances of key
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_vector< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_vector< T >::size_type old_nvalues = ctl_count( *pC, key ) );

 	pC->erase( remove( pC->begin(), pC->end(), key ), pC->end() );

	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains( pC, key ) );
}

template < class T, class UNARY_PREDICATE >
void ctl_erase_all_instances_if( ctl_vector< T > * pC, UNARY_PREDICATE p )
// erase all members of pC which satisfy p
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_vector< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_vector< T >::size_type old_nvalues = ctl_count_if( *pC, p ) );

 	pC->erase( remove_if( pC->begin(), pC->end(), p ), pC->end() );

	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains_if( pC, p ) );
}

/* //////////////////////////////////////////////////////////////// */

template < class T >
bool ctl_erase_first_instance( ctl_list< T > * pC, const T& key )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_list< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_list< T >::size_type old_nvalues = ctl_count( *pC, key ) );

	typename ctl_list< T >::iterator i = find( pC->begin(), pC->end(), key );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count( *pC, key ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	return result;
}

template < class T, class UNARY_PREDICATE >
bool ctl_erase_first_instance_if( ctl_list< T > * pC, UNARY_PREDICATE p )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_list< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_list< T >::size_type old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_list< T >::iterator i = find_if( pC->begin(), pC->end(), p );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count_if( *pC, p ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	return result;
}

///////////////////////////////////

template < class T >
void ctl_erase_all_instances( ctl_list< T > * pC, const T& key )
// erase all instances of key
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_list< T >::size_type old_size = pC->size() );
	POST_DATA( typename ctl_list< T >::size_type old_nvalues = ctl_count( *pC, key ) );

	pC->remove( key );

	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains( pC, key ) );
}

template < class T, class UNARY_PREDICATE >
void ctl_erase_all_instances_if( ctl_list< T > * pC, UNARY_PREDICATE p )
// erase all members of pC which satisfy p
{
	PRE( pC != NULL );
	POST_DATA( typename ctl_list< T >::size_type old_size = pC->size() );
	POST_INFO( old_size );
	POST_DATA( typename ctl_list< T >::size_type old_nvalues = ctl_count_if( *pC, p ) );
	POST_INFO( old_nvalues );

 	pC->erase( remove_if( pC->begin(), pC->end(), p ), pC->end() );

	POST_INFO( pC->size() );
	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains_if( pC, p ) );
}

//////////////////////////////////////////////////////////////////////

template < class T >
bool ctl_erase( ctl_set< T, std::less< T > > * pC, const T& key )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );

	typename ctl_set< T, std::less< T > >::iterator i = pC->find( key );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	POST( not ctl_contains( pC, key ) );
	return result;
}

template < class T, class UNARY_PREDICATE >
bool ctl_erase_first_instance_if( ctl_set< T, std::less< T > > * pC, UNARY_PREDICATE p )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );
	POST_DATA( size_t old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_set< T, std::less< T > >::iterator i = find_if( pC->begin(), pC->end(), p );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count_if( *pC, p ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	POST( not ctl_contains_if( pC, p ) );
	return result;
}

template < class T, class UNARY_PREDICATE >
void ctl_erase_all_instances_if( ctl_set< T, std::less< T > > * pC, UNARY_PREDICATE p )
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );
	POST_DATA( size_t old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_set< T, std::less< T > >::iterator i = pC->begin();
	while( i != pC->end() )
	{
		typename ctl_set< T, std::less< T > >::iterator last = i;
		++i;

		if( p( *last ) )
			pC->erase( last );
	}

	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains_if( pC, p ) );
}

//////////////////////////////////////////////////////////////////////

template < class KEY, class VALUE >
bool ctl_erase( ctl_map< KEY, VALUE, std::less< KEY > > * pC, const KEY& key )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );
	POST_DATA( 	size_t old_nvalues = ctl_count_if( *pC, compose1( bind1st( std::equal_to< int >(), key ), ctl_select1st< const int, int >() ) ) );

	typename ctl_map< KEY, VALUE, std::less< KEY > >::iterator i = pC->find( key );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	POST( not ctl_contains_if( pC, compose1( bind1st( std::equal_to< int >(), key ), ctl_select1st< const int, int >() ) ) );
	return result;
}

template < class KEY, class VALUE, class UNARY_PREDICATE >
bool ctl_erase_first_instance_if( ctl_map< KEY, VALUE, std::less< KEY > > * pC, UNARY_PREDICATE p )
// returns true iff an instance of key was erased
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );
	POST_DATA( size_t old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_map< KEY, VALUE, std::less< KEY > >::iterator i = find_if( pC->begin(), pC->end(), p );
	bool result = i != pC->end();
	if( result )
		pC->erase( i );

	POST( implies( result, pC->size() == old_size - 1 ) );
	POST( implies( result, ctl_count_if( *pC, p ) == old_nvalues - 1 ) );
	POST( implies( not result, pC->size() == old_size ) );
	return result;
}

template < class KEY, class VALUE, class UNARY_PREDICATE >
void ctl_erase_all_instances_if( ctl_map< KEY, VALUE, std::less< KEY > > * pC, UNARY_PREDICATE p )
{
	PRE( pC != NULL );
	POST_DATA( size_t old_size = pC->size() );
	POST_DATA( size_t old_nvalues = ctl_count_if( *pC, p ) );

	typename ctl_map< KEY, VALUE, std::less< KEY > >::iterator i = pC->begin();
	while( i != pC->end() )
	{
		typename ctl_map< KEY, VALUE, std::less< KEY > >::iterator last = i;
		++i;

		if( p( *last ) )
			pC->erase( last );
	}

	POST( pC->size() == old_size - old_nvalues );
	POST( not ctl_contains_if( pC, p ) );
}

#endif	// #ifndef _CTL_ERASE_HPP

/* End ERASE.HPP ****************************************************/
