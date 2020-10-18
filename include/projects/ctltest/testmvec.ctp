/*
 * T E S T M V E C . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "testh/testmvec.hpp"
#include "testh/soaktest.hpp"
#include "testh/random.hpp"
#include "testh/clargs.hpp"


//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorOperatorSquareBrackets( )
{
    size_t  n = TestMinMemoryVectorAssert::randomIndex1();
    
    if( n < TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestType    i = TestMinMemoryVectorAssert::vector1()[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorAddition( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();
    size_t  n = TestMinMemoryVectorAssert::randomIndex1();

    if( index + n <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

// INSPECT_ON( cout, TestMinMemoryVectorAssert::vector1Size() );
// INSPECT_ON( cout, index );
// INSPECT_ON( cout, n );
    
    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i + n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorAddEquals( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();
    size_t  n = TestMinMemoryVectorAssert::randomIndex1();

    if( index + n <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    
    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    i += n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorSubtraction( void )
{
    int  index = TestMinMemoryVectorAssert::randomIndex1();
    int  n = TestMinMemoryVectorAssert::randomIndex1();

    if( ( index <= TestMinMemoryVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i - n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorSubtractEquals( void )
{
    int  index = TestMinMemoryVectorAssert::randomIndex1();
    int  n = TestMinMemoryVectorAssert::randomIndex1();

    if( ( index <= TestMinMemoryVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    i -= n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorSquareBrackets( void )
{
    int  index = TestMinMemoryVectorAssert::randomIndex1();
    int  n = TestMinMemoryVectorAssert::randomIndex1();

    if( index + n <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );

    TestType    t = i[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorPlusPlusPrefix( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();

    if( index < TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    ++i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorPlusPlusPostfix( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();

    if( index < TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    i++;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorMinusMinusPrefix( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();

    if( index > 0 && index <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    --i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorOperatorMinusMinusPostfix( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();

    if( index > 0 && index <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    i--;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorDereference( void )
{
    size_t  index = TestMinMemoryVectorAssert::randomIndex1();

    LOG_DATA( index );
    LOG_DATA( TestMinMemoryVectorAssert::vector1Size() );
    
    if( index < TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), index, &i );
    *i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::TestMinMemoryVectorIteratorComparison( void )
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
    LOG_DATA( TestMinMemoryVectorAssert::vector1Size() );
    LOG_DATA( TestMinMemoryVectorAssert::vector2Size() );

    TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    if( dA == VECTOR1 && indexA <= TestMinMemoryVectorAssert::vector1Size() &&
        dB == VECTOR1 && indexB <= TestMinMemoryVectorAssert::vector1Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    if( dA == VECTOR2 && indexA <= TestMinMemoryVectorAssert::vector2Size() &&
        dB == VECTOR2 && indexB <= TestMinMemoryVectorAssert::vector2Size() )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    TestIterator    i1;
    TestIterator    i2;

    if( dA == VECTOR1 )
        TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), indexA, &i1 );
    if( dA == VECTOR2 )
        TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector2(), indexA, &i1 );
        
    if( dB == VECTOR1 )
        TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector1(), indexB, &i2 );
    if( dB == VECTOR2 )
        TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorAssert::vector2(), indexB, &i2 );

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
// ostream& operator <<( ostream& o, const TestMinMemoryVector< VECTOR, ITERATOR >::Destinations& d )
// {
//     switch( d )
//     {
//         case TestMinMemoryVector< VECTOR, ITERATOR >::NOTHING:
//             o << "NOTHING";
//             break;
//         case TestMinMemoryVector< VECTOR, ITERATOR >::VECTOR1:
//             o << "VECTOR1";
//             break;
//         case TestMinMemoryVector< VECTOR, ITERATOR >::VECTOR2:
//             o << "VECTOR2";
//             break;
//     }
//     
//     return o;
// }

// static
template < class VECTOR, class ITERATOR >
void TestMinMemoryVector< VECTOR, ITERATOR >::comparisonData( Destinations* pDest, size_t* pIndex )
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
        *pIndex = TestMinMemoryVectorAssert::randomIndex1();
    if( *pDest == VECTOR2 )
        *pIndex = TestMinMemoryVectorAssert::randomIndex2();
}


template < class VECTOR, class ITERATOR >
TestMinMemoryVector< VECTOR, ITERATOR >::~TestMinMemoryVector()
{
    TEST_INVARIANT;

}

// static
template < class VECTOR, class ITERATOR >
void    TestMinMemoryVector< VECTOR, ITERATOR >::iteratorInvalidation( void )
{
    size_t  index;
    TestIterator    i;

    if( TestMinMemoryVectorAssert::vector1().size() == 0 )
        TestMinMemoryVectorAssert::vector1().push_back( TestMinMemoryVectorAssert::vector1().size() );
        
    TestMinMemoryVectorAssert::getValidRandomIterator( TestMinMemoryVectorAssert::vector1(), &index, &i );

    if( randomInt( 2 ) == 0 )
    {
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
#ifndef NDEBUG
        TestMinMemoryVectorAssert::dontInvalidateIterator( i, TestMinMemoryVectorAssert::vector1() );
#endif
    }
    else
    {
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
#ifndef NDEBUG
        TestMinMemoryVectorAssert::invalidateIterator( i, TestMinMemoryVectorAssert::vector1() );
#endif
    }
    
    TestIterator    i1;
    TestMinMemoryVectorAssert::getValidRandomIteratorInclusive( TestMinMemoryVectorAssert::vector1(), &index, &i1 );

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
void TestMinMemoryVector< VECTOR, ITERATOR >::CLASS_INVARIANT
{
}

template < class VECTOR, class ITERATOR >
ostream& operator <<( ostream& o, const TestMinMemoryVector< VECTOR, ITERATOR >& t )
{

    o << "TestMinMemoryVector " << (void*)&t << " start" << endl;
    o << "TestMinMemoryVector " << (void*)&t << " end" << endl;

    return o;
}

/* End TESTVEC.CPP **************************************************/
