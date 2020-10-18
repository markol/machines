/*
 * C O M P O U N D . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    NetCompoundMessage

    A brief description of the class should go in here
*/

#ifndef _NETWORK_COMPOUND_HPP
#define _NETWORK_COMPOUND_HPP

#include "base/base.hpp"
//#include "stdlib/memory.hpp"
#include <memory>

class NetMessageBody;

struct NetCompoundMessageHeader
{
	NetCompoundMessageHeader();
	char		subMessages_;
	size_t		length_;
};


class NetCompoundMessage
// Canonical form revoked
{
public:
	//For creation of a new buffer to send messages
    NetCompoundMessage( size_t maxBufferSize );
	//For a buffer returned from DX
    NetCompoundMessage( const uint8* pBuffer );

    ~NetCompoundMessage();

	//Add a message into the system
	bool 			addNetMessageBody( const NetMessageBody& );
	//Remove all messages after transmission
	void			clearAll();

	//Current total length of buffer that needs to be transmitted
	size_t			length() 		const;
	//Noddy function simple returns number of subMessages added since last transmit.
	char			subMessages() 	const;

	//Use this accessor when copying the data around.
	const uint8*	buffer() 		const;

    void CLASS_INVARIANT;

	static const uint8* compoundId();
	static size_t compoundIdLength();

	std::auto_ptr< NetMessageBody > operator[]( size_t ) const;

private:
    friend ostream& operator <<( ostream& o, const NetCompoundMessage& t );

    NetCompoundMessage( const NetCompoundMessage& );
    NetCompoundMessage& operator =( const NetCompoundMessage& );

	NetCompoundMessageHeader 	header_;

	size_t					 	currentOffset_;
	size_t						maxBufferSize_;

	uint8*						pBuffer_;

};


#endif

/* End COMPOUND.HPP *************************************************/
