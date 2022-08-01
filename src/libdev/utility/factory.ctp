/*
 * F A C T O R Y . C T P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

template < class ID, class PART >
UtlFactory< ID, PART >::UtlFactory()
{
	// intentionally empty
}

template < class ID, class PART >
UtlFactory< ID, PART >::~UtlFactory()
{
	// clear up
	typename UtlFactory::PartMap::iterator i = partMap_.begin();
	for( ; i != partMap_.end(); ++i )
		_DELETE( (*i).second );

	partMap_.erase( partMap_.begin(), partMap_.end() );
}

// virtual
template < class ID, class PART >
PART& UtlFactory< ID, PART >::part( const ID& id )
{
	PART* pResult = NULL;

	if( not containsPart( id ) )
	{
		pResult = _NEW( PART( id ) );

		UtlFactory::PartPair newPair( id, pResult );

		// insert the new pair into the map and check the result
		bool insertionResult = partMap_.insert( newPair ).second;

		ASSERT( insertionResult, "Cannot add part to factory." );
	}
	else
	{
		pResult = partMap_[ id ];
	}

	POST( pResult != NULL );

    return *pResult;
}

template < class ID, class PART >
bool UtlFactory< ID, PART >::containsPart( const ID& id ) const
{
	bool result = ( partMap_.find( id ) != partMap_.end() );

	return result;
}

template < class ID, class PART >
void UtlFactory< ID, PART >::deletePart( const ID& id )
{
	PRE( containsPart( id ) );

	// remove from the map
	PART* pPart = partMap_[ id ];
	partMap_.erase( id );

	// delete the actual item
	_DELETE( pPart );
}

template < class ID, class PART >
typename UtlFactory< ID, PART >::PartMap& UtlFactory< ID, PART >::partMap()
{
    return partMap_;
}

template < class ID, class PART >
const typename UtlFactory< ID, PART >::PartMap& UtlFactory< ID, PART >::partMap() const
{
    return partMap_;
}

//////////////////////////////////////////////////////////////////////
