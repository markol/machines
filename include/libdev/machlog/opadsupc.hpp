/*
 * O P S U P C O N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAdminSuperConstructOperation

    This class takes an actor, a vector of constructions and one of 4 valid operation types,
	and makes that actor perform that operation on each construction in turn, executing
	suboperations in sequence. 
*/

#ifndef _MACHLOG_OPADSUPC_HPP
#define _MACHLOG_OPADSUPC_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "ctl/pvector.hpp"
#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "machlog/operatio.hpp"


class MachLogAdministrator;
class MachLogConstructor;
class MachLogConstruction;

// orthodox canonical (revoked)

class MachLogAdminSuperConstructOperation
: public MachLogOperation
{
	typedef ctl_pvector< MachLogConstruction >	Constructions;
	typedef ctl_pvector< MachLogConstructor >	Constructors;

public:

	MachLogAdminSuperConstructOperation( MachLogAdministrator * pActor, const Constructions& constructionArray, MachLogOperation::OperationType opType );
	virtual ~MachLogAdminSuperConstructOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminSuperConstructOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminSuperConstructOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	
	virtual void doOutputOperator( ostream& ) const;
	virtual PhysRelativeTime doUpdate( );
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminSuperConstructOperation( const MachLogAdminSuperConstructOperation& );
    MachLogAdminSuperConstructOperation& operator =( const MachLogAdminSuperConstructOperation& );
    bool operator ==( const MachLogAdminSuperConstructOperation& );
	
	void addReservationToAllConstructions();
	void cancelReservationFromAllConstructions();
	
	int nConstructorsRemaining()	const;
	
	MachLogAdministrator *						pActor_;
	Constructions								constructionArray_;
	MachLogOperation::OperationType				opType_;
	MachLogConstruction *						pTargetConstruction_;
   	
	PhysAbsoluteTime							lastUpdateTime_;

};

PER_DECLARE_PERSISTENT( MachLogAdminSuperConstructOperation );


#endif

/* End OPCONSTR.HPP *************************************************/
