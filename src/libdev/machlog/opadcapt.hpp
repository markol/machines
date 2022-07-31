/*
 * O P A D C A P T . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_OPADCAPT_HPP
#define _MACHLOG_OPADCAPT_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"

class MachLogAdministrator;
class MachLogConstructor;
class MachLogResourceCarrier;
class MachLogConstruction;

// orthodox canonical revoked

class MachLogAdminCaptureOperation
: public MachLogOperation
{
public:

	MachLogAdminCaptureOperation( MachLogAdministrator * pActor, MachLogConstruction* );

	virtual ~MachLogAdminCaptureOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminCaptureOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminCaptureOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminCaptureOperation( const MachLogAdminCaptureOperation& );
    MachLogAdminCaptureOperation& operator =( const MachLogAdminCaptureOperation& );
    bool operator ==( const MachLogAdminCaptureOperation& );


	MachLogAdministrator *					pActor_;
	MachLogConstruction *					pConstruction_;
	
    bool 									complete_;
	bool									currentlyAttached_;
};

PER_DECLARE_PERSISTENT( MachLogAdminCaptureOperation );

#endif