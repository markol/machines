/*
 * I C O N S E Q . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/internal/iconsei.hpp"
#include "gui/iconseq.hpp"

#define CB_GUIICONSEQUENCE_DEPIMPL() \
		CB_DEPIMPL( ActiveChildren,	activeChildren_ ); \
		CB_DEPIMPL( FixedChildren, fixedChildren_ ); \
		CB_DEPIMPL( unsigned, nFixedChildren_ ); \
		CB_DEPIMPL( ctl_bit_vector, allocatedPositions_ ); \
		CB_DEPIMPL( Coords, coords_ ); \
		CB_DEPIMPL(	bool, isDoingDisplay_ );

#define CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL() \
		CB_DEPIMPL(	GuiDisplayable *, pLeftScroller_ );	\
		CB_DEPIMPL(	GuiDisplayable *, pRightScroller_ ); \
		CB_DEPIMPL(	GuiDisplayable *, pTwoWayScroller_ ); \
		CB_DEPIMPL(	Coords::size_type, leftScrollerIndex_ ); \
		CB_DEPIMPL(	Coords::size_type, rightScrollerIndex_ ); \
		CB_DEPIMPL(	Coords::size_type, twoWayScrollerIndex_ );	\
		CB_DEPIMPL(	bool, canScrollLeft_ );	\
		CB_DEPIMPL(	bool, canScrollRight_ ); \
		CB_DEPIMPL(	unsigned, offset_ ); \
		CB_DEPIMPL(	unsigned, scrollIncrement_ );

//////////////////////////////////////////////////////////////////////

GuiIconSequence::GuiIconSequence( GuiDisplayable * pParent,
									const Gui::Box& rel,
									const Coords& c )
: GuiDisplayable( pParent, rel ),
  pImpl_( NULL )
{
	pImpl_ = _NEW( GuiIconSequenceImpl( c ) );

	CB_GUIICONSEQUENCE_DEPIMPL();

	//activeChildren_,
	//allocatedPositions_,
	//fixedChildren_ constructed in ctor of GuiIconSequenceImpl
	nFixedChildren_ = 0;
	coords_ = c;
	isDoingDisplay_ = false;
}

GuiIconSequence::~GuiIconSequence()
{
	_DELETE( pImpl_ );
}

void GuiIconSequence::doRemoveChild( GuiDisplayable * pChild )
{
	PRE( hasChild( pChild ) );
	unfixChild( pChild );
}

//////////////////////////////////////////////////////////////////////

const GuiIconSequence::Coords&
GuiIconSequence::coords() const
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	return coords_;
}

void GuiIconSequence::updateMetrics()
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	nFixedChildren_ = 0;
	unsigned nActiveChildren = 0;
	for( Children::iterator i = children().begin(); i != children().end(); ++i )
	{
		if( not isScroller( *i ) and (*i)->isEligableForVisibility() )
		{
			Coords::size_type fixIndex;
			if( isFixedChild( *i, &fixIndex ) )
				++nFixedChildren_;
			else
				++nActiveChildren;
		}
	}

	doUpdateMetrics( nFixedChildren_, nActiveChildren );
}

void GuiIconSequence::update()
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	activeChildren_.erase( activeChildren_.begin(), activeChildren_.end() );

	nFixedChildren_ = 0;
	allocatedPositions_.clear();
	for( Children::iterator i = children().begin(); i != children().end(); ++i )
	{
		if( not isScroller( *i ) and (*i)->isEligableForVisibility() )
		{
			Coords::size_type fixIndex;
			if( isFixedChild( *i, &fixIndex ) )
			{
				++nFixedChildren_;
				ASSERT( fixIndex < allocatedPositions_.size(), "index out of range" );
				allocatedPositions_[ fixIndex ] = true;
				(*i)->isVisible( true );
				// moveChildTo( *i, coords()[ fixIndex ] );
				positionChildRelative( *i, coords()[ fixIndex ] );
			}
			else
				activeChildren_.push_back( *i );
		}
	}

	doUpdate( nFixedChildren_, activeChildren_.size() );
}

bool GuiIconSequence::canScroll() const
{
	return false;
}

void GuiIconSequence::doUpdate( unsigned, unsigned )
{
	// Intentionally Empty
}

void GuiIconSequence::doUpdateMetrics( unsigned, unsigned )
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////

bool GuiIconSequence::isPositionOfScroller( Coords::size_type coordIndex ) const
{
	PRE( coordIndex < coords().size() );
	return false;
}

bool GuiIconSequence::isScroller( GuiDisplayable * pChild ) const
{
	PRE( hasChild( pChild ) );
	return false;
}

unsigned GuiIconSequence::offset() const { return 0; }

bool GuiIconSequence::isDoingDisplay() const
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	return isDoingDisplay_;
}

//////////////////////////////////////////////////////////////////////

void GuiIconSequence::repositionChildren()
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	update();

	ctl_bit_vector allocatedChild( std::max( activeChildren_.size(), allocatedPositions_.size() ) );
	ctl_bit_vector currentAllocatedPositions = allocatedPositions_;

	// place all non-fixed children
	Coords::size_type firstUnavailablePosition = std::min( activeChildren_.size() - offset(), coords().size() );
	for( Coords::size_type i = 0; i<coords().size(); ++i )
	{
		bool iThPositionAvailable = i < firstUnavailablePosition
										and not currentAllocatedPositions[ i ];

		bool isScrollPos = canScroll() and isPositionOfScroller( i );
		if( iThPositionAvailable and not isScrollPos )
		{
			// place next active child in the i-th position
			bool foundChild = false;
			for( Coords::size_type j = std::max( i, _STATIC_CAST(Coords::size_type, offset()) );
                    j<activeChildren_.size() and not foundChild; ++j )
			{
				ASSERT( j < allocatedChild.size(), "index out of range" );
				foundChild = not currentAllocatedPositions[ j - offset() ];
				if( foundChild )
				{
					allocatedChild[ j ] = true;
					ASSERT( allocatedChild[ j ], "logic_error" );
					currentAllocatedPositions[ j - offset() ] = true;
					activeChildren_[ j ]->isVisible( true );
					// moveChildTo( activeChildren_[ j ], coords()[ i ] );
					positionChildRelative( activeChildren_[ j ], coords()[ i ] );
				}
			}
		}
	}

	// disable all unplaced children
	for( Coords::size_type j = 0; j<activeChildren_.size(); ++j )
	{
		ASSERT( j < allocatedChild.size(), "index out of range" );
		bool notAllocated = ( allocatedChild[ j ] == false );
		if( notAllocated )
		{
			ASSERT( j < activeChildren_.size(), "index out of range" );
			activeChildren_[ j ]->isVisible( false );
		}
	}
}

void GuiIconSequence::doDisplay()
{
	CB_GUIICONSEQUENCE_DEPIMPL();

	isDoingDisplay_ = true;
	repositionChildren();
	isDoingDisplay_ = false;
}

/* //////////////////////////////////////////////////////////////// */

