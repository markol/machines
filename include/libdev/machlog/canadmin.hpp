/*
 * C A N A D M I N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogCanAdministor

    A brief description of the class should go in here
*/

#ifndef _CANADMIN_HPP
#define _CANADMIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machlog/message.hpp"

//forward erfs
//class MachLogAdministrator;
//class MachLogAggressor;
//class MachLogLocator;
//class MachLogConstructor;
//class MachLogTechnician;
//class MachLogTransporter;
class MachLogMachine;

class MachLogCanAdminister
{
public:
    virtual ~MachLogCanAdminister();

    void CLASS_INVARIANT;
	virtual void moveOutOfTheWay( MachLogMachine * );
	void moveOutOfTheWay( MachLogMachine *, int assemblyPoint );

    friend ostream& operator <<( ostream& o, const MachLogCanAdminister& t );

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogCanAdminister );
	PER_FRIEND_READ_WRITE( MachLogCanAdminister );

	

protected:

    MachLogCanAdminister( MachPhys::Race race );

	virtual void handleIdleGeoLocator( MachLogCommsId );
	virtual void handleIdleSpyLocator( MachLogCommsId );
	virtual void handleIdleTechnician( MachLogCommsId ) = 0 ;
	virtual void handleIdleConstructor( MachLogCommsId );
	virtual void handleIdleAggressor( MachLogCommsId );
	virtual void handleIdleAdministrator( MachLogCommsId );
	virtual void handleIdleAPC( MachLogCommsId );
	virtual void handleIdleResourceCarrier( MachLogCommsId );

private:
//	MachPhys::Race race() const { return race_; };
    // Operation deliberately revoked
    MachLogCanAdminister( const MachLogCanAdminister& );
    MachLogCanAdminister& operator =( const MachLogCanAdminister& );

    bool operator ==( const MachLogCanAdminister& );

	MachPhys::Race	canAdminRace_;

};

PER_DECLARE_PERSISTENT( MachLogCanAdminister );


#endif

/* End CANADMIN.HPP *************************************************/
