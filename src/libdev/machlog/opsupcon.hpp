/*
 * O P S U P C O N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogSuperConstructOperation

    This class takes an actor, a vector of constructions and one of 4 valid operation types,
	and makes that actor perform that operation on each construction in turn, executing
	suboperations in sequence. 
*/

#ifndef _MACHLOG_OPSUPCON_HPP
#define _MACHLOG_OPSUPCON_HPP

#include "base/base.hpp"

#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"

#include "phys/phys.hpp"

#include "machlog/constron.hpp"
#include "machlog/operatio.hpp"



class MachLogConstructor;
class MachLogConstruction;

// canonical form revoked

class MachLogSuperConstructOperation
: public MachLogOperation
{
	typedef ctl_pvector< MachLogConstruction >	Constructions;
	typedef ctl_pvector< MachLogConstructor >	Constructors;

public:

	MachLogSuperConstructOperation( MachLogConstructor * pActor, const Constructions& constructionArray, MachLogOperation::OperationType opType );
	virtual ~MachLogSuperConstructOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSuperConstructOperation );
	PER_FRIEND_READ_WRITE( MachLogSuperConstructOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	
	virtual void doOutputOperator( ostream& ) const;
	virtual PhysRelativeTime doUpdate( );
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogSuperConstructOperation( const MachLogSuperConstructOperation& );
    MachLogSuperConstructOperation& operator =( const MachLogSuperConstructOperation& );
    bool operator ==( const MachLogSuperConstructOperation& );
	
	// void addConstructorToAllConstructions();
	// void removeConstructorFromAllConstructions();
	
	void addReservationToAllConstructions();
	void cancelReservationFromAllConstructions();

	
	MachLogConstructor *						pActor_;
	Constructions								constructionArray_;
	MachLogOperation::OperationType				opType_;
	MachLogConstruction *						pTargetConstruction_;
   	
	PhysAbsoluteTime							lastUpdateTime_;
	
	bool										doingFirstOp_;

};

PER_DECLARE_PERSISTENT( MachLogSuperConstructOperation );


#endif

/* End OPCONSTR.HPP *************************************************/