bool GuiIconSequence::isFixedPosition( Coords::size_type coordIndex ) const
{
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	bool result = isPositionOfScroller( coordIndex ) or isPositionOfFixedChild( coordIndex );
	POST( iff( result, isPositionOfScroller( coordIndex ) or isPositionOfFixedChild( coordIndex ) ) );
	return result;
}

bool GuiIconSequence::isPositionOfFixedChild( Coords::size_type coordIndex ) const
{
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );

	CB_GUIICONSEQUENCE_DEPIMPL();

	return fixedChildren_[ coordIndex ] != NULL;
}

bool GuiIconSequence::isFixedChild( GuiDisplayable *pChild, Coords::size_type * pResult ) const
{
	PRE( hasChild( pChild ) );
	PRE( not isScroller( pChild ) );

	CB_GUIICONSEQUENCE_DEPIMPL();

	bool result = false;
	for( FixedChildren::size_type i = 0; i < fixedChildren_.size() and not result; ++i )
	{
		result = ( fixedChildren_ [ i ] == pChild );
		if( result )
			*pResult = i;
	}

	return result;
}

void GuiIconSequence::fixChild( GuiDisplayable *pChild, Coords::size_type coordIndex )
{
	PRE( hasChild( pChild ) );
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	PRE( not isScroller( pChild ) );
	PRE( not isFixedPosition( coordIndex ) );
	PRE_DATA( Coords::size_type dummy );
	PRE( not isFixedChild( pChild, &dummy) );

	CB_GUIICONSEQUENCE_DEPIMPL();

	fixedChildren_[ coordIndex ] = pChild;

	POST_DATA( Coords::size_type atIndex );
	POST( isFixedChild( pChild, &atIndex ) and coordIndex == atIndex );
}

