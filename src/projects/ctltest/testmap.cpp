#include <string>
#include <iostream>
// #include <function.hpp>

#include "ctl/map.hpp"

void	testMap( void );
void	testMultiMap( void );

int main( void )
{
	testMap();
	testMultiMap();

	return 0;
}

using std::string;
using std::less;

void	testMap( void )
{
	ctl_map< int, string, less< int > >	intToString;

// 	intToString[ 0 ] = "zero";
// 	intToString[ 2 ] = "two";
 	intToString[ 1 ] = "one";
    intToString.insert( ctl_map< int, string, less< int > >::value_type( 3, "three" ) );
    intToString.insert( 4, "four" );
    intToString.insert( 4, "four again" );

    intToString[ 4 ] = "four yet again";

    ctl_map< int, string, less< int > >::iterator i;

    for( i = intToString.begin(); i != intToString.end(); ++i )
        std::cout << (*i).first << "  " << (*i).second << std::endl;
}


void	testMultiMap( void )
{
	ctl_multimap< int, string, less< int > >	intToString;

// 	intToString[ 0 ] = "zero";
// 	intToString[ 2 ] = "two";
// 	intToString[ 1 ] = "one";
    intToString.insert( ctl_map< int, string, less< int > >::value_type( 3, "three" ) );
    intToString.insert( ctl_map< int, string, less< int > >::value_type( 4, "four" ) );
    intToString.insert( ctl_map< int, string, less< int > >::value_type( 4, "four again" ) );
    intToString.insert( ctl_map< int, string, less< int > >::value_type( 4, "four yet again" ) );

//    intToString[ 4 ] = "four yet again";

    ctl_map< int, string, less< int > >::iterator i;

    for( i = intToString.begin(); i != intToString.end(); ++i )
        std::cout << (*i).first << "  " << (*i).second << std::endl;
}
