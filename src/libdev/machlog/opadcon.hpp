/*
 * O P A D C O N . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_OPADCON_HPP
#define _MACHLOG_OPADCON_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"

class MachLogAdministrator;
class MachLogMine;
class MachLogConstructor;
class MachLogResourceCarrier;
class MachLogConstruction;

//orthodox canonical (revoked)

class MachLogAdminConstructOperation
: public MachLogOperation
{
public:

	MachLogAdminConstructOperation( MachLogAdministrator * pActor );
	MachLogAdminConstructOperation( MachLogAdministrator * pActor, MachLogConstruction* );

	virtual ~MachLogAdminConstructOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminConstructOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminConstructOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminConstructOperation( const MachLogAdminConstructOperation& );
    MachLogAdminConstructOperation& operator =( const MachLogAdminConstructOperation& );
    bool operator ==( const MachLogAdminConstructOperation& );
	
    bool findIncompleteMine( MachLogMine** ppMine );
//	enum State { MOVING, BUILDING, MOVING_WHILE_BUILDING };

	MachLogAdministrator *					pActor_;
	MachLogConstruction *					pConstruction_;
    bool complete_;
	bool									currentlyAttached_;
};

PER_DECLARE_PERSISTENT( MachLogAdminConstructOperation );

#endif