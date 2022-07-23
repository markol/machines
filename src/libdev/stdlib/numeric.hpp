/*
 * N U M E R I C
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#ifndef _STD_NUMERIC
#define _STD_NUMERIC

//////////////////////////////////////////////////////////////////////

template < class INPUT_ITER, class T >
T accumulate( INPUT_ITER first, INPUT_ITER last, T init );

template < class INPUT_ITER, class T, class BINARY_OP >
T accumulate(	INPUT_ITER first,
				INPUT_ITER last,
				T init,
	     		BINARY_OP binary_op );

///////////////////////////////////

template < class INPUT_ITER1, class INPUT_ITER2, class T >
T inner_product( INPUT_ITER1 first1,
					INPUT_ITER1 last1,
					INPUT_ITER2 first2,
					T init );

template < 	class INPUT_ITER1,
			class INPUT_ITER2,
			class T,
	  		class BINARY_OP1,
	  		class BINARY_OP2 >
T inner_product( 	INPUT_ITER1 first1,
					INPUT_ITER1 last1,
					INPUT_ITER2 first2, 
					T init, 
					BINARY_OP1 binary_op1,
					BINARY_OP2 binary_op2 ) ;

///////////////////////////////////

template < class INPUT_ITER, class OUTPUT_ITER >
OUTPUT_ITER
partial_sum( 	INPUT_ITER first,
				INPUT_ITER last,
			   	OUTPUT_ITER result );

template < 	class INPUT_ITER, 
			class OUTPUT_ITER, 
			class BINARY_OP >
OUTPUT_ITER 
partial_sum(	INPUT_ITER first,
				INPUT_ITER last,
			   	OUTPUT_ITER result,
			   	BINARY_OP binary_op );

///////////////////////////////////

template < class INPUT_ITER, class OUTPUT_ITER >
OUTPUT_ITER 
adjacent_difference(	INPUT_ITER first,
						INPUT_ITER last, 
				   		OUTPUT_ITER result );

template <	class INPUT_ITER, 
			class OUTPUT_ITER, 
	  		class BINARY_OP >
OUTPUT_ITER 
adjacent_difference(	INPUT_ITER first,
						INPUT_ITER last,
				   		OUTPUT_ITER result,
				   		BINARY_OP binary_op );

///////////////////////////////////

template < class FORWARD_ITER, class T >
void 
iota( FORWARD_ITER first, FORWARD_ITER last, T value );

//////////////////////////////////////////////////////////////////////

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "stdlib/numeric.ctf"
#endif

//////////////////////////////////////////////////////////////////////

#endif	/*	#ifndef _STD_NUMERIC	*/
