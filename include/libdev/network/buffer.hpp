/*
 * B U F F E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#ifndef _UTL_BUFFER_HPP
#define _UTL_BUFFER_HPP

#include "base/base.hpp"
//#include "stdlib/algorith.hpp"
#include <cstring>

template < class BITWISE >
class UtlBuffer
{
public:

        typedef size_t                          size_type;
        typedef BITWISE                         value_type;
        typedef value_type *            iterator;
        typedef const value_type *      const_iterator;

        ///////////////////////////////

        enum { DEFAULT_N_ITEMS = 1 };
        UtlBuffer( size_type nItems = DEFAULT_N_ITEMS );

        UtlBuffer( BITWISE * data, size_type nItems );

        UtlBuffer( const UtlBuffer& );

        ~UtlBuffer();

        UtlBuffer& operator =( const UtlBuffer& );

        size_type size() const;
        bool empty() const { return size() == 0; };

        iterator begin() { return data_; };
        iterator end()  { return &data_[ size_ ]; };

        const_iterator begin() const { return data_; };
        const_iterator end() const { return &data_[ size_ ]; };

        value_type * data();
        const value_type * data() const;

protected:

        size_type               size_;
        value_type *    data_;

        friend bool operator  <( const UtlBuffer< BITWISE >&, const UtlBuffer< BITWISE >& );
        friend bool operator ==( const UtlBuffer< BITWISE >&, const UtlBuffer< BITWISE >& );
};

//////////////////////////////////////////////////////////////////////

template < class BITWISE >
UtlBuffer< BITWISE >::UtlBuffer( size_type nItems )
: size_( 0 ),
  data_( NULL )
{
        data_ = _NEW_ARRAY( BITWISE, nItems );
        size_ = nItems;
}

template < class BITWISE >
UtlBuffer< BITWISE >::UtlBuffer( BITWISE * dat, size_type sz )
: size_( 0 ),
  data_( NULL )
{
        data_ = _NEW_ARRAY( BITWISE, sz );
        size_ = sz;
        memcpy( data_, dat, size_ );
}

template < class BITWISE >
UtlBuffer< BITWISE >::UtlBuffer( const UtlBuffer& rhs )
: size_( 0 ),
  data_( NULL )
{
        data_ = _NEW_ARRAY( BITWISE, rhs.size() );
        size_ = rhs.size();
        memcpy( data_, rhs.data_, size_ );
}

template < class BITWISE >
UtlBuffer< BITWISE >::~UtlBuffer()
{
        _DELETE_ARRAY( data_ );
}

template < class BITWISE >
UtlBuffer< BITWISE >&
UtlBuffer< BITWISE >::operator =( const UtlBuffer& rhs )
{
        if( this != &rhs )
        {
                if( size() != rhs.size() )
                {
                        BITWISE * newData = _NEW_ARRAY( BITWISE, rhs.size() );
                        _DELETE_ARRAY( data_ );
                        data_ = newData;
                        size_ = rhs.size();
                }

                memcpy( data_, rhs.data_, size_ );
        }

        return *this;
}

template < class BITWISE >
typename UtlBuffer< BITWISE >::size_type
UtlBuffer< BITWISE >::size() const { return size_; }

template < class BITWISE >
BITWISE *
UtlBuffer< BITWISE >::data() { return data_; };

template < class BITWISE >
const BITWISE *
UtlBuffer< BITWISE >::data() const { return data_; };

int compare( int a, int b );

template < class BITWISE >
bool operator <( const UtlBuffer< BITWISE >& a, const UtlBuffer< BITWISE >& b )
{
        return lexicographical_compare( a.begin(), a.end(), b.begin(), b.end() );
}

template < class BITWISE >
bool operator ==( const UtlBuffer< BITWISE >& a, const UtlBuffer< BITWISE >& b )
{
        bool result = a.size() == b.size();
        for( typename UtlBuffer< BITWISE >::size_type i=0; i<a.size() and result; ++i )
                result = a.data()[ i ] == b.data()[ i ];

        return result;
}

#ifdef _INLINE
    #include "network/buffer.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif  //      #ifndef _UTL_BUFFER_HPP

/* End BUFFER.HPP ***************************************************/
