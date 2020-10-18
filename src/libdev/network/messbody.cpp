#include "network/messbody.hpp"

NetMessageBody::NetMessageBody( const uint8* body, size_t bodyLengthInBytes )
: body_((uint8*)body, bodyLengthInBytes)
{
}

size_t NetMessageBody::lengthInBytes() const
{
	return body_.size();
}

const uint8* NetMessageBody::body() const
{
	return body_.data();
}

ostream& operator<<( ostream& o, const NetMessageBody& t )
{
	o << "size " << t.body_.size() << std::endl;
	o << "data as char: " << std::endl;
	for( int i = 0; i < t.body_.size(); ++i )
	{
		o << (int)t.body_.data()[i] << " " << t.body_.data()[i] << " ";
	}
	o << std::endl;
	return o;
}
