#ifndef _MESSAGE_HEADER_HPP
#define _MESSAGE_HEADER_HPP

#include "network/netdefs.hpp"
#include "network/priority.hpp"
#include "network/nodeuid.hpp"
#include <enet/enet.h>

class NetMessageHeader
{
public:
	NetMessageHeader( ENetPeer* pPeer, const NetPriority& priority);
	NetMessageHeader( const NetMessageHeader& );
	//NetMessageHeader( NetMessageHeader& );

	//ENetPeer*	    sender();
	const ENetPeer*	    sender() const;

	const NetPriority&	priority() const;
	size_t				length() const;

private:
	friend ostream& operator<<( ostream&, const NetMessageHeader& );
	ENetPeer* 	sender_;
	NetPriority	priority_;
	size_t		length_;
};

#endif //_MESSAGE_HEADER_HPP
