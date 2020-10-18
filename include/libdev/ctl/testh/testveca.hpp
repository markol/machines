/*
 * T E S T V E C A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    TestVectorAssert

    Allow the vector assertion code to be tested
*/

#ifndef _TESTVECA_HPP
#define _TESTVECA_HPP

#include "base/base.hpp"

#include "ctl/vector.hpp"

//#include "ctl/testh/testvec.hpp"
#include "ctl/testh/soaktest.hpp"
#include "ctl/testh/random.hpp"
#include "ctl/testh/clargs.hpp"

template < class VECTOR, class ITERATOR >
class TestVector;

class TestVectorAssert : public SoakTestAssert
{
public:
    static TestVectorAssert& instance( void );
    ~TestVectorAssert( void );

    typedef int                         TestType;
    typedef ctl_vector< TestType >      TestVectorType;

    static  TestVectorType& vector1( void );
    static  TestVectorType& vector2( void );

    static  size_t  vector1Size( void );
    static  size_t  vector2Size( void );

    static  size_t  randomIndex1( void );
    static  size_t  randomIndex2( void );

    static  void invalidateIterator( const TestVectorType::iterator& i, TestVectorType& vector );
    static  void invalidateIterator( const TestVectorType::const_iterator& i, TestVectorType& vector );
    static  void dontInvalidateIterator( const TestVectorType::iterator& i, TestVectorType& vector );
    static  void dontInvalidateIterator( const TestVectorType::const_iterator& i, TestVectorType& vector );

    static  void getValidRandomIterator( TestVectorType& vector, size_t* pIndex, TestVectorType::iterator* pI );
    static  void getValidRandomIterator( TestVectorType& vector, size_t* pIndex, TestVectorType::const_iterator* pI );
    static  void getValidRandomIteratorInclusive( TestVectorType& vector, size_t* pIndex, TestVectorType::iterator* pI );
    static  void getValidRandomIteratorInclusive( TestVectorType& vector, size_t* pIndex, TestVectorType::const_iterator* pI );
    static  void getIterator( TestVectorType& vector, size_t index, TestVectorType::iterator* pI );
    static  void getIterator( TestVectorType& vector, size_t index, TestVectorType::const_iterator* pI );

    static  void getRangeIncluding( const TestVectorType::iterator& i, const TestVectorType& vector, TestVectorType::iterator* pFrom, TestVectorType::iterator* pTo );
    static  void getRangeIncluding( const TestVectorType::const_iterator& i, const TestVectorType& vector, TestVectorType::const_iterator* pFrom, TestVectorType::const_iterator* pTo );

    static  void frontAndBack();
    static  void constFrontAndBack();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    TestVectorAssert( const TestVectorAssert& );

    // Operation deliberately revoked
    TestVectorAssert& operator =( const TestVectorAssert& );

    TestVectorAssert( void );

    void    initialiseTestFunctions( void );
    void    initialiseVectors( void );

    // Operation deliberately revoked
    bool operator ==( const TestVectorAssert& );

    typedef TestVector< TestVectorType, TestVectorType::iterator >        TestVectorIterator;
    typedef TestVector< TestVectorType, TestVectorType::const_iterator >  TestVectorConstIterator;

    TestVectorType  vector1_;
    TestVectorType  vector2_;

};

#include "ctl/testh/testvec.hpp"

#endif

/* End TESTVECA.HPP *************************************************/
