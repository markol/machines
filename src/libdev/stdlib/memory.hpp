#pragma once

#include <cstddef>

//void * operator new( size_t, void * ) noexcept;

// the following template function is replaced by the following two
// functions due to the fact that the Borland compiler doesn't change
// ptrdiff_t to type long when compile with -ml or -mh:
// template < class T > T * allocate( ptrdiff_t size, T * );

template <class T>
T * allocate( int size, T * );

template < class T >
T * allocate( long size, T * );

//////////////////////////////////////////////////////////////////////

template < class T >
void deallocate( T * buffer );

//////////////////////////////////////////////////////////////////////

template < class ForwardIter >
void destroy( ForwardIter first, ForwardIter last );

template < class T >
void destroy( T* pointer );

void destroy( char * );
void destroy( unsigned char * );
void destroy( short * );
void destroy( unsigned short * );
void destroy( int * );
void destroy( unsigned int * );
void destroy( long * );
void destroy( unsigned long * );
void destroy( float * );
void destroy( double * );

void destroy( char * * );
void destroy( unsigned char * * );
void destroy( short * * );
void destroy( unsigned short * * );
void destroy( int * * );
void destroy( unsigned int * * );
void destroy( long * * );
void destroy( unsigned long * * );
void destroy( float * * );
void destroy( double * * );

void destroy( char *, char * );
void destroy( unsigned char *, unsigned char * );
void destroy( short *, short * );
void destroy( unsigned short *, unsigned short * );
void destroy( int *, int * );
void destroy( unsigned int *, unsigned int * );
void destroy( long *, long * );
void destroy( unsigned long *, unsigned long * );
void destroy( float *, float * );
void destroy( double *, double * );

void destroy( char * *, char * * );
void destroy( unsigned char * *, unsigned char * * );
void destroy( short * *, short * * );
void destroy( unsigned short * *, unsigned short * * );
void destroy( int * *, int * * );
void destroy( unsigned int * *, unsigned int * * );
void destroy( long * *, long * * );
void destroy( unsigned long * *, unsigned long * * );
void destroy( float * *, float * * );
void destroy( double * *, double * * );

//////////////////////////////////////////////////////////////////////

template < class T1, class T2 >
void construct( T1 *p, const T2& value );

//////////////////////////////////////////////////////////////////////

template < class InputIter, class ForwardIter >
ForwardIter
uninitialized_copy( InputIter first,
    InputIter last,
    ForwardIter result );

template < class ForwardIter, class T >
void uninitialized_fill( ForwardIter first,
    ForwardIter last,
    const T& );

template < class ForwardIter, class Size, class T >
void uninitialized_fill_n( ForwardIter first,
    Size n,
    const T& );

template < class X >
class auto_ptr_array
{
public:
    typedef X element_type;

    /* _EXPLICIT */ auto_ptr_array( X *tPtr = 0 );
    auto_ptr_array( const auto_ptr_array& );
    auto_ptr_array& operator =( const auto_ptr_array& ) ;

    //  Member templates not yet available under Watcom
    //	template< class Y >    auto_ptr_array( const auto_ptr_array< Y >& );
    //	template< class Y >    auto_ptr_array< Y >& operator =( const auto_ptr_array< Y >& );

    ~auto_ptr_array();


    X& operator  *() const;
    X* operator ->() const;

    X* get() const;
    X* release() const;

private:

    X*  pData_;
    /* mutable */ bool owner_;

    // template<class Y> friend class auto_ptr_array;
} ;

#include "stdlib/memory.ctp"
#include "stdlib/memory.ctf"
