/*
 * E R R O R H N D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    ErrorHandler

    A brief description of the class should go in here
*/

#ifndef _MACHINES_ERRORHND_HPP
#define _MACHINES_ERRORHND_HPP

#include "base/base.hpp"

class RenDisplay;

class ErrorHandler
{
public:
    //  Singleton class
    static ErrorHandler& instance( void );
    ~ErrorHandler( void );

    void pDisplay( RenDisplay* pDisplay );

#ifndef NDEBUG
    static  BaseAssertion::AssertionAction handleAssertionError( const BaseAssertion::AssertionInfo& );
#endif
    
    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    ErrorHandler( const ErrorHandler& );

    // Operation deliberately revoked
    ErrorHandler& operator =( const ErrorHandler& );

    // Operation deliberately revoked
    bool operator ==( const ErrorHandler& );

    ErrorHandler( void );
    
    RenDisplay* pDisplay_;
};


#endif

/* End ERRORHND.HPP *************************************************/
