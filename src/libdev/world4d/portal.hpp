/*
 * P O R T A L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    W4dPortal

    Represents an aperture through which light and other objects may pass
    between exactly two adjoining W4dDomains.
*/

#ifndef _PORTAL_HPP
#define _PORTAL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/t3dkey.hpp"
#include "world4d/entity.hpp"

class W4dDomain;

class W4dPortal : public W4dEntity
{
public:
    ~W4dPortal();

    void partner (W4dDomain* pNewPartner);
    // Changes the adjoining domain. Could be used for mobile domains which pass into new
    // domains.

	// The hole represented by this portal in *global* coordinates.
	const MexQuad3d& globalAperture() const;

    // Return the owner/partner domains
    W4dDomain* pOwner() const;
    W4dDomain* pPartner() const;

	// Returns the attached domain that isn't the given one.
	// PRE(d == pOwner() || d == pPartner());
	W4dDomain* otherDomain(const W4dDomain* d) const;

    void open (bool);
    // Sets portal open/closed in both directions

	void openFrom(const W4dDomain* d);
	// Sets the portal open so you can see from d into otherDomain(d).

	bool isOpenFrom(const W4dDomain* d) const;
	// Returns true if you can see from d into otherDomain(d).
	// PRE(d == pOwner() || d == pPartner());

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    //True iff global line intersects the aperture between line.end1() and distance limit
    //in the direction of line's first to second point.
    bool apertureIntersectsLine( const MexLine3d& line, MATHEX_SCALAR limit ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dPortal );
    PER_FRIEND_READ_WRITE( W4dPortal );

protected:
    virtual void doDebugOutput( ostream& ostr );

private:
    friend class W4dDomain;
    W4dPortal (W4dDomain* owner, W4dDomain* partner, const MexQuad3d& aperture,
    		const W4dTransform3d& localTransform);
    // 'this' is permanently owned by owner, and currently adjoins partner.
    // Added to lists of W4dPortals owned by both W4dDomains.
    // The window area is defined by portal (- probably subclass of rectangles for now)
    // Position relative to parent is localTransform.
    // State initialised to open.
    // This constructor only for use by W4dDomain

	void updateGlobalAperture(const MexTransform3d& xform);

    W4dDomain*  pOwner_;
    W4dDomain*  pPartner_;
    bool        ownerToPartnerOpen_;	// open in owner -> partner direction
    bool        partnerToOwnerOpen_;	// open in partner -> owner direction
	MexQuad3d	aperture_;				// hole in local  coords

    //The time when the cached global aperture was last updated
    PhysAbsoluteTime timeGlobalApertureLastUpdated_;
    MexTransform3dKey lastGlobalTransformKey_;
	MexQuad3d	gAperture_;				// hole in global coords

    // Operations deliberately revoked
    W4dPortal( const W4dPortal& );
    W4dPortal& operator =( const W4dPortal& );
    bool operator ==( const W4dPortal& );
};

PER_DECLARE_PERSISTENT( W4dPortal );

#endif

/* End PORTAL.HPP ***************************************************/
