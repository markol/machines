/*
 * R E S O U R C E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _AFX_RESOURCE_HPP
#define _AFX_RESOURCE_HPP

#include "base/base.hpp"
#include "system/pathname.hpp"
#include <map>

//template <class T> class basic_string;
//typedef basic_string< char > string;

// Represents a resource file which can contain strings and/or bitmaps.
class AfxResourceLib
{
public:
	// PRE(libFile.existsAsFile());
    AfxResourceLib(const SysPathName& libFile);
    virtual ~AfxResourceLib();

	// The "get" is not ideal, but "string" conflicts with the type name.
	std::string getString(const uint id) const;
	ulong bitmapHandle(const std::string& name) const;

	// POST(retval.existsAsFile());  must be created with a valid file
	const SysPathName& fileName() const;

    void CLASS_INVARIANT;
    friend ostream& operator <<(ostream& o, const AfxResourceLib& t);

private:
	std::map<uint, std::string> resourceStrings_;
	SysPathName	fileName_;

    // Operations deliberately revoked.
    AfxResourceLib( const AfxResourceLib& );
    AfxResourceLib& operator =( const AfxResourceLib& );
    bool operator ==( const AfxResourceLib& );
};

#endif

/* End RESOURCE.HPP *************************************************/
