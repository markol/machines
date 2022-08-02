/*
 * D R I V S E L . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    RenDriverSelector

    A brief description of the class should go in here
*/

#ifndef _DRIVSEL_HPP
#define _DRIVSEL_HPP

#include "base/base.hpp"
#include "render/driv.hpp"

class RenDisplay;
class RenDriverSelectorImpl;
template <class T> class ctl_list;

class RenDriverSelector
// Canonical form revoked
{
public:
    RenDriverSelector( const RenDisplay* const display );

	// selects only direct draw drivers
    RenDriverSelector();

    ~RenDriverSelector();

    void CLASS_INVARIANT;

	typedef ctl_list< RenDriverPtr > RenDrivers;

	enum ReturnValue { SUCCEEDED, FAILED };

	// List all the drivers	the direct draw drivers
	const RenDrivers& dDrawDrivers() const;

	// List all the drivers	the direct3d drivers for the current direct draw object
	const RenDrivers& d3dDrivers() const;

	// returns the driver selected in the window registries 
	const RenDriverPtr& currentDDrawDriver() const;
	const RenDriverPtr& currentD3dDriver() const;

	ReturnValue useDDrawDriver( const RenDriverPtr& directDrawDriver );
	ReturnValue useD3dDriver( const RenDriverPtr& direct3DDriver );

	ReturnValue updateDriverRegistries();

	RenDriverSelectorImpl& driverSelectorImpl();
	const RenDriverSelectorImpl& driverSelectorImpl() const;

	const RenDisplay& display() const;

private:
    friend ostream& operator <<( ostream& o, const RenDriverSelector& t );

    RenDriverSelector( const RenDriverSelector& );
    RenDriverSelector& operator =( const RenDriverSelector& );

	void streamOut( ostream& o ) const;

	// build the list of drivers (including the dummy automatic driver)
	// set the current and automatic Drivers
	void buildDDrawDrivers();
	void buildD3dDrivers();

	// build the real list of drivers (excluding dummy drivers) supported
	bool enumDDrawDrivers();
	bool enumD3dDrivers();

	// Is the currently selected driveer the automatic driver
	bool currentDDrawDriverIsAutomatic() const;
	bool currentD3dDriverIsAutomatic() const;

	RenDriverSelectorImpl* pImpl_;
};

#endif

/* End DRIVSEL.HPP **************************************************/
