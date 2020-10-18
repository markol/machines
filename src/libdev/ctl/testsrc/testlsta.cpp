/*
 * T E S T L S T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/testh/testlsta.hpp"
#include "ctl/testh/random.hpp"
#include "ctl/testh/clargs.hpp"

// static
TestListAssert& TestListAssert::instance()
{
    static TestListAssert instance_;
    return instance_;
}

TestListAssert::TestListAssert()
{
    initialiseTestFunctions();
    initialiseLists();

    TEST_INVARIANT;
}

TestListAssert::~TestListAssert()
{
    TEST_INVARIANT;

}

void    TestListAssert::initialiseTestFunctions( void )
{
    #define ADD_FUNCTION( fn )      \
        addTestFunction( #fn, fn );

    ADD_FUNCTION( iteratorInvalidation );

    ADD_FUNCTION( TestListIterator::operatorPlusPlusPrefix );
    ADD_FUNCTION( TestListIterator::operatorPlusPlusPostfix );
    ADD_FUNCTION( TestListIterator::operatorMinusMinusPrefix );
    ADD_FUNCTION( TestListIterator::operatorMinusMinusPostfix );

    ADD_FUNCTION( TestListConstIterator::operatorPlusPlusPrefix );
    ADD_FUNCTION( TestListConstIterator::operatorPlusPlusPostfix );
    ADD_FUNCTION( TestListConstIterator::operatorMinusMinusPrefix );
    ADD_FUNCTION( TestListConstIterator::operatorMinusMinusPostfix );

    ADD_FUNCTION( frontAndBack );
    ADD_FUNCTION( constFrontAndBack );
}

void    TestListAssert::initialiseLists( void )
{
    size_t  size1;
    size_t  size2;

    //  Do most of the testing with small lists to test for boundary
    //  conditions but run the occasional test with a large list.

    if( randomInt( 100 ) == 0 )
        size1 = randomInt( 10000 );
    else
        size1 = randomInt( 10 );

    if( randomInt( 100 ) == 0 )
        size2 = randomInt( 10000 );
    else
        size2 = randomInt( 10 );

    for( size_t i = 0; i < size1; ++i )
        list1_.push_back( i );

    for( size_t i = 0; i < size2; ++i )
        list2_.push_back( i );
}

// static
TestListAssert::TestListType& TestListAssert::list1( void )
{
    return  instance().list1_;
}

// static
TestListAssert::TestListType& TestListAssert::list2( void )
{
    return  instance().list2_;
}

// static
size_t  TestListAssert::list1Size( void )
{
    return  instance().list1_.size();
}

// static
size_t  TestListAssert::list2Size( void )
{
    return  instance().list2_.size();
}

// static
size_t  TestListAssert::randomIndex1( void )
{
    //  The index will not necessarily be valid

    size_t  n;

    if( list1Size() == 0 )
        n = randomInt( 100 );
    else
        n = randomInt( list1Size() * 2 );

    return n;
}

// static
size_t  TestListAssert::randomIndex2( void )
{
    //  The index will not necessarily be valid

    size_t  n;

    if( list2Size() == 0 )
        n = randomInt( 100 );
    else
        n = randomInt( list2Size() * 2 );

    return n;
}

// static
void    TestListAssert::iteratorInvalidation( void )
{
    //  This function can only work correctly if list1 has at least one element in it.

    if( list1Size() == 0 )
        list1().push_back( randomInt( 100 ) );

    size_t  index;

    TestListType::iterator    i = getValidRandomIterator( list1(), &index );

    if( randomInt( 2 ) == 0 )
    {
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
        dontInvalidateIterator( i, TestListAssert::list1() );
    }
    else
    {
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );
        invalidateIterator( i, TestListAssert::list1() );
    }

    TestListType::iterator    i1 = getValidRandomIterator( list1(), &index );

    switch( randomInt( 3 ) )
    {
        case 0:
        {
            i == i1;
        } break;
        case 1:
        {
            TestListType::iterator    i2 = i;
            i2 == i1;
        } break;
        case 2:
        {
            TestListType::iterator    i2( i );
            i2 == i1;
        } break;
    }
}

//  Get a random iterator that excludes end()
//  static
TestListAssert::TestListType::iterator    TestListAssert::getValidRandomIterator( TestListType& list, size_t* pIndex )
{
    PRE( list.size() != 0 );

    size_t index = randomInt( list.size() );
    *pIndex = index;

    TestListType::iterator    i = list.begin();

    for( size_t j = 0; j < index; ++j )
        ++i;

    return i;
}

//  static
void TestListAssert::invalidateIterator( const TestListType::iterator& i, TestListType& list )
{
    PRE( i != list.end() );

    size_t  option = randomInt( 10 );

    LOG_DATA( option );

    if( option == 0 )
    {
        list.erase( i );
    }
    else if( option == 1 )
    {
        //  Erase a range that includes i

        TestListType::iterator  from;
        TestListType::iterator  to;

        getRangeIncluding( i, list, &from, &to );

        list.erase( from, to );
    }
    else if( option == 2 )
    {
        TestListType    tempList;

        tempList.splice( tempList.end(), list );
    }
    else if( option == 3 )
    {
        TestListType    tempList;

        tempList.splice( tempList.end(), list, i );
    }
    else if( option == 4 )
    {
        TestListType    tempList;

        TestListType::iterator  from;
        TestListType::iterator  to;

        getRangeIncluding( i, list, &from, &to );

        tempList.splice( tempList.end(), list, from, to );
    }
    else if( option == 5 )
    {
        size_t  index;
        TestListType    l1( randomInt( 200 ), randomInt( 200 ) );

        list = l1;
    }
    else if( option == 6 )
    {
        if( i == list.begin() )
            list.pop_front();
        else
            list.erase( i );
    }
    else if( option == 7 )
    {
        TestListType::iterator    j( i );
        ++j;

        if( j == list.end() )
            list.pop_back();
        else
            list.erase( i );
    }
    else if( option == 8 )
    {
        size_t  index;
        TestListType    l1( randomInt( 200 ), randomInt( 200 ) );

        list.swap( l1 );
    }
    else if( option == 9 )
    {
        size_t  index;
        TestListType    l1( randomInt( 200 ), randomInt( 200 ) );

        l1.swap( list );
    }
}

//  static
void TestListAssert::dontInvalidateIterator( const TestListType::iterator& i, TestListType& list )
{
    size_t  option = randomInt( 9 );

    LOG_DATA( option );

    switch( option )
    {
        case 0:
        {
            if( list.size() > 1 )
            {
                TestListType::iterator j = iteratorThatIsnt( i, list );
                list.erase( j );
            }
        } break;
        case 1:
        {
            if( list.size() > 1 )
            {
                TestListType::iterator from;
                TestListType::iterator to;

                getRangeExcluding( i, list, &from, &to );
                list.erase( from, to );
            }
        } break;
        case 2:
        {
            size_t  index;
            list.insert( getValidRandomIterator( list, &index ), randomInt( 200 ) );
        } break;
        case 3:
        {
            size_t  index;
            list.insert( getValidRandomIterator( list, &index ), randomInt( 200 ), randomInt( 200 ) );
        } break;
        case 4:
        {
            size_t  index;
            TestListType    l1( randomInt( 200 ), randomInt( 200 ) );

            list.insert( getValidRandomIterator( list, &index ), l1.begin(), l1.end() );
        } break;
        case 5:
        {
            list.push_back( list.size() );
        } break;
        case 6:
        {
            list.push_front( list.size() );
        } break;
        case 7:
        {
            if( i != list.end() )
            {
                TestListType::iterator  j( i );
                ++j;

                if( j != list.end() )
                    list.pop_back();
            }

        } break;
        case 8:
        {
            if( i != list.begin() )
                list.pop_front();
        } break;
    }
}

//  static
TestListAssert::TestListType::iterator TestListAssert::iteratorThatIsnt( const TestListType::iterator& i, TestListType& list )
{
    TestListType::iterator  result;

    do
    {
        size_t  index = randomInt( list.size() );
        result = list.begin();

        for( size_t i = 0; i < index; ++i )
            ++result;

    } while ( result == i );

    return result;
}

//  static
void TestListAssert::getRangeIncluding( const TestListType::iterator& i, const TestListType& list, TestListType::iterator* pFrom, TestListType::iterator* pTo )
{
    PRE( list.end() != i );

    //  Get a range that includes i

    *pFrom = i;

    bool    finished = false;
    while( !finished )
    {
        if( list.begin() == *pFrom )
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
        if( list.end() == *pTo )
            finished = true;
        else if (randomInt( 2 ) == 0 )
            finished = true;
        else
            ++(*pTo);
    }
}

//  static
void TestListAssert::getRangeExcluding( const TestListType::iterator& i, TestListType& list, TestListType::iterator* pFrom, TestListType::iterator* pTo )
{
    LOG_DATA( list.size() );
    PRE( list.size() > 1 );

    //  Get a range that excludes i

    bool    getRangeAfter = ( randomInt( 2 ) == 0 );

    if( list.end() == i )
        getRangeAfter = false;

    TestListType::iterator j( i );
    ++j;

    if( list.end() == j )
        getRangeAfter = false;

    if( list.begin() == i )
        getRangeAfter = true;

    TestListType::iterator rangeBegin;
    TestListType::iterator rangeEnd;

    if( getRangeAfter )
    {
        rangeBegin = i;
        ++rangeBegin;
        rangeEnd = list.end();
    }
    else
    {
        rangeBegin = list.begin();
        rangeEnd = i;
    }

    *pFrom = rangeBegin;

    while( *pFrom != rangeEnd && randomInt( 2 ) == 0 )
        ++*pFrom;

    if( *pFrom == rangeEnd )
        --*pFrom;

    *pTo = *pFrom;
    ++*pTo;

    while( *pTo != rangeEnd && randomInt( 2 ) == 0 )
        ++*pTo;
}

// static
void    TestListAssert::frontAndBack()
{
    if( list1Size() == 0 )
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = list1().front();
    }
    else
    {
        TestType    i = list1().back();
    }
}

// static
void    TestListAssert::constFrontAndBack()
{
    if( list1Size() == 0 )
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );

    const   TestListType  v1( list1() );

    if( randomInt( 2 ) == 0 )
    {
        TestType    i = v1.front();
    }
    else
    {
        TestType    i = v1.back();
    }
}

//  static
void TestListAssert::output( TestListType& list )
{
    std::cout << "List " << (void*)&list << std::endl;

    for( TestListType::iterator i = list.begin(); i != list.end(); ++i )
        std::cout << (*i) << std::endl;
    std::cout << std::endl;
}

void TestListAssert::CLASS_INVARIANT
{
}


/* End TESTLSTA.CPP *************************************************/
