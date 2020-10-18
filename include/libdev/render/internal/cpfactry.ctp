/*
 * F A C T O R Y . C T P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

template < class ID, class PART >
UtlCPFactory< ID, PART >::UtlCPFactory()
{
	// intentionally empty
}

template < class ID, class PART >
UtlCPFactory< ID, PART >::~UtlCPFactory()
{
	partMap_.erase( partMap_.begin(), partMap_.end() );
}

// virtual
template < class ID, class PART >
PART UtlCPFactory< ID, PART >::part( const ID& id )
{
	if( not containsPart( id ) )
	{
		PART result = doCreatePart( id );

		UtlCPFactory::PartPair newPair( id, result );

		// insert the new pair into the map and check the result
		bool insertionResult = partMap_.insert( newPair ).second;

		ASSERT( insertionResult, "Cannot add part to factory." );
		return result;
	}
	else
	{
		return partMap_[ id ];
	}
}

template < class ID, class PART >
bool UtlCPFactory< ID, PART >::containsPart( const ID& id ) const
{
	bool result = ( partMap_.find( id ) != partMap_.end() );

	return result;
}

template < class ID, class PART >
void UtlCPFactory< ID, PART >::deletePart( const ID& id )
{
	PRE( containsPart( id ) );

	// remove from the map
	PART& pPart = partMap_[ id ];
	partMap_.erase( id );
}

template < class ID, class PART >
typename UtlCPFactory< ID, PART >::PartMap& UtlCPFactory< ID, PART >::partMap()
{
    return partMap_;
}

template < class ID, class PART >
const typename UtlCPFactory< ID, PART >::PartMap& UtlCPFactory< ID, PART >::partMap() const
{
    return partMap_;
}

//////////////////////////////////////////////////////////////////////
