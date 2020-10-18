/*
 * D O M A S I G N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _WORLD4D_DOMASIGN_HPP
#define _WORLD4D_DOMASIGN_HPP

#include "base/base.hpp"

class W4dRoot;
class W4dLight;

// An abstract base-class which is responsible for assigning domains to
// world4d objects.  Derived classes should use application-specific 
// knowledge of domain layouts to determine which domain an object 
// intersects with.  It is conceivable that a world4d class could do this
// however, application-specific code can probably be more efficient.
class W4dDomainAssignor
{
public:
    virtual ~W4dDomainAssignor()	{}

	// Reparent the light so that it's containing domain is the one in which
	// the light's centre falls.  Set intersecting domains correctly.	
	virtual void assignDomains(W4dRoot*, W4dLight*) = 0;
};

#endif

/* End DOMASIGN.HPP *************************************************/
