/*
 * C O N S T R U C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_CONSTRUC_HPP
#define _MACHLOG_CONSTRUC_HPP

#include "machlog/machine.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysConstructor;
class MachPhysConstructorData;
class MachLogConstruction;

class MachLogConstructor
: public MachLogMachine // ,
//  public MachLogCanConstruct,
//  public MachLogCanPickUp
// cannonical form revoked
{
public:

	MachLogConstructor( const MachPhys::ConstructorSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogConstructor( const MachPhys::ConstructorSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogConstructor();

	///////////////////////////////
	
	void constructing( bool, UtlId constructionId = 0 );
	bool constructing() const;
	
	// useful for evade logic to know
	bool constructingMissileEmplacement() const;
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysConstructorData& data() const;
	
	// inherited from MachLogCanPickUp
	virtual PhysRelativeTime pickUp();
	
	void isDoingSuperConstruct( bool );
	bool isDoingSuperConstruct() const;

	///////////////////////////////

	const MachPhysConstructor& physConstructor() const;
	const MachPhys::ConstructorSubType& subType() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogConstructor );
	PER_FRIEND_READ_WRITE( MachLogConstructor );

private:

	MachLogConstructor( const MachLogConstructor& );
	MachLogConstructor& operator =( const MachLogConstructor& );
	bool operator ==( const MachLogConstructor& ) const;

	MachPhysConstructor& physConstructor();

    static MachPhysConstructor* pNewPhysConstructor( const MachPhys::ConstructorSubType& subType, Level hwLevel, Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );

	virtual void doOutputOperator( ostream& o ) const;
	
	//------------------------------data members-------------------------

	MachPhys::ConstructorSubType subType_;
		
	bool isConstructing_;		// actually engaged in construction, rather than merely en route to a construction site etc.
	
	bool isDoingSuperConstruct_;	// if true, means it has at least one more construction to deal with after the
									// current op its doing
};

PER_DECLARE_PERSISTENT( MachLogConstructor );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_CONSTRUC_HPP	*/

/* End CONSTRUC.HPP *************************************************/