// STL helper header.
// Copyright (c) 1994,1995 by ObjectSpace, Inc.  All rights reserved.
// Email: sales@objectspace.com

#ifndef _CTL_ALGORITH_HPP
#define _CTL_ALGORITH_HPP

#include <algorithm>
// #include "ctl/numeric.hpp"

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif


//////////////////////////////////////////////////////////////////////

// This header defines several "helper" algorithms that can be used
// with STL implementations.  The idea is to make some of the more
// common operations easier to use and less error prone.

// Permission is granted to anyone to use this software for any
// purpose on any computer system, and to redistribute it freely,
// as long as the ObjectSpace copyright is included in the source.
// There is no guarantee provided or implied for these algorithms.

// If your compiler has problems with the Container::size_type and
// Container::difference_type typedefs, try changing them to
// size_t and ptrdiff_t respectively.

//////////////////////////////////////////////////////////////////////


// Forward declarations.

template< class T, class InputIterator >
void ctl_find_value( InputIterator, InputIterator, const T&, T*& );

template< class T, class Predicate, class InputIterator >
void ctl_find_predicate( InputIterator, InputIterator, Predicate, T*& );

template< class InputIterator, class T >
void ctl_min_element_aux( InputIterator, InputIterator, T*& );

template< class InputIterator, class T >
void ctl_max_element_aux( InputIterator, InputIterator, T * & );

template < class ForwardIterator >
void ctl_release( ForwardIterator first, ForwardIterator last );

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Locate an item in a sorted container.
// DESCRIPTION
//    Return `true` if `value` is in the container `c`.  This algorithm
//    assumes that the elements in the container are sorted using
//    `operator<()`.
// SEE ALSO
//    binary_search()

template< class Container, class T >
inline bool
ctl_binary_search( const Container& c, const T& value )
{
	return binary_search( c.begin(), c.end(), value );
}

//////////////////////////////////////////////////////////////////////

template < class Container, class T >
_CODE_INLINE
void ctl_prepend( Container *cPtr, const T& t )
{
	cPtr->insert( cPtr->begin(), t );
}

template < class Container, class T >
_CODE_INLINE
void ctl_append( Container *cPtr, const T& t )
{
	cPtr->insert( cPtr->end(), t );
}

//////////////////////////////////////////////////////////////////////

template< class Container, class T >
// inline
// Container::size_type
size_t
ctl_count( const Container& c, const T& value )
{
//	Container::size_type n = 0;
	size_t n = 0;
	// TODO std::count( c.begin(), c.end(), value, n );
	n = std::count( c.begin(), c.end(), value);
	return n;
}

template< class Container, class Predicate >
inline
// Container::size_type
size_t
ctl_count_if( const Container& c, Predicate pred )
{
	typename Container::size_type n = 0;
	count_if( c.begin(), c.end(), pred, n );
	return n;
}

//////////////////////////////////////////////////////////////////////
//  The ctl_erase methods have been removed because they do not work
//  properly on associative containers - what is worse they compile
//  but then fail in action. When we get a compiler which allows
//  partial specialisation we will re introduce them.
//////////////////////////////////////////////////////////////////////

// // Erase all elements from container.
// // Unlike `remove()`, the size of the container decreases
// // by the number of items that are erased.
//
// template< class CONTAINER, class T >
// inline void
// ctl_erase( CONTAINER *cPtr, const T& t )
// {
// 	cPtr->erase( remove( cPtr->begin(), cPtr->end(), t ), cPtr->end() );
// }
//
// //////////////////////////////////////////////////////////////////////
//
// // Erase all elements that satisfy `pred` from container.
// // Unlike `remove_if()`, the size of the container decreases
// // by the number of items that are erased.
//
// template< class CONTAINER, class UNARY_PRED >
// inline
// void
// ctl_erase_if( CONTAINER *cPtr, UNARY_PRED pred )
// {
// 	cPtr->erase( remove_if( cPtr->begin(), cPtr->end(), pred ), cPtr->end() );
// }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Locate an item in a container.
// DESCRIPTION
//    Searches for an element in container `c` that matches `value` using
//    `operator==()`.  `result` is set to point to the first matching
//    element, or `0` if no match is found.
// SEE ALSO
//    find()

