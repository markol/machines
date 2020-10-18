/*
 * E R R O R . C P P
 */

#include "base/error.hpp"
#include "system/winapi.hpp"

namespace BaseErr
{
    void TerminateOnError( const char* pMsg )
    {
        SysWindowsAPI::messageBoxError( pMsg, "Error" );
        exit(1);
    }
}


/* End ERROR.CPP *************************************************/
