
#include "utility/tostring.hpp"

#include <stdio.h>

#include <cstring>

//////////////////////////////////////////////////////////////////////

#ifndef _WIN32
static void itoa(int val, char* buff, int base)
{
    sprintf(buff, "%d", val);
}
#endif // WINDOWS

string utlToString( int i )
{
	return utlToString( std::make_pair( i, UtlRadix( 10 ) ) );
}

string utlToString( std::pair< int, UtlRadix > p )
{
	char buffer[ 8 * sizeof( int ) + 1 ];
	itoa( p.first, buffer, p.second );
	return string( buffer );
}

///////////////////////////////////

string utlToString( unsigned i )
{
	return utlToString( std::make_pair( i, UtlRadix( 10 ) ) );
}

string utlToString( std::pair< unsigned, UtlRadix > p )
{
	char buffer[ 8 * sizeof( unsigned ) + 1 ];
	//utoa( p.first, buffer, p.second );
	itoa( p.first, buffer, p.second );
	return string( buffer );
}

///////////////////////////////////

string utlToString( long i )
{
	return utlToString( std::make_pair( i, UtlRadix( 10 ) ) );
}

string utlToString( std::pair< long, UtlRadix > p )
{
	char buffer[ 33 ];
	//ltoa( p.first, buffer, p.second );
	itoa( p.first, buffer, p.second );
	return string( buffer );
}

///////////////////////////////////

string utlToString( unsigned long i )
{
	return utlToString( std::make_pair( i, UtlRadix( 10 ) ) );
}

string utlToString( std::pair< unsigned long, UtlRadix > p )
{
	char buffer[ 33 ];
	//ultoa( p.first, buffer, p.second );
	itoa( p.first, buffer, p.second );
	return string( buffer );
}

///////////////////////////////////

string utlToString( float f )
{
	long double d = f;
	return utlToString( d );
}

string utlToString( double f )
{
	long double d = f;
	return utlToString( d );
}

string utlToString( long double f )
{
	enum { BUFFER_SIZE = 64 };
	char buffer[ BUFFER_SIZE ];
	int len = sprintf( buffer, "%lf", f );
	ASSERT( len <= BUFFER_SIZE, "" );
	return string( buffer );
}

//////////////////////////////////////////////////////////////////////

/* End TOSTRING.CPP *************************************************/
