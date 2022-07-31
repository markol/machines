/*
 * O P D E C O N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogDeconstructOperation

    This operation is used by constructors to capture enemy buildings.
	The building can be captured if it is in the red zone of health.
*/

#ifndef _MACHLOG_OPDECONS_HPP
#define _MACHLOG_OPDECONS_HPP

#include "machlog/oplabour.hpp"

class MachLogConstructor;
class MachLogConstruction;

// orthodox canonical (revoked)

class MachLogDeconstructOperation
: public MachLogLabourOperation
{
public:

	MachLogDeconstructOperation( MachLogConstructor * pActor, MachLogConstruction * );
	virtual ~MachLogDeconstructOperation();
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogDeconstructOperation );
	PER_FRIEND_READ_WRITE( MachLogDeconstructOperation );

protected:
	
	virtual bool doIsFinished() const;
	
	virtual void doOutputOperator( ostream& ) const;
	
	// inherited from MLLabourOperation
	virtual bool clientSpecificNotification( int clientData );

private:

	// Operations deliberately revoked
    MachLogDeconstructOperation( const MachLogDeconstructOperation& );
    MachLogDeconstructOperation& operator =( const MachLogDeconstructOperation& );
    bool operator ==( const MachLogDeconstructOperation& );
	
	// inherited from MachLogLabourOperation
	virtual PhysRelativeTime interactWithBuilding();
};

PER_DECLARE_PERSISTENT( MachLogDeconstructOperation );


#endif

/* End OPDECONS.HPP *************************************************/
