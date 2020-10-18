/*
 * D E V I C E I . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

// static
_CODE_INLINE
RenDevice*& RenIDeviceImpl::current()
{
	static RenDevice* d = NULL;
	return d;
}

// static
_CODE_INLINE
RenIDeviceImpl*& RenIDeviceImpl::currentPimpl()
{
	static RenIDeviceImpl* d = NULL;
	return d;
}

_CODE_INLINE
const glm::mat4& RenIDeviceImpl::projViewMatrix() const
{
	PRE(projViewMatrix_);
	return *projViewMatrix_;
}

_CODE_INLINE
RenIPriorityPostSorter& RenIDeviceImpl::coplanarSorter()
{
	PRE(coplanarSorter_);
	return *coplanarSorter_;
}

_CODE_INLINE
RenIDepthPostSorter& RenIDeviceImpl::alphaSorter()
{
	PRE(alphaSorter_);
	PRE(isAlphaSortingEnabled());
	return *alphaSorter_;
}

_CODE_INLINE
bool RenIDeviceImpl::isAlphaSortingEnabled() const
{
	return alphaSorter_ != NULL;
}

_CODE_INLINE
RenStats* RenIDeviceImpl::statistics()
{
	return stats_;
}

_CODE_INLINE
const RenStats* RenIDeviceImpl::statistics() const
{
	return stats_;
}

_CODE_INLINE
RenIIlluminator* RenIDeviceImpl::illuminator() const
{
	return illuminator_;
}

_CODE_INLINE
RenCamera* RenIDeviceImpl::currentCamera() const
{
	return currentCamera_;
}

_CODE_INLINE
const RenIViewportMapping* RenIDeviceImpl::viewportMapping() const
{
	PRE(vpMapping_);
	return vpMapping_;
}

_CODE_INLINE
bool RenIDeviceImpl::rendering() const
{
	return rendering_;
}

_CODE_INLINE
bool RenIDeviceImpl::rendering2D() const
{
	return rendering2D_;
}

_CODE_INLINE
bool RenIDeviceImpl::rendering3D() const
{
	return rendering3D_;
}

_CODE_INLINE
bool RenIDeviceImpl::idleRendering() const
{
	return rendering() and not rendering3D() and not rendering2D();
}

_CODE_INLINE
const RenCapabilities& RenIDeviceImpl::capabilities() const
{
	PRE(caps_);			// This is checked by the class invariant.
	return *caps_;
}

_CODE_INLINE
RenIDeviceIdentifier::DeviceType RenIDeviceImpl::getDeviceType() const
{
	return deviceType_;
}

_CODE_INLINE
bool RenIDeviceImpl::fogMultiplierIsNeutral() const
{
	return materialFogMultiplier_ == 1.0;
}

_CODE_INLINE
void RenIDeviceImpl::enableAlphaBlending()
{
	glEnable(GL_BLEND);
}

_CODE_INLINE
void RenIDeviceImpl::disableAlphaBlending()
{
	glDisable(GL_BLEND);
}

/* End DEVICEI.IPP **************************************************/
