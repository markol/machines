/*
 * O P A D D C O N . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_OPADDECO_HPP
#define _MACHLOG_OPADDECO_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"

class MachLogAdministrator;
class MachLogConstructor;
class MachLogResourceCarrier;
class MachLogConstruction;

// orthodox canonical (revoked)

class MachLogAdminDeconstructOperation
: public MachLogOperation
{
public:

	MachLogAdminDeconstructOperation( MachLogAdministrator * pActor, MachLogConstruction* );

	virtual ~MachLogAdminDeconstructOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminDeconstructOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminDeconstructOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminDeconstructOperation( const MachLogAdminDeconstructOperation& );
    MachLogAdminDeconstructOperation& operator =( const MachLogAdminDeconstructOperation& );
    bool operator ==( const MachLogAdminDeconstructOperation& );

	MachLogAdministrator *					pActor_;
	MachLogConstruction *					pConstruction_;
	
    bool 									complete_;
	bool									currentlyAttached_;
};

PER_DECLARE_PERSISTENT( MachLogAdminDeconstructOperation );

#endif