#include "network/nodeuid.hpp"

const NetProcessUid& NetNodeUid::processUid() const
{
	return processUid_;
}

NetInterProcessUid NetNodeUid::interprocessUid() const
{
	return interProcessUid_;
}

const NetNodeName& NetNodeUid::nodeName() const
{
	return nodeName_;
}

NetNodeUid::NetNodeUid( const NetProcessUid& pUid,
						NetInterProcessUid iUid,
						const NetNodeName& nodeName):
interProcessUid_(iUid),
processUid_(pUid),
nodeName_(nodeName)
{
}

NetNodeUid::NetNodeUid( const NetProcessorUid& pUid,
						NetThreadUid tUid,
						NetInterProcessUid iUid,
						const NetNodeName& nodeName):
interProcessUid_(iUid),
processUid_(pUid, tUid),
nodeName_(nodeName)
{
}

NetNodeUid::NetNodeUid( const NetNodeUid& copyUid )
: interProcessUid_(copyUid.interProcessUid_),
  processUid_(copyUid.processUid_),
  nodeName_(copyUid.nodeName_)
{
}

NetNodeUid::NetNodeUid()
: nodeName_( "" ),
  processUid_( NetProcessUid("",0) ),
  interProcessUid_( 0 )
{
}

bool operator ==( const NetNodeUid& lhs, const NetNodeUid& rhs )
{

//remove reliance on == operator of ctl_pair and replace with explicit check
//	if( ( lhs.interProcessUid_ == rhs.interProcessUid_ ) and
//	    ( lhs.processUid_ == rhs.processUid_ ) and
//	    ( lhs.nodeName_ == rhs.nodeName_ ) )
//		return true;

	if( ( lhs.interProcessUid_ == rhs.interProcessUid_ ) and
	    ( lhs.processUid_.threadUid() == rhs.processUid_.threadUid() ) and
	    ( lhs.processUid_.processorUid() == rhs.processUid_.processorUid() ) and
	    ( lhs.nodeName_ == rhs.nodeName_ ) )
		return true;

	return false;
}

bool operator !=( const NetNodeUid& lhs, const NetNodeUid& rhs )
{
    return !(lhs == rhs);
}

bool operator <( const NetNodeUid& lhs, const NetNodeUid& rhs)
{
	if( lhs.interProcessUid_ < rhs.interProcessUid_ )
		return true;
	if( rhs.interProcessUid_ < lhs.interProcessUid_ )
		return false;

	if(	lhs.processUid_ < rhs.processUid_ )
		return true;
	if(	rhs.processUid_ < lhs.processUid_ )
		return false;

	if( lhs.nodeName_ < rhs.nodeName_ )
		return true;
	if( rhs.nodeName_ < lhs.nodeName_ )
		return false;

	return false;

}



ostream& operator <<( ostream& o, const NetNodeUid& uid)
{
	o << "ProcessorUid " << uid.processUid_.processorUid() <<
	" ThreadUid " << uid.processUid_.threadUid() <<
	" InterProcessUid " << uid.interProcessUid_ <<
	" NodeName " << uid.nodeName_ << std::endl;

	return o;
}

NetNodeUid& NetNodeUid::operator =( const NetNodeUid& rhs)
{
	if( &rhs != this )
	{
		interProcessUid_ = rhs.interProcessUid_;
		processUid_ = rhs.processUid_;
		processUid_ = rhs.processUid_;
		nodeName_ = rhs.nodeName_;
	}
	return *this;
}
