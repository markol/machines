#ifndef _NETAPPSESSIONUID_HPP
#define _NETAPPSESSIONUID_HPP

#include "network/netnet.hpp"

class NetINetwork;
class NetIAppSession;
class NetIRecorder;

class NetAppSessionUid
{
public:

	NetAppUid			appUid() const;
	NetAppInstanceUid	appInstanceUid() const;
	const NetAppSessionName& 	appSessionName() const;
	
	///////////////////////////////
	NetAppSessionUid operator =( const NetAppSessionUid& );

	NetAppSessionUid( const NetAppSessionUid& );

private:
	NetAppSessionUid( NetAppUid, NetAppInstanceUid, const NetAppSessionName& );

	NetAppUid			appUid_;
	NetAppInstanceUid	appInstanceUid_;
	NetAppSessionName	appSessionName_;

	friend bool operator ==(const NetAppSessionUid& lhs, const NetAppSessionUid& rhs);

	friend class NetINetwork;
	friend class NetNode;
	friend class NetIAppSession;

    friend class NetIRecorder;

    //  These functions are purely to allow the playback and recording to work
    size_t rawBufferSize() const;
    void rawBuffer( uint8* ) const;
    // PRE( supplied buffer is at least rawBufferSize bytes big );
    NetAppSessionUid( uint8* );
};

#endif //_NETAPPSESSIONUID_HPP
