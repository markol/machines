/*
 * W I N A P I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysWindowsAPI

    A brief description of the class should go in here
*/

#ifndef _SYSTEM_WINAPI_HPP
#define _SYSTEM_WINAPI_HPP

#include "base/base.hpp"

class SysWindowsAPI
// Canonical form revoked
{
public:
    ~SysWindowsAPI();

    void CLASS_INVARIANT;

	//calls into Sleep
	static void sleep( double milliseconds );
	//calls MessageBox with NULL parent and MB_OK as the two other parameters.
	static void messageBox( const char* pMessage, const char* pTitle );
	static void messageBoxError( const char* pMessage, const char* pTitle );

	//calls peekMessage, translatemessage and dispatchmessage
	static void peekMessage();

private:
    friend ostream& operator <<( ostream& o, const SysWindowsAPI& t );

    SysWindowsAPI();
    SysWindowsAPI( const SysWindowsAPI& );
    SysWindowsAPI& operator =( const SysWindowsAPI& );

};


#endif

/* End WINAPI.HPP ***************************************************/
