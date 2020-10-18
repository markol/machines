#include "network/messhead.hpp"
#include "network/node.hpp"
#include "network/netdefs.hpp"
//#include "network/priority.hpp"

//NetMessageHeader::NetMessageHeader( NetNode* pSender, NetPriority priority, const Recipients& recipients):
//sender_(pSender->nodeUid()),
//priority_(priority),
//recipients_(recipients)
//{
//}

/*NetMessageHeader::NetMessageHeader( const NetNodeUid& sender, const NetPriority& priority ):
sender_(sender),
priority_(priority)
{
	length_ = sizeof(NetProcessorUid) + sizeof(NetThreadUid) + sizeof(NetInterProcessUid)
			+ sizeof(NetProcessUid) + sender.nodeName().length();
}*/

NetMessageHeader::NetMessageHeader( ENetPeer* pPeer, const NetPriority& priority ):
sender_(pPeer),
priority_(priority)
{
//	length_ = sizeof(NetProcessorUid) + sizeof(NetThreadUid) + sizeof(NetInterProcessUid)
//			+ sizeof(NetProcessUid) + sender.nodeName().length();
}

NetMessageHeader::NetMessageHeader( const NetMessageHeader& copyMe ):
sender_(_CONST_CAST(ENetPeer*,copyMe.sender()) ),
priority_(copyMe.priority())
{
}

//const NetNodeUid& NetMessageHeader::sender() const
const ENetPeer* NetMessageHeader::sender() const
{
	return sender_;
}

const NetPriority& NetMessageHeader::priority() const
{
	return priority_;
}

ostream& operator<<( ostream& o, const NetMessageHeader& t )
{
	o << "sender_   " << t.sender_ << std::endl;
	o << "priority_ " << t.priority_.urgency() << std::endl;
	o << "length_   " << t.length_ << std::endl;
	return o;
}
