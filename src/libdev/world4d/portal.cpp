/*
 * P O R T A L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "world4d/portal.hpp"
#include "world4d/manager.hpp"
#include "world4d/domain.hpp"
#include "mathex/line3d.hpp"
#include "mathex/abox3d.hpp"

PER_DEFINE_PERSISTENT( W4dPortal );

W4dPortal::W4dPortal( W4dDomain* pOwner, W4dDomain* pPartner, const MexQuad3d& aperture,
    		const W4dTransform3d& newLocalTransform )
: W4dEntity( pOwner, newLocalTransform, NOT_SOLID, DONT_ADD_TO_CHILD_LIST ),
  pOwner_( pOwner ),
  pPartner_( pPartner ),
  aperture_(aperture),
  gAperture_(aperture),			// not really the true value
  timeGlobalApertureLastUpdated_(-1),
  ownerToPartnerOpen_( true ),
  partnerToOwnerOpen_( true )
{
    pOwner->addPortalToList( this );
    pPartner->addPortalToList( this );
	updateGlobalAperture(globalTransform());

    TEST_INVARIANT;
}

W4dPortal::~W4dPortal()
{
    TEST_INVARIANT;

    pOwner()->removePortalFromList( this );
    pPartner()->removePortalFromList( this );
}

//#$-VIEW:Domain class-$#
void W4dPortal::partner( W4dDomain* pNewPartner )
// Changes the adjoining domain. Could be used for mobile domains which pass into new
// domains.
{
    TEST_INVARIANT;

    PRE( pNewPartner != NULL );

    pPartner()->removePortalFromList( this );
    pPartner_ = pNewPartner;
    pPartner()->addPortalToList( this );

    TEST_INVARIANT;
}

void W4dPortal::open( bool newState )
{
    ownerToPartnerOpen_ = newState;
    partnerToOwnerOpen_ = newState;
}

void W4dPortal::openFrom(const W4dDomain* d)
{
	PRE(d == pOwner() || d == pPartner());

	if (d == pOwner_)
		ownerToPartnerOpen_	= true;
	else
		partnerToOwnerOpen_ = true;
}

bool W4dPortal::isOpenFrom(const W4dDomain* d) const
{
	PRE(d == pOwner() || d == pPartner());

	if (d == pOwner_)
		return ownerToPartnerOpen_;
	else
		return partnerToOwnerOpen_;
}

W4dDomain* W4dPortal::pOwner() const
{
    return pOwner_;
}

W4dDomain* W4dPortal::pPartner() const
{
    return pPartner_;
}

void W4dPortal::updateGlobalAperture(const MexTransform3d& xform)
{
	// The global transform has changed, we need to recompute the aperture
	// in global coordinates
	MexPoint3d v0 = aperture_.vertex(0);
	MexPoint3d v1 = aperture_.vertex(1);
	MexPoint3d v2 = aperture_.vertex(2);
	MexPoint3d v3 = aperture_.vertex(3);

	xform.transform(&v0);
	xform.transform(&v1);
	xform.transform(&v2);
	xform.transform(&v3);

	gAperture_ = MexQuad3d(v0, v1, v2, v3);
	lastGlobalTransformKey_ = xform.key();
}

const MexQuad3d& W4dPortal::globalAperture() const
{
	// Get the global transform
	IAIN_STREAM("Getting portal for " << (W4dEntity*) this << "\n");
	const MexTransform3d& xform = globalTransform();

	//See if the transform keys still match. If so no need to recompute.
	if( xform.key() != lastGlobalTransformKey_ )
	{
		IAIN_STREAM("  portal needs recomputation.\n");
		W4dPortal* nonConstThis = _CONST_CAST(W4dPortal*, this);
		nonConstThis->updateGlobalAperture(xform);
	}

	return gAperture_;
}

// Returns the attached domain that isn't the given one.
W4dDomain* W4dPortal::otherDomain(const W4dDomain* d) const
{
	PRE(d == pOwner() || d == pPartner());

	if (d == pOwner_)
		return pPartner_;
	else
		return pOwner_;
}

// virtual
void W4dPortal::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
    ostr << "Start W4dPortal " << (void*)this << std::endl;
    ostr << "End   W4dPortal " << (void*)this << std::endl;
}

void W4dPortal::CLASS_INVARIANT
{
    INVARIANT( pOwner()->hasPortal( this ) );
    INVARIANT( pPartner()->hasPortal( this ) );
}

bool W4dPortal::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const
{
    //Just check against bounding volume
    return hasMesh()  and  defaultIntersectsLine( line, pDistance, accuracy );
}

bool W4dPortal::apertureIntersectsLine( const MexLine3d& line, MATHEX_SCALAR limit ) const
{
    //Get the global transform to the portal and invert it
    MexTransform3d inverseGlobal;
    globalTransform().invert( &inverseGlobal );

    //Hence get a line in the local axis system of the portal
    MexLine3d localLine( line );
    localLine.transform( inverseGlobal );

    //Construct a locally aligned bounding box for the aperture
    const MexPoint3d& v0 = aperture_.vertex( 0 );
    MATHEX_SCALAR xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = xMax = v0.x();
    yMin = yMax = v0.y();
    zMin = zMax = v0.z();

    for( uint i = 1; i != 4; ++i )
    {
        const MexPoint3d& vi = aperture_.vertex( i );

        MATHEX_SCALAR x = vi.x();
        if( x > xMax )
            xMax = x;
        else if( x < xMin )
            xMin = x;

        MATHEX_SCALAR y = vi.y();
        if( y > yMax )
            yMax = y;
        else if( y < yMin )
            yMin = y;

        MATHEX_SCALAR z = vi.z();
        if( z > zMax )
            zMax = z;
        else if( z < zMin )
            zMin = z;
    }

    MexAlignedBox3d bv(xMin, yMin, zMin, xMax, yMax, zMax );

    MATHEX_SCALAR entryDistance, exitDistance;
    bool result = bv.intersects( localLine, &entryDistance, &exitDistance ) and
                  entryDistance <= limit;

    return result;
}

void perWrite( PerOstream& ostr, const W4dPortal& portal )
{
    const W4dEntity& base = portal;

    ASSERT_INFO( "a" );

    ostr << base;

    ASSERT_INFO( "a" );
    PER_WRITE_RAW_POINTER( ostr, portal.pOwner_ );
    ASSERT_INFO( "a" );
    PER_WRITE_RAW_POINTER( ostr, portal.pPartner_ );
    ASSERT_INFO( "a" );
    ostr << portal.ownerToPartnerOpen_;
    ASSERT_INFO( "a" );
    ostr << portal.partnerToOwnerOpen_;
    ASSERT_INFO( "a" );
    ostr << portal.aperture_;
    ASSERT_INFO( "a" );
    ostr << portal.gAperture_;
    ASSERT_INFO( "a" );
    ostr << portal.timeGlobalApertureLastUpdated_;
    ASSERT_INFO( "a" );
}

void perRead( PerIstream& istr, W4dPortal& portal )
{
    W4dEntity& base = portal;

    istr >> base;

    PER_READ_RAW_POINTER( istr, portal.pOwner_ );
    PER_READ_RAW_POINTER( istr, portal.pPartner_ );
    istr >> portal.ownerToPartnerOpen_;
    istr >> portal.partnerToOwnerOpen_;
    istr >> portal.aperture_;
    istr >> portal.gAperture_;
    istr >> portal.timeGlobalApertureLastUpdated_;
}

W4dPortal::W4dPortal( PerConstructor con )
: W4dEntity( con ),
  aperture_( con ),
  gAperture_( con )
{
}

/* End PORTAL.CPP ***************************************************/
