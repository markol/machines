/*
 * S T R I N G . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 *
 * 26/3/96	MCR added < operator
 */

#ifndef _STD_STRING_HPP
#define _STD_STRING_HPP

//////////////////////////////////////////////////////////////////////

#include <string>
#include <limits.h>

#include "base/base.hpp"
#include "base/persist.hpp"

//////////////////////////////////////////////////////////////////////

enum capacity { default_size, reserve };

// forward refs
//class ostream;
//class istream;
//class BinaryFile;

template< class T >
class basic_string: public std::basic_string< T >
{
public:

    using std::basic_string< T >::basic_string;

    PER_MEMBER_PERSISTENT_DEFAULT( basic_string );
    //PER_DEFINE_PERSISTENT_INLINE_T1( basic_string );

    basic_string() {}
    basic_string(const std::basic_string< T >& s): std::basic_string< T >(s)
    {    }

    //operator std::basic_string< T >() const
    //std::basic_string< T >& operator() ( )
    //{     return *this;   }
    //using std::basic_string< T >::operator;

    template<class T1>
    friend void perWrite( PerOstream&, const basic_string< T1 >& );
    template<class T1>
    friend void perRead( PerIstream&, basic_string< T1 >& );

private:

};

PER_DECLARE_PERSISTENT_T1( basic_string );
//PER_DEFINE_PERSISTENT_INLINE_T1( basic_string );

//PER_DEFINE_PERSISTENT_T1( basic_string, char );

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

//template< class T > size_t ntcs_length( T *str );

// This is only implemented for T=char.
//template< class T > istream& operator>>(istream&, basic_string<T>&);

//////////////////////////////////////////////////////////////////////

//#ifdef _INLINE
    //#include "stdlib/string.itp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    //#include "stdlib/string.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    //#include "stdlib/string.ctf"

    //hmm
//#endif

//////////////////////////////////////////////////////////////////////

typedef basic_string< char > string;
//typedef basic_string_iter< char > stringIter;



//////////////////////////////////////////////////////////////////////

#endif     /*    #ifndef  STRING_HPP     */
