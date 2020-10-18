/*
 * D R I V E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_DRIVER_HPP
#define _REN_DRIVER_HPP

#include "base/base.hpp"
#include <string>
#include "ctl/list.hpp"

class RenDisplay;

// A class wrapper for Direct3D driver info.  Objects of this type will
// be created by an EnumDevices callback.
class RenIDriverInfo
{
public:
    ~RenIDriverInfo();

	// Drivers can render to more than one bit depth.  Perhaps it
	// would be more convenient to return a list of depths.
	bool canRenderToDepth(int bitDepth) const;

	// Various constant properties.
	bool					hardware() const;
	const std::string& 			name() const;
	const std::string& 			description() const;
	int						zBufferBitDepth() const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const RenIDriverInfo& t );

private:
	const std::string		name_, description_;
	const bool 			hw_;

    // Operations deliberately revoked
    RenIDriverInfo( const RenIDriverInfo& );
    RenIDriverInfo& operator =( const RenIDriverInfo& );
    bool operator ==( const RenIDriverInfo& );
};

// A list of drivers with a selection algorithm incorporated.
class RenIDriverSelector
{
public:
	virtual ~RenIDriverSelector();

	// Embodies a preference for a particular driver, e.g. do we choose
	// ramp or RGB, hardware or software, etc?  For debugging purposes,
	// you can specify that the software HEL should be used.  The returned
	// objects are owned by this class, hence, deleted when this is deleted.
	virtual const RenIDriverInfo* chooseForFullScreen(RenDisplay&, bool ignoreHw=false) const;

	size_t nDrivers() const;

private:
	void addDriver(RenIDriverInfo*);
	bool enumDevices();
	const RenIDriverInfo* chooseForDepth(int bitDepth, bool ignoreHw=false) const;

	ctl_list<RenIDriverInfo*> 	driverList_;
};

#endif

/* End DRIVER.HPP ***************************************************/
