/*
 * M E S S A G E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    NetMessage

    A brief description of the class should go in here
*/

#ifndef _NETWORK_MESSAGE_HPP
#define _NETWORK_MESSAGE_HPP

#include "base/base.hpp"

#include "ctl/pvector.hpp"

#include "network/messhead.hpp"
#include "network/messbody.hpp"

class NetMessage
// Canonical form revoked
{
public:
	typedef ctl_pvector< NetNodeUid > NetMessageRecipients;

    NetMessage( const NetMessageHeader& , const NetMessageBody& );
    NetMessage( const NetMessage& );
    ~NetMessage( void );

	const NetMessageHeader& header() const;
	const NetMessageBody& body() const;

//	void* data() const;
//	size_t length() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const NetMessage& t );

private:

	NetMessageHeader header_;
	NetMessageBody body_;

	unsigned char* pData_;

	size_t length_;


    NetMessage& operator =( const NetMessage& );
    bool operator ==( const NetMessage& );

};


#endif

/* End MESSAGE.HPP **************************************************/
