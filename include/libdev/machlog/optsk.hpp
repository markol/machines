/*
 * O P T S K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogTaskOperation

    This is the common abstract base class for the AI Tasks
	They are also used for task queuing in a squadron
*/

#ifndef _MACHLOG_OPTSK_HPP
#define _MACHLOG_OPTSK_HPP

#include "machlog/operatio.hpp"


// canonical form revoked

class MachLogTaskOperation
: public MachLogOperation
{
public:

	MachLogTaskOperation( const char*, MachLogOperation::OperationType );
	virtual ~MachLogTaskOperation();

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogTaskOperation );
protected:

private:

	// Operations deliberately revoked
    MachLogTaskOperation( const MachLogTaskOperation& );
    MachLogTaskOperation& operator =( const MachLogTaskOperation& );
    bool operator ==( const MachLogTaskOperation& );
};

PER_READ_WRITE( MachLogTaskOperation );
PER_DECLARE_PERSISTENT( MachLogTaskOperation );


#endif

/* End OPTSK.HPP *************************************************/
