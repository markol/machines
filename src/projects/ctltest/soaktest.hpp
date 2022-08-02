/*
 * S O A K T E S T . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    SoakTestAssert

    A base class that provides some basic soak testing functionality for
    testing for correct assertion failures.
*/

#ifndef _SOAKTEST_HPP
#define _SOAKTEST_HPP

#include <string.hpp>

#include "base/base.hpp"
#include "ctl/vector.hpp"

class SoakTestAssert
{
public:
    virtual ~SoakTestAssert();

    void    runSoakTestAssert();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SoakTestAssert& t );

    enum    ExpectedResult { SUCCEED, FAIL, NOT_SET };

    void    expectedResult( ExpectedResult result );
    ExpectedResult    expectedResult() const;

protected:
    SoakTestAssert();

    typedef void (*v_fn_v)();

    void    addTestFunction( const string& name, v_fn_v fn );
    
private:
    // Operation deliberately revoked
    SoakTestAssert( const SoakTestAssert& );

    // Operation deliberately revoked
    SoakTestAssert& operator =( const SoakTestAssert& );

    // Operation deliberately revoked
    bool operator ==( const SoakTestAssert& );

    ctl_vector< v_fn_v >    testFunctions_;
    ctl_vector< string >    testFunctionName_;
    
    void    readCount();
    void    writeCount();
    void    updateFailureCount();
    void    createFile( const string& fileName );
    void    writeSucceedFailFiles( ExpectedResult expectedResult );
    
    size_t  nTests_;
    size_t  nFailures_;
    bool    displayLogInfo_;
    
    ExpectedResult  expectedResult_;
};


#endif

/* End SOAKTEST.HPP *************************************************/
