/*
 * P I N G H E L P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    NetPingHelper

    A central repository of all ping information collected so far.
	This class also contains a few helper functions to determine the ping - hence the name
*/

#ifndef _NETWORK_PINGHELP_HPP
#define _NETWORK_PINGHELP_HPP

#include "base/base.hpp"
#include "ctl/map.hpp"
#include "network/nodeuid.hpp"
#include "phys/phys.hpp"

class NetPingHelper
// Canonical form revoked
{
public:
    NetPingHelper();
    ~NetPingHelper();
	struct PingInformation
	{
		PhysAbsoluteTime		lastTimePingAsked_;
		PhysAbsoluteTime		lastTimePingSet_;
		int						ping_;
	};

	typedef ctl_map< NetNodeUid, PingInformation, std::less< NetNodeUid > >		Ping;

	void pingAll() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const NetPingHelper& t );

    NetPingHelper( const NetPingHelper& );
    NetPingHelper& operator =( const NetPingHelper& );

	Ping			ping_;

};


#endif

/* End PINGHELP.HPP *************************************************/
