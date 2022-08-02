/*
 * M E S H I D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MESHID_HPP
#define _RENDER_MESHID_HPP

#include "base/base.hpp"
#include <string>
#include "system/pathname.hpp"

class RenIMeshID
{
public:
    RenIMeshID();
    //  The combined name is what is read from the .x file - it has the
    //  instance name and the (optional) mesh name separated by the separator character
    RenIMeshID( const SysPathName& pathName, const std::string & combinedName, double scale );

    ~RenIMeshID();

	const SysPathName& pathName() const;
	const std::string   combinedName() const;
	const std::string &  instanceName() const;
	const std::string &  meshName() const;
	double		scale() const;

	bool operator< (const RenIMeshID&) const;
	bool operator==(const RenIMeshID&) const;

private:
	SysPathName pathName_;
	std::string 		instanceName_;
	std::string 		meshName_;
	double		scale_;

    static char separator();
};

ostream& operator <<( ostream& o, const RenIMeshID& id );

#endif
