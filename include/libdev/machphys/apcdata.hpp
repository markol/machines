/*
 * A P C D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysAPCData

    APC specific default data class.
*/

#ifndef _MACHPHYS_APCDATA_HPP
#define _MACHPHYS_APCDATA_HPP

#include "machphys/machdata.hpp"

class MachPhysAPCData : public MachPhysMachineData
{
public:
    ~MachPhysAPCData();

	//Get methods public
	MachPhys::SizeUnits capacity() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysAPCData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysAPCData( const MachPhysAPCData& );

    // Operation deliberately revoked
    MachPhysAPCData& operator =( const MachPhysAPCData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysAPCData& );

    MachPhysAPCData();

	//set methods private
	void capacity( const MachPhys::SizeUnits& );

	MachPhys::SizeUnits		capacity_;
};


#endif

/* End PCARDATA.HPP *************************************************/