template< class CONTAINER, class T >
inline
void
ctl_find( CONTAINER *cPtr, const T& value, T * & result )
{
	ctl_find_value( cPtr->begin(), cPtr->end(), value, result );
}

//////////////////////////////////////////////////////////////////////

template< class CONTAINER, class T >
inline
bool ctl_contains( CONTAINER *cPtr, const T& value )
{
	return find( cPtr->begin(), cPtr->end(), value ) != cPtr->end();
}

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Locate an item that satisfies a predicate in a container.
// DESCRIPTION
//    Set `result` to point to the first element in container `c`
//    that causes `pred` to evaulate to `true`, or `0` if no such
//    element exists.
// SEE ALSO
//    find_if()

template< class Container, class Predicate, class T >
inline void
ctl_find_if( Container& c, Predicate pred, T*& result )
{
	ctl_find_predicate( c.begin(), c.end(), pred, result );
}

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Apply a function to every element in a container.
// DESCRIPTION
//    Apply `f` to every element in container `c` and return the
//    input parameter `f`.
// SEE ALSO
//    for_each()

template< class Container, class Function >
inline Function
ctl_for_each( Container& c, Function f )
{
	return for_each( c.begin(), c.end(), f );
}

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Searches for one container in another.
// DESCRIPTION
//    Return `true` if container `c1` is embedded in container `c2`.
//    This algorithm assumes that both sequences are sorted using
//    `operator<()`.
// SEE ALSO
//    includes()

template< class Container1, class Container2 >
inline bool
ctl_includes( const Container1& c1, const Container2& c2 )
{
	return includes( c1.begin(), c1.end(), c2.begin(), c2.end() );
}

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Searches for one container in another.
// DESCRIPTION
//    Return `true` if container `c1` is embedded in container `c2`.
//    This algorithm assumes that both sequences are sorted using
//    the comparitor `compare`.
// SEE ALSO
//    includes()

