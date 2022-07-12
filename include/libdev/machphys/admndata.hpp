/*
 * A D M N D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysAdministratorData

    Administrator specific default data
*/

#ifndef _ADMNDATA_HPP
#define _ADMNDATA_HPP

#include "machphys/machdata.hpp"

class MachPhysAdministratorData : public MachPhysMachineData
{
public:
    ~MachPhysAdministratorData();

	MATHEX_SCALAR accuracy() const;
	MATHEX_SCALAR nWeapons() const;
	MATHEX_SCALAR maxSubordinates() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysAdministratorData& t );

private:
	friend class MachPhysDataParser;

    // Operations deliberately revoked
    MachPhysAdministratorData( const MachPhysAdministratorData& );
    MachPhysAdministratorData& operator =( const MachPhysAdministratorData& );
    bool operator ==( const MachPhysAdministratorData& );

    MachPhysAdministratorData();

	//Set methods private
	void accuracy( const MATHEX_SCALAR& );
	void nWeapons( const MATHEX_SCALAR& );
	void maxSubordinates( const MATHEX_SCALAR& );

	MATHEX_SCALAR	accuracy_;
	MATHEX_SCALAR	nWeapons_;
	MATHEX_SCALAR	maxSubordinates_;

};


#endif

/* End ADMNDATA.HPP *************************************************/
