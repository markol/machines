#include <string.hpp>
// #include <function.hpp>

#include "ctl/set.hpp"

void	testSet();
void	testMultiSet();

int main()
{
	testSet();
	testMultiSet();

	return 0;
}

void	testSet()
{
	ctl_set< int, less< int > >	intSet;

    intSet.insert( 4 );
    intSet.insert( 1 );
    intSet.insert( -50 );
    intSet.insert( 3063764 );
    intSet.insert( 2 );
    intSet.insert( 4 );
    intSet.insert( 4 );
    intSet.insert( 4 );

    ctl_set< int, less< int > >::iterator i;
    
    for( i = intSet.begin(); i != intSet.end(); ++i )
        cout << (*i) << endl;

    cout << endl;
}


void	testMultiSet()
{
	ctl_multiset< int, less< int > >	intSet;

    intSet.insert( 4 );
    intSet.insert( 1 );
    intSet.insert( -50 );
    intSet.insert( 3063764 );
    intSet.insert( 2 );
    intSet.insert( 4 );
    intSet.insert( 4 );
    intSet.insert( 4 );

    ctl_multiset< int, less< int > >::iterator i;
    
    for( i = intSet.begin(); i != intSet.end(); ++i )
        cout << (*i) << endl;
}