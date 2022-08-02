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
    static TestVector& instance();
    ~TestVector();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestVector< VECTOR, ITERATOR >& t );

    static void    testVectorOperatorSquareBrackets();
    static void    testVectorIteratorOperatorAddition();
    static void    testVectorIteratorOperatorAddEquals();
    static void    testVectorIteratorOperatorSubtraction();
    static void    testVectorIteratorOperatorSubtractEquals();
    static void    testVectorIteratorOperatorPlusPlusPrefix();
    static void    testVectorIteratorOperatorPlusPlusPostfix();
    static void    testVectorIteratorOperatorMinusMinusPrefix();
    static void    testVectorIteratorOperatorMinusMinusPostfix();
    static void    testVectorIteratorOperatorSquareBrackets();
    static void    testVectorIteratorDereference();
    static void    testVectorIteratorComparison();
    static void    iteratorInvalidation();

private:
    // Operation deliberately revoked
    TestVector( const TestVector& );

    // Operation deliberately revoked
    TestVector& operator =( const TestVector& );

    // Operation deliberately revoked
    bool operator ==( const TestVector& );

    TestVector();

    void    initialiseVectors();
    void    initialiseTestFunctions();

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
