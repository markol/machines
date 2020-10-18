/*
 * D R I V E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/driver.hpp"
#include <algorithm>
#include "render/display.hpp"

RenIDriverInfo::~RenIDriverInfo()
{
    TEST_INVARIANT;
}

bool RenIDriverInfo::hardware() const
{
	return hw_;
}

bool RenIDriverInfo::canRenderToDepth(int bitDepth) const
{
    return true;
}

const std::string& RenIDriverInfo::name() const
{
	return name_;
}

const std::string& RenIDriverInfo::description() const
{
	return description_;
}

// Goodness knows if this function is adequate.  For example, the z-buffer and
// textures often share the same memory on a hardware card.  If there is spare
// memory after all the texture has been loaded, we might be able to afford a
// better z-buffer resolution.
int RenIDriverInfo::zBufferBitDepth() const
{
	// Prefer smallest first, except for 8-bit which would be pathetic.
    /*if (d3dDescr_.dwDeviceZBufferBitDepth & DDBD_16)
        return 16;
    else if (d3dDescr_.dwDeviceZBufferBitDepth & DDBD_24)
        return 24;
    else if (d3dDescr_.dwDeviceZBufferBitDepth & DDBD_32)
        return 32;
    else if (d3dDescr_.dwDeviceZBufferBitDepth & DDBD_8)
        return 8;
    else*/
	    return 0;
}

void RenIDriverInfo::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenIDriverInfo& t )
{
	o << "D3D driver " << t.name() << ((t.hardware())? " (hw)": " (sw)");
    return o;
}

static void destroyInfo(RenIDriverInfo* info)
{
	_DELETE(info);
}

// virtual
RenIDriverSelector::~RenIDriverSelector()
{
	for_each(driverList_.begin(), driverList_.end(), destroyInfo);
}

// virtual
const RenIDriverInfo* RenIDriverSelector::chooseForFullScreen
(
	RenDisplay& display,
	bool ignoreHw
) const
{
	const RenDisplay::Mode& mode = display.currentMode();
	return chooseForDepth(mode.bitDepth(), ignoreHw);
}


inline const RenIDriverInfo* preferRGB(const RenIDriverInfo* current, const RenIDriverInfo* proposed)
{
		return proposed;
}

inline const RenIDriverInfo* preferMono(const RenIDriverInfo* current, const RenIDriverInfo* proposed)
{
		return proposed;
}

const RenIDriverInfo* RenIDriverSelector::chooseForDepth
(
	int bitDepth,
	bool ignoreHw
) const
{
	const RenIDriverInfo* choice = NULL;

	ctl_list<RenIDriverInfo*>::const_iterator it = driverList_.begin();
	while (it != driverList_.end())
	{
		const RenIDriverInfo* driver = *it;

		if (driver->canRenderToDepth(bitDepth))
		{
			if (!choice)
			{
				// There's no previous choice, so go with this driver.
				choice = driver;
			}
			else
			{
				ASSERT(choice, logic_error());

				if (driver->hardware() && !ignoreHw)
				{
					if (!choice->hardware())
					{
						choice = driver;
					}
					else
					{
						// Choose an RGB colour model if possible for hardware.
						choice = preferRGB(choice, driver);
					}
				}
				else	// driver isn't hardware
				{
					if (!choice->hardware())
					{
						// TBD: revert to choosing mono, when D3D works.  For the moment,
						// ramp mode and DrawPrimitive do not work together.
						// Choose a mono ramp colour model if possible for hardware.
						// choice = preferMono(choice, driver);
						choice = preferRGB(choice, driver);
					}
				}
			}
		}
		++it;
	}

	// Assumes that we can always find a driver of some sort.
	// TBD: we should probably handle this elegantly and inform the user that
	// their DirectX isn't installed correctly (or something).
	POST(choice);
	POST(implies(ignoreHw, !choice->hardware()));

	RENDER_STREAM("Chose " << *choice << std::endl);
	return choice;
}

void RenIDriverSelector::addDriver(RenIDriverInfo* driver)
{
	PRE(driver);
	driverList_.push_back(driver);
	RENDER_STREAM("Found " << *driver << std::endl);
}

size_t RenIDriverSelector::nDrivers() const
{
	return driverList_.size();
}


// Enumerate the available D3D drivers and choose one to use.
bool RenIDriverSelector::enumDevices()
{
    return true;
}

/* End DRIVER.CPP ***************************************************/
