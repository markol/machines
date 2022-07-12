#include <stdlib.h>
#include <time.h>

#include <string>
#include "system/pathname.hpp"

#include "pertest/master.hpp"
// #include "pertest/class8.hpp"
#include "pertest/class3.hpp"

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

#include "utility/time.hpp"

void    registerDerivedClasses();
void    testNonPersistent();
void    writeFile( const SysPathName& pathname, const Master& master );
void    readFile( const SysPathName& pathname, Master* pMaster );

int main( int, char ** )
{
WHERE;

    UtlDebugTimer::calibrate();

    PER_LOG_WRITE( true );
    PER_LOG_READ( true );

    unsigned int seed = time( NULL );

    srand( seed );

    registerDerivedClasses();
    testNonPersistent();

    const SysPathName   pathname( "test.bin" );
    Master  m1;

    writeFile( pathname, m1 );

    Master  m2;

    readFile( pathname, &m2 );


WHERE;
    if( m1 == m2 )
        std::cout << "Write and read successful" << std::endl;
    else
    {
        std::cout << "Write and read failed" << std::endl;

        std::cout << "Original" << std::endl;
        std::cout << m1;

        std::cout << "New" << std::endl;
        std::cout << m2;

        std::cout << "Write and read failed" << std::endl;
    }

WHERE;
    std::ofstream    log( "testper.log" );
WHERE;

    log << "Original" << std::endl;
WHERE;
    log << m1;
WHERE;

    log << std::endl << "New" << std::endl;
WHERE;
    log << m2;
WHERE;

    return 0;
}

// PER_DEFINE_PERSISTENT_T1( Class5, int );
// PER_DEFINE_PERSISTENT_T1( Class5, double );
// PER_DEFINE_PERSISTENT_T1( Class6, double );
// PER_DEFINE_PERSISTENT_T2( Class7, int, double );
// PER_DEFINE_PERSISTENT_ABSTRACT_T1( Class9, int );

void    registerDerivedClasses()
{
//     PER_REGISTER_DERIVED_CLASS_T2( Class7, int, double );
//     PER_REGISTER_DERIVED_CLASS( Class3 );
//     PER_REGISTER_DERIVED_CLASS( Class10 );
}

void testNonPersistent()
{
//     ctl_vector< Class8 >            v1;
//     CtlCountedPtr< Class8 >         p1;
//     CtlConstCountedPtr< Class8 >    p2;
//     UtlProperty< Class8 >           property;
}

void    writeFile( const SysPathName& pathname, const Master& master )
{
	std::ofstream	str( pathname.c_str(), std::ios::binary );
    PerOstream	ostr( str );

    // TODO handle open once?
    //PerOstream	ostr2( str );

    ostr << master;

    Class3 cl3;
    ostr << cl3;

}

void    readFile( const SysPathName& pathname, Master* pMaster )
{
   	std::ifstream	str( pathname.c_str(), std::ios::binary );
    PerIstream	istr( str );

    istr >> *pMaster;

    Class3 cl3;
    istr >> cl3;
    std::cout << cl3.rawString1_ << std::endl;
}
