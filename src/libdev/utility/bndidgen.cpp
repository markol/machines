/*
 * B N D I D G E N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "utility/bndidgen.hpp"

#ifndef _INLINE
    #include "utility/bndidgen.ipp"
#endif

UtlBoundedIdGenerator::UtlBoundedIdGenerator( UtlId upperBound )
:   upperBound_( upperBound ),
    lastAllocatedId_( 0 ),
    nUnused_( upperBound ),
    aFlags_( NULL )
{
    PRE( upperBound != 0 );

    //Allocate an array of flags
    aFlags_ = _NEW_ARRAY( uchar, upperBound );

    //Initialise them
    for( UtlId i = upperBound; i--; )
        aFlags_[i] = 0;

    TEST_INVARIANT;
}

UtlBoundedIdGenerator::~UtlBoundedIdGenerator()
{
    TEST_INVARIANT;

    //Delete the flag array
    _DELETE_ARRAY( aFlags_ );
}

UtlId UtlBoundedIdGenerator::nextId()
{
    PRE( nUnusedIds() > 0 );

    //Find an unallocated id
	#ifndef PRODUCTION
	long loops = 0;
	#endif
    do
    {
        ++lastAllocatedId_;
        if( lastAllocatedId_ == upperBound_ )
            lastAllocatedId_ = 0;
		#ifndef PRODUCTION
		++loops;
		if( loops > upperBound_ )
		{
			Diag::instance().forceCrash();
		}
		#endif
    }
    while( aFlags_[ lastAllocatedId_ ] != 0 );

    //Decrement remaining count
    --nUnused_;

    //Mark as in use
    aFlags_[ lastAllocatedId_ ] = uchar(1);

    return lastAllocatedId_;
}

UtlId UtlBoundedIdGenerator::nextId( UtlId minId, UtlId maxId )
{
    PRE( nUnusedIds() > 0 );
	PRE( maxId > minId );
	PRE( maxId <= upperBound_ );

	//bounds check the lastAllocatedId_ - this could mean that for each Id we start again at minId
	//but if the client code is correct then this shouldn't happen.
	if( lastAllocatedId_ < minId )
		lastAllocatedId_ = minId;

    //Find an unallocated id
    do
    {
        ++lastAllocatedId_;
        if( lastAllocatedId_ >= maxId )
            lastAllocatedId_ = minId;
    }
    while( aFlags_[ lastAllocatedId_ ] != 0 );

    //Decrement remaining count
    --nUnused_;

    //Mark as in use
    aFlags_[ lastAllocatedId_ ] = uchar(1);

    return lastAllocatedId_;
}

void UtlBoundedIdGenerator::allocateId( UtlId id )
{
    PRE( not isAllocated( id ) );

    //Decrement remaining count
    --nUnused_;

    //Mark as in use
    aFlags_[ id ] = uchar(1);
}


void UtlBoundedIdGenerator::freeId( UtlId id )
{
    PRE( isAllocated( id ) );

    //Reset the flag and increment the free count
    aFlags_[ id ] = 0;
    ++nUnused_;
}

void UtlBoundedIdGenerator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const UtlBoundedIdGenerator& t )
{

    o << "UtlBoundedIdGenerator " << (void*)&t << " start" << std::endl;
    o << "UtlBoundedIdGenerator " << (void*)&t << " end" << std::endl;

    return o;
}

/* End BNDIDGEN.CPP *************************************************/
