/*
 * T E S T V E C . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "testh/testvec.hpp"
#include "testh/soaktest.hpp"
#include "testh/random.hpp"
#include "testh/clargs.hpp"


//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorOperatorSquareBrackets( )
{
    size_t  n = TestVectorAssert::randomIndex1();
    
    if( n < TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestType    i = TestVectorAssert::vector1()[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorAddition( void )
{
    size_t  index = TestVectorAssert::randomIndex1();
    size_t  n = TestVectorAssert::randomIndex1();

    if( index + n <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

// INSPECT_ON( cout, TestVectorAssert::vector1Size() );
// INSPECT_ON( cout, index );
// INSPECT_ON( cout, n );
    
    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i + n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorAddEquals( void )
{
    size_t  index = TestVectorAssert::randomIndex1();
    size_t  n = TestVectorAssert::randomIndex1();

    if( index + n <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    
    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    i += n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorSubtraction( void )
{
    int  index = TestVectorAssert::randomIndex1();
    int  n = TestVectorAssert::randomIndex1();

    if( ( index <= TestVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i - n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorSubtractEquals( void )
{
    int  index = TestVectorAssert::randomIndex1();
    int  n = TestVectorAssert::randomIndex1();

    if( ( index <= TestVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    i -= n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorSquareBrackets( void )
{
    int  index = TestVectorAssert::randomIndex1();
    int  n = TestVectorAssert::randomIndex1();

    if( index + n <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );

    TestType    t = i[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorPlusPlusPrefix( void )
{
    size_t  index = TestVectorAssert::randomIndex1();

    if( index < TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    ++i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorPlusPlusPostfix( void )
{
    size_t  index = TestVectorAssert::randomIndex1();

    if( index < TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    i++;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorMinusMinusPrefix( void )
{
    size_t  index = TestVectorAssert::randomIndex1();

    if( index > 0 && index <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    --i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorOperatorMinusMinusPostfix( void )
{
    size_t  index = TestVectorAssert::randomIndex1();

    if( index > 0 && index <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    i--;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorDereference( void )
{
    size_t  index = TestVectorAssert::randomIndex1();

    LOG_DATA( index );
    LOG_DATA( TestVectorAssert::vector1Size() );
    
    if( index < TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestVectorAssert::getIterator( TestVectorAssert::vector1(), index, &i );
    *i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::testVectorIteratorComparison( void )
{
    Destinations    dA;
    Destinations    dB;

    size_t  indexA;
    size_t  indexB;
    
    comparisonData( &dA, &indexA );
    comparisonData( &dB, &indexB );
        
    LOG_DATA( dA );
    LOG_DATA( indexA );
    LOG_DATA( dB );
    LOG_DATA( indexB );
    LOG_DATA( TestVectorAssert::vector1Size() );
    LOG_DATA( TestVectorAssert::vector2Size() );

    TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    if( dA == VECTOR1 && indexA <= TestVectorAssert::vector1Size() &&
        dB == VECTOR1 && indexB <= TestVectorAssert::vector1Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    if( dA == VECTOR2 && indexA <= TestVectorAssert::vector2Size() &&
        dB == VECTOR2 && indexB <= TestVectorAssert::vector2Size() )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    TestIterator    i1;
    TestIterator    i2;

    if( dA == VECTOR1 )
        TestVectorAssert::getIterator( TestVectorAssert::vector1(), indexA, &i1 );
    if( dA == VECTOR2 )
        TestVectorAssert::getIterator( TestVectorAssert::vector2(), indexA, &i1 );
        
    if( dB == VECTOR1 )
        TestVectorAssert::getIterator( TestVectorAssert::vector1(), indexB, &i2 );
    if( dB == VECTOR2 )
        TestVectorAssert::getIterator( TestVectorAssert::vector2(), indexB, &i2 );

    size_t  comparisonOperator = randomInt( 6 );
    
    LOG_DATA( comparisonOperator );
    
    switch( comparisonOperator )
    {
        case 0:        
            i1 == i2;
            break;
        case 1:        
            i1 <= i2;
            break;
        case 2:        
            i1 >= i2;
            break;
        case 3:        
            i1 < i2;
            break;
        case 4:        
            i1 > i2;
            break;
        case 5:        
            i1 != i2;
            break;
    }
}

// template< class VECTOR, class ITERATOR >
// ostream& operator <<( ostream& o, const TestVector< VECTOR, ITERATOR >::Destinations& d )
// {
//     switch( d )
//     {
//         case TestVector< VECTOR, ITERATOR >::NOTHING:
//             o << "NOTHING";
//             break;
//         case TestVector< VECTOR, ITERATOR >::VECTOR1:
//             o << "VECTOR1";
//             break;
//         case TestVector< VECTOR, ITERATOR >::VECTOR2:
//             o << "VECTOR2";
//             break;
//     }
//     
//     return o;
// }

// static
template < class VECTOR, class ITERATOR >
void TestVector< VECTOR, ITERATOR >::comparisonData( Destinations* pDest, size_t* pIndex )
{
    switch( randomInt( 3 ) )
    {
        case 0:
            *pDest = NOTHING;
        case 1:
            *pDest = VECTOR1;
            break;
        case 2:
            *pDest = VECTOR2;
            break;
    }
    
    if( *pDest == VECTOR1 )
        *pIndex = TestVectorAssert::randomIndex1();
    if( *pDest == VECTOR2 )
        *pIndex = TestVectorAssert::randomIndex2();
}


template < class VECTOR, class ITERATOR >
TestVector< VECTOR, ITERATOR >::~TestVector()
{
    TEST_INVARIANT;

}

// static
template < class VECTOR, class ITERATOR >
void    TestVector< VECTOR, ITERATOR >::iteratorInvalidation( void )
{
    size_t  index;
    TestIterator    i;

    if( TestVectorAssert::vector1().size() == 0 )
        TestVectorAssert::vector1().push_back( TestVectorAssert::vector1().size() );
        
    TestVectorAssert::getValidRandomIterator( TestVectorAssert::vector1(), &index, &i );

    if( randomInt( 2 ) == 0 )
    {
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
#ifndef NDEBUG
        TestVectorAssert::dontInvalidateIterator( i, TestVectorAssert::vector1() );
#endif
    }
    else
    {
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
#ifndef NDEBUG
        TestVectorAssert::invalidateIterator( i, TestVectorAssert::vector1() );
#endif
    }
    
    TestIterator    i1;
    TestVectorAssert::getValidRandomIteratorInclusive( TestVectorAssert::vector1(), &index, &i1 );

    switch( randomInt( 3 ) )
    {
        case 0:    
        {
            i == i1;
        } break;
        case 1:
        {
            TestIterator    i2 = i;
            i2 == i1;
        } break;
        case 2:
        {
            TestIterator    i2( i );
            i2 == i1;
        } break;
    }
}

template < class VECTOR, class ITERATOR >
void TestVector< VECTOR, ITERATOR >::CLASS_INVARIANT
{
}

template < class VECTOR, class ITERATOR >
ostream& operator <<( ostream& o, const TestVector< VECTOR, ITERATOR >& t )
{

    o << "TestVector " << (void*)&t << " start" << endl;
    o << "TestVector " << (void*)&t << " end" << endl;

    return o;
}

/* End TESTVEC.CPP **************************************************/
