/*
 * E R R O R . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    ErrorHandler

    A brief description of the class should go in here
*/

#ifndef _ERROR_HPP
#define _ERROR_HPP

#include <string.hpp>

#include "base/base.hpp"

class ErrorHandler
{
public:
    //  Singleton class
    static ErrorHandler& instance();
    ~ErrorHandler();

    void    initialise();
    size_t  nTests() const;
    size_t  nFailures() const;
    
    static  BaseAssertion::AssertionAction assertionHandler( const BaseAssertion::AssertionInfo& );
    
    enum    ExpectedResult { SUCCEED, FAIL };

    void    errorCheck( ExpectedResult, const string& where );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const ErrorHandler& t );

private:
    // Operation deliberately revoked
    ErrorHandler( const ErrorHandler& );

    // Operation deliberately revoked
    ErrorHandler& operator =( const ErrorHandler& );

    // Operation deliberately revoked
    bool operator ==( const ErrorHandler& );

    ErrorHandler();

    size_t  nFailures_;
    size_t  nTests_;
    bool    assertionFailed_;
};

ErrorHandler& ERROR_HANDLER()
{
    return ErrorHandler::instance();
}

#endif

/* End ERROR.HPP ****************************************************/
