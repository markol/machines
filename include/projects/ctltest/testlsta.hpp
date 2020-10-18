/*
 * T E S T L S T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    TestListAssert

    Allow the list assertion code to be tested
*/

#ifndef _TESTLSTA_HPP
#define _TESTLSTA_HPP

#include "base/base.hpp"

#include "ctl/list.hpp"

#include "testh/testlist.hpp"

class TestListAssert : public SoakTestAssert
{
public:
    static TestListAssert& instance( void );
    ~TestListAssert( void );

    typedef int                     TestType;
    typedef ctl_list< TestType >    TestListType;

    static  TestListType& list1( void );
    static  TestListType& list2( void );

    static  size_t  list1Size( void );
    static  size_t  list2Size( void );

    static  size_t  randomIndex1( void );
    static  size_t  randomIndex2( void );

    static  void frontAndBack();
    static  void constFrontAndBack();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    TestListAssert( const TestListAssert& );

    // Operation deliberately revoked
    TestListAssert& operator =( const TestListAssert& );

    TestListAssert( void );

    void    initialiseTestFunctions( void );
    void    initialiseLists( void );

    // Operation deliberately revoked
    bool operator ==( const TestListAssert& );

    static  void    iteratorInvalidation( void );
    static  TestListType::iterator    getValidRandomIterator( TestListType& list, size_t* pIndex );
    static  void invalidateIterator( const TestListType::iterator& i, TestListType& list );
    static  void dontInvalidateIterator( const TestListType::iterator& i, TestListType& list );
    static  void output( TestListType& list );
    static  void getRangeIncluding( const TestListType::iterator& i, const TestListType& list, TestListType::iterator* pFrom, TestListType::iterator* pTo );
    static  void getRangeExcluding( const TestListType::iterator& i, TestListType& list, TestListType::iterator* pFrom, TestListType::iterator* pTo );
    static  TestListType::iterator iteratorThatIsnt( const TestListType::iterator& i, TestListType& list );

    typedef TestList< TestListType, TestListType::iterator >        TestListIterator;
    typedef TestList< TestListType, TestListType::const_iterator >  TestListConstIterator;

    TestListType  list1_;
    TestListType  list2_;

};


#endif

/* End TESTLSTA.HPP *************************************************/
