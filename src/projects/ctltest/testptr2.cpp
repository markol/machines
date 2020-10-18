#include "base/base.hpp"

#include "ctl/vector.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/mmvector.hpp"
#include "ctl/countptr.hpp"


void    testPtr2( int* pInt )
{
    int* pI = _NEW( int );

    *pI = 10;
    
    CtlCountedPtr< int >    iPtr1( pI );
    CtlCountedPtr< int >    iPtr2 = iPtr1;

    CtlCountedPtr< int >    iPtr3;

    CtlConstCountedPtr< int >   icPtr1 = iPtr1;    
    
    icPtr1 = pInt;
    
//    iPtr3 = pI;

    double* pD = _NEW( double );

    *pD = 30.0;
    
    CtlCountedPtr< double >    dPtr1( pD );
    CtlCountedPtr< double >    dPtr2 = dPtr1;

    CtlCountedPtr< double >    dPtr3;

    dPtr3 = dPtr2;
}