#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/vector.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/mmvector.hpp"
#include "ctl/countptr.hpp"

#include "ctl/testh/testveca.hpp"
#include "ctl/testh/testlsta.hpp"
#include "ctl/testh/testfva.hpp"
#include "ctl/testh/testmva.hpp"
#include "ctl/testh/clargs.hpp"
#include "ctl/testh/random.hpp"

#include "stdlib/string.hpp"

void	testMap();
void	testPair();
void    testPtr();
void    testPtr2( int* );
void    testPtr3();
void    testPtr4();
void    testMinMemVector();

typedef string PerString;

using namespace std;

int	main( int argc, char** argv )
{
    //testMap();
    //testPair();

    testPtr4();
    testPtr();

    testMinMemVector();

     ARGS().parseArguments( argc, argv );

     size_t  option = randomInt( 3 );

     //option = 3;

     switch( option )
     {
         case 0:
             TestVectorAssert::instance().runSoakTestAssert();
             break;
         case 1:
             TestListAssert::instance().runSoakTestAssert();
             break;
         case 2:
             TestFixedVectorAssert::instance().runSoakTestAssert();
             break;
         case 3:
             TestMinMemoryVectorAssert::instance().runSoakTestAssert();
             break;
     }

	return 0;
}

// void    test1()
// {
//     struct S
//     {
//         int x;
//     };
//
//     ctl_min_memory_vector< int >    v1;
//
// //    ctl_fixed_vector< S >   v1;
// //     ctl_vector< S >   v1;
// //     ctl_vector< S >   v2;
// //
// //     if( v1 == v2 )
// //         cout << "equal" << endl;
// }


 void    testPtr()
 {
     int* pI = _NEW( int );

     *pI = 10;

     CtlCountedPtr< int >    iPtr1( pI );
     CtlCountedPtr< int >    iPtr2 = iPtr1;

     CtlCountedPtr< int >    iPtr3;

     CtlConstCountedPtr< int >   icPtr1 = iPtr1;
     icPtr1 = pI;

 //    iPtr3 = pI;

 //    testPtr2( pI );
 //    testPtr2( _NEW( int ) );

     double* pD = _NEW( double );

     *pD = 30.0;

     CtlCountedPtr< double >    dPtr1( pD );
     CtlCountedPtr< double >    dPtr2 = dPtr1;

     CtlCountedPtr< double >    dPtr3;

     dPtr3 = dPtr2;
 }

 void testPtr3()
 {
     CtlCountedPtr< double >    dPtr1( NULL );

     double* pD = _NEW( double );

     *pD = 30.0;

     CtlCountedPtr< double >    dPtr2;

     dPtr2 = pD;
 }

 void testPtr4()
 {
     CtlCountedPtr< double >    dPtr1[ 10 ];

     double* pD = _NEW( double );

     *pD = 30.0;

     dPtr1[ 0 ] = pD;
 }

 void    testMinMemVector()
 {
     typedef ctl_min_memory_vector< size_t >    IVector;

     IVector v1;

     for( size_t i = 0; i < 5; ++i )
         v1.push_back( i );

// persistence test
     {
         ofstream    ostr( "test.bin", ios::binary );
         PerOstream  postr( ostr );

         postr << v1;

         PerString name = "Test case";
         postr << name;

         ctl_min_memory_vector< PerString* > mmv;
         PerString str("string in vec");
         mmv.push_back(&str);
         postr << mmv;

         ostr.close();
     }

     IVector v2;
     PerString name2;
     {
         ifstream    istr( "test.bin", ios::in | ios::binary );
         if (istr.good() && istr.is_open())
         {
             istr.seekg(0, ios::beg);
             PerIstream  pistr( istr );

             pistr >> v2;
             pistr >> name2;
            /* uint test = 123;
             cout << test << endl;
             //istr >> dupa;
             pistr.read((char*)&test, sizeof(uint));
            cout << test << endl;
            //istr >> dupa;
            istr.read((char*)&test, sizeof(uint));
            cout << test << endl;
            */
            ctl_min_memory_vector< PerString* > mmv;
            pistr >> mmv;
            cout << *mmv[0];

             istr.close();
         }
         else
            cout << "Unable to open file.\n";
     }
     cout << v2;
     cout << name2 << endl;
 }
