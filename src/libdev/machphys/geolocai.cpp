/*
 * G E O L O C A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/geolocai.hpp"
#include "machphys/locator.hpp"

PER_DEFINE_PERSISTENT( MachPhysGeoLocatorImpl );

MachPhysGeoLocatorImpl::MachPhysGeoLocatorImpl()
:   isLocating_( false ),
    locatingPlanEndTime_( 0 ),
	pLocator_( NULL ),
	locatorSize_( 0 )
{

    TEST_INVARIANT;
}

MachPhysGeoLocatorImpl::~MachPhysGeoLocatorImpl()
{
	_DELETE( pLocator_ );
    TEST_INVARIANT;
}

void MachPhysGeoLocatorImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGeoLocatorImpl& t )
{

    o << "MachPhysGeoLocatorImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysGeoLocatorImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysGeoLocatorImpl::MachPhysGeoLocatorImpl( PerConstructor )
: isLocating_( false ),
  locatingPlanEndTime_( 0 ),
  pLocator_( NULL ),
  locatorSize_( 0 )
{
}

void perWrite( PerOstream& str, const MachPhysGeoLocatorImpl& geoLocatorImpl )
{
	//str << geoLocatorImpl.pLocator_;
	//str << geoLocatorImpl.locatorSize_;
}

void perRead( PerIstream& str, MachPhysGeoLocatorImpl& geoLocatorImpl)
{
	//str >> geoLocatorImpl.pLocator_;
	//str >> geoLocatorImpl.locatorSize_;
}

/* End GEOLOCAI.CPP *************************************************/
