/*
 * C O M P P L A N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "world4d/entyplan.hpp"
#include "world4d/planentr.hpp"
#include "world4d/compplan.hpp"
#include "world4d/internal/compplai.hpp"

#define CB_W4dCompositePlan_DEPIMPL() \
	CB_DEPIMPL( W4dEntityPlan*, pCompositePlan_ ); \
	CB_DEPIMPL( Entries, entries_ ); \
	CB_DEPIMPL( void*, pad1_ ); \
	CB_DEPIMPL( void*, pad2_ ); \
	CB_DEPIMPL( string, name_ ); \
	CB_DEPIMPL( PhysRelativeTime, cachedFinishTime_ );

PER_DEFINE_PERSISTENT( W4dCompositePlan );

//////////////////////////////////////////////////////////////////////////////////////////

W4dCompositePlan::W4dCompositePlan( const string& name )
:    pImpl_( _NEW( W4dCompositePlanImpl() ) )
{
	CB_W4dCompositePlan_DEPIMPL();
//    pImpl_ = _NEW( W4dCompositePlanImpl ); // don't be a MERGE MORON - don't put this line back in
    pImpl_->name_ = name;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dCompositePlan::~W4dCompositePlan()
{
	CB_W4dCompositePlan_DEPIMPL();
    TEST_INVARIANT;
    _DELETE( pImpl_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dCompositePlan::compositePlan( const W4dEntityPlan& plan )
{
	CB_W4dCompositePlan_DEPIMPL();
    PRE( pCompositePlan_ == NULL );

    //Construct and store the plan
    pCompositePlan_ = _NEW( W4dEntityPlan( plan ) );
    pImpl_->cachedFinishTime_ = -1.0; //indicates needs re-evaluation

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dCompositePlan::linkPlan( W4dLinkId id, const W4dEntityPlan& plan )
{
	CB_W4dCompositePlan_DEPIMPL();
    //Construct a new entry
    W4dCompositePlanEntry* pEntry = _NEW( W4dCompositePlanEntry( id, plan ) );

    //Add to the list
    entries_.push_back( pEntry );

    cachedFinishTime_ = -1.0; //indicates needs re-evaluation
}
//////////////////////////////////////////////////////////////////////////////////////////

const W4dCompositePlan::Entries& W4dCompositePlan::entries() const
{
	CB_W4dCompositePlan_DEPIMPL();
	return entries_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool W4dCompositePlan::hasCompositePlan() const
{
	CB_W4dCompositePlan_DEPIMPL();
	return pCompositePlan_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////

const W4dEntityPlan& W4dCompositePlan::compositePlan() const
{
	CB_W4dCompositePlan_DEPIMPL();
	return *pCompositePlan_;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime W4dCompositePlan::finishTime() const
{
    TEST_INVARIANT;
	CB_W4dCompositePlan_DEPIMPL();

    if( cachedFinishTime_ < 0.0 )
    {
        PhysRelativeTime time = 0;

        //Get the duration of the composite's plan
        if( pCompositePlan_ ) time = pCompositePlan_->endTime();

        //Check each link's plan to see if longer
        for( Entries::const_iterator i = entries_.begin(); i != entries_.end(); ++i)
        {
            PhysRelativeTime entryDuration = (*i)->plan().endTime();
            if( entryDuration > time ) time = entryDuration;
        }

        cachedFinishTime_ = time;
    }

    return cachedFinishTime_;
}

const string& W4dCompositePlan::name() const
{
	CB_W4dCompositePlan_DEPIMPL();
    return name_;
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dCompositePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const W4dCompositePlan& t )
{

    o << "W4dCompositePlan " << (void*)&t << " start" << std::endl;
    o << "W4dCompositePlan " << (void*)&t << " end" << std::endl;

    return o;
}

W4dCompositePlan::W4dCompositePlan( PerConstructor )
: pImpl_( NULL )
{
}

void perWrite( PerOstream& ostr, const W4dCompositePlan& plan )
{
    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, W4dCompositePlan& plan )
{
    _DELETE( plan.pImpl_ );
    istr >> plan.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End COMPPLAN.CPP *************************************************/