template< class Container1, class Container2, class Compare >
inline bool
ctl_includes( const Container1& c1, const Container2& c2, Compare compare )
  {
  return includes( c1.begin(), c1.end(), c2.begin(), c2.end(), compare );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the maximum element in a container.
// DESCRIPTION
//    Set `t` to point to the maximum valued element of container `c`,
//    or `0` if `c` is empty.
// SEE ALSO
//    max_element()

template< class Container, class T >
inline void
ctl_max_element( Container& c, T*& t )
  {
  ctl_max_element_aux( c.begin(), c.end(), t );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the maximum element in a container.
// DESCRIPTION
//    Set `t` to maximum valued element of container `c` and return `true`,
//    or simply return `false` if `c` is empty.
// SEE ALSO
//    max_element()

template< class Container, class T >
inline bool
ctl_max_element_value( Container& c, T& t )
  {
  T* e = 0;
  if ( !c.empty() )
    {
    ctl_max_element_aux( c.begin(), c.end(), e );
    if ( e != 0 )
      t = *e;
    }
  return e != 0;
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the minimum element in a container.
// DESCRIPTION
//    Set `t` to point to the minimum valued element of container `c`,
//    or `0` if `c` is empty.
// SEE ALSO
//    min_element()

template< class Container, class T >
inline void
ctl_min_element( Container& c, T*& t )
  {
  ctl_min_element_aux( c.begin(), c.end(), t );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the minimum element in a container.
// DESCRIPTION
//    Set `t` to minimum valued element of container `c` and return `true`,
//    or simply return `false` if `c` is empty.
// SEE ALSO
//    min_element()

template< class Container, class T >
inline bool
ctl_min_element_value( Container& c, T& t )
  {
  T* e = 0;
  if ( !c.empty() )
    {
    ctl_min_element_aux( c.begin(), c.end(), e );
    if ( e != 0 )
      t = *e;
    }
  return e != 0;
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Randomize the order of elements in a container.
// DESCRIPTION
//    Shuffle all elements in the container `c` using uniformly
//    selected random swaps.  Use `operator=()` to perform the swaps.
// SEE ALSO
//    random_shuffle()

template< class Container >
inline void
ctl_random_shuffle( Container& c )
  {
  random_shuffle( c.begin(), c.end() );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Delete a container consisting of heap-based objects.
// DESCRIPTION
//    Calls `operator delete()` on every item in the container `c`.
//    Assume that each item is a pointer to a unique heap-based object.
//    The values of the pointers do not change, only what they point do
//    changes.  `c` has the same number of elements after the call as it
//    does before.  This algorithm will have unpredictable results if
//    two elements point to the same object on the heap.

template< class Container >
inline void
ctl_release( Container& c )
  {
  ctl_release( c.begin(), c.end() );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Replace a specified value in a sequence with another.
// DESCRIPTION
//    Replaces every occurrence of `old_value` in the container `c`
//    with `new_value`.
// SEE ALSO
//    replace()

template< class Container, class T >
inline void
ctl_replace( Container& c, const T& old_value, const T& new_value )
  {
  replace( c.begin(), c.end(), old_value, new_value );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Replace specified values in a container that satisfy a predicate.
// DESCRIPTION
//    Replaces every element in the container `c` that causes `pred`
//    to evaluate to `true` with `new_value`.
// SEE ALSO
//    replace_if()

template< class Container, class Predicate, class T >
inline void
ctl_replace_if( Container& c, Predicate pred, const T& new_value )
  {
  replace_if( c.begin(), c.end(), pred, new_value );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Rotate a sequence.
// DESCRIPTION
//    If `n` is positive, rotate the container to the left by `n`
//    positions; otherwise, rotate the container to the right by `n`
//    positions.
// SEE ALSO
//    rotate()

template< class Container >
inline void
ctl_rotate( Container& c, ptrdiff_t n )// Container::difference_type n )
  {
  typename Container::size_type size = c.size();
  if ( n < 0 )
    n = size - ( -n % size );
  else
    n %= size;
  typename Container::iterator midpoint = c.begin();
  advance( midpoint, n );
  rotate( c.begin(), midpoint, c.end() );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Sort a sequence.
// DESCRIPTION
//    Sort all elements in the container `c` using `operator<()` to
//    compare values.
// SEE ALSO
//    sort()

template< class Container >
inline void
ctl_sort( Container& c )
  {
  sort( c.begin(), c.end() );
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Sort a sequence.
// DESCRIPTION
//    Sort all elements in the container `c` using `compare` to
//    compare values.
// SEE ALSO
//    sort()

template< class Container, class Compare >
inline void
ctl_sort( Container& c, Compare compare )
  {
  sort( c.begin(), c.end(), compare );
  }

//////////////////////////////////////////////////////////////////////

// Non-inlined helpers.

// SYNOPSIS
//    Locate an item in a container.
// DESCRIPTION
//    Searches for the first element in the range [`first`..`last`) that
//    matches `value` using `operator==()`.  `result` is set to point to
//    the first matching element, or `0` if no match is found.
// SEE ALSO
//    find()

template< class T, class InputIterator >
void ctl_find_value( InputIterator first,
						InputIterator last,
  						const T& value,
  						T * & result )
{
	first = find( first, last, value );
	if( first != last )
    	result = &*first;
  	else
    	result = 0;
}

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Locate an item in a container.
// DESCRIPTION
//    Searches for the first element in the range [`first`..`last`) that
//    matches `value` using `pred`.  `result` is set to point to
//    the first matching element, or `0` if no match is found.
// SEE ALSO
//    find_if()

template< class T, class Predicate, class InputIterator >
void ctl_find_predicate
  (
  InputIterator first,
  InputIterator last,
  Predicate pred,
  T*& result
  )
  {
  first = find_if( first, last, pred );
  if ( first != last )
    result = &*first;
  else
    result = 0;
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the minimum element in a container.
// DESCRIPTION
//    Set `t` to point to the minimum valued element of the range
//    [`first`..`last) or `0` if `c` is empty.
// SEE ALSO
//    min_element()

template< class InputIterator, class T >
void ctl_min_element_aux( InputIterator first, InputIterator last, T*& t )
  {
  t = 0;
  InputIterator result = min_element( first, last );
  if ( result != last )
    t = &*result;
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Find the maximum element in a container.
// DESCRIPTION
//    Set `t` to point to the maximum valued element of the range
//    [`first`..`last) or `0` if `c` is empty.
// SEE ALSO
//    max_element()

template< class InputIterator, class T >
void ctl_max_element_aux( InputIterator first, InputIterator last, T*& t )
  {
  t = 0;
  InputIterator result = max_element( first, last );
  if ( result != last )
    t = &*result;
  }

//////////////////////////////////////////////////////////////////////

// SYNOPSIS
//    Delete a range of heap-based objects.
// DESCRIPTION
//    Calls `operator delete()` on every item in the range [`first`..`last`).
//    Assume that each item is a pointer to a unique heap-based object.
//    The values of the pointers do not change, only what they point do
//    changes.  This algorithm will have unpredictable results if two
//    elements point to the same object on the heap.

template< class ForwardIterator >
void
ctl_release( ForwardIterator first, ForwardIterator last )
  {
  while ( first != last )
    {
    delete *first;
    ++first;
    }
  }

//////////////////////////////////////////////////////////////////////

template < class INPUT_ITER, class PREDICATE >
INPUT_ITER
ctl_for_each_while( INPUT_ITER first, INPUT_ITER last, PREDICATE p )
// apply p( *i ), for all i in the range [ first, last ), while p( *i ) returns true.
// return the first i in the range [ first, last ) for which p( *i ) returns false,
// or last if p( *i ) holds true for all *i in [ first, last ).
{
	while( first != last and p( *first ) )
    		++first;

	return first;
}

template < class FWD_ITER, class BINARY_FN >
void ctl_for_product( FWD_ITER first, FWD_ITER last, BINARY_FN f )
// apply f( *i, *j ) for all i and j in the range ( first, last ]
{
	for( FWD_ITER i = first; i != last; ++i )
		for( FWD_ITER j = first; j != last; ++j )
			f( *i, *j );
}

template < class FWD_ITER, class BINARY_FN >
void ctl_for_lesser_product( FWD_ITER first, FWD_ITER last, BINARY_FN f )
// apply f( *i, *j ) for all j in the range ( i, last ),
// for all i in the range [ first, last )
{
	for( FWD_ITER i = first; i != last; ++i )
	{
		FWD_ITER j = i;
		for( ++j; j != last; ++j )
			f( *i, *j );
	}
}

template < class FWD_ITER, class BINARY_FN >
void ctl_for_lesser_equal_product( FWD_ITER first, FWD_ITER last, BINARY_FN f )
// apply f( *i, *j ) for all j in the range [ i, last ),
// for all i in the range [ first, last )
{
	for( FWD_ITER i = first; i != last; ++i )
		for( FWD_ITER j = i; j != last; ++j )
			f( *i, *j );
}

template < class FWD_ITER, class BINARY_FN >
void ctl_for_greater_product( FWD_ITER first, FWD_ITER last, BINARY_FN f )
// apply f( *j, *i ) for all j in the range ( i, last ),
// for all i in the range [ first, last )
{
	for( FWD_ITER i = first; i != last; ++i )
	{
		FWD_ITER j = i;
		for( ++j; j != last; ++j )
			f( *j, *i );
	}
}

template < class FWD_ITER, class BINARY_FN >
void ctl_for_greater_equal_product( FWD_ITER first, FWD_ITER last, BINARY_FN f )
// apply f( *j, *i ) for all j in the range [ i, last ),
// for all i in the range [ first, last )
{
	for( FWD_ITER i = first; i != last; ++i )
		for( FWD_ITER j = i; j != last; ++j )
			f( *j, *i );
}

//////////////////////////////////////////////////////////////////////

template < class FWD_ITER, class BINARY_PRED >
bool ctl_for_product_while( FWD_ITER first, FWD_ITER last, BINARY_PRED p )
// apply p( *i, *j ) for all i and j in the range ( first, last ] while p( *i, *j ) holds
// return false iff p( *i, *j ) returns false.
{
	bool result = true;
	for( FWD_ITER i = first; result and i != last; ++i )
		for( FWD_ITER j = first; result and j != last; ++j )
			result = p( *i, *j );

	return result;
}

template < class FWD_ITER, class BINARY_PRED >
bool ctl_for_lesser_product_while( FWD_ITER first, FWD_ITER last, BINARY_PRED p )
// apply p( *i, *j ) for all j in the range ( i, last ),
// for all i in the range [ first, last ),
// while p( *i, *j ) return true.
// return false iff p( *i, *j ) returns false.
{
	bool result = true;
	for( FWD_ITER i = first; result and i != last; ++i )
	{
		FWD_ITER j = i;
		for( ++j; result and j != last; ++j )
			result = p( *i, *j );
	}

	return result;
}

template < class FWD_ITER, class BINARY_PRED >
bool ctl_for_lesser_equal_product_while( FWD_ITER first, FWD_ITER last, BINARY_PRED p )
// apply p( *i, *j ) for all j in the range [ i, last ),
// for all i in the range [ first, last ),
// while p( *i, *j ) return true.
// return false iff p( *i, *j ) returns false.
{
	bool result = true;
	for( FWD_ITER i = first; result and i != last; ++i )
		for( FWD_ITER j = i; result and j != last; ++j )
			result = p( *i, *j );

	return result;
}

template < class FWD_ITER, class BINARY_PRED >
bool ctl_for_greater_product_while( FWD_ITER first, FWD_ITER last, BINARY_PRED p )
// apply p( *i, *j ) for all j in the range ( i, last ),
// for all i in the range [ first, last ),
// while p( *i, *j ) return true.
// return false iff p( *i, *j ) returns false.
{
	bool result = true;
	for( FWD_ITER i = first; result and i != last; ++i )
	{
		FWD_ITER j = i;
		for( ++j; result and j != last; ++j )
			result = p( *j, *i );
	}

	return result;
}

template < class FWD_ITER, class BINARY_PRED >
bool ctl_for_greater_equal_product_while( FWD_ITER first, FWD_ITER last, BINARY_PRED p )
// apply p( *i, *j ) for all j in the range [ i, last ),
// for all i in the range [ first, last ),
// while p( *i, *j ) return true.
// return false iff p( *i, *j ) returns false.
{
	bool result = true;
	for( FWD_ITER i = first; result and i != last; ++i )
		for( FWD_ITER j = i; result and j != last; ++j )
			result = p( *j, *i );

	return result;
}

//////////////////////////////////////////////////////////////////////

template< class CONTAINER, class UNARY_PREDICATE >
inline
bool ctl_contains_if( CONTAINER *cPtr, const UNARY_PREDICATE& p )
{
	return find_if( cPtr->begin(), cPtr->end(), p ) != cPtr->end();
}

//////////////////////////////////////////////////////////////////////

// #ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
//     #include "ctl/algorith.ctf"
// #endif

#endif
