/*
 * G E O L O C A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGeoLocatorImpl

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_GEOLOCAI_HPP
#define _MACHPHYS_GEOLOCAI_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"
#include "phys/phys.hpp"
#include "world4d/compplan.hpp"

class MachPhysLocator;

class MachPhysGeoLocatorImpl
// Canonical form revoked
{
public:
    MachPhysGeoLocatorImpl();

    ~MachPhysGeoLocatorImpl();

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysGeoLocatorImpl );
    PER_FRIEND_READ_WRITE( MachPhysGeoLocatorImpl );

private:
    friend class MachPhysGeoLocator;
    friend ostream& operator <<( ostream& o, const MachPhysGeoLocatorImpl& t );

    MachPhysGeoLocatorImpl( const MachPhysGeoLocatorImpl& );
    MachPhysGeoLocatorImpl& operator =( const MachPhysGeoLocatorImpl& );

    //Data members
    W4dCompositePlanPtr locatingPlanPtr_; //Animation for locating
    PhysAbsoluteTime locatingPlanEndTime_; //Time at which any queued locating animation ends
    bool isLocating_; //True if in locating mode
	MachPhysLocator* pLocator_;
	MATHEX_SCALAR 	 locatorSize_;
};

PER_DECLARE_PERSISTENT( MachPhysGeoLocatorImpl );


#endif

/* End GEOLOCAI.HPP *************************************************/
