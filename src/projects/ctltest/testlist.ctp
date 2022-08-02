/*
 * T E S T V E C . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"

#include "testh/testvec.hpp"
#include "testh/soaktest.hpp"
#include "testh/random.hpp"

//  static
template < class VECTOR, class ITERATOR >
void    TestList< VECTOR, ITERATOR >::operatorPlusPlusPrefix( void )
{
    size_t  index = TestListAssert::randomIndex1();

    if( index < TestListAssert::list1Size() )
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );

    LOG_DATA( TestListAssert::list1Size() );
    LOG_DATA( index );
    
    TestIterator    i = getIterator( TestListAssert::list1(), index );
    ++i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestList< VECTOR, ITERATOR >::operatorPlusPlusPostfix( void )
{
    size_t  index = TestListAssert::randomIndex1();

    if( index < TestListAssert::list1Size() )
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );

    LOG_DATA( TestListAssert::list1Size() );
    LOG_DATA( index );
    
    TestIterator    i = getIterator( TestListAssert::list1(), index );
    i++;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestList< VECTOR, ITERATOR >::operatorMinusMinusPrefix( void )
{
    size_t  index = TestListAssert::randomIndex1();

    if( index > 0 and index <= TestListAssert::list1Size() )
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );

    LOG_DATA( TestListAssert::list1Size() );
    LOG_DATA( index );
    
    TestIterator    i = getIterator( TestListAssert::list1(), index );
    --i;
}

//  static
template < class VECTOR, class ITERATOR >
void    TestList< VECTOR, ITERATOR >::operatorMinusMinusPostfix( void )
{
    size_t  index = TestListAssert::randomIndex1();

    if( index > 0 and index <= TestListAssert::list1Size() )
        TestListAssert::instance().expectedResult( SoakTestAssert::SUCCEED );
    else
        TestListAssert::instance().expectedResult( SoakTestAssert::FAIL );

    LOG_DATA( TestListAssert::list1Size() );
    LOG_DATA( index );
    
    TestIterator    i = getIterator( TestListAssert::list1(), index );
    i--;
}

// static
template < class VECTOR, class ITERATOR >
void TestList< VECTOR, ITERATOR >::comparisonData( Destinations* pDest, size_t* pIndex )
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
        *pIndex = TestListAssert::randomIndex1();
    if( *pDest == VECTOR2 )
        *pIndex = TestListAssert::randomIndex2();
}

//  Get a random iterator that excludes end()
//  static
template < class VECTOR, class ITERATOR >
TestList< VECTOR, ITERATOR >::TestIterator    TestList< VECTOR, ITERATOR >::getValidRandomIterator( TestListType& list, size_t* pIndex )
{
    PRE( list.size() != 0 );
    
    size_t index = randomInt( list.size() );
    
    *pIndex = index;
    
    TestIterator    i = list.begin();
    
    for( size_t j = 0; j < index; ++j )
        ++i;
    
    return i;
}

//  Get a random iterator that includes end()
//  static
template < class VECTOR, class ITERATOR >
TestList< VECTOR, ITERATOR >::TestIterator    TestList< VECTOR, ITERATOR >::getValidRandomIteratorInclusive( TestListType& list, size_t* pIndex )
{
    size_t index = randomInt( list.size() + 1 );
    
    *pIndex = index;
    
    TestIterator    i = list.begin();
    
    for( size_t j = 0; j < index; ++j )
        ++i;
    
    return i;
}

//  Get the iterator corresponding to index. Note that this function might fail if
//  index is greater than the size of the list

//  static
template < class VECTOR, class ITERATOR >
TestList< VECTOR, ITERATOR >::TestIterator    TestList< VECTOR, ITERATOR >::getIterator( TestListType& list, size_t index )
{
    TestIterator    i = list.begin();
    
    for( size_t j = 0; j < index; ++j )
        ++i;
    
    return i;
}

template < class VECTOR, class ITERATOR >
TestList< VECTOR, ITERATOR >::~TestList()
{
    TEST_INVARIANT;

}

template < class VECTOR, class ITERATOR >
void TestList< VECTOR, ITERATOR >::CLASS_INVARIANT
{
}

template < class VECTOR, class ITERATOR >
ostream& operator <<( ostream& o, const TestList< VECTOR, ITERATOR >& t )
{

    o << "TestList " << (void*)&t << " start" << endl;
    o << "TestList " << (void*)&t << " end" << endl;

    return o;
}

/* End TESTLIST.CPP **************************************************/
