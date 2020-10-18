/*
 * M S E M D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMissileEmplacementData

    Contains specific data for Missile MissileEmplacements.
*/

#ifndef _MISSILEEMPLACEMENTDATA_HPP
#define _MISSILEEMPLACEMENTDATA_HPP

#include "machphys/consdata.hpp"
#include "mathex/angle.hpp"

class MexTransform3d;

class MachPhysMissileEmplacementData : public MachPhysConstructionData
{
public:
    MachPhysMissileEmplacementData( void );
    MachPhysMissileEmplacementData( const MachPhysMissileEmplacementData&, const MexTransform3d& );
    ~MachPhysMissileEmplacementData( void );

	//Get methods public
	MATHEX_SCALAR nWeapons( void ) const;
	MATHEX_SCALAR accuracy( void ) const;
	MexRadians maxRotation( void ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMissileEmplacementData& t );

private:
	friend class MachPhysDataParser;
    // Operation deliberately revoked
    MachPhysMissileEmplacementData( const MachPhysMissileEmplacementData& );

    // Operation deliberately revoked
    MachPhysMissileEmplacementData& operator =( const MachPhysMissileEmplacementData& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMissileEmplacementData& );

	//Set methods public
	void nWeapons( const MATHEX_SCALAR& );
	void accuracy( const MATHEX_SCALAR& );
	void maxRotation( const MexRadians& );

	MATHEX_SCALAR			nWeapons_;
	MATHEX_SCALAR			accuracy_;
	MexRadians				maxRotation_;


};


#endif

/* End TURRDATA.HPP *************************************************/
