/*
 * M E S S A G E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/message.hpp"
//#include <windows.h>

NetMessage::NetMessage( const NetMessageHeader& header, const NetMessageBody& body)
: header_(header),
  body_(body)
{
//	size_t headerSize = sizeof(NetMessageHeader);
//	length_ = headerSize + body.lengthInBytes();
//
//	pData_ = _NEW_ARRAY(unsigned char, length_);
//
//	pHeader_ = (NetMessageHeader*)pData_;
//	pBody_ = (NetMessageBody*)(pData_ + headerSize);
//
//	::CopyMemory((void*)pHeader_, pData_, headerSize);
//	::CopyMemory((void*)pBody_, pData_, body.lengthInBytes());
//
//	cout << "Name1 == " << header.sender().nodeName()<< std::endl;
//	cout << "Name2 == " << pHeader_->sender().nodeName()<< std::endl;
//
    TEST_INVARIANT;
}

NetMessage::NetMessage( const NetMessage& copyMe )
: header_( copyMe.header_ ),
  body_( copyMe.body_ )
{
}

NetMessage::~NetMessage()
{
    TEST_INVARIANT;

}

const NetMessageHeader& NetMessage::header() const
{
	return header_;
}

const NetMessageBody& NetMessage::body() const
{
	return body_;
}

//void* NetMessage::data() const
//{
//	return (void*)this;
//}
//
//size_t NetMessage::length() const
//{
//	return length_;
//}



void NetMessage::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const NetMessage& t )
{

    o << "NetMessage " << (void*)&t << " start" << std::endl;
	o << " header:\n" << t.header_ << std::endl;
	o << " body:\n" << t.body_ << std::endl;
    o << "NetMessage " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MESSAGE.CPP **************************************************/
