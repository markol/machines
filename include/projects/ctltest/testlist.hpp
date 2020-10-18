/*
 * T E S T L I S T . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    TestList

    Soak test the list class
*/

#ifndef _TESTLIST_HPP
#define _TESTLIST_HPP

#include "ctl/list.hpp"

#include "base/base.hpp"
#include "testh/soaktest.hpp"

template < class VECTOR, class ITERATOR >
class TestList
{
public:
    //  Singleton class
    static TestList& instance( void );
    ~TestList( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const TestList< VECTOR, ITERATOR >& t );

    static void    operatorPlusPlusPrefix( void );
    static void    operatorPlusPlusPostfix( void );
    static void    operatorMinusMinusPrefix( void );
    static void    operatorMinusMinusPostfix( void );
    static void    iteratorInvalidation( void );

private:
    // Operation deliberately revoked
    TestList( const TestList& );

    // Operation deliberately revoked
    TestList& operator =( const TestList& );

    // Operation deliberately revoked
    bool operator ==( const TestList& );

    TestList( void );

    void    initialiseLists( void );
    void    initialiseTestFunctions( void );

    typedef int                         TestType;
    typedef ctl_list< TestType >        TestListType;
    typedef ITERATOR                    TestIterator;
    
    enum    Destinations { NOTHING, VECTOR1, VECTOR2 };
//    friend  ostream& operator <<( ostream& o, const TestList< ITERATOR >::Destinations& d );
    static  void    comparisonData( Destinations* pDest, size_t* pIndex );
        
    static  TestIterator    getValidRandomIterator( TestListType& list, size_t* pIndex );
    static  TestIterator    getValidRandomIteratorInclusive( TestListType& list, size_t* pIndex );

    static  TestIterator    getIterator( TestListType& list, size_t index );

    static  void invalidateIterator( const VECTOR::iterator& i, TestListType& list );
    static  void dontInvalidateIterator( const VECTOR::iterator& i, TestListType& list );


};

// template< class ITERATOR >
// ostream& operator <<( ostream& o, const TestList< ITERATOR >::Destinations& d );

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "testh/testlist.ctp"
#endif

#endif

/* End TESTLIST.HPP **************************************************/
