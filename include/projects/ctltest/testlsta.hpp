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
    static TestListAssert& instance();
    ~TestListAssert();

    typedef int                     TestType;
    typedef ctl_list< TestType >    TestListType;

    static  TestListType& list1();
    static  TestListType& list2();

    static  size_t  list1Size();
    static  size_t  list2Size();

    static  size_t  randomIndex1();
    static  size_t  randomIndex2();

    static  void frontAndBack();
    static  void constFrontAndBack();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    TestListAssert( const TestListAssert& );

    // Operation deliberately revoked
    TestListAssert& operator =( const TestListAssert& );

    TestListAssert();

    void    initialiseTestFunctions();
    void    initialiseLists();

    // Operation deliberately revoked
    bool operator ==( const TestListAssert& );

    static  void    iteratorInvalidation();
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