void GuiIconSequence::unfixChild( GuiDisplayable *pChild )
{
	PRE( hasChild( pChild ) );
	PRE( not isScroller( pChild ) );

	CB_GUIICONSEQUENCE_DEPIMPL();

	bool found = false;
	FixedChildren::size_type i = 0;
	for( ; i < fixedChildren_.size() and not found; ++i )
	{
		found = ( fixedChildren_[ i ] == pChild );
		if( found )
			fixedChildren_[ i ] = NULL;
	}

	POST_DATA( Coords::size_type dummy );
	POST( not isFixedChild( pChild, &dummy ) );
}

void GuiIconSequence::fixScroller( GuiDisplayable *pChild, Coords::size_type coordIndex )
{
	PRE( hasChild( pChild ) );
	PRE( isScroller( pChild ) );
	PRE(  pChild->isEligableForVisibility() );
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	PRE( not isPositionOfFixedChild( coordIndex ) );

	CB_GUIICONSEQUENCE_DEPIMPL();

	ASSERT( coordIndex < allocatedPositions_.size(), "index out of range" );
	allocatedPositions_[ coordIndex ] = true;
	pChild->isVisible( true );
	// moveChildTo( pChild, coords()[ coordIndex ] );
	positionChildRelative( pChild, coords()[ coordIndex ] );
}

void GuiIconSequence::doOutputOperator( ostream& o ) const
{
	CB_GUIICONSEQUENCE_DEPIMPL();

    o << '\t' << "fixedChildren_" << " start" << std::endl;
	for( FixedChildren::size_type i=0; i< fixedChildren_.size(); ++i )
	{
		o << "\t\t";
		if( fixedChildren_[ i ] == NULL )
			o << "NULL";
		else
			o << (void *)fixedChildren_[ i ];
		o << std::endl;
	}

    o << '\t' << "activeChildren_ " << " start" << std::endl;
	for( ActiveChildren::const_iterator i=activeChildren_.begin(); i != activeChildren_.end(); ++i )
	{
		o << "\t\t";
		if( *i == NULL )
			o << "NULL";
		else
			o << (void *)*i;
		o << std::endl;
	}
	o << '\t' << "activeChildren_ " << " end" << std::endl;

    o << '\t' << "allocatedPositions_ " << " start" << std::endl;
	for( unsigned i=0; i<allocatedPositions_.size(); ++i )
		o << "\t\t" << ( allocatedPositions_[ i ] ? "true" : "false" ) << std::endl;
    o << '\t' << "allocatedPositions_ " << " end" << std::endl;

    o << '\t' << "children() " << " start" << std::endl;
	for( Children::const_iterator i=children().begin(); i != children().end(); ++i )
	{
		o << "\t\t";
		o << "id : " << (void*)*i << ' ';
		o << "position : " << (*i)->absoluteCoord() << ' ';
		o << ( (*i)->isVisible() ? "active " : "inactive " );
		if( isScroller( *i ) )
			o << "scroller ";
		else
		{
			o << "non-scroller ";
			Coords::size_type fixIndex;
			if(  isFixedChild( *i, &fixIndex ) )
				o << "fixed at " << fixIndex;
			else
				o << "unfixed ";
		}

		o << std::endl;
	}
    o << '\t' << "children() " << " end" << std::endl;
}

