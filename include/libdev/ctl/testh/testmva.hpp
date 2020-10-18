/*
 * T E S T M V A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    TestMinMemoryVectorAssert

    Allow the min_memory_vector assertion code to be tested
*/

#ifndef _TESTMVA_HPP
#define _TESTMVA_HPP

#include "base/base.hpp"

#include "ctl/mmvector.hpp"

#include "ctl/testh/soaktest.hpp"
//#include "ctl/testh/testmvec.hpp"
//#include "ctl/testh/random.hpp"
//#include "ctl/testh/clargs.hpp"

template < class VECTOR, class ITERATOR >
class TestMinMemoryVector;

class TestMinMemoryVectorAssert : public SoakTestAssert
{
public:
    static TestMinMemoryVectorAssert& instance( void );
    ~TestMinMemoryVectorAssert( void );

    typedef int                         TestType;
    typedef ctl_min_memory_vector< TestType >      TestMinMemoryVectorType;

    static  TestMinMemoryVectorType& vector1( void );
    static  TestMinMemoryVectorType& vector2( void );

    static  size_t  vector1Size( void );
    static  size_t  vector2Size( void );

    static  size_t  randomIndex1( void );
    static  size_t  randomIndex2( void );

    static  void invalidateIterator( const TestMinMemoryVectorType::iterator& i, TestMinMemoryVectorType& vector );
    static  void invalidateIterator( const TestMinMemoryVectorType::const_iterator& i, TestMinMemoryVectorType& vector );
    static  void dontInvalidateIterator( const TestMinMemoryVectorType::iterator& i, TestMinMemoryVectorType& vector );
    static  void dontInvalidateIterator( const TestMinMemoryVectorType::const_iterator& i, TestMinMemoryVectorType& vector );

    static  void getValidRandomIterator( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::iterator* pI );
    static  void getValidRandomIterator( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::const_iterator* pI );
    static  void getValidRandomIteratorInclusive( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::iterator* pI );
    static  void getValidRandomIteratorInclusive( TestMinMemoryVectorType& vector, size_t* pIndex, TestMinMemoryVectorType::const_iterator* pI );
    static  void getIterator( TestMinMemoryVectorType& vector, size_t index, TestMinMemoryVectorType::iterator* pI );
    static  void getIterator( TestMinMemoryVectorType& vector, size_t index, TestMinMemoryVectorType::const_iterator* pI );

    static  void getRangeIncluding( const TestMinMemoryVectorType::iterator& i, const TestMinMemoryVectorType& vector, TestMinMemoryVectorType::iterator* pFrom, TestMinMemoryVectorType::iterator* pTo );
    static  void getRangeIncluding( const TestMinMemoryVectorType::const_iterator& i, const TestMinMemoryVectorType& vector, TestMinMemoryVectorType::const_iterator* pFrom, TestMinMemoryVectorType::const_iterator* pTo );

    static  void frontAndBack();
    static  void constFrontAndBack();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    TestMinMemoryVectorAssert( const TestMinMemoryVectorAssert& );

    // Operation deliberately revoked
    TestMinMemoryVectorAssert& operator =( const TestMinMemoryVectorAssert& );

    TestMinMemoryVectorAssert( void );

    void    initialiseTestFunctions( void );
    void    initialiseVectors( void );

    // Operation deliberately revoked
    bool operator ==( const TestMinMemoryVectorAssert& );

    typedef TestMinMemoryVector< TestMinMemoryVectorType, TestMinMemoryVectorType::iterator >        TestMinMemoryVectorIterator;
    typedef TestMinMemoryVector< TestMinMemoryVectorType, TestMinMemoryVectorType::const_iterator >  TestMinMemoryVectorConstIterator;

    TestMinMemoryVectorType  vector1_;
    TestMinMemoryVectorType  vector2_;

};

#include "ctl/testh/testmvec.hpp"

#endif

/* End TESTVECA.HPP *************************************************/
