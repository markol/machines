/*
 * T E S T F V A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    TestFixedVectorAssert

    Allow the fixed_vector assertion code to be tested
*/

#ifndef _TESTFVA_HPP
#define _TESTFVA_HPP

#include "base/base.hpp"

#include "ctl/fixedvec.hpp"

#include "testh/soaktest.hpp"
#include "testh/testfvec.hpp"

class TestFixedVectorAssert : public SoakTestAssert
{
public:
    static TestFixedVectorAssert& instance( void );
    ~TestFixedVectorAssert( void );

    typedef int                         TestType;
    typedef ctl_fixed_vector< TestType >      TestFixedVectorType;

    static  TestFixedVectorType& vector1( void );
    static  TestFixedVectorType& vector2( void );

    static  size_t  vector1Size( void );
    static  size_t  vector2Size( void );

    static  size_t  randomIndex1( void );
    static  size_t  randomIndex2( void );

    static  void invalidateIterator( const TestFixedVectorType::iterator& i, TestFixedVectorType& vector );
    static  void invalidateIterator( const TestFixedVectorType::const_iterator& i, TestFixedVectorType& vector );
    static  void dontInvalidateIterator( const TestFixedVectorType::iterator& i, TestFixedVectorType& vector );
    static  void dontInvalidateIterator( const TestFixedVectorType::const_iterator& i, TestFixedVectorType& vector );

    static  void getValidRandomIterator( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::iterator* pI );
    static  void getValidRandomIterator( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::const_iterator* pI );
    static  void getValidRandomIteratorInclusive( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::iterator* pI );
    static  void getValidRandomIteratorInclusive( TestFixedVectorType& vector, size_t* pIndex, TestFixedVectorType::const_iterator* pI );
    static  void getIterator( TestFixedVectorType& vector, size_t index, TestFixedVectorType::iterator* pI );
    static  void getIterator( TestFixedVectorType& vector, size_t index, TestFixedVectorType::const_iterator* pI );

    static  void getRangeIncluding( const TestFixedVectorType::iterator& i, const TestFixedVectorType& vector, TestFixedVectorType::iterator* pFrom, TestFixedVectorType::iterator* pTo );
    static  void getRangeIncluding( const TestFixedVectorType::const_iterator& i, const TestFixedVectorType& vector, TestFixedVectorType::const_iterator* pFrom, TestFixedVectorType::const_iterator* pTo );

    static  void frontAndBack();
    static  void constFrontAndBack();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    TestFixedVectorAssert( const TestFixedVectorAssert& );

    // Operation deliberately revoked
    TestFixedVectorAssert& operator =( const TestFixedVectorAssert& );

    TestFixedVectorAssert( void );

    void    initialiseTestFunctions( void );
    void    initialiseVectors( void );

    // Operation deliberately revoked
    bool operator ==( const TestFixedVectorAssert& );

    typedef TestFixedVector< TestFixedVectorType, TestFixedVectorType::iterator >        TestFixedVectorIterator;
    typedef TestFixedVector< TestFixedVectorType, TestFixedVectorType::const_iterator >  TestFixedVectorConstIterator;

    TestFixedVectorType  vector1_;
    TestFixedVectorType  vector2_;

};


#endif

/* End TESTFVA.HPP *************************************************/
