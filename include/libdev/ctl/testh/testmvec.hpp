/*
 * T E S T M V E C . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    TestMinMemoryVector

    Soak test the min_memory_vector class
*/

#ifndef _TESTMVEC_HPP
#define _TESTMVEC_HPP

//#include "ctl/vector.hpp"

#include "base/base.hpp"
#include "ctl/testh/soaktest.hpp"

template < class VECTOR, class ITERATOR >
class TestMinMemoryVector
{
public:
    //  Singleton class
    static TestMinMemoryVector& instance( void );
    ~TestMinMemoryVector( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestMinMemoryVector< VECTOR, ITERATOR >& t );

    static void    TestMinMemoryVectorOperatorSquareBrackets( void );
    static void    TestMinMemoryVectorIteratorOperatorAddition( void );
    static void    TestMinMemoryVectorIteratorOperatorAddEquals( void );
    static void    TestMinMemoryVectorIteratorOperatorSubtraction( void );
    static void    TestMinMemoryVectorIteratorOperatorSubtractEquals( void );
    static void    TestMinMemoryVectorIteratorOperatorPlusPlusPrefix( void );
    static void    TestMinMemoryVectorIteratorOperatorPlusPlusPostfix( void );
    static void    TestMinMemoryVectorIteratorOperatorMinusMinusPrefix( void );
    static void    TestMinMemoryVectorIteratorOperatorMinusMinusPostfix( void );
    static void    TestMinMemoryVectorIteratorOperatorSquareBrackets( void );
    static void    TestMinMemoryVectorIteratorDereference( void );
    static void    TestMinMemoryVectorIteratorComparison( void );
    static void    iteratorInvalidation( void );

private:
    // Operation deliberately revoked
    TestMinMemoryVector( const TestMinMemoryVector& );

    // Operation deliberately revoked
    TestMinMemoryVector& operator =( const TestMinMemoryVector& );

    // Operation deliberately revoked
    bool operator ==( const TestMinMemoryVector& );

    TestMinMemoryVector( void );

    void    initialiseVectors( void );
    void    initialiseTestFunctions( void );

    typedef int                         TestType;
    typedef ctl_vector< TestType >      TestMinMemoryVectorType;
    typedef ITERATOR                    TestIterator;

    enum    Destinations { NOTHING, VECTOR1, VECTOR2 };
//    friend  ostream& operator <<( ostream& o, const TestMinMemoryVector< ITERATOR >::Destinations& d );
    static  void    comparisonData( Destinations* pDest, size_t* pIndex );
};

// template< class ITERATOR >
// ostream& operator <<( ostream& o, const TestMinMemoryVector< ITERATOR >::Destinations& d );

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "ctl/testh/testmvec.ctp"
//#endif

#endif

/* End TESTVEC.HPP **************************************************/
