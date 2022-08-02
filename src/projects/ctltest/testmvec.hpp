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

#include "ctl/vector.hpp"

#include "base/base.hpp"
#include "testh/soaktest.hpp"

template < class VECTOR, class ITERATOR >
class TestMinMemoryVector
{
public:
    //  Singleton class
    static TestMinMemoryVector& instance();
    ~TestMinMemoryVector();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestMinMemoryVector< VECTOR, ITERATOR >& t );

    static void    TestMinMemoryVectorOperatorSquareBrackets();
    static void    TestMinMemoryVectorIteratorOperatorAddition();
    static void    TestMinMemoryVectorIteratorOperatorAddEquals();
    static void    TestMinMemoryVectorIteratorOperatorSubtraction();
    static void    TestMinMemoryVectorIteratorOperatorSubtractEquals();
    static void    TestMinMemoryVectorIteratorOperatorPlusPlusPrefix();
    static void    TestMinMemoryVectorIteratorOperatorPlusPlusPostfix();
    static void    TestMinMemoryVectorIteratorOperatorMinusMinusPrefix();
    static void    TestMinMemoryVectorIteratorOperatorMinusMinusPostfix();
    static void    TestMinMemoryVectorIteratorOperatorSquareBrackets();
    static void    TestMinMemoryVectorIteratorDereference();
    static void    TestMinMemoryVectorIteratorComparison();
    static void    iteratorInvalidation();

private:
    // Operation deliberately revoked
    TestMinMemoryVector( const TestMinMemoryVector& );

    // Operation deliberately revoked
    TestMinMemoryVector& operator =( const TestMinMemoryVector& );

    // Operation deliberately revoked
    bool operator ==( const TestMinMemoryVector& );

    TestMinMemoryVector();

    void    initialiseVectors();
    void    initialiseTestFunctions();

    typedef int                         TestType;
    typedef ctl_vector< TestType >      TestMinMemoryVectorType;
    typedef ITERATOR                    TestIterator;
    
    enum    Destinations { NOTHING, VECTOR1, VECTOR2 };
//    friend  ostream& operator <<( ostream& o, const TestMinMemoryVector< ITERATOR >::Destinations& d );
    static  void    comparisonData( Destinations* pDest, size_t* pIndex );
};

// template< class ITERATOR >
// ostream& operator <<( ostream& o, const TestMinMemoryVector< ITERATOR >::Destinations& d );

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "testh/testmvec.ctp"
#endif

#endif

/* End TESTVEC.HPP **************************************************/
