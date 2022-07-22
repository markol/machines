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

#pragma once

#include "base/persist.hpp"

#include <vector>

template < class T >
class ctl_min_memory_vector : public std::vector< T >
{
public:

    typedef T                                                     value_type;

public:

    ctl_min_memory_vector() : std::vector<T>(), persistAsRaw_(false)
    {
    };

    ctl_min_memory_vector( typename std::vector< T >::size_type n ) :
    std::vector<T>( n ), persistAsRaw_(false)
    {
    };

    ctl_min_memory_vector( typename std::vector< T >::size_type n, const T& value ) :
    std::vector<T>(n, value), persistAsRaw_(false)
    {
    };

    ctl_min_memory_vector( const ctl_min_memory_vector < T >& value ) :
    std::vector<T>(value), persistAsRaw_(value.persistAsRaw())
    {
    };

    ctl_min_memory_vector( typename std::vector<T>::const_iterator first, typename std::vector<T>::const_iterator last ) :
    std::vector<T>(first, last), persistAsRaw_(false)
    {
    };
    //using std::vector< T >::vector;


    virtual ~ctl_min_memory_vector() = default;

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

template< class T >
std::ostream& operator <<( std::ostream&, const ctl_min_memory_vector< T >& );

template < class T >
void ctl_min_memory_vector< T >::persistAsRaw( bool flag )
{
    persistAsRaw_ = flag;
}

template < class T >
bool ctl_min_memory_vector< T >::persistAsRaw() const
{
    return persistAsRaw_;
}

template < class T >
bool operator ==( const ctl_min_memory_vector< T >& x,
                  const ctl_min_memory_vector< T > & y )
{
    return x.size() == y.size()
           and equal( x.begin(), x.end(), y.begin() );
}

template < class T >
bool operator <( const ctl_min_memory_vector< T >& x, const ctl_min_memory_vector< T >& y )
{
    return lexicographical_compare( x.begin(), x.end(),
                                    y.begin(), y.end() );
}

template< class T >
void perWrite( PerOstream& ostr, const ctl_min_memory_vector< T >& vec )
{
    if ( vec.persistAsRaw() )
    {
        size_t nEntries = vec.size();
        PER_WRITE_RAW_OBJECT( ostr, nEntries );

        std::vector<T> const* v = static_cast<std::vector<T> const*> (&vec);

        T const* const dataPtr{ v->data()};

        char const* const castPtr{ reinterpret_cast<const char*> (dataPtr)};

        Persistence::instance().writeRawData( ostr, castPtr, nEntries * sizeof ( T ) );
    }
    else
    {
        PER_WRITE_RAW_OBJECT( ostr, vec.size() );

        for ( typename ctl_min_memory_vector< T >::const_iterator i = vec.begin(); i != vec.end(); ++i )
        {
            ostr << *i;
        }
    }
}

template< class T >
void perRead( PerIstream& istr, ctl_min_memory_vector< T >& vec )
{
    vec.erase( vec.begin(), vec.end() );
    ASSERT( vec.size() == 0, "Vector has not been properly cleared" );

    if ( vec.persistAsRaw() )
    {
        size_t nEntries;
        PER_READ_RAW_OBJECT( istr, nEntries );

        ctl_min_memory_vector< T >   dummyVec( nEntries );
        vec = dummyVec;

        PER_READ_RAW_DATA( istr, &vec[ 0 ], nEntries * sizeof ( T ) );
    }
    else
    {
        size_t  size;
        PER_READ_RAW_OBJECT( istr, size );

        vec.reserve( size );

        for ( size_t i = 0; i < size; ++i )
        {
            T   t;
            istr >> t;
            vec.push_back( t );
        }
    }
}

template< class T >
std::ostream& operator <<( std::ostream& ostr, const ctl_min_memory_vector< T >& vec )
{
    for ( typename ctl_min_memory_vector< T >::const_iterator i = vec.begin(); i != vec.end(); ++i )
    {
        ostr << *i << std::endl;
    }

    return ostr;
}
