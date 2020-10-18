/*
 * O P E A P C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogDeployAPCOperation

    This operation will be applied to a machine wishing to enter
	an APC.
*/

#ifndef _MACHLOG_OPDAPC_HPP
#define _MACHLOG_OPDAPC_HPP

#include "base/base.hpp"
#include "machlog/operatio.hpp"

class MachLogMachine;
class MachLogAPC;
class MexPoint3d;
class MachLogDeployAPCOperationImpl;

// orthodox canonical (revoked)

class MachLogDeployAPCOperation : public MachLogOperation
{
public:

    MachLogDeployAPCOperation( MachLogAPC* , const MexPoint3d& );

    ~MachLogDeployAPCOperation( void );

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogDeployAPCOperation );
	PER_FRIEND_READ_WRITE( MachLogDeployAPCOperation );
	
	friend class MachLogDeployAPCOperationImpl;

protected:

	virtual bool doStart();
	// PRE( not isFinished() );
	virtual void doFinish();
	// PRE( isFinished() );
	
	virtual bool doIsFinished() const;
		
	virtual void doOutputOperator( ostream& ) const;

	virtual PhysRelativeTime doUpdate( );

	virtual bool doBeInterrupted();
	
	virtual void pushFurtherOut( MachLogMachine* pMachine, MATHEX_SCALAR additionalDistance );
	// PRE (pMachine != NULL );
	
	///////////////////////////////
	
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogDeployAPCOperation& t );

private:

	enum Status { GOTO_DESTINATION, DEPLOY_MACHINES };
	PER_FRIEND_ENUM_PERSISTENT( Status );

    // Operations deliberately revoked
    MachLogDeployAPCOperation( const MachLogDeployAPCOperation& );
    MachLogDeployAPCOperation& operator =( const MachLogDeployAPCOperation& );
    bool operator ==( const MachLogDeployAPCOperation& );

	MachLogDeployAPCOperationImpl* pImpl_;	   
};

PER_DECLARE_PERSISTENT( MachLogDeployAPCOperation );

#endif

/* End OPEAPC.HPP ***************************************************/
