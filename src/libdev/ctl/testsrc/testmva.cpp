/*
 * T E S T M V A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/testh/testmva.hpp"

// static
TestMinMemoryVectorAssert& TestMinMemoryVectorAssert::instance()
{
    static TestMinMemoryVectorAssert instance_;
    return instance_;
}

TestMinMemoryVectorAssert::TestMinMemoryVectorAssert()
{
    initialiseTestFunctions();
    initialiseVectors();

    TEST_INVARIANT;
}

TestMinMemoryVectorAssert::~TestMinMemoryVectorAssert()
{
    TEST_INVARIANT;

}

void    TestMinMemoryVectorAssert::initialiseTestFunctions( void )
{
    #define ADD_FUNCTION( fn )      \
        addTestFunction( #fn, fn );

    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorOperatorSquareBrackets );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorOperatorSquareBrackets );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorAddition );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorAddition );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorAddEquals );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorAddEquals );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorSubtraction );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorSubtraction );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorSubtractEquals );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorPlusPlusPrefix );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorPlusPlusPostfix );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorMinusMinusPrefix );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorMinusMinusPostfix );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorOperatorSquareBrackets );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorDereference );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorDereference );
    ADD_FUNCTION( TestMinMemoryVectorIterator::TestMinMemoryVectorIteratorComparison );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::TestMinMemoryVectorIteratorComparison );
    ADD_FUNCTION( TestMinMemoryVectorIterator::iteratorInvalidation );
    ADD_FUNCTION( TestMinMemoryVectorConstIterator::iteratorInvalidation );

    ADD_FUNCTION( frontAndBack );
    ADD_FUNCTION( constFrontAndBack );
}

void    TestMinMemoryVectorAssert::initialiseVectors( void )
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
TestMinMemoryVectorAssert::TestMinMemoryVectorType& TestMinMemoryVectorAssert::vector1( void )
{
    return  instance().vector1_;
}

// static
TestMinMemoryVectorAssert::TestMinMemoryVectorType& TestMinMemoryVectorAssert::vector2( void )
{
    return  instance().vector2_;
}

// static
size_t  TestMinMemoryVectorAssert::vector1Size( void )
{
    return  instance().vector1_.size();
}

// static
size_t  TestMinMemoryVectorAssert::vector2Size( void )
{
    return  instance().vector2_.size();
}

// static
size_t  TestMinMemoryVectorAssert::randomIndex1( void )
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
size_t  TestMinMemoryVectorAssert::randomIndex2( void )
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
void TestMinMemoryVectorAssert::invalidateIterator( const TestMinMemoryVectorType::iterator& i, TestMinMemoryVectorType& vector )
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
            TestMinMemoryVectorType::iterator    from;
            TestMinMemoryVectorType::iterator    to;
            getRangeIncluding( i, vector, &from, &to );
            LOG_DATA( *i );
            LOG_DATA( *from );
            if( to == vector.end() )
                LOG_DATA( "to end" );
            else
                LOG_DATA( *to );
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
                TestMinMemoryVectorType::iterator    j( i );
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
            TestMinMemoryVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector.insert( i, v1.begin(), v1.end() );
        } break;
        case 7:
        {
            TestMinMemoryVectorType::iterator    j( i );

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
            TestMinMemoryVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            v1.swap( vector );
        } break;
        case 10:
        {
            TestMinMemoryVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector.swap( v1 );
        } break;
        case 11:
        {
            TestMinMemoryVectorType  v1( randomInt( 20 ) + 1, randomInt( 100 ) );

            vector = v1;
        } break;
    }
}

//  static
void TestMinMemoryVectorAssert::invalidateIterator( const TestMinMemoryVectorType::const_iterator& i, TestMinMemoryVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestMinMemoryVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    invalidateIterator( i1, vector );
#endif
}

//  static
void TestMinMemoryVectorAssert::dontInvalidateIterator( const TestMinMemoryVectorType::iterator& i, TestMinMemoryVectorType& vector )
{
    PRE( i != vector.end() );
    size_t  option = randomInt( 2 );

    switch( option )
    {
        case 0:
            vector.size();
            break;
        case 1:
            TestMinMemoryVectorType::iterator j( i );
            ++j;
            if( j != vector.end() )
                vector.erase( j );
            break;
    }
}

//  static
void TestMinMemoryVectorAssert::dontInvalidateIterator( const TestMinMemoryVectorType::const_iterator& i, TestMinMemoryVectorType& vector )
{
    size_t  index = i - vector.begin();

    TestMinMemoryVectorType::iterator    i1 = vector.begin();
    i1 += index;

#ifndef NDEBUG
    dontInvalidateIterator( i1, vector );
#endif
}

//  Get a random iterator that excludes end()
void    TestMinMemoryVectorAssert::getValidRandomIterator( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that excludes end()
void    TestMinMemoryVectorAssert::getValidRandomIterator( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::const_iterator* pI )
{
    PRE( vector.size() != 0 );

    *pIndex = randomInt( vector.size() );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random iterator that includes end()
//  static
void    TestMinMemoryVectorAssert::getValidRandomIteratorInclusive( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get a random const_iterator that includes end()
//  static
void    TestMinMemoryVectorAssert::getValidRandomIteratorInclusive( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::const_iterator* pI )
{
    *pIndex = randomInt( vector.size() + 1 );

    *pI = vector.begin();
    *pI += *pIndex;
}

//  Get the iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorType& vector, size_t index, TestMinMemoryVectorType::iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  Get the const_iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the vector

//  static
void TestMinMemoryVectorAssert::getIterator( TestMinMemoryVectorType& vector, size_t index, TestMinMemoryVectorType::const_iterator* pI )
{
    *pI = vector.begin();
    *pI += index;
}

//  static
void TestMinMemoryVectorAssert::getRangeIncluding( const TestMinMemoryVectorType::iterator& i, const TestMinMemoryVectorType& vector, TestMinMemoryVectorType::iterator* pFrom, TestMinMemoryVectorType::iterator* pTo )
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
void TestMinMemoryVectorAssert::getRangeIncluding( const TestMinMemoryVectorType::const_iterator& i, const TestMinMemoryVectorType& vector, TestMinMemoryVectorType::const_iterator* pFrom, TestMinMemoryVectorType::const_iterator* pTo )
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
void    TestMinMemoryVectorAssert::frontAndBack()
{
    if( vector1Size() == 0 )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

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
void    TestMinMemoryVectorAssert::constFrontAndBack()
{
    if( vector1Size() == 0 )
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestMinMemoryVectorAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    const   TestMinMemoryVectorType  v1( vector1() );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = v1.front();
    }
    else
    {
        TestType    i = v1.back();
    }
}

void TestMinMemoryVectorAssert::CLASS_INVARIANT
{
}


/* End TESTVECA.CPP *************************************************/
