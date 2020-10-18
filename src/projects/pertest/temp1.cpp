#include "base/persist.hpp"

//////////////////////////////////////////////////////////////////////

class NewStream;



template < class T >
class A
{
};

template< class T1 >
PerOstream& operator <<( PerOstream& ostr, const A< T1 >* pOb );

template< class T1 >
NewStream& operator <<( NewStream& ostr, const A< T1 >* pOb );


template < class T >
class B : public A< int >
{
};

template< class T1 >
PerOstream& operator <<( PerOstream& ostr, const B< T1 >* pOb );

template< class T1 >
ostream& operator <<( ostream& ostr, const B< T1 >* pOb );

template< class T1 >
NewStream& operator <<( NewStream& ostr, const B< T1 >* pOb );


template< class T1 >
void dummy1( const B< T1 >* );

fn( NewStream& ostr )
{
    B<int>* pB;

//    ostr << pB;

//	dummy1( pB );

	ostr << pB;
}
