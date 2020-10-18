/*
 * D E V I C E I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "ctl/countptr.hpp"
#include "render/internal/devicei.hpp"

#include "render/device.hpp"
#include "render/display.hpp"
#include "render/texture.hpp"
#include "render/surfmgr.hpp"
#include "render/drivsel.hpp"
#include "render/material.hpp"

#include "system/pathname.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/displayi.hpp"
#include "render/internal/surfmgri.hpp"
#include "render/internal/matbody.hpp"

#ifndef _INLINE
    #include "render/internal/devicei.ipp"
#endif

RenIDeviceImpl::RenIDeviceImpl(RenDisplay* dis, RenDevice* parent):
	RenIDisplayModeObserver(dis),
	parent_(parent),
	alphaBlendingEnabled_(false),
	driverSelector_( NULL ),
	materialFogMultiplier_( 1.0 ),
	debugX_(0), debugY_(0),
	frameTimer_(),
	deviceType_(RenIDeviceIdentifier::UNKNOWN),
	videoMemoryShared_(false),
	videoMemorySharedInitialized_(false)

{
	PRE(dis);
	PRE(parent);
	frameTimer_.pause();
	frameTimer_.time(0);
	driverSelector_ = _NEW( RenDriverSelector( dis ) );

}

RenIDeviceImpl::~RenIDeviceImpl()
{
	_DELETE( driverSelector_ );
}

// virtual
void RenIDeviceImpl::prepareForModeChange
(
	const RenDisplay::Mode&,
	const RenDisplay::Mode&
)
{
	// Ensure that any front and back surfaces release their COM ptrs.
	const RenIDisplay& dis = display_->displayImpl();

	RENDER_STREAM("Releasing D3D stuff owned by RenDevice.\n");
    _DELETE(surfBackBuf_);
	surfBackBuf_ = NULL;
    _DELETE(surfFrontBuf_);
	surfFrontBuf_ = NULL;

	RENDER_STREAM("...done.\n\n");
}

// virtual
bool RenIDeviceImpl::modeChanged(const RenDisplay::Mode&)
{
	RENDER_STREAM("Reinitialising RenDevice after mode change.\n");
	return parent_->fitToDisplay(display_);
}

// static
void RenIDeviceImpl::useDevice(RenDevice* newDev, RenI::UpdateType updateType)
{
	PRE(newDev);

	current() = newDev;
	currentPimpl() = &(newDev->impl());

	RenIMatManager::instance().useDevice(*newDev);

	const RenIDisplay& dis = newDev->display()->displayImpl();
	RenSurfaceManager::instance().impl().useDevice(newDev, updateType);
}

bool RenIDeviceImpl::hasSharedVideoMemory() const
{
	PRE( videoMemorySharedInitialized_ );
	return videoMemoryShared_;
}

void RenIDeviceImpl::hasSharedVideoMemory( bool setVideoMemoryShared )
{
	videoMemoryShared_ = setVideoMemoryShared;
	videoMemorySharedInitialized_ = true;
}

void RenIDeviceImpl::resetFrameTimer()
{
	frameTimer_.time(0);
	frameTimer_.resume();
}

double RenIDeviceImpl::frameTime() const
{
	return frameTimer_.time();
}

void RenIDeviceImpl::setMaterialHandles
(
	const RenMaterial& mat
)
{
	if( fogOn_ )
		updateFogMultiplier( mat );

}

void RenIDeviceImpl::updateFogMultiplier( const RenMaterial& mat )
{
	PRE( fogOn_ );

	const RenIMatBody& body = *mat.body_;

	if( body.hasFogMultiplier() )
	{
		if( body.fogMultiplier() != materialFogMultiplier_ )
		{
			materialFogMultiplier_ = body.fogMultiplier();
			parent_->fogEnd( fogEnd_ * materialFogMultiplier_ );
		}
	}
	else if ( not fogMultiplierIsNeutral() )
	{
		materialFogMultiplier_ = 1.0;
		parent_->fogEnd( fogEnd_ );
	}
}

/* End DEVICEI.CPP ***************************************************/
