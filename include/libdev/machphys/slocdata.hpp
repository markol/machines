/*
 * S L O C D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSpyLocatorData

    Spy locator specific data lives here
*/

#ifndef _MACHPHYS_SLOCDATA_HPP
#define _MACHPHYS_SLOCDATA_HPP

#include "machphys/machdata.hpp"
#include "machphys/machphys.hpp"

class MachPhysSpyLocatorData : public MachPhysMachineData
{
public:
    ~MachPhysSpyLocatorData();

    void CLASS_INVARIANT;

	// Get methods public
	int				mineCapacity() const;
	MachPhys::ResearchUnits	downloadRate() const;

    friend ostream& operator <<( ostream& o, const MachPhysSpyLocatorData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysSpyLocatorData( const MachPhysSpyLocatorData& );

    // Operation deliberately revoked
    MachPhysSpyLocatorData& operator =( const MachPhysSpyLocatorData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSpyLocatorData& );

	// Set methods private
	void mineCapacity( int );
	void downloadRate( MachPhys::ResearchUnits );

    MachPhysSpyLocatorData();
	int					mineCapacity_;
	MachPhys::ResearchUnits		downloadRate_;

};


#endif

/* End SLOCDATA.HPP *************************************************/
