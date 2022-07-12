/*
 * D R I V . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    RenDriver

    A brief description of the class should go in here
*/

#ifndef _DRIV_HPP
#define _DRIV_HPP

#include "base/base.hpp"

class RenIDriverImpl;
template < class T > class CtlCountedPtr;

// A class wrapper for DirectDraw/Direct3D driver info.
// Objects of this type will be created by an RenDriverSelector only.
class RenDriver
{
public:
    ~RenDriver();

    void CLASS_INVARIANT;

	const std::string& name() const;
	const std::string& description() const;

	bool isBetterChoiceThan( const RenDriver& ) const;

	const RenIDriverImpl& driverImpl() const;
	RenIDriverImpl& driverImpl();

private:
	friend class RenDriverSelector;
	friend class RenDriverSelectorImpl;

    RenDriver( RenIDriverImpl* );
    RenDriver( const RenDriver& driver );

    friend ostream& operator <<( ostream& o, const RenDriver& t );

    RenDriver& operator =( const RenDriver& );

	RenIDriverImpl *pImpl_;
};

typedef CtlCountedPtr< RenDriver > RenDriverPtr;

#endif

/* End DRIV.HPP **************************************************/
