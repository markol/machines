/*
 * R E U I D G E N . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    FtlReusingIdGenerator

	A serial id generator, which will reuse released ids.
*/

#ifndef _FTL_REUIDGEN_HPP
#define _FTL_REUIDGEN_HPP

#include "base/base.hpp"
#include "ftl/serialid.hpp"

class FtlReusingIdGenerator
{
public:
    //ctor start ids at zero
    FtlReusingIdGenerator()
    : highTideId_( 0 ),
	  nFreeIds_( 0 ),
	  freedIdsCapacity_( 128 ),
	  aFreedIds_( _NEW_ARRAY( FtlSerialId, freedIdsCapacity_ ) )
    {};

	//dtor
	~FtlReusingIdGenerator()
	{
		_DELETE_ARRAY( aFreedIds_ );
	};

    //Generate and return next id in sequence    
    FtlSerialId next()
    {
		//If we have a freed id, use that. Otherwise generate a new one
		if( nFreeIds_ == 0 )
		{
			FtlSerialId id = highTideId_;
			++highTideId_;
			return id;
		}
		else
			return aFreedIds_[ --nFreeIds_ ];
    };

	void free( const FtlSerialId& reUseId )
	{
		//Ensure we have the capacity for an extra id
		if( nFreeIds_ == freedIdsCapacity_ )
			doubleCapacity();
		
		aFreedIds_[ nFreeIds_++ ] = reUseId;
	};

    //First/last+1 generated ids
    FtlSerialId begin() const { return FtlSerialId( 0 ); };
    const FtlSerialId& end() const { return highTideId_; };

private:
	//Double the capacity of the freed ids array, 
	void doubleCapacity()
	{
		FtlSerialId* aNew = _NEW_ARRAY( FtlSerialId, freedIdsCapacity_ * 2 );
		FtlSerialId* p = aNew;
		FtlSerialId* q = aFreedIds_;
		for( size_t n = nFreeIds_; n--; )
			*p++ = *q++;

		freedIdsCapacity_ *= 2;

		_DELETE_ARRAY( aFreedIds_ );
		aFreedIds_ = aNew;
	};

    FtlSerialId highTideId_; //One more than the highest id allocated
	size_t nFreeIds_; //Number of freed ids listed in aFreedIds_
	size_t freedIdsCapacity_;// Number of elements in aFreedIds_
    FtlSerialId* aFreedIds_; //Dynamically allocated array of freed ids
};
//////////////////////////////////////////////////////////////////////////////////////////

#endif

/* End REUIDGEN.HPP *************************************************/
