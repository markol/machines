/*
 * O P P I C K U P . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogPickupOperation

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_OPPICKUP_HPP
#define _MACHLOG_OPPICKUP_HPP

#include "base/base.hpp"

#include "machlog/operatio.hpp"


class MachLogResourceCarrier;
class MachLogPickUpOperationImpl;

// canonical form revoked

class MachLogPickUpOperation
: public MachLogOperation
{
public:
	
    MachLogPickUpOperation( MachLogResourceCarrier* );
	// PRE( pActor_->isNormalResourceCarrier() );	
	
    ~MachLogPickUpOperation();
							 
	void CLASS_INVARIANT;							 
							 
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPickUpOperation );
	PER_FRIEND_READ_WRITE( MachLogPickUpOperation );


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
    MachLogPickUpOperation( const MachLogPickUpOperation& );
    MachLogPickUpOperation& operator =( const MachLogPickUpOperation& );
    bool operator ==( const MachLogPickUpOperation& );

	bool moveToNextSupplier();	
	
	void terminateOp();
	
	// will perform a find space and change *pClearPickupPoint if necessary to that of a position near its original
	// value that is guaranteed to be both within pickup range of the pad and clear of all obstacles for the clearance
	// designated
	bool attemptToGuaranteeClearPickupPoint( const MexPoint2d& actorPos, MATHEX_SCALAR carrierClearance, MexPoint2d* pClearPickupPoint );
	
	MachLogPickUpOperationImpl*		pImpl_;
	
	
};

PER_DECLARE_PERSISTENT( MachLogPickUpOperation );


#endif

/* End OPPICKUP.HPP *************************************************/
