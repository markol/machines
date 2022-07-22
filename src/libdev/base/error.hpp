/*
 * E R R O R . H P P
 */

/*
    Checks for errors and communicate them in message boxes.
*/

#ifndef _ERROR_HPP
#define _ERROR_HPP

namespace BaseErr
{
    void TerminateOnError( const char* pMsg );
}

#define ALWAYS_ASSERT( exp, xmsg )		\
    if( !( exp ) ) BaseErr::TerminateOnError( xmsg );

#define ALWAYS_ASSERT_FAIL( xmsg ) BaseErr::TerminateOnError( xmsg );

#endif

/* End ERROR.HPP *************************************************/