ostream& operator <<( ostream& o, const GuiIconSequence& s )
{
    o << "GuiIconSequence " << (void*)&s << " start" << std::endl;
	s.doOutputOperator( o );
    return o << "GuiIconSequence " << (void*)&s << " end" << std::endl;
}

//////////////////////////////////////////////////////////////////////

// static
GuiIconSequence::Coords
GuiIconSequence::translateCoords( const Coords& c, const Gui::Coord& rel )
// return c translated by rel
{
	Coords result = c;
	for( Coords::iterator i = result.begin(); i != result.end(); ++i )
		(*i) = Gui::Coord( (*i).x() + rel.x(), (*i).y() + rel.y() );

	return result;
}

// static
GuiIconSequence::Coords
GuiIconSequence::fitCoordsToBox( unsigned boxWidth,
									unsigned boxHeight,
									unsigned iconWidth,
									unsigned iconHeight )
{
	unsigned nColumns = boxWidth / iconWidth;
	unsigned nRows = boxHeight / iconHeight;
	return arrayCoords( iconWidth, iconHeight, nRows, nColumns );
}

// static
GuiIconSequence::Coords
GuiIconSequence::arrayCoords( unsigned iconWidth, unsigned iconHeight,
										unsigned nRows, unsigned nColumns )
{
	Coords result;
    result.reserve( nRows * nColumns );

	for( unsigned i = 0; i<nRows; ++i )
		for( unsigned j = 0; j<nColumns; ++j )
			result.push_back( Gui::Coord( j * iconWidth, i * iconHeight ) );

	return result;
}

// static
Gui::Box GuiIconSequence::arrayDimensions( unsigned iconWidth, unsigned iconHeight,
										unsigned nRows, unsigned nCols )
{
	return Gui::Box( 0, 0, nCols * ( 1 + iconWidth ), nRows * ( 1 + iconHeight ) );
}

// static
Gui::Box
GuiIconSequence::coordBoundary( const Coords& c, unsigned iconWidth, unsigned iconHeight )
// bounding box for a seqeunce of coords
{
	PRE( not c.empty() );
	Gui::Box result( *c.begin(), 1, 1 );

	for( Coords::const_iterator i=c.begin(); i != c.end(); ++i )
	{
		if( (*i).x() < result.minCorner().x() )
			result.minCorner( Gui::Coord( (*i).x(), result.minCorner().y() ) );
		if( (*i).x() > result.maxCorner().x() )
			result.maxCorner( Gui::Coord( (*i).x(), result.maxCorner().y() ) );

		if( (*i).y() < result.minCorner().y() )
			result.minCorner( Gui::Coord( result.minCorner().x(), (*i).y() ) );
		if( (*i).y() > result.maxCorner().y() )
			result.maxCorner( Gui::Coord( result.maxCorner().x(), (*i).y() ) );
	}

	result.maxCorner( Gui::Coord( result.maxCorner().x() + iconWidth, result.maxCorner().y() + iconHeight ) );
	return result;
}

//////////////////////////////////////////////////////////////////////

GuiScrollableIconSequence::GuiScrollableIconSequence( GuiDisplayable * pParent,
									const Gui::Box& rel,
									const Coords& coords,
									unsigned scrollIncrement )
: GuiIconSequence( pParent, rel, coords ),
  pImpl_( NULL )
{
	pImpl_ = _NEW( GuiScrollableIconSequenceImpl );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	pLeftScroller_ = NULL;
	pRightScroller_ = NULL;
	pTwoWayScroller_ = NULL;
	leftScrollerIndex_ = 0;
	rightScrollerIndex_ = 0;
	twoWayScrollerIndex_ = 0;
	canScrollLeft_ = false;
	canScrollRight_ = false;
	offset_ = 0;
	scrollIncrement_ = scrollIncrement;

	PRE( scrollIncrement < coords.size() );
}

GuiScrollableIconSequence::~GuiScrollableIconSequence()
{
	_DELETE( pImpl_ );
}

