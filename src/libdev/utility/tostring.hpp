
#ifndef _UTL_TOSTRING_HPP
#define _UTL_TOSTRING_HPP

#include "base/base.hpp"

//#include <string.hpp>
#include "stdlib/string.hpp"
#include <utility>

#include "utility/subrange.hpp"

//////////////////////////////////////////////////////////////////////

UTL_INT_SUBRANGE( 2, 35, UtlRadix );

string utlToString( int i );
string utlToString( std::pair< int, UtlRadix > p );

string utlToString( unsigned i );
string utlToString( std::pair< unsigned, UtlRadix > p );

string utlToString( long i );
string utlToString( std::pair< long, UtlRadix > p );

string utlToString( unsigned long i );
string utlToString( std::pair< unsigned long, UtlRadix > p );

string utlToString( float );
string utlToString( double );
string utlToString( long double );

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _UTL_TOSTRING_HPP

/* End TOSTRING.HPP *************************************************/
