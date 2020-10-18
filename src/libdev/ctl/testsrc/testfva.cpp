/*
 * T E S T F V A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/testh/testfva.hpp"
#include "ctl/testh/random.hpp"
#include "ctl/testh/clargs.hpp"

// static
TestFixedVectorAssert& TestFixedVectorAssert::instance()
{
    static TestFixedVectorAssert instance_;
    return instance_;
}

TestFixedVectorAssert::TestFixedVectorAssert()
{
    initialiseTestFunctions();
    initialiseVectors();

    TEST_INVARIANT;
}

TestFixedVectorAssert::~TestFixedVectorAssert()
{
    TEST_INVARIANT;

}

void    TestFixedVectorAssert::initialiseTestFunctions( void )
{
    #define ADD_FUNCTION( fn )      \
        addTestFunction( #fn, fn );

    ADD_FUNCTION( TestFixedVectorIterator::operatorSquareBrackets );
    ADD_FUNCTION( TestFixedVectorConstIterator::operatorSquareBrackets );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorAddition );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorAddition );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorAddEquals );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorAddEquals );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorSubtraction );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorSubtraction );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorDereference );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorDereference );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorComparison );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorComparison );
    ADD_FUNCTION( TestFixedVectorIterator::iteratorInvalidation );
    ADD_FUNCTION( TestFixedVectorConstIterator::iteratorInvalidation );

    ADD_FUNCTION( frontAndBack );
    ADD_FUNCTION( constFrontAndBack );
}

void    TestFixedVectorAssert::initialiseVectors( void )
{
    size_t  size1;
    size_t  size2;

    //  Do most of the testing with small vectors to test for boundary
    //  conditions but run the occasional test with a large vector.

    if( randomInt( 100 ) == 0 )
        size1 = randomInt( 10000 );
    else
        size1 = randomInt( 10 );

    if( randomInt( 100 ) == 0 )
        size2 = randomInt( 10000 );
    else
        size2 = randomInt( 10 );

    vector1_ = TestFixedVectorType( size1 );
    vector2_ = TestFixedVectorType( size2 );

    for( size_t i = 0; i < size1; ++i )
        vector1_[ i ] = i;

    for( size_t i = 0; i < size2; ++i )
        vector2_[ i ] = i;
}

// static
TestFixedVectorAssert::TestFixedVectorType& TestFixedVectorAssert::vector1( void )
{
    return  instance().vector1_;
}

// static
TestFixedVectorAssert::TestFixedVectorType& TestFixedVectorAssert::vector2( void )
{
    return  instance().vector2_;
}

// static
size_t  TestFixedVectorAssert::vector1Size( void )
{
    return  instance().vector1_.size();
}

// static
size_t  TestFixedVectorAssert::vector2Size( void )
{
    return  instance().vector2_.size();
}

// static
size_t  TestFixedVectorAssert::randomIndex1( void )
{
    //  The index will not necessarily be valid

    size_t  n;

    if( vector1Size() == 0 )
        n = randomInt( 100 );
    else
        n = randomInt( vector1Size() * 2 );

    return n;
}

// static
size_t  TestFixedVectorAssert::randomIndex2( void )
{
    //  The index will not necessarily be valid

    size_t  n;

    if( vector2Size() == 0 )
        n = randomInt( 100 );
    else
        n = randomInt( vector2Size() * 2 );

    return n;
}

//  static
void TestFixedVectorAssert::invalidateIterator( const TestFixedVectorType::iterator& i, TestFixedVectorType& vector )
{
    size_t  option = randomInt( 3 );

    LOG_DATA( option );

    switch( option )
    {
        case 0:
        {
            TestFixedVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            v1.swap( vector );
        } break;
        case 1:
        {
            TestFixedVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector.swap( v1 );
        } break;
        case 2:
        {
            TestFixedVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector = v1;
        } break;
    }
}

//  static
void TestFixedVectorAssert::invalidateIterator( const TestFixedVectorType::const_iterator& i, TestFixedVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestFixedVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    invalidateIterator( i1, vector );
#endif
}

//  static
void TestFixedVectorAssert::dontInvalidateIterator( const TestFixedVectorType::iterator& i, TestFixedVectorType& vector )
{
    PRE( i != vector.end() );
    size_t  option = randomInt( 2 );

    switch( option )
    {
        case 0:
            vector.size();
            break;
        case 1:
            TestFixedVectorType::iterator j( i );
            ++j;
            break;
    }
}

//  static
void TestFixedVectorAssert::dontInvalidateIterator( const TestFixedVectorType::const_iterator& i, TestFixedVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestFixedVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    dontInvalidateIterator( i1, vector );
#endif
}

//  Get a random iterator that excludes end()
void    TestFixedVectorAssert::getValidRandomIterator( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that excludes end()
void    TestFixedVectorAssert::getValidRandomIterator( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::const_iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random iterator that includes end()
//  static
void    TestFixedVectorAssert::getValidRandomIteratorInclusive( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that includes end()
//  static
void    TestFixedVectorAssert::getValidRandomIteratorInclusive( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::const_iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get the iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestFixedVectorAssert::getIterator( TestFixedVectorType& vector, size_t index, TestFixedVectorType::iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  Get the const_iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestFixedVectorAssert::getIterator( TestFixedVectorType& vector, size_t index, TestFixedVectorType::const_iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  static
void TestFixedVectorAssert::getRangeIncluding( const TestFixedVectorType::iterator& i, const TestFixedVectorType& vector, TestFixedVectorType::iterator* pFrom, TestFixedVectorType::iterator* pTo )
{
    PRE( vector.end() != i );

    //  Get a range that includes i

    *pFrom = i;

    bool    finished = false;
    while( !finished )
    {
        if( vector.begin() == *pFrom )
            finished = true;
        else if (randomInt( 2 ) == 0 )
            finished = true;
        else
            --(*pFrom);
    }

    *pTo = i;

    //  Make sure that to points past i otherwise i might not get erased.

    ++(*pTo);
    finished = false;

    while( !finished )
    {
        if( vector.end() == *pTo )
            finished = true;
        else if (randomInt( 2 ) == 0 )
            finished = true;
        else
            ++(*pTo);
    }
}

//  static
void TestFixedVectorAssert::getRangeIncluding( const TestFixedVectorType::const_iterator& i, const TestFixedVectorType& vector, TestFixedVectorType::const_iterator* pFrom, TestFixedVectorType::const_iterator* pTo )
{
    PRE( vector.end() != i );

    //  Get a range that includes i

    *pFrom = i;

    bool    finished = false;
    while( !finished )
    {
        if( vector.begin() == *pFrom )
            finished = true;
        else if (randomInt( 2 ) == 0 )
            finished = true;
        else
            --(*pFrom);
    }

    *pTo = i;

    //  Make sure that to points past i otherwise i might not get erased.

    ++(*pTo);
    finished = false;

    while( !finished )
    {
        if( vector.end() == *pTo )
            finished = true;
        else if (randomInt( 2 ) == 0 )
            finished = true;
        else
            ++(*pTo);
    }
}

// static
void    TestFixedVectorAssert::frontAndBack()
{
    if( vector1Size() == 0 )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = vector1().front();
    }
    else
    {
        TestType    i = vector1().back();
    }
}

// static
void    TestFixedVectorAssert::constFrontAndBack()
{
    if( vector1Size() == 0 )
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestFixedVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    const   TestFixedVectorType  v1( vector1() );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = v1.front();
    }
    else
    {
        TestType    i = v1.back();
    }
}

void TestFixedVectorAssert::CLASS_INVARIANT
{
}


/* End TESTVECA.CPP *************************************************/
