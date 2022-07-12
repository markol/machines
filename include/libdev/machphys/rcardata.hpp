/*
 * R C A R D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysResourceCarrierData

    Resource specific default data class.
*/

#ifndef _MACHPHYS_RCARDATA_HPP
#define _MACHPHYS_RCARDATA_HPP

#include "machphys/machdata.hpp"

class MachPhysResourceCarrierData : public MachPhysMachineData
{
public:
    ~MachPhysResourceCarrierData();

	//Get methods public
	MachPhys::SizeUnits capacity() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysResourceCarrierData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysResourceCarrierData( const MachPhysResourceCarrierData& );

    // Operation deliberately revoked
    MachPhysResourceCarrierData& operator =( const MachPhysResourceCarrierData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysResourceCarrierData& );

    MachPhysResourceCarrierData();

	//set methods private
	void capacity( const MachPhys::SizeUnits& );

	MachPhys::SizeUnits		capacity_;
};


#endif

/* End RCARDATA.HPP *************************************************/
