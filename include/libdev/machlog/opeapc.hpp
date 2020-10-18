/*
 * O P E A P C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogEnterAPCOperation

    This operation will be applied to a machine wishing to enter
	an APC.
*/

#ifndef _MACHLOG_OPEAPC_HPP
#define _MACHLOG_OPEAPC_HPP

#include "base/base.hpp"
#include "machlog/operatio.hpp"

class MachLogMachine;
class MachLogAPC;

// orthodox canonical (revoked)

class MachLogEnterAPCOperation : public MachLogOperation
{
public:

    MachLogEnterAPCOperation( MachLogMachine* , MachLogAPC* );
    MachLogEnterAPCOperation( MachLogMachine* , MachLogAPC*, PhysPathFindingPriority );

    ~MachLogEnterAPCOperation( void );

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogEnterAPCOperation );
	PER_FRIEND_READ_WRITE( MachLogEnterAPCOperation );

protected:


	virtual bool doStart();
	// PRE( not isFinished() );
	virtual void doFinish();
	// PRE( isFinished() );
	
	virtual bool doIsFinished() const;
		
	virtual void doOutputOperator( ostream& ) const;

	virtual PhysRelativeTime doUpdate( );

	virtual bool doBeInterrupted();
	///////////////////////////////
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogEnterAPCOperation& t );

private:

	// Operations deliberately revoked
    MachLogEnterAPCOperation( const MachLogEnterAPCOperation& );
    MachLogEnterAPCOperation& operator =( const MachLogEnterAPCOperation& );
    bool operator ==( const MachLogEnterAPCOperation& );
	
	enum Status { PREPARING_TO_GO_TO_APC, GOING_TO_APC, ENTERING_APC };
	PER_FRIEND_ENUM_PERSISTENT( Status );
    	
	void checkAndDoStopGoingToAPC();
	
	
	// data members
	MachLogMachine*		pActor_;
	MachLogAPC*			pAPC_;
	bool 				finished_;
	MATHEX_SCALAR		desiredRange_;
	Status				status_;

};

PER_DECLARE_PERSISTENT( MachLogEnterAPCOperation );


#endif

/* End OPEAPC.HPP ***************************************************/
