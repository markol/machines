#define _INSTANTIATE_TEMPLATE_CLASSES

#include "base/base.hpp"
//#include "stdlib/utility.hpp"
#include "ctl/utility.hpp"
#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "ctl/map.hpp"
#include "ctl/map.ctp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

void testPair( void )
{
//	CTL_PAIR( pairTest, int, key, MexTransform3d, transform );
	CTL_PAIR( pairTest, int, key, MexTransform3d, transform );

	ctl_vector< pairTest >	vec1;

    pairTest    p1;
    pairTest    p2;

    ctl_map< pairTest, int, less< pairTest > >  map1;

	CTL_TRIPLE( tripleTest, int, key, MexTransform3d, transform, char, th3 );

	ctl_vector< tripleTest >	vec2;

    tripleTest    t1;
    tripleTest    t2;

    t1.key( 1 );
    t1.transform( MexPoint3d( 1.0, 2.0, 3.0 ) );
    t1.th3( 'a' );

    if( t1 == t2 )
        cout << "Equal " << endl;
            
    if( t1 < t2 )
        cout << "Less " << endl;
    
    less< tripleTest >  lt1;
    
    ctl_map< tripleTest, int, less< tripleTest > >  map2;

	CTL_QUAD( quadTest,
      int, key,
      MexTransform3d, transform,
      char, th3,
      double, d );

	ctl_vector< quadTest >	vec3;

    quadTest    q1;
    quadTest    q2;

    q1.key( 1 );
    q1.transform( MexPoint3d( 1.0, 2.0, 3.0 ) );
    q1.th3( 'a' );
    q1.d( -354.064 );

    if( q1 == q2 )
        cout << "Equal " << endl;
            
    if( q1 < q2 )
        cout << "Less " << endl;
    
    less< quadTest >  lq1;
    
    ctl_map< quadTest, int, less< quadTest > >  map3;
}
