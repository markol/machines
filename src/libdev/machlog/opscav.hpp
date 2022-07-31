/*
 * O P S C A V . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogScavengeOperation

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_OPSCAV_HPP
#define _MACHLOG_OPSCAV_HPP

#include "base/base.hpp"

#include "machlog/operatio.hpp"


class MachLogDebris;
class MachLogResourceCarrier;
class MachLogScavengeOperationImpl;
template < class X > class ctl_pvector;

// canonical form revoked

class MachLogScavengeOperation
: public MachLogOperation
{
public:

	typedef ctl_pvector< MachLogDebris >	Suppliers;
	
	// PRE( targetActor != NULL );
    MachLogScavengeOperation( MachLogResourceCarrier* , MachLogDebris* );
	
	// PRE( listOfSuppliers.size() > 0 );
    MachLogScavengeOperation( MachLogResourceCarrier* , const Suppliers& );
	
    ~MachLogScavengeOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
							 
	void CLASS_INVARIANT;							 
							 
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogScavengeOperation );
	PER_FRIEND_READ_WRITE( MachLogScavengeOperation );


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
	
private:

	// Operations deliberately revoked
    MachLogScavengeOperation( const MachLogScavengeOperation& );
    MachLogScavengeOperation& operator =( const MachLogScavengeOperation& );
    bool operator ==( const MachLogScavengeOperation& );
	
	Suppliers& suppliers();

	MachLogScavengeOperationImpl*		pImpl_;
	
	
};

PER_DECLARE_PERSISTENT( MachLogScavengeOperation );


#endif

/* End OPPICKUP.HPP *************************************************/