void GuiScrollableIconSequence::doRemoveChild( GuiDisplayable * pChild )
{
	PRE( hasChild( pChild ) );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	if( pChild == pLeftScroller_ )
		pLeftScroller_ = NULL;
	else if( pChild == pRightScroller_ )
		pRightScroller_ = NULL;
	else if( pChild == pTwoWayScroller_ )
		pTwoWayScroller_ = NULL;

	GuiIconSequence::doRemoveChild( pChild );
}

//////////////////////////////////////////////////////////////////////

unsigned GuiScrollableIconSequence::offset() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return offset_;
}

unsigned GuiScrollableIconSequence::scrollIncrement() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return scrollIncrement_;
}

bool GuiScrollableIconSequence::canScroll() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return canScrollLeft_ or canScrollRight_;
}

bool GuiScrollableIconSequence::canScrollLeft() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	GuiScrollableIconSequence * pNonConstThis = (GuiScrollableIconSequence *)this;
	pNonConstThis->updateMetrics();
	return canScrollLeft_;
}

bool GuiScrollableIconSequence::canScrollRight() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	GuiScrollableIconSequence * pNonConstThis = (GuiScrollableIconSequence *)this;
	pNonConstThis->updateMetrics();
	return canScrollRight_;
}

bool GuiScrollableIconSequence::isPositionOfScroller( Coords::size_type coordIndex ) const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	PRE( coordIndex < coords().size() );
	return pLeftScroller_ != NULL and coordIndex == leftScrollerIndex_
			or pRightScroller_ != NULL and coordIndex == rightScrollerIndex_
				or pTwoWayScroller_ != NULL and coordIndex == twoWayScrollerIndex_;
}

bool GuiScrollableIconSequence::hasLeftScroller() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return pLeftScroller_ != NULL or pTwoWayScroller_ != NULL;
}

bool GuiScrollableIconSequence::hasRightScroller() const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return pRightScroller_ != NULL or pTwoWayScroller_ != NULL;
}

bool GuiScrollableIconSequence::isScroller( GuiDisplayable *pChild ) const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	return pLeftScroller_ != NULL and pChild == pLeftScroller_
			or pRightScroller_ != NULL and pChild == pRightScroller_
				or pTwoWayScroller_ != NULL and pChild == pTwoWayScroller_;
}

//////////////////////////////////////////////////////////////////////

void GuiScrollableIconSequence::leftScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex )
{
	PRE( hasChild( pNewScroller ) );
	PRE_DATA( Coords::size_type dummy );
	PRE( not isFixedChild( pNewScroller, &dummy) );
	PRE( not isScroller( pNewScroller ) );
	PRE( not hasLeftScroller() );
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	PRE( not isFixedPosition( coordIndex ) );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	pLeftScroller_ = pNewScroller;
	leftScrollerIndex_ = coordIndex;

	POST( hasLeftScroller() );
	POST( isScroller( pNewScroller ) );
	POST( isPositionOfScroller( coordIndex ) );
}

void GuiScrollableIconSequence::rightScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex )
{
	PRE( hasChild( pNewScroller ) );
	PRE_DATA( Coords::size_type dummy );
	PRE( not isFixedChild( pNewScroller, &dummy) );
	PRE( not isScroller( pNewScroller ) );
	PRE( not hasLeftScroller() );
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	PRE( not isFixedPosition( coordIndex ) );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	pRightScroller_ = pNewScroller;
	rightScrollerIndex_ = coordIndex;

	POST( hasRightScroller() );
	POST( isScroller( pNewScroller ) );
	POST( isPositionOfScroller( coordIndex ) );
}

void GuiScrollableIconSequence::twoWayScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex )
{
	PRE( hasChild( pNewScroller ) );
	PRE_DATA( Coords::size_type dummy );
	PRE( not isFixedChild( pNewScroller, &dummy) );
	PRE( not isScroller( pNewScroller ) );
	PRE( not hasLeftScroller() );
	PRE( not hasRightScroller() );
	PRE_INFO( coordIndex );
	PRE_INFO( coords().size() );
	PRE( coordIndex < coords().size() );
	PRE( not isFixedPosition( coordIndex ) );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	pTwoWayScroller_ = pNewScroller;
	twoWayScrollerIndex_ = coordIndex;

	POST( hasLeftScroller() );
	POST( hasRightScroller() );
	POST( isScroller( pNewScroller ) );
	POST( isPositionOfScroller( coordIndex ) );
}

