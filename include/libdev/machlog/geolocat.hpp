/*
 * G E O L O C A T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_GEOLOCATOR_HPP
#define _MACHLOG_GEOLOCATOR_HPP

#include "machlog/machine.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysGeoLocator;
class MachPhysGeoLocatorData;

class MachLogGeoLocator
: public MachLogMachine // ,
//  public MachLogCanLocate
// cannonical form revoked
{
public:

	MachLogGeoLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogGeoLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogGeoLocator();
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysGeoLocatorData& data() const;

	// inherited from MachLogCanLocate
	virtual PhysRelativeTime locate();

	///////////////////////////////

    //Set/get locating mode
    void isLocating( bool doLocate );
    bool isLocating() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogGeoLocator );
	PER_FRIEND_READ_WRITE( MachLogGeoLocator );
	
	friend class MachLogLocateOperation;

private:

	MachLogGeoLocator( const MachLogGeoLocator& );
	MachLogGeoLocator& operator =( const MachLogGeoLocator& );
	bool operator ==( const MachLogGeoLocator& ) const;

    //The physical locator
    MachPhysGeoLocator& physGeoLocator();
    const MachPhysGeoLocator& physGeoLocator() const;


    static MachPhysGeoLocator* pNewPhysGeoLocator( Level hwLevel, Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );

	virtual void doOutputOperator( ostream& o ) const;
};

PER_DECLARE_PERSISTENT( MachLogGeoLocator );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _LOG_LOCATOR_HPP	*/

/* End GEOLOCAT.HPP **************************************************/