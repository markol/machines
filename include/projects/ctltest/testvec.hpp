/*
 * T E S T V E C . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    TestVector

    Soak test the vector class
*/

#ifndef _TESTVEC_HPP
#define _TESTVEC_HPP

#include "ctl/vector.hpp"

#include "base/base.hpp"
#include "testh/soaktest.hpp"

template < class VECTOR, class ITERATOR >
class TestVector
{
public:
    //  Singleton class
    static TestVector& instance( void );
    ~TestVector( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestVector< VECTOR, ITERATOR >& t );

    static void    testVectorOperatorSquareBrackets( void );
    static void    testVectorIteratorOperatorAddition( void );
    static void    testVectorIteratorOperatorAddEquals( void );
    static void    testVectorIteratorOperatorSubtraction( void );
    static void    testVectorIteratorOperatorSubtractEquals( void );
    static void    testVectorIteratorOperatorPlusPlusPrefix( void );
    static void    testVectorIteratorOperatorPlusPlusPostfix( void );
    static void    testVectorIteratorOperatorMinusMinusPrefix( void );
    static void    testVectorIteratorOperatorMinusMinusPostfix( void );
    static void    testVectorIteratorOperatorSquareBrackets( void );
    static void    testVectorIteratorDereference( void );
    static void    testVectorIteratorComparison( void );
    static void    iteratorInvalidation( void );

private:
    // Operation deliberately revoked
    TestVector( const TestVector& );

    // Operation deliberately revoked
    TestVector& operator =( const TestVector& );

    // Operation deliberately revoked
    bool operator ==( const TestVector& );

    TestVector( void );

    void    initialiseVectors( void );
    void    initialiseTestFunctions( void );

    typedef int                         TestType;
    typedef ctl_vector< TestType >      TestVectorType;
    typedef ITERATOR                    TestIterator;
    
    enum    Destinations { NOTHING, VECTOR1, VECTOR2 };
//    friend  ostream& operator <<( ostream& o, const TestVector< ITERATOR >::Destinations& d );
    static  void    comparisonData( Destinations* pDest, size_t* pIndex );
};

// template< class ITERATOR >
// ostream& operator <<( ostream& o, const TestVector< ITERATOR >::Destinations& d );

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "testh/testvec.ctp"
#endif

#endif

/* End TESTVEC.HPP **************************************************/
