/*
 * O P A D R E P A . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_OPADREPA_HPP
#define _MACHLOG_OPADREPA_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"

class MachLogAdministrator;
class MachLogConstructor;
class MachLogResourceCarrier;
class MachLogConstruction;

// orthodox canonical (revoked)

class MachLogAdminRepairOperation
: public MachLogOperation
{
public:

	MachLogAdminRepairOperation( MachLogAdministrator * pActor, MachLogConstruction* );

	virtual ~MachLogAdminRepairOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminRepairOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminRepairOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminRepairOperation( const MachLogAdminRepairOperation& );
    MachLogAdminRepairOperation& operator =( const MachLogAdminRepairOperation& );
    bool operator ==( const MachLogAdminRepairOperation& );

	MachLogAdministrator *					pActor_;
	MachLogConstruction *					pConstruction_;
	
    bool 									complete_;
	bool									currentlyAttached_;
};

PER_DECLARE_PERSISTENT( MachLogAdminRepairOperation );

#endif