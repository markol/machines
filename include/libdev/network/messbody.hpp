#ifndef _MESSAGE_BODY_HPP
#define _MESSAGE_BODY_HPP

#include "base/base.hpp"

#include "network/buffer.hpp"

class NetMessageBody
{
public:

	NetMessageBody( const uint8* body, size_t bodyLengthInBytes );

	size_t lengthInBytes() const;
	const uint8* body() const;

private:
	friend ostream& operator<<( ostream&, const NetMessageBody& );
	UtlBuffer< uint8 >	body_;
};


#endif //_MESSAGE_BODY_HPP