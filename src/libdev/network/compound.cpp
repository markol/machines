/*
 * C O M P O U N D . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/internal/compound.hpp"
#include "network/messbody.hpp"
#include <cstring>

NetCompoundMessageHeader::NetCompoundMessageHeader()
: subMessages_( 0 ),
  length_( NetCompoundMessage::compoundIdLength() + sizeof( NetCompoundMessageHeader ) )
{
}


NetCompoundMessage::NetCompoundMessage( size_t maxBufferSize )
:	pBuffer_( _NEW_ARRAY( uint8, maxBufferSize ) ),
	maxBufferSize_( maxBufferSize ),
	currentOffset_( NetCompoundMessage::compoundIdLength() + sizeof( NetCompoundMessageHeader ) )
{
    TEST_INVARIANT;
}

NetCompoundMessage::NetCompoundMessage( const uint8* pBuffer )
{
	memcpy( &header_, &pBuffer[ compoundIdLength() ], sizeof( NetCompoundMessageHeader ) );
	pBuffer_ = _NEW_ARRAY( uint8, header_.length_ );
	memcpy( pBuffer_, pBuffer, header_.length_ );
	maxBufferSize_ = header_.length_;
	currentOffset_ = header_.length_;
}

NetCompoundMessage::~NetCompoundMessage()
{
    TEST_INVARIANT;
	_DELETE_ARRAY( pBuffer_ );
}

void NetCompoundMessage::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const NetCompoundMessage& t )
{

    o << "NetCompoundMessage " << (void*)&t << " start" << std::endl;
    o << "NetCompoundMessage " << (void*)&t << " end" << std::endl;

    return o;
}

bool NetCompoundMessage::addNetMessageBody( const NetMessageBody& message )
{
	size_t length = message.lengthInBytes();
	size_t addedLength = message.lengthInBytes() + sizeof( short );

	//Out of space?
	if( header_.length_ + addedLength > maxBufferSize_ )
	{
		return false;
	}

	short* pShort = (short*)&pBuffer_[ currentOffset_ ];
	*pShort = (short)message.lengthInBytes();
	memcpy( &pBuffer_[ currentOffset_ + sizeof( short ) ], message.body(), length );
	currentOffset_ += addedLength;
	header_.subMessages_++;
	header_.length_ += addedLength;

	return true;
}

void NetCompoundMessage::clearAll()
{
	currentOffset_ = NetCompoundMessage::compoundIdLength() + sizeof( NetCompoundMessageHeader );
	header_.subMessages_ = 0;
	header_.length_ = currentOffset_;
}

size_t NetCompoundMessage::length() const
{
	return header_.length_;
}

char NetCompoundMessage::subMessages() 	const
{
	return header_.subMessages_;
}

const uint8* NetCompoundMessage::buffer() const
{
	//Make sure the header information inside the buffer is uptodate.
	memcpy( pBuffer_, compoundId(), compoundIdLength() );
	memcpy( &pBuffer_[ compoundIdLength() ], &header_, sizeof( NetCompoundMessageHeader ) );
	return pBuffer_;
}

//static
const uint8* NetCompoundMessage::compoundId()
{
	static uint8 id[] = { 00,0xFD,00 };
	return id;
}

//static
size_t NetCompoundMessage::compoundIdLength()
{
	return 3;
}

std::auto_ptr< NetMessageBody > NetCompoundMessage::operator[]( size_t index ) const
{
	PRE( index < header_.subMessages_ );

	size_t startPos = NetCompoundMessage::compoundIdLength() + sizeof( NetCompoundMessageHeader );
	short lengthOfSub = *(short*)(pBuffer_ + startPos );
	startPos += sizeof( short );

	size_t upCounter = 0;
	std::auto_ptr< NetMessageBody > pResult (NULL);

	while( upCounter <= index )
	{
		if( upCounter == index )
		{
			//Whatch out for that dosgy length -1 crap on the end of the next line.
			pResult = std::auto_ptr< NetMessageBody>( _NEW( NetMessageBody( &pBuffer_[ startPos ], lengthOfSub - 1 ) ) );
		}

		startPos += lengthOfSub;
		if( startPos < header_.length_ )
			lengthOfSub = *(short*)(pBuffer_ + startPos );
		startPos += sizeof( short );

		++upCounter;
	}
	return pResult;
}


/* End COMPOUND.CPP *************************************************/