//////////////////////////////////////////////////////////////////////

void GuiScrollableIconSequence::scrollLeft()
{
	PRE( canScrollLeft() );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	offset_ = offset_ < scrollIncrement_ ? 0 : offset_ - scrollIncrement_;
	update();

 	changed();
}

void GuiScrollableIconSequence::scrollRight()
{
	PRE( canScrollRight() );

	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	offset_ += scrollIncrement_;
	update();

	changed();
}

//////////////////////////////////////////////////////////////////////

void GuiScrollableIconSequence::doUpdate( unsigned nFixedChildren, unsigned nActiveChildren )
{
	GuiScrollableIconSequence::doUpdateMetrics( nFixedChildren, nActiveChildren );
	positionScrollers();
}

void GuiScrollableIconSequence::doUpdateMetrics( unsigned nFixedChildren, unsigned nActiveChildren )
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	unsigned nActiveIconPositions = coords().size() - nFixedChildren;
	unsigned maxOffset = ( nActiveChildren < nActiveIconPositions )
							? 0 : nActiveChildren - nActiveIconPositions + scrollIncrement_ - 1;

	if( offset_ > maxOffset )
		offset_ = maxOffset;

	offset_ -= offset_ % scrollIncrement_; // make sure offset is multiple of scroll increament.

	canScrollLeft_ = offset_ > 0;
	if( canScrollLeft_ )
	{
		if( pTwoWayScroller_ != NULL )
			--nActiveIconPositions;
		else
			nActiveIconPositions -= 2;
	}

	canScrollRight_ = nActiveChildren - offset_ > nActiveIconPositions;
}

//////////////////////////////////////////////////////////////////////

void GuiScrollableIconSequence::positionScrollers()
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	if( pTwoWayScroller_ != NULL )
	{
		if( canScrollLeft_ or canScrollRight_ )
			fixScroller( pTwoWayScroller_, twoWayScrollerIndex_ );
		else
			pTwoWayScroller_->isVisible( false );
	}
	else
	{
		if( canScrollLeft_ or canScrollRight_ )
		{
			if( pLeftScroller_ != NULL )
				fixScroller( pLeftScroller_, leftScrollerIndex_ );
			if( pRightScroller_ != NULL )
				fixScroller( pRightScroller_, rightScrollerIndex_ );
		}
		else
		{
			if( pLeftScroller_ != NULL )
				pLeftScroller_->isVisible( false );
			if( pRightScroller_ != NULL )
				pRightScroller_->isVisible( false );
		}
	}
}

void GuiScrollableIconSequence::doOutputOperator( ostream& o ) const
{
	CB_GUISCROLLABLEICONSEQUENCE_DEPIMPL();

	GuiIconSequence::doOutputOperator( o );
	o << "offset_ " << offset_ << std::endl;

	if( pLeftScroller_ != NULL )
	{
		o << "pLeftScroller_ " << (void *)pLeftScroller_ << std::endl;
		o << "leftScrollerIndex_ " << leftScrollerIndex_ << std::endl;
	}
	if( pRightScroller_ != NULL )
	{
		o << "pRightScroller_ " << (void *)pRightScroller_ << std::endl;
		o << "rightScrollerIndex_ " << rightScrollerIndex_ << std::endl;
	}
	if( pTwoWayScroller_ != NULL )
	{
		o << "pTwoWayScroller_ " << (void *)pTwoWayScroller_ << std::endl;
		o << "twoWayScrollerIndex_ " << twoWayScrollerIndex_ << std::endl;
	}

	o << "canScrollLeft_ " << ( canScrollLeft_ ? "true" : "false" ) << std::endl;
	o << "canScrollRight_ " << ( canScrollRight_ ? "true" : "false" ) << std::endl;
	o << "scrollIncrement_ " << scrollIncrement_ << std::endl;
}

//////////////////////////////////////////////////////////////////////

/* End ICONSEQ.CPP **************************************************/
