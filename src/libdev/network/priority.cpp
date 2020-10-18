#include "network/priority.hpp"

NetPriority::NetPriority( Urgency urgency, RedeliveryAttempts redeliveryAttempts):
urgency_(urgency),
redeliveryAttempts_(redeliveryAttempts)
{
}

NetPriority::Urgency NetPriority::urgency() const
{
	return urgency_;
}

NetPriority::RedeliveryAttempts NetPriority::redeliveryAttempts() const
{
	return redeliveryAttempts_;
}
