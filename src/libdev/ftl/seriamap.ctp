/*
 * S E R I A M A P . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#ifndef _INLINE
    #include "ftl/seriamap.itp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
template< class VALUE >
//virtual
void FtlSerialMap< VALUE >::add( const FtlPair< FtlSerialId, VALUE >& addMe )
{
    PRE( not contains( addMe.first ) );

    //get id and current highest entry + 1
    size_t id = addMe.first.asScalar();
    size_t n = index_.size();
	size_t index = values_.size();

	//Push the new value onto the values vector, with its id
	values_.push_back( Entry( id, addMe.second ) );

    //Add the index to the index vector
    if( id < n )
	{
        index_[ id ] = index;
    }
    else
    {
        //If necessary, push some unused entries into the index vector
        n = id - n;
        while( n-- != 0 )
            index_.push_back( FTL_SERIALMAP_NOENTRY );

        index_.push_back( index );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
template< class VALUE >
//virtual
void FtlSerialMap< VALUE >::remove( const FtlSerialId& id )
{
    PRE( contains( id ) );

	size_t i = id.asScalar();
	size_t valueIndex = index_[ i ];
	size_t highestValueIndex = values_.size() - 1;

	if( valueIndex != highestValueIndex )
	{
		//Copy the final entry into the vacated slot
		const std::pair< size_t, VALUE >& highPair = values_[ highestValueIndex ];
		values_[ valueIndex ] = highPair;

		//reset the index entry for the value we moved
		index_[ highPair.first ] = valueIndex;
	}

	//Erase the final entry
	values_.pop_back();

	//Mark the index entry for the removed value as unused
	index_[ i ] = FTL_SERIALMAP_NOENTRY;

	POST( not contains( id ) );
}
//////////////////////////////////////////////////////////////////////////////////////////
template< class VALUE >
void FtlSerialMap< VALUE >::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End SERIAMAP.CTP *************************************************/
