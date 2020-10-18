#include "network/sessuid.hpp"
#include <cstring>

#include "network/internal/mapping.hpp"

NetAppSessionUid::NetAppSessionUid( const NetAppSessionUid& copyMe):
appUid_(copyMe.appUid_),
appInstanceUid_(copyMe.appInstanceUid_),
appSessionName_(copyMe.appSessionName_)
{
}

NetAppUid NetAppSessionUid::appUid() const
{
	return appUid_;
}

NetAppInstanceUid NetAppSessionUid::appInstanceUid() const
{
	return appInstanceUid_;
}

const NetAppSessionName& NetAppSessionUid::appSessionName() const
{
	return appSessionName_;
}

NetAppSessionUid::NetAppSessionUid
( NetAppUid appUid, NetAppInstanceUid appInstanceUid, const NetAppSessionName& appSessionName):
appUid_(appUid),
appInstanceUid_(appInstanceUid),
appSessionName_(appSessionName)
{
}

//  Construct a NetAppSessionUid from raw data
NetAppSessionUid::NetAppSessionUid( uint8* buffer )
{
    size_t index = 0;

    memcpy( &appUid_, &buffer[ index ], sizeof( appUid_ ) );
    index += sizeof( appUid_ );

    memcpy( &appInstanceUid_, &buffer[ index ], sizeof( appInstanceUid_ ) );
    index += sizeof( appInstanceUid_ );

    size_t length;
    memcpy( &length, &buffer[ index ], sizeof( length ) );
    index += sizeof( length );

    char* str = _NEW_ARRAY( char, length + 1 );
    memcpy( str, &buffer[ index ], length );
    str[ length ] = '\0';
    appSessionName_ = str;
    _DELETE_ARRAY( str );
    index += length;

    POST( index == rawBufferSize() );
}

NetAppSessionUid NetAppSessionUid::operator =( const NetAppSessionUid& rhs )
{
	if(&rhs != this)
	{
		appUid_ = rhs.appUid_;
		appInstanceUid_ = rhs.appInstanceUid_;
		appSessionName_ = rhs.appSessionName_;
	}
	return (*this);
}

bool operator ==(const NetAppSessionUid& lhs, const NetAppSessionUid& rhs)
{
	if
	(
		NetMappings::mapUnsignedtoGUID(lhs.appUid_)
		 == NetMappings::mapUnsignedtoGUID(rhs.appUid_) and

	   	NetMappings::mapUnsignedtoGUID(lhs.appInstanceUid_)
	   	 == NetMappings::mapUnsignedtoGUID(rhs.appInstanceUid_) and

		lhs.appSessionName_ == rhs.appSessionName_
	)
		return true;

	return false;
}

size_t NetAppSessionUid::rawBufferSize() const
{
    const size_t size =
    	sizeof( appUid_ ) +
	    sizeof( appInstanceUid_ ) +
        sizeof( size_t ) +
        appSessionName_.length();

    return size;
}

//  Convert a NetAppSessionUid into raw data
void NetAppSessionUid::rawBuffer( uint8* buffer ) const
{
    size_t index = 0;

    memcpy( &buffer[ index ], &appUid_, sizeof( appUid_ ) );
    index += sizeof( appUid_ );

    memcpy( &buffer[ index ], &appInstanceUid_, sizeof( appInstanceUid_ ) );
    index += sizeof( appInstanceUid_ );

    const size_t length = appSessionName_.length();
    memcpy( &buffer[ index ], &length, sizeof( length ) );
    index += sizeof( length );

    memcpy( &buffer[ index ], appSessionName_.c_str(), length );
    index += length;

    POST( index == rawBufferSize() );
}

