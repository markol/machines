/*
 * T E S T V E C . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "ctl/testh/testfvec.hpp"
#include "ctl/testh/soaktest.hpp"
#include "ctl/testh/random.hpp"
#include "ctl/testh/clargs.hpp"


//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::operatorSquareBrackets( )
{
    size_t  n = TestFixedVectorAssert::randomIndex1();

    if( n < TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestType    i = TestFixedVectorAssert::vector1()[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorAddition( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();
    size_t  n = TestFixedVectorAssert::randomIndex1();

    if( index + n <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

// INSPECT_ON( cout, TestFixedVectorAssert::vector1Size() );
// INSPECT_ON( cout, index );
// INSPECT_ON( cout, n );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i + n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorAddEquals( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();
    size_t  n = TestFixedVectorAssert::randomIndex1();

    if( index + n <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    i += n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorSubtraction( void )
{
    int  index = TestFixedVectorAssert::randomIndex1();
    int  n = TestFixedVectorAssert::randomIndex1();

    if( ( index <= TestFixedVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    TestIterator    i2 = i - n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorSubtractEquals( void )
{
    int  index = TestFixedVectorAssert::randomIndex1();
    int  n = TestFixedVectorAssert::randomIndex1();

    if( ( index <= TestFixedVectorAssert::vector1Size() ) and ( index - n >= 0 ) )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    i -= n;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorSquareBrackets( void )
{
    int  index = TestFixedVectorAssert::randomIndex1();
    int  n = TestFixedVectorAssert::randomIndex1();

    if( index + n <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );

    TestType    t = i[ n ];
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorPlusPlusPrefix( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();

    if( index < TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    ++i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorPlusPlusPostfix( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();

    if( index < TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    i++;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorMinusMinusPrefix( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();

    if( index > 0 && index <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    --i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorOperatorMinusMinusPostfix( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();

    if( index > 0 && index <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    i--;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorDereference( void )
{
    size_t  index = TestFixedVectorAssert::randomIndex1();

    LOG_DATA( index );
    LOG_DATA( TestFixedVectorAssert::vector1Size() );

    if( index < TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    TestIterator    i;
    TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), index, &i );
    *i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorComparison( void )
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
    LOG_DATA( TestFixedVectorAssert::vector1Size() );
    LOG_DATA( TestFixedVectorAssert::vector2Size() );

    TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );

    if( dA == VECTOR1 && indexA <= TestFixedVectorAssert::vector1Size() &&
        dB == VECTOR1 && indexB <= TestFixedVectorAssert::vector1Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    if( dA == VECTOR2 && indexA <= TestFixedVectorAssert::vector2Size() &&
        dB == VECTOR2 && indexB <= TestFixedVectorAssert::vector2Size() )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    TestIterator    i1;
    TestIterator    i2;

    if( dA == VECTOR1 )
        TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), indexA, &i1 );
    if( dA == VECTOR2 )
        TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector2(), indexA, &i1 );

    if( dB == VECTOR1 )
        TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector1(), indexB, &i2 );
    if( dB == VECTOR2 )
        TestFixedVectorAssert::getIterator( TestFixedVectorAssert::vector2(), indexB, &i2 );

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
// ostream& operator <<( ostream& o, const TestFixedVector< VECTOR, ITERATOR >::Destinations& d )
// {
//     switch( d )
//     {
//         case TestFixedVector< VECTOR, ITERATOR >::NOTHING:
//             o << "NOTHING";
//             break;
//         case TestFixedVector< VECTOR, ITERATOR >::VECTOR1:
//             o << "VECTOR1";
//             break;
//         case TestFixedVector< VECTOR, ITERATOR >::VECTOR2:
//             o << "VECTOR2";
//             break;
//     }
//
//     return o;
// }

// static
template < class VECTOR, class ITERATOR >
void TestFixedVector< VECTOR, ITERATOR >::comparisonData( Destinations* pDest, size_t* pIndex )
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
        *pIndex = TestFixedVectorAssert::randomIndex1();
    if( *pDest == VECTOR2 )
        *pIndex = TestFixedVectorAssert::randomIndex2();
}


template < class VECTOR, class ITERATOR >
TestFixedVector< VECTOR, ITERATOR >::~TestFixedVector()
{
    TEST_INVARIANT;

}

// static
template < class VECTOR, class ITERATOR >
void    TestFixedVector< VECTOR, ITERATOR >::iteratorInvalidation( void )
{
    size_t  index;
    TestIterator    i;

    if( TestFixedVectorAssert::vector1().size() == 0 )
    {
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
        return;
    }

    TestFixedVectorAssert::getValidRandomIterator( TestFixedVectorAssert::vector1(), &index, &i );

    if( randomInt( 2 ) == 0 )
    {
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
#ifndef NDEBUG
        TestFixedVectorAssert::dontInvalidateIterator( i, TestFixedVectorAssert::vector1() );
#endif
    }
    else
    {
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
#ifndef NDEBUG
        TestFixedVectorAssert::invalidateIterator( i, TestFixedVectorAssert::vector1() );
#endif
    }

    TestIterator    i1;
    TestFixedVectorAssert::getValidRandomIteratorInclusive( TestFixedVectorAssert::vector1(), &index, &i1 );

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
void TestFixedVector< VECTOR, ITERATOR >::CLASS_INVARIANT
{
}

template < class VECTOR, class ITERATOR >
ostream& operator <<( ostream& o, const TestFixedVector< VECTOR, ITERATOR >& t )
{

    o << "TestFixedVector " << (void*)&t << " start" << std::endl;
    o << "TestFixedVector " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TESTVEC.CPP **************************************************/
