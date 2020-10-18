/*
 * D O M A I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
#include "world4d/root.hpp"
#include "world4d/subject.hpp"
#include "world4d/observer.hpp"
#include "world4d/entyfilt.hpp"
#include "world4d/internal/subjecti.hpp"
#include "world4d/domain.hpp"
#include "world4d/internal/domaini.hpp"
#include "mathex/line3d.hpp"

#define CB_W4dDomain_DEPIMPL() \
	CB_DEPIMPL( W4dPortals, portals_ ); \
	CB_DEPIMPL( W4dEntity::W4dEntities, intersectingEntities_ ); \
	CB_DEPIMPL( W4dObservers, observers_ ); \
	CB_DEPIMPL( W4dSubjects, subjects_ );

PER_DEFINE_PERSISTENT( W4dDomain );

W4dDomain::W4dDomain( W4dEntity* parent, const W4dTransform3d& localTransform )
: W4dEntity( parent, localTransform, NOT_SOLID ),
  pImpl_( _NEW( W4dDomainImpl() ) )
{
	CB_W4dDomain_DEPIMPL();
    isDomain( true );

    //Set suitable vector sizes
    observers_.reserve( 4 );
    subjects_.reserve( 4 );

	// Update the root's tally of domains.
	findRoot()->adjustDomainCount(1);
}

W4dDomain::~W4dDomain()
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();

    // Inform all observers that the domain is being deleted, while clearing the
    //collection
    while( observers_.size() != 0 )
    {
        //Notify the last one, and if the observer doesn't remove itself, we do it
        W4dObserver* pObserver = observers_.back();

        size_t n = observers_.size();
        pObserver->domainDeleted( this );
        if( observers_.size() == n )
            observers_.pop_back();
    }

    //  Make sure that no entities intersect with this domain any more
    while( intersectingEntities_.size() != 0 )
    {
        intersectingEntities_.back()->intersects( this, false );
    }

    while( portals_.size() != 0 )
        _DELETE( portals_.front() );

	// Update the root's tally of domains.
	findRoot()->adjustDomainCount(-1);

	_DELETE( pImpl_ );
}

const W4dDomain::W4dPortals& W4dDomain::portals() const
{
	CB_W4dDomain_DEPIMPL();
    return portals_;
}

bool W4dDomain::intersects( const W4dEntity& entity ) const
{
	CB_W4dDomain_DEPIMPL();
    PRE( this != NULL );

// Returns true if an intersect relation exists between 'this' and entity

    bool result = true;

    if( find( intersectingEntities_.begin(),
     intersectingEntities_.end(),
     &entity ) == intersectingEntities_.end() )
        result = false;

    return result;
}

const W4dDomain::W4dEntities& W4dDomain::intersectingEntities() const
{
	CB_W4dDomain_DEPIMPL();
    return intersectingEntities_;
}

void W4dDomain::intersects( W4dEntity* pEntity, bool intersects )
{
	CB_W4dDomain_DEPIMPL();
    PRE( pEntity != NULL );
    PRE( this != NULL );

    W4dEntities::iterator   entityPosition;

    entityPosition = find(
      intersectingEntities_.begin(),
      intersectingEntities_.end(),
      pEntity );

    if( intersects )
    {
        ASSERT( entityPosition == intersectingEntities_.end(), "Domain is already in intersecting domains list" );

        intersectingEntities_.push_back( pEntity );
    }
    else
    {
        ASSERT( entityPosition != intersectingEntities_.end(), "Domain is not in intersecting domains list" );

        intersectingEntities_.erase( entityPosition );
    }
}

W4dPortal* W4dDomain::addPortal
(
	W4dDomain* pPartner,
	const MexQuad3d& aperture,
  	const W4dTransform3d& newLocalTransform
)
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();
    PRE( pPartner != NULL );

    W4dPortal* result = _NEW( W4dPortal( this, pPartner, aperture, newLocalTransform ) );

    TEST_INVARIANT;

    return result;
}

void W4dDomain::removePortal( W4dPortal* pPortal )
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();

    _DELETE( pPortal );

    TEST_INVARIANT;
}


void W4dDomain::addPortalToList( W4dPortal* pPortal )
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();

    PRE( pPortal != NULL );
    PRE( not hasPortal( pPortal ) );

    portals_.push_back( pPortal );

    TEST_INVARIANT;
}

void W4dDomain::removePortalFromList( W4dPortal* pPortal )
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();

    PRE( pPortal != NULL );
    PRE( hasPortal( pPortal ) );

    W4dDomain::W4dPortals::iterator i = find( portals_.begin(), portals_.end(), pPortal );
    ASSERT( i != portals_.end(), "Portal not attached to domain" );
    portals_.erase( i );

    TEST_INVARIANT;
}

bool W4dDomain::hasPortal( const W4dPortal* pPortal ) const
{
    TEST_INVARIANT;
	CB_W4dDomain_DEPIMPL();

    PRE( pPortal != NULL );

    W4dDomain::W4dPortals::const_iterator i = find( portals_.begin(), portals_.end(), pPortal );

    bool result = ( i != portals_.end() );

    TEST_INVARIANT;

    return result;
}


W4dDomainImpl& W4dDomain::domainImpl()
{
	CB_W4dDomain_DEPIMPL();
	PRE( pImpl_ != NULL )
	return *pImpl_;
}

const W4dDomainImpl& W4dDomain::domainImpl() const
{
	CB_W4dDomain_DEPIMPL();
	PRE( pImpl_ != NULL )

	return *pImpl_;
}

// virtual
void W4dDomain::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
    ostr << "Start W4dDomain " << (void*)this << std::endl;
    ostr << "End   W4dDomain " << (void*)this << std::endl;
}

void W4dDomain::CLASS_INVARIANT
{
	CB_W4dDomain_DEPIMPL();

	INVARIANT( pImpl_ );

    for( W4dPortals::const_iterator i = portals_.begin(); i != portals_.end(); ++i )
    {
        INVARIANT( (*i)->pOwner() == this or (*i)->pPartner() == this );
    }
}

ostream& operator <<( ostream& o, const W4dDomain& t )
{

    o << "W4dDomain " << (void*)&t << " start" << std::endl;
    o << "W4dDomain " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dDomain::attach( W4dObserver* pObserver )
{
	CB_W4dDomain_DEPIMPL();
    PRE( pObserver != NULL );
    TEST_INVARIANT;

    //Check to see if the observer is not already an observer before adding it
    W4dObservers::iterator it =
        find( observers_.begin(), observers_.end(), pObserver );

    if( it == observers_.end() )
	{
        observers_.push_back( pObserver );
	}
}

void W4dDomain::detach( W4dObserver* pObserver )
{
	CB_W4dDomain_DEPIMPL();
    PRE( pObserver != NULL );
    TEST_INVARIANT;

    //Find the argument in the collection, then erase it
    W4dObservers::iterator it =
        find( observers_.begin(), observers_.end(), pObserver );

    if( it != observers_.end() )
	{
        observers_.erase( it );
	}
}

const W4dSubjects& W4dDomain::subjects() const
{
	CB_W4dDomain_DEPIMPL();
    return subjects_;
}

const W4dObservers& W4dDomain::observers() const
{
	CB_W4dDomain_DEPIMPL();
    return observers_;
}

//static
W4dSubjects& W4dDomain::newSubjects()
{
    static bool firstTime= true;
    static W4dSubjects theNewSubjects;

    //Ensure sensible allocation size
    if( firstTime )
    {
        firstTime = false;
        theNewSubjects.reserve( 32 );
    }

    return theNewSubjects;
}

//static
void W4dDomain::initialiseNewSubjects()
{
    //Do each one
    W4dSubjects& subjects = newSubjects();
    while( subjects.size() != 0 )
    {
        //Pop the subject from the list
        W4dSubject* pSubject = subjects.back();
        subjects.pop_back();

        //Do its initial notification
        pSubject->impl().initialiseDomainObservers();
    }
}

//static
void W4dDomain::addNewSubject( W4dSubject* pSubject )
{
    newSubjects().push_back( pSubject );
}

//static
void W4dDomain::flushDeletedSubjectFromNewSubjects( W4dSubject* pSubject )
{
    //remove the subject from the list if in it
    W4dSubjects& subjects = newSubjects();
    W4dSubjects::iterator it = find( subjects.begin(), subjects.end(), pSubject );
    if( it != subjects.end() )
        subjects.erase( it );
}

bool W4dDomain::findNearerEntity
(
    const MexLine3d& line, MATHEX_SCALAR limit,
    ulong checkId, W4dEntity::Accuracy accuracy,
    W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
    int filter
)
{
    //Use standard filter
    W4dStandardFilter standardFilter( filter );
    return findNearerEntity( line, limit, checkId, accuracy, ppEntity, pDistance, &standardFilter );
}

bool W4dDomain::findNearerEntity
(
    const MexLine3d& line, MATHEX_SCALAR limit,
    ulong checkId, W4dEntity::Accuracy accuracy,
    W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
    W4dEntityFilter* pEntityFilter
)
{
	CB_W4dDomain_DEPIMPL();
    //Check the domain entity and all its descendants
    bool result = false;
    bool stopProcessing = false;
    MATHEX_SCALAR foundDistance;
    if( recursiveFindNearerEntity( this, line, limit, checkId, accuracy,
                                   ppEntity, &foundDistance, pEntityFilter ) )
    {
        result = true;
        limit = foundDistance;
    }

    //Check all entities having an intersect relation with this domain
    const W4dEntities& intersectors = intersectingEntities();

    for( W4dEntities::const_iterator it = intersectors.begin();
         not pEntityFilter->isFinished()  and  it != intersectors.end(); ++it )
    {
        //Check this entity
        W4dEntity* pIntersector = *it;
        //JON_STREAM("  Checking via intersect relation with " << (void*)pIntersector << std::endl;)
        if( recursiveFindNearerEntity( pIntersector, line, limit, checkId, accuracy,
                                       ppEntity, &foundDistance, pEntityFilter ) )
        {
            //This is the nearest so far. Update the limit and nearest entity
            result = true;
            limit = foundDistance;
        }
    }

    //Try any adjoining domains by considering any portals
    for( W4dPortals::iterator it = portals_.begin(); not pEntityFilter->isFinished()  and  it != portals_.end(); ++it )
    {
        //Check we can see through it
        W4dPortal* pPortal = *it;
        if( not pPortal->isOpenFrom( this ) )
            continue;

        //See if the other domain has already been done
        W4dDomain* pNextDomain = pPortal->otherDomain( this );
        if( pNextDomain->checkId() == checkId)
            continue;

        //See if the line intersects the portal's boundary
        if( pPortal->apertureIntersectsLine( line, limit ) )
        {
            //Check the adjoining domain
            if( pNextDomain->findNearerEntity( line, limit, checkId, accuracy,
                                               ppEntity, &foundDistance, pEntityFilter ) )
            {
                result = true;
                limit = foundDistance;
                break;
            }
        }
    }

    //Update the distance to the nearest found, if any
    if( result )
        *pDistance = limit;

    return result;
}

bool W4dDomain::recursiveFindNearerEntity
(
    W4dEntity* pEntity,
    const MexLine3d& line, MATHEX_SCALAR limit,
    ulong checkId, W4dEntity::Accuracy accuracy,
    W4dEntity** ppEntity, MATHEX_SCALAR* pDistance,
    W4dEntityFilter* pEntityFilter
)
{
	CB_W4dDomain_DEPIMPL();
    //See if this entity already checked
    bool result = false;


    if( pEntity->checkId() != checkId )
    {
        //Set the check id, since we're going to check it now
        pEntity->checkId( checkId );

        //Try intersecting with this entity, if not excluded
        MATHEX_SCALAR foundDistance;
        W4dEntityFilter::TreeOption filterTreeOption;

        if( pEntityFilter->check( *pEntity, &filterTreeOption )  and
            pEntity->intersectsLine( line, &foundDistance, accuracy )  and
            foundDistance >= 0.0  and
            foundDistance <= limit )
        {
            //This is the nearest so far. Update the limit and nearest entity
            result = true;
            limit = foundDistance;
            *ppEntity = pEntity;
        }

        //Make recursive call to every non-domain child of the argument entity unless excluded
        if( filterTreeOption == W4dEntityFilter::PROCESS_SUBTREE )
        {
            const W4dEntities& children = pEntity->children();
            for( W4dEntities::const_iterator it = children.begin();
                 not pEntityFilter->isFinished()  and  it != children.end(); ++it )
            {
                //Check entity not a domain
                W4dEntity* pChild = *it;
                if( not pChild->isDomain() )
                {
                    //Find nearer
                    if( recursiveFindNearerEntity( pChild, line, limit, checkId, accuracy,
                                                   ppEntity, &foundDistance, pEntityFilter ) )
                    {
                        //This is the nearest so far. Update the limit and nearest entity
                        result = true;
                        limit = foundDistance;
                    }
                }
            }
        }
    }
    else
    {
        //JON_STREAM("  Already checked" << std::endl;)
    }

    //Update the distance to the nearest found, if any
    if( result )
        *pDistance = limit;

    return result;
}

bool W4dDomain::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const W4dDomain& domain )
{
    const W4dEntity& base = domain;

    ostr << base;
    ostr << domain.pImpl_;
}

void perRead( PerIstream& istr, W4dDomain& domain )
{
    _DELETE( domain.pImpl_ );

    W4dEntity& base = domain;

    istr >> base;
    istr >> domain.pImpl_;
}

W4dDomain::W4dDomain( PerConstructor con )
: 	W4dEntity( con ),
	pImpl_( NULL )
{
}

/* End DOMAIN.CPP ***************************************************/
