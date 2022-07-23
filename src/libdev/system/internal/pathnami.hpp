/***********************************************************

  P A T H N A M I . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifndef _SYSTEM_INTERNAL_PATHNAME_IMPL_HPP
#define _SYSTEM_INTERNAL_PATHNAME_IMPL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

// Needed for the friend declaration.
class SysPathName;

// Pattern: Pimple-Implementation part.
class SysPathNameImpl
// Memberwise Cannonical.
{
public:
	friend class SysPathName;

	string pathname_;

	// If this is a relative pathname this includes the root directory.
    string fullPathname_;
    ctl_vector<string> components_;

    // True iff the pathname has been explicitly set.
    bool set_;

    size_t rootId_;
    bool fullPathnameSet_;
    bool componentsSet_;

    bool containsCapitals_;
};

#endif /* _SYSTEM_INTERNAL_PATHNAME_IMPL_HPP **************/

