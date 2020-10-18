#ifndef _NETNODEUID_HPP
#define _NETNODEUID_HPP

#include "network/netdefs.hpp"

//class NetProcessUid;
class NetNetwork;
class NetNodeUid;
class NetNode;

//typedef ctl_vector< pair< NetNodeUid*, NetMessageUid* > > NetMessageRecipients;

class NetNodeUid
// memberwise cannonical
{
public:

	NetNodeUid( 	const NetProcessorUid& pUid,
					NetThreadUid tUid,
					NetInterProcessUid iUid,
					const NetNodeName&	nodeName );

	NetNodeUid(const NetNodeUid& copyUid);
	NetNodeUid& operator =( const NetNodeUid& );

	//Added a default CTOR so I can stick it into maps etc.
	NetNodeUid();

	const NetProcessUid&		processUid() const;
	NetInterProcessUid			interprocessUid() const;
	const NetNodeName&			nodeName() const;

	///////////////////////////////

private:

	//revoked.
	bool operator <( const NetNodeUid& );

	NetNodeUid( 	const NetProcessUid& pUid,
					NetInterProcessUid iUid,
					const NetNodeName&	nodeName );



	NetInterProcessUid 	interProcessUid_;
	NetProcessUid 		processUid_;
	NetNodeName			nodeName_;


	friend bool operator ==( const NetNodeUid&, const NetNodeUid& );
	friend bool operator !=( const NetNodeUid&, const NetNodeUid& );
	friend bool operator <( const NetNodeUid&, const NetNodeUid& );
	friend ostream& operator <<( ostream& o, const NetNodeUid& );
	friend class NetNode;
	friend class NetIAppSession;
};


#endif //_NETNODEUID_HPP
