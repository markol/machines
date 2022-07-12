/*
 * T E S T V E C A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/testh/testveca.hpp"

// static
TestVectorAssert& TestVectorAssert::instance()
{
    static TestVectorAssert instance_;
    return instance_;
}

TestVectorAssert::TestVectorAssert()
{
    initialiseTestFunctions();
    initialiseVectors();

    TEST_INVARIANT;
}

TestVectorAssert::~TestVectorAssert()
{
    TEST_INVARIANT;

}

void    TestVectorAssert::initialiseTestFunctions()
{
    #define ADD_FUNCTION( fn )      \
        addTestFunction( #fn, fn );

    ADD_FUNCTION( TestVectorIterator::testVectorOperatorSquareBrackets );
    ADD_FUNCTION( TestVectorConstIterator::testVectorOperatorSquareBrackets );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorAddition );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorAddition );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorAddEquals );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorAddEquals );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorSubtraction );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorSubtraction );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorDereference );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorDereference );
    ADD_FUNCTION( TestVectorIterator::testVectorIteratorComparison );
    ADD_FUNCTION( TestVectorConstIterator::testVectorIteratorComparison );
    ADD_FUNCTION( TestVectorIterator::iteratorInvalidation );
    ADD_FUNCTION( TestVectorConstIterator::iteratorInvalidation );

    ADD_FUNCTION( frontAndBack );
    ADD_FUNCTION( constFrontAndBack );
}

void    TestVectorAssert::initialiseVectors()
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

    for( size_t i = 0; i < size1; ++i )
        vector1_.push_back( i );

    for( size_t i = 0; i < size2; ++i )
        vector2_.push_back( i );
}

// static
TestVectorAssert::TestVectorType& TestVectorAssert::vector1()
{
    return  instance().vector1_;
}

// static
TestVectorAssert::TestVectorType& TestVectorAssert::vector2()
{
    return  instance().vector2_;
}

// static
size_t  TestVectorAssert::vector1Size()
{
    return  instance().vector1_.size();
}

// static
size_t  TestVectorAssert::vector2Size()
{
    return  instance().vector2_.size();
}

// static
size_t  TestVectorAssert::randomIndex1()
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
size_t  TestVectorAssert::randomIndex2()
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
void TestVectorAssert::invalidateIterator( const TestVectorType::iterator& i, TestVectorType& vector )
{
    size_t  option = randomInt( 12 );

    LOG_DATA( option );

    switch( option )
    {
        case 0:
        {
            vector.push_back( vector.size() );
        } break;
        case 1:
        {
            TestVectorType::iterator    from;
            TestVectorType::iterator    to;
            getRangeIncluding( i, vector, &from, &to );
            vector.erase( from, to );
        }   break;
        case 2:
        {
            vector.erase( i );
        } break;
        case 3:
        {
            if( i == vector.begin() )
                vector.erase( i );
            else
            {
                TestVectorType::iterator    j( i );
                --j;
                vector.erase( j );
            }
        } break;
        case 4:
        {
            vector.insert( i, vector.size() );
        } break;
        case 5:
        {
            vector.insert( i, randomInt( 10 ), vector.size() );
        } break;
        case 6:
        {
            TestVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector.insert( i, v1.begin(), v1.end() );
        } break;
        case 7:
        {
            TestVectorType::iterator    j( i );

            ++j;

            if( j == vector.end() )
                vector.pop_back();
            else
                vector.erase( i );
        }
        case 8:
        {
            vector.reserve( vector.size() * 2 );
        } break;
        case 9:
        {
            TestVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            v1.swap( vector );
        } break;
        case 10:
        {
            TestVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector.swap( v1 );
        } break;
        case 11:
        {
            TestVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector = v1;
        } break;
    }
}

//  static
void TestVectorAssert::invalidateIterator( const TestVectorType::const_iterator& i, TestVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    invalidateIterator( i1, vector );
#endif
}

//  static
void TestVectorAssert::dontInvalidateIterator( const TestVectorType::iterator& i, TestVectorType& vector )
{
    PRE( i != vector.end() );
    size_t  option = randomInt( 2 );

    switch( option )
    {
        case 0:
            vector.size();
            break;
        case 1:
            TestVectorType::iterator j( i );
            ++j;
            if( j != vector.end() )
                vector.erase( j );
            break;
    }
}

//  static
void TestVectorAssert::dontInvalidateIterator( const TestVectorType::const_iterator& i, TestVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    dontInvalidateIterator( i1, vector );
#endif
}

//  Get a random iterator that excludes end()
void    TestVectorAssert::getValidRandomIterator( TestVectorType& vector, size_t* pIndex, TestVectorType::iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that excludes end()
void    TestVectorAssert::getValidRandomIterator( TestVectorType& vector, size_t* pIndex, TestVectorType::const_iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random iterator that includes end()
//  static
void    TestVectorAssert::getValidRandomIteratorInclusive( TestVectorType& vector, size_t* pIndex, TestVectorType::iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that includes end()
//  static
void    TestVectorAssert::getValidRandomIteratorInclusive( TestVectorType& vector, size_t* pIndex, TestVectorType::const_iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get the iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestVectorAssert::getIterator( TestVectorType& vector, size_t index, TestVectorType::iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  Get the const_iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestVectorAssert::getIterator( TestVectorType& vector, size_t index, TestVectorType::const_iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  static
void TestVectorAssert::getRangeIncluding( const TestVectorType::iterator& i, const TestVectorType& vector, TestVectorType::iterator* pFrom, TestVectorType::iterator* pTo )
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
void TestVectorAssert::getRangeIncluding( const TestVectorType::const_iterator& i, const TestVectorType& vector, TestVectorType::const_iterator* pFrom, TestVectorType::const_iterator* pTo )
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
void    TestVectorAssert::frontAndBack()
{
    if( vector1Size() == 0 )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

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
void    TestVectorAssert::constFrontAndBack()
{
    if( vector1Size() == 0 )
        TestVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    const   TestVectorType  v1( vector1() );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = v1.front();
    }
    else
    {
        TestType    i = v1.back();
    }
}

void TestVectorAssert::CLASS_INVARIANT
{
}


/* End TESTVECA.CPP *************************************************/
