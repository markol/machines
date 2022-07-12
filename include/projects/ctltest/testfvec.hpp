/*
 * T E S T V E C . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    TestFixedVector

    Soak test the fixed_vector class
*/

#ifndef _TESTFVEC_HPP
#define _TESTFVEC_HPP

#include "ctl/fixedvec.hpp"

#include "base/base.hpp"
#include "testh/soaktest.hpp"

template < class VECTOR, class ITERATOR >
class TestFixedVector
{
public:
    //  Singleton class
    static TestFixedVector& instance();
    ~TestFixedVector();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestFixedVector< VECTOR, ITERATOR >& t );

    static void    operatorSquareBrackets();
    static void    iteratorOperatorAddition();
    static void    iteratorOperatorAddEquals();
    static void    iteratorOperatorSubtraction();
    static void    iteratorOperatorSubtractEquals();
    static void    iteratorOperatorPlusPlusPrefix();
    static void    iteratorOperatorPlusPlusPostfix();
    static void    iteratorOperatorMinusMinusPrefix();
    static void    iteratorOperatorMinusMinusPostfix();
    static void    iteratorOperatorSquareBrackets();
    static void    iteratorDereference();
    static void    iteratorComparison();
    static void    iteratorInvalidation();

private:
    // Operation deliberately revoked
    TestFixedVector( const TestFixedVector& );

    // Operation deliberately revoked
    TestFixedVector& operator =( const TestFixedVector& );

    // Operation deliberately revoked
    bool operator ==( const TestFixedVector& );

    TestFixedVector();

    void    initialiseVectors();
    void    initialiseTestFunctions();

    typedef int                         TestType;
    typedef ctl_vector< TestType >      TestFixedVectorType;
    typedef ITERATOR                    TestIterator;
    
    enum    Destinations { NOTHING, VECTOR1, VECTOR2 };
//    friend  ostream& operator <<( ostream& o, const TestFixedVector< ITERATOR >::Destinations& d );
    static  void    comparisonData( Destinations* pDest, size_t* pIndex );
};

// template< class ITERATOR >
// ostream& operator <<( ostream& o, const TestFixedVector< ITERATOR >::Destinations& d );

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "testh/testfvec.ctp"
#endif

#endif

/* End TESTVEC.HPP **************************************************/
