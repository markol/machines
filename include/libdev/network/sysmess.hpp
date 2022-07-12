/*
 * S Y S M E S S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    NetSystemMessageHandler

    ABC designed to give higher level libraries access to directx system messages.
	The messages are sent via virtual functions and hence there is no dependancy on directx
*/

#ifndef _NETWORK_SYSMESS_HPP
#define _NETWORK_SYSMESS_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"

class NetSystemMessageHandler
// Canonical form revoked
{
public:
    NetSystemMessageHandler();
    virtual ~NetSystemMessageHandler() = 0;
	virtual bool	handleHostMessage() = 0;
	virtual bool	handleDestroyPlayerMessage( const string& ) = 0;
	virtual bool	handleSessionLostMessage() = 0;

    void CLASS_INVARIANT;
	void playerHasBeenLost( const string& );

private:
    friend ostream& operator <<( ostream& o, const NetSystemMessageHandler& t );

    NetSystemMessageHandler( const NetSystemMessageHandler& );
    NetSystemMessageHandler& operator =( const NetSystemMessageHandler& );

};


#endif

/* End SYSMESS.HPP **************************************************/
