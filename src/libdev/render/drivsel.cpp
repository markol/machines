/*
 * D R I V S E L . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "ctl/list.hpp"
#include "render/drivsel.hpp"
#include "render/driv.hpp"
#include "render/internal/drivi.hpp"

#include "render/display.hpp"

class RenDriverSelectorImpl
{
public:
    RenDriverSelectorImpl( const RenDisplay* const display );
	~RenDriverSelectorImpl();

private:
	friend class RenDriverSelector;

	void eraseD3dDrivers();
	void eraseNonAutomaticD3dDrivers();

	enum PushWhere { BACK, FRONT };

	void addDDrawDriver( const RenDriverPtr& driver, PushWhere pushWhere = BACK );
	void addD3dDriver( const RenDriverPtr& driver, PushWhere pushWhere = BACK );

	static RenDriverPtr createAutomaticDriver(const RenDriverSelector::RenDrivers& );


	const RenDisplay* const			display_;

	RenDriverSelector::RenDrivers 	dDrawDrivers_;
	RenDriverPtr					currentDDrawDriver_;
	RenDriverPtr					automaticDDrawDriver_;

	RenDriverSelector::RenDrivers 	d3dDrivers_;
	RenDriverPtr					currentD3dDriver_;
	RenDriverPtr					automaticD3dDriver_;
};


RenDriverSelectorImpl::RenDriverSelectorImpl( const RenDisplay* const display )
: display_( display ),
  currentDDrawDriver_( NULL ),
  automaticDDrawDriver_( NULL ),
  currentD3dDriver_( NULL ),
  automaticD3dDriver_( NULL )
{
}

RenDriverSelectorImpl::~RenDriverSelectorImpl()
{
}

// private
// static
RenDriverPtr RenDriverSelectorImpl::createAutomaticDriver( const RenDriverSelector::RenDrivers& fromList )
{
	RenDriverPtr bestDriverInList = NULL;

	RenDriverSelector::RenDrivers::const_iterator it;
	for( it=fromList.begin(); it != fromList.end(); ++it )
	{
		//if( (bestDriverInList == NULL) or (*it)->isBetterChoiceThan( *bestDriverInList ) )
		if( (bestDriverInList == RenDriverPtr(NULL) ) or (*it)->isBetterChoiceThan( *bestDriverInList ) )
			bestDriverInList = (*it);
	}
	ASSERT( bestDriverInList.isDefined(), " Could not find a acceptable driver in list ");

	// create a new dummy entry for the list
	RenDriver* automaticDriver = _NEW( RenDriver( *bestDriverInList ) );
	automaticDriver->driverImpl().name( "Automatic" );
	automaticDriver->driverImpl().isAutomatic( true );
	RenDriverPtr automaticDriverPtr( automaticDriver );

	return automaticDriverPtr;
}

void RenDriverSelectorImpl::eraseD3dDrivers()
{
	d3dDrivers_.erase( d3dDrivers_.begin(), d3dDrivers_.end() );
}



void RenDriverSelectorImpl::eraseNonAutomaticD3dDrivers()
{
	// only gonna work with lists...
	RenDriverSelector::RenDrivers::iterator it = d3dDrivers_.begin();
	RenDriverSelector::RenDrivers::iterator nextIt;
	while( it != d3dDrivers_.end() )
	{
		nextIt = it;
		++nextIt;
		if( not (*it)->driverImpl().isAutomatic() )
			d3dDrivers_.erase( it );
		it = nextIt;
	}
}

void RenDriverSelectorImpl::addDDrawDriver( const RenDriverPtr& driverPtr, PushWhere pushWhere )
{
	switch( pushWhere )
	{
		case FRONT:
			dDrawDrivers_.push_front( driverPtr );
			break;
		case BACK:
			dDrawDrivers_.push_back( driverPtr );
		default:
			break;
	}
}

void RenDriverSelectorImpl::addD3dDriver( const RenDriverPtr& driverPtr, PushWhere pushWhere )
{
	switch( pushWhere )
	{
		case FRONT:
			d3dDrivers_.push_front( driverPtr );
			break;
		case BACK:
			d3dDrivers_.push_back( driverPtr );
		default:
			break;
	}
}

//
// class RenDriverSelector
//

#define CB_RenDriverSelector_DEPIMPL() \
	CB_DEPIMPL( const RenDisplay* const,	  	display_ ); \
	CB_DEPIMPL( RenDriverSelector::RenDrivers, 	dDrawDrivers_ ); \
	CB_DEPIMPL( RenDriverPtr,					currentDDrawDriver_ ); \
	CB_DEPIMPL( RenDriverPtr,					automaticDDrawDriver_ ); \
	CB_DEPIMPL( RenDriverSelector::RenDrivers, 	d3dDrivers_ ); \
	CB_DEPIMPL( RenDriverPtr,					currentD3dDriver_ ); \
	CB_DEPIMPL( RenDriverPtr,					automaticD3dDriver_ );


RenDriverSelector::RenDriverSelector( const RenDisplay* const display )
: pImpl_( _NEW( RenDriverSelectorImpl( display ) ) )
{
	CB_RenDriverSelector_DEPIMPL();
	RENDER_STREAM( std::endl );
	RENDER_STREAM( "Building list of DirectDraw drivers " << std::endl);
	buildDDrawDrivers();
	// build the list of direct 3d driver for the current direct draw driver
	if( display_ )
	{
		RENDER_STREAM( std::endl );
	}
}

RenDriverSelector::RenDriverSelector( )
: pImpl_( _NEW( RenDriverSelectorImpl( NULL ) ) )
{
	CB_RenDriverSelector_DEPIMPL();
	RENDER_STREAM( std::endl );
	RENDER_STREAM( "Building list of DirectDraw drivers " << std::endl);
	buildDDrawDrivers();

}

RenDriverSelectorImpl& RenDriverSelector::driverSelectorImpl()
{
	PRE( pImpl_ );
	return *pImpl_;
}

const RenDriverSelectorImpl& RenDriverSelector::driverSelectorImpl() const
{
	PRE( pImpl_ );
	return *pImpl_;
}

// private
bool RenDriverSelector::enumDDrawDrivers()
{
		return false;
}

void RenDriverSelector::buildDDrawDrivers()
{
	RENDER_STREAM( "Driver selector does nothing" << std::endl);
    TEST_INVARIANT;
}

bool RenDriverSelector::enumD3dDrivers()
{
	CB_RenDriverSelector_DEPIMPL();

	return true;
}

void RenDriverSelector::buildD3dDrivers()
{
	CB_RenDriverSelector_DEPIMPL();

	PRE( display_ );

	RENDER_INDENT(2);

	driverSelectorImpl().eraseD3dDrivers();

	ASSERT( d3dDrivers_.size() == 0, "list of d3d drivers non zero" );

	currentD3dDriver_ = NULL;
	automaticD3dDriver_ = NULL;

	// enum all the (hardware) compatible devices
	enumD3dDrivers();

	// Get the best driver from the list andf copy it in a new entry of the list
	RenDriverPtr bestDriver = RenDriverSelectorImpl::createAutomaticDriver( d3dDrivers_ );
	driverSelectorImpl().addD3dDriver( bestDriver, RenDriverSelectorImpl::FRONT );

	automaticD3dDriver_ = bestDriver;

	RENDER_STREAM( "Automatic (best) Direct3d Driver (" << *automaticD3dDriver_ << ")" << std::endl);
	// if the direct draw driver is automatic we do not want the list of possible driver for d3d to be displayed
	// thus dessimate the non automatic d3d drivers
	if( currentDDrawDriver_->driverImpl().isAutomatic() )
		driverSelectorImpl().eraseNonAutomaticD3dDrivers();

	ASSERT( automaticD3dDriver_.isDefined(), "Unable to find an appropriate D3D driver " );

	bool isAutomatic;
	if( not isAutomatic )
	{
		RENDER_STREAM( "The user has requested not to use the automatic driver " << std::endl);

		if(	currentD3dDriver_.isDefined() )
			RENDER_STREAM("Requested Direct3d Driver found (" << currentD3dDriver_->name() << ", " << currentD3dDriver_->description() << ")" << std::endl);
		else
			RENDER_STREAM("Requested Direct3d Driver not found (automatic driver will be used) " << std::endl);
	}

	if( not currentD3dDriver_.isDefined() )
		currentD3dDriver_ = automaticD3dDriver_;

	RENDER_STREAM( "Current Direct3d Driver (" << currentD3dDriver_->name() << ", " << currentD3dDriver_->description() << ")" << std::endl);

	RENDER_INDENT(-2);

	POST( currentD3dDriver_.isDefined() );
    TEST_INVARIANT;
}

const RenDriverSelector::RenDrivers& RenDriverSelector::dDrawDrivers() const
{
	CB_RenDriverSelector_DEPIMPL();
	return dDrawDrivers_;
}

const RenDriverSelector::RenDrivers& RenDriverSelector::d3dDrivers() const
{
	CB_RenDriverSelector_DEPIMPL();
	ASSERT( display_, "Need a display to handle d3d drivers" );
	return d3dDrivers_;
}

const RenDriverPtr& RenDriverSelector::currentDDrawDriver() const
{
	CB_RenDriverSelector_DEPIMPL();
	return currentDDrawDriver_;
}

const RenDriverPtr& RenDriverSelector::currentD3dDriver() const
{
	CB_RenDriverSelector_DEPIMPL();
	ASSERT( display_, "Needz a display to handle d3d drivers" );
	return currentD3dDriver_;
}

RenDriverSelector::ReturnValue RenDriverSelector::useDDrawDriver( const RenDriverPtr& driver )
{

	CB_RenDriverSelector_DEPIMPL();

	ReturnValue result = FAILED;
	//look for the driver in the list
	RenDrivers::const_iterator it;
	for( it = dDrawDrivers_.begin(); it != dDrawDrivers_.end() and result == FAILED; ++it )
	{
		if( (*it) == driver )
		{
			currentDDrawDriver_	= driver;
			result = SUCCEEDED;
		}
	}

	// update the list of available d3d drivers
	if( result == SUCCEEDED )
	{
		RENDER_STREAM( std::endl );
		RENDER_STREAM( "New current Directdraw driver (" << *(currentDDrawDriver()) << ")" << std::endl);
		if( display_ )
		{
			RENDER_STREAM( "Rebuilding list of Direct3d drivers for direct draw driver (" << currentDDrawDriver_->name() << ", " << currentDDrawDriver_->description() << ")"  << std::endl);
			buildD3dDrivers();
		}
	}
	else
	{
		RENDER_STREAM( "Failed to find DirectDraw driver (" << driver->name() << ", " << driver->description() << ")" << std::endl);
	}

	return result;
}

RenDriverSelector::ReturnValue RenDriverSelector::useD3dDriver( const RenDriverPtr& driver )
{
	CB_RenDriverSelector_DEPIMPL();

	ASSERT( display_, "Need a display to handle d3d drivers" );

	ReturnValue result = FAILED;
	//look for the driver in the list
	RenDrivers::const_iterator it;
	for( it = d3dDrivers_.begin(); it != d3dDrivers_.end() and result == FAILED; ++it )
	{
		if( (*it) == driver )
		{
			currentD3dDriver_ = driver;
			result = SUCCEEDED;
		}
	}
	if(  result == SUCCEEDED )
	{
		RENDER_STREAM( "New current Direct3d driver (" << currentD3dDriver_->name() << ", " << currentD3dDriver_->description() << ")" << std::endl);
	}
	else
	{
		RENDER_STREAM( "Failed to find Direct3d driver (" << driver->name() << ", " << driver->description() << ") in list of drivers " << std::endl);
	}

	return result;
}

bool RenDriverSelector::currentDDrawDriverIsAutomatic()	const
{
	CB_RenDriverSelector_DEPIMPL();
	return currentDDrawDriver_ == automaticDDrawDriver_;
}

bool RenDriverSelector::currentD3dDriverIsAutomatic() const
{
	CB_RenDriverSelector_DEPIMPL();
	ASSERT( display_, "Need a display to handle d3d drivers" );
	return currentD3dDriver_ == automaticD3dDriver_;
}

const RenDisplay& RenDriverSelector::display() const
{
	CB_RenDriverSelector_DEPIMPL();
	PRE( display_ );
	return *pImpl_->display_;
}

RenDriverSelector::ReturnValue RenDriverSelector::updateDriverRegistries()
{
	CB_RenDriverSelector_DEPIMPL();
	PRE( currentDDrawDriver().isDefined() );

	currentDDrawDriver()->driverImpl().writeToRegistry();
	if( display_ )
		currentD3dDriver()->driverImpl().writeToRegistry();

	return SUCCEEDED;
}


RenDriverSelector::~RenDriverSelector()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void RenDriverSelector::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void RenDriverSelector::streamOut( ostream& o )	const
{
	CB_RenDriverSelector_DEPIMPL();
	RenDrivers::const_iterator it;

	o << std::endl;
	o << "DirectDraw drivers:" << std::endl;
	for( it = dDrawDrivers_.begin(); it != dDrawDrivers_.end(); ++it )
	{
		o << " (" << (*it)->name() << ", " << (*it)->description() << ")" << std::endl;
	}
	o << "Automatic DirectDraw driver " << " (" << automaticDDrawDriver_->name() << ", " << automaticDDrawDriver_->description() << ")" << std::endl;
	o << "Current DirectDraw driver "   << " (" << currentDDrawDriver_->name() << ", " << currentDDrawDriver_->description() << ")" << std::endl;

	if( display_ )
	{
		o << "Direct3d drivers:" << std::endl;
		for( it = d3dDrivers_.begin(); it != d3dDrivers_.end(); ++it )
		{
			o << " (" << (*it)->name() << ", " << (*it)->description() << ")" << std::endl;
		}
		o << "Automatic Direct3d driver " << " (" << automaticD3dDriver_->name() << ", " << automaticD3dDriver_->description() << ")" << std::endl;
		o << "Current Direct3d driver "   << " (" << currentD3dDriver_->name() << ", " << currentD3dDriver_->description() << ")" << std::endl;
	}
}

ostream& operator <<( ostream& o, const RenDriverSelector& t )
{
	t.streamOut ( o );
    return o;
}

/* End DRIVSEL.CPP **************************************************/
