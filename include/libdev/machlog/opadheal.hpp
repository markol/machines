/*
 * O P A D A T T A . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 * 
 *	Administrator Heal operation.
 *  This operation will move machines in a convoy centred around the
 *  Administrator and then heal whatever the target is.
 */

#ifndef _MACHLOG_OPADHEAL_HPP
#define _MACHLOG_OPADHEAL_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogAdministrator;
class MachActor;

// orthodox canonical (revoked)

class MachLogAdminHealOperation
: public MachLogOperation
{
public:

	MachLogAdminHealOperation( MachLogAdministrator * pActor, MachActor * pDirectObject );
	virtual ~MachLogAdminHealOperation();
	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminHealOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminHealOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminHealOperation( const MachLogAdminHealOperation& );
    MachLogAdminHealOperation& operator =( const MachLogAdminHealOperation& );
    bool operator ==( const MachLogAdminHealOperation& );

	MachLogAdministrator *	pActor_;
	MachActor		*	pDirectObject_;
    bool 				complete_;
	bool				currentlyAttached_;
//	MexPoint3d			dest_;
};

PER_DECLARE_PERSISTENT( MachLogAdminHealOperation );

/* //////////////////////////////////////////////////////////////// */

#endif