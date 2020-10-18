/*
 * C O M P L E X I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/internal/complexi.hpp"


W4dComplexity::W4dRangeModifier::W4dRangeModifier():
 factor_( 2.0 ),
 minLevel_( -3.0 ),
 maxLevel_( 3.0 ),
 level_( 0 ),
 rangeMultiplierUptodate_( true ),
 rangeMultiplier_( 1.0 )
{
}

W4dComplexity::W4dRangeModifier::~W4dRangeModifier()
{
}

void W4dComplexity::W4dRangeModifier::decreaseLevel()
{
   --level_;
   rangeMultiplierUptodate_=false;
}

void W4dComplexity::W4dRangeModifier::resetLevel()
{
   level_=0;
   rangeMultiplierUptodate_=false;
}

void W4dComplexity::W4dRangeModifier::increaseLevel()
{
   ++level_;
   rangeMultiplierUptodate_=false;
}

const MATHEX_SCALAR& W4dComplexity::W4dRangeModifier::rangeMultiplier()	const
{
  	if( not rangeMultiplierUptodate_)
  	{
		W4dRangeModifier* nonConstThis=_CONST_CAST( W4dRangeModifier*, this );
    	nonConstThis->updateRangeMultiplier();
  	}
	return rangeMultiplier_;
}


void W4dComplexity::W4dRangeModifier::updateRangeMultiplier()
{
   rangeMultiplier_=pow( factor_, -level_ );
   rangeMultiplierUptodate_=true;
}

W4dComplexity::W4dComplexity( uint numPriorities ):
  sequencingType_( W4dComplexity::BY_LOD ),
  priorityCycler_( 0 ),
  lowestComplexityReached_( false ),
  highestComplexityReached_( false ),
  lowestComplexityReachedUptodate_( true ),
  highestComplexityReachedUptodate_( true ),
  range_( 0 ),
  modifiedRange_( 0 ),
  enabled_( false ),
  enabledLOD_( true )
{

	rangeModifiers_.reserve( numPriorities );
	for( uint i=0; i<numPriorities; ++i)
	  rangeModifiers_.push_back( W4dRangeModifier() );

    TEST_INVARIANT;
}

W4dComplexity::~W4dComplexity()
{
    TEST_INVARIANT;

}

uint W4dComplexity::nPriorities( void )	const
{
 return rangeModifiers_.size();
}

void W4dComplexity::reset()
{
  for( W4dRangeModifiers::iterator it=rangeModifiers_.begin(); it!=rangeModifiers_.end(); ++it)
  {
	(*it).resetLevel();
  }
  lowestComplexityReachedUptodate_=false;
  highestComplexityReachedUptodate_=false;
}

bool W4dComplexity::highestComplexityReached( void ) const
{
   if( not highestComplexityReachedUptodate_ )
   {
   		W4dComplexity* nonConstThis = _CONST_CAST( W4dComplexity*, this );
		nonConstThis->updateHighestComplexityReached();
	}
	return highestComplexityReached_;
}

bool W4dComplexity::lowestComplexityReached( void )	const
{
   if( not lowestComplexityReachedUptodate_ )
   {
		W4dComplexity* nonConstThis = _CONST_CAST( W4dComplexity*, this );
		nonConstThis->updateLowestComplexityReached();
   }
   return lowestComplexityReached_;
}

void W4dComplexity::updateHighestComplexityReached( void )
{

	highestComplexityReached_=true;
	for( W4dRangeModifiers::iterator it=rangeModifiers_.begin(); it!=rangeModifiers_.end() and highestComplexityReached_; ++it)
	{
		highestComplexityReached_ = ( (*it).level() >= (*it).maxLevel() );
	}
	highestComplexityReachedUptodate_=true;
}

void W4dComplexity::updateLowestComplexityReached( void )
{

	lowestComplexityReached_=true;
	for( W4dRangeModifiers::iterator it=rangeModifiers_.begin(); it!=rangeModifiers_.end() and lowestComplexityReached_; ++it)
	{
		lowestComplexityReached_ = ( (*it).level() <= (*it).minLevel() );
	}
	lowestComplexityReachedUptodate_=true;
}


// increase the complexity of the scene
bool W4dComplexity::increase()
{
	return increaseComplexityByLOD();
	// as opposed to increaseComplexityByPriority();
}

bool W4dComplexity::increaseComplexityByLOD()
{
	if( not highestComplexityReached() )
	{
   		// Go to previous then modify
		// entities having lower priority have their lod range modified
		priorityCycler_=( ( priorityCycler_ - 1 ) % nPriorities() );
		rangeModifiers_[priorityCycler_].increaseLevel();
		// We may have reached the highest complexity if we have just updated the
		// lod modifier of the lower priority entities
		if( priorityCycler_==0)
			highestComplexityReachedUptodate_=false;
		lowestComplexityReachedUptodate_=false;
	}
	return true;
}

// decrease the complexity of the scene
bool W4dComplexity::decrease()
{
	return decreaseComplexityByLOD();
	// as opposed to decreaseComplexityByPriority();
}

bool W4dComplexity::decreaseComplexityByLOD()
{
	if( not lowestComplexityReached() )
	{
		// Modify then go to next
		rangeModifiers_[priorityCycler_].decreaseLevel();
		priorityCycler_=( ( priorityCycler_ + 1 ) % nPriorities() );
		// We may have reached the lowest complexity when we have modified
		// the lod of the highest priority entities
		if( priorityCycler_== 0 )
			lowestComplexityReachedUptodate_=false;
		highestComplexityReachedUptodate_=false;
	}

	return true;
}

void W4dComplexity::enabled( bool setEnabled )
{
	enabled_=setEnabled;
	if( not enabled_ )
  		reset();
}

void W4dComplexity::enabledLOD( bool setEnabled )
{
	enabledLOD_=setEnabled;
}

void  W4dComplexity::usePriority( uint priority )
{
	PRE(priority<nPriorities());
	priority_=priority;
	updateModifiedRange();
}

const MATHEX_SCALAR& W4dComplexity::range( ) const
{
	return range_;
}

void W4dComplexity::range( const MATHEX_SCALAR& setRange )
{
	range_=setRange;
	updateModifiedRange();
}

const MATHEX_SCALAR& W4dComplexity::modifiedRange( ) const
{
	return modifiedRange_;
}

void W4dComplexity::updateModifiedRange()
{
	modifiedRange_= rangeModifiers_[priority_].rangeMultiplier() * range_;
}

const int& W4dComplexity::level( ) const
{
	return rangeModifiers_[ priority_ ].level();
}

const MATHEX_SCALAR& W4dComplexity::rangeMultiplier( ) const
{
	return rangeModifiers_[priority_].rangeMultiplier();

}

void W4dComplexity::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dComplexity& t )
{

    o << "W4dComplexity " << (void*)&t << " start" << std::endl;
    o << "W4dComplexity " << (void*)&t << " end" << std::endl;

    return o;
}

/* End COMPLEXI.CPP *************************************************/
