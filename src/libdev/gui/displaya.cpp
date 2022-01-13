/*
 * D I S P L A Y A B L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#define CB_GUIDISPLAYABLE_DEPIMPL() \
		CB_DEPIMPL( GuiDisplayable*, pParent_ ); \
		CB_DEPIMPL( Gui::Box, relativeBox_ ); \
		CB_DEPIMPL( Gui::Box, absoluteBox_ ); \
		CB_DEPIMPL( bool, isVisible_ ); \
		CB_DEPIMPL( int, changed_ ); \
		CB_DEPIMPL( bool, redrawEveryFrame_ ); \
		CB_DEPIMPL_ARRAY( Children, children_ ); \
		CB_DEPIMPL( Children, allChildren_ ); \
		CB_DEPIMPL( bool, useFastSecondDisplay_ );

#include "gui/displaya.hpp"
#include "gui/internal/displayi.hpp"
#include "ctl/vector.hpp"
#include "ctl/algorith.hpp"
#include "mathex/vec2.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"
#include "device/butevent.hpp"

//////////////////////////////////////////////////////////////////////

GuiDisplayable::GuiDisplayable( GuiDisplayable * pParent, const Gui::Boundary& relativeBoundary )
:	pImpl_( _NEW( GuiDisplayableImpl ) )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( pParent != NULL );

	pParent_ 			= pParent;
  	relativeBox_ 		= relativeBoundary;
  	absoluteBox_ 		= translateBox( relativeBoundary, pParent->absoluteCoord() );
  	isVisible_ 			= true;
	redrawEveryFrame_ 	= false;
	useFastSecondDisplay_= true;
	changed( true );

	pParent_->addChild( this, LAYER1 );
	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		children_[layer].reserve( 2 );
	}
	allChildren_.reserve( 4 );

	POST_INFO( pParent->absoluteBoundary() );
	POST_INFO( absoluteBoundary() );
	//POST( pParent->absoluteBoundary().contains( absoluteBoundary() ) ); //TODO fails by few pixels sometimes
	POST ( useFastSecondDisplay() );
}

GuiDisplayable::GuiDisplayable( GuiDisplayable * pParent, const Gui::Boundary& relativeBoundary, Layer myLayer )
:	pImpl_( _NEW( GuiDisplayableImpl ) )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( pParent != NULL );

	pParent_ 			= pParent;
  	relativeBox_ 		= relativeBoundary;
  	absoluteBox_ 		= translateBox( relativeBoundary, pParent->absoluteCoord() );
  	isVisible_ 			= true;
	redrawEveryFrame_ 	= false;
	useFastSecondDisplay_=true;
	changed( true );

	pParent_->addChild( this, myLayer );
	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		children_[layer].reserve( 2 );
	}
	allChildren_.reserve( 4 );

	POST_INFO( pParent->absoluteBoundary() );
	POST_INFO( absoluteBoundary() );
	POST( pParent->absoluteBoundary().contains( absoluteBoundary() ) );
	POST ( useFastSecondDisplay() );
}

GuiDisplayable::GuiDisplayable( const Gui::Boundary& absBoundary )
:	pImpl_( _NEW( GuiDisplayableImpl ) )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	pParent_ 			= NULL;
  	relativeBox_ 		= absBoundary;
  	absoluteBox_ 		= absBoundary;
  	isVisible_ 			= true;
	redrawEveryFrame_ 	= false;
	useFastSecondDisplay_=true;
	changed( true );

	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		children_[layer].reserve( 2 );
	}
	allChildren_.reserve( 4 );

	POST ( useFastSecondDisplay() );
}

GuiDisplayable::~GuiDisplayable()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	deleteAllChildren();

	if( pParent_ != NULL )
		pParent_->removeChild( this );

	GuiManager::instance().isBeingDeleted( this );

	_DELETE( pImpl_ );
}

//////////////////////////////////////////////////////////////////////

Gui::Coord GuiDisplayable::relativeCoord() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return relativeBox_.minCorner();
}

Gui::Coord GuiDisplayable::absoluteCoord() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return absoluteBox_.minCorner();
}

void GuiDisplayable::relativeCoord( const Gui::Coord& relCoord )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

    // Construct the displacement vector
    MexVec2 diff( relativeBox_.minCorner(), relCoord );
	PRE_INFO( relativeBox_.minCorner() );
	PRE_INFO( relCoord );

	POST_DATA( Gui::Box oldBoundary = absoluteBoundary() );

	PRE_DATA( Gui::Coord c( diff.x(), diff.y() ) );
	PRE_DATA( Gui::Box parentBoundary = isRoot() ? absoluteBoundary() : parent().absoluteBoundary() );
	PRE_DATA( Gui::Box newBoundary = translateBox( absoluteBoundary(), c ) );
	PRE_INFO( c );
	PRE_INFO( absoluteBoundary() );
	PRE_INFO( parentBoundary );
	PRE_INFO( newBoundary );

	Gui::Coord newRelMin = relativeBox_.minCorner();
	newRelMin += diff;
	Gui::Coord newRelMax = relativeBox_.maxCorner();
	newRelMax += diff;
	relativeBox_.corners( newRelMin, newRelMax );

	Gui::Coord absCoord = relativeCoord();
	if( not isRoot() )
		absCoord += parent().absoluteCoord();

	absoluteBox_ = Gui::Box( absCoord, Gui::width( relativeBox_ ), Gui::height( relativeBox_ ) );

	for( Children::iterator i = allChildren_.begin(); i != allChildren_.end(); ++i )
	{
		Gui::Coord childCoord = (*i)->absoluteCoord();
		childCoord += diff;
		positionChildAbsolute( *i, childCoord );
	}
}

void GuiDisplayable::absoluteCoord( const Gui::Coord& absCoord )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE_DATA( Gui::Box newBoundary( absCoord, width(), height() ) );
	PRE_DATA( Gui::Box parentBoundary = isRoot() ? absoluteBoundary() : parent().absoluteBoundary() );
	PRE_INFO( absCoord );
	PRE_INFO( newBoundary );
	PRE_INFO( parentBoundary );
	PRE( implies( not isRoot() and isVisible(), parentBoundary.contains( newBoundary ) ) );

    // Construct the displacement vector
    MexVec2 diff( absoluteBox_.minCorner(), absCoord );

	Gui::Coord newAbsMin = absoluteBox_.minCorner();
	newAbsMin += diff;

	Gui::Coord newAbsMax = absoluteBox_.maxCorner();
	newAbsMax += diff;
	absoluteBox_.corners( newAbsMin, newAbsMax );

	Gui::Coord relCoord = absCoord;
	if ( not isRoot() )
		relCoord -= parent().absoluteCoord();
	relativeBox_ = Gui::Box( relCoord, Gui::width( absoluteBox_ ), Gui::height( absoluteBox_ ) );

	for( Children::iterator i = allChildren_.begin(); i != allChildren_.end(); ++i )
	{
		Gui::Coord childCoord = (*i)->absoluteCoord();
		childCoord += diff;
		positionChildAbsolute( *i, childCoord );
	}

	POST( absoluteCoord() == absCoord );
	POST( absoluteBoundary() == Gui::Box( absCoord, width(), height() ) );
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::positionChildAbsolute( GuiDisplayable * pChild, const Gui::Coord& absCoord )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( pChild != NULL );
	PRE( hasChild( pChild ) );

	pChild->absoluteCoord( absCoord );

	POST( pChild->absoluteCoord() == absCoord );
}

void GuiDisplayable::positionChildRelative( GuiDisplayable * pChild, const Gui::Coord& relCoord )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( pChild != NULL );
	PRE( hasChild( pChild ) );

	pChild->relativeCoord( relCoord );

	POST( pChild->relativeCoord() == relCoord );
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::isVisible( bool visible )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool makeVisible = visible and isEligableForVisibility() and not isVisible_;

	if( makeVisible )
		changed( true );

	isVisible_ = visible;
}

//////////////////////////////////////////////////////////////////////

bool GuiDisplayable::isVisible() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool parentVisible = true;

	// Check parents visibility
	if ( pParent_ )
		parentVisible = pParent_->isVisible();

	// Return true if "this" is visible and my parent(s) is visible	etc.
	return isVisible_ and parentVisible and isEligableForVisibility();
}

// virtual
bool GuiDisplayable::isEligableForVisibility() const
{
	return true;
}

void GuiDisplayable::changed( bool change )
{
	CB_DEPIMPL( int, changed_ );

	if ( change )
	{
		// This indicates that the gui displayable needs to be rendered twice ( back buffer and front buffer )
		if ( useFourTimesRender() )
		{
			changed_ = 4;
		}
		else
		{
			changed_ = 2;
		}
	}
	else if ( changed_ != 0 )
	{
		--changed_;
	}
}

bool GuiDisplayable::hasChanged() const
{
	CB_DEPIMPL( int, changed_ );

	return changed_ != 0;
}

bool GuiDisplayable::isRoot() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return pParent_ == NULL;
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::addChild( GuiDisplayable* pNewChild, Layer childsLayer )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( not hasChild( pNewChild ) );
	PRE_INFO( absoluteBoundary() );
	PRE_INFO( pNewChild->absoluteBoundary() );

	allChildren_.push_back( pNewChild );
	children_[childsLayer].push_back( pNewChild );

	POST( hasChild( pNewChild ) );
}

// virtual
void GuiDisplayable::doRemoveChild( GuiDisplayable* /*pChild*/ )
{}

void GuiDisplayable::removeChild( GuiDisplayable* pChild )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( hasChild( pChild ) );

	Children::iterator i = find( allChildren_.begin(), allChildren_.end(), pChild );

	if ( i != allChildren_.end() )
	{
		doRemoveChild( pChild );
		allChildren_.erase( i );
	}

	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		Children::iterator i = find( children_[layer].begin(), children_[layer].end(), pChild );

		if ( i != children_[layer].end() )
		{
			children_[layer].erase( i );
			break;
		}
	}

	POST( not hasChild( pChild ) );
}

void GuiDisplayable::deleteChild( GuiDisplayable * pChild )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	Children::iterator i = find( allChildren_.begin(), allChildren_.end(), pChild );

	if ( i != allChildren_.end() )
	{
		_DELETE( *i ); // Child automatically removes itself from allChildren_ collection on deletion
	}

	POST( not hasChild( pChild ) );
}

void GuiDisplayable::deleteAllChildren()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	// This works because on deletion the child will remove itself from the parents child collection.
	while( allChildren_.size() != 0 )
		_DELETE( *allChildren_.begin() );
}

//////////////////////////////////////////////////////////////////////

GuiDisplayable::Children& GuiDisplayable::children()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return allChildren_;
}

const GuiDisplayable::Children& GuiDisplayable::children() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return allChildren_;
}

GuiDisplayable::Children& GuiDisplayable::children( Layer layer )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return children_[layer];
}

const GuiDisplayable::Children& GuiDisplayable::children( Layer layer ) const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return children_[layer];
}

//////////////////////////////////////////////////////////////////////

bool GuiDisplayable::hasChild( const GuiDisplayable* pChild ) const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool result = false;

	Children::iterator i = find( allChildren_.begin(), allChildren_.end(), pChild );

	if ( i != allChildren_.end() )
	{
		result = true;
	}

	return result;
}

bool GuiDisplayable::recursivelyHasChild( const GuiDisplayable* pChild ) const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool found = false;

	if ( this == pChild )
	{
		found = true;
	}

	for ( Children::iterator i = allChildren_.begin(); not found and i != allChildren_.end(); ++i )
	{
		found = recursivelyHasChild( *i );
	}

	return found;
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::display()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	if ( isVisible() ) // No point continuing if this displayable is invisible
	{
		// If this has changed and needs rendering then draw it followed by all it's children
		if( hasChanged() or redrawEveryFrame() )
		{
			if ( secondDisplay() and useFastSecondDisplay() and not redrawEveryFrame() )
			{
				fastDisplay();
			}
			else
			{
				normalDisplay();
			}

			changed( false );
		}
		else
		{
			// Check children to see if they need displaying
			for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
			{
				for ( Children::iterator i = children_[layer].begin(); i != children_[layer].end(); ++i )
				{
					// Call childs display method.
					(*i)->display();
				}
			}
		}
	}
}

void GuiDisplayable::normalDisplay()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	doDisplay();

	// Display all children
	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		for ( Children::iterator i = children_[layer].begin(); i != children_[layer].end(); ++i )
		{
			if ( (*i)->isVisible() )
			{
				// If the parent ( this ) has just changed ( i.e. this is the first display out of two )
				// then tell all children to display for the next 2 frames.
				if ( firstDisplay() or redrawEveryFrame() )
				{
					(*i)->changed( true );
				}
				// Call childs display method.
				(*i)->display();
			}
		}
	}
}

void GuiDisplayable::fastDisplay()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	// Blit from front to back buffer.
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	GuiPainter::instance().blit( frontBuffer, absoluteBoundary(), absoluteBoundary().minCorner() );

	// Display children if necessary
	fastDisplayChildren();
}

void GuiDisplayable::fastDisplayChildren()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	for ( Layer layer = LAYER1; layer < NUMLAYERS; ++((int&)layer) )
	{
		for ( Children::iterator i = children_[layer].begin(); i != children_[layer].end(); ++i )
		{
			GuiDisplayable* pChild = (*i);
			if ( pChild->isVisible() )
			{
   				if ( 	pChild->firstDisplay() or
   						pChild->redrawEveryFrame() or
   						( pChild->secondDisplay() and not pChild->useFastSecondDisplay() ) )
				{
					// Call childs display method.
					pChild->display();
				}
				else
				{
					// Don't render child ( taken care of by fast display of parent! )
					pChild->changed( false );
					pChild->fastDisplayChildren();
				}
			}
		}
	}
}

bool GuiDisplayable::firstDisplay() const
{
	CB_DEPIMPL( int, changed_ );

	return changed_ > 1;
}

bool GuiDisplayable::secondDisplay() const
{
	CB_DEPIMPL( int, changed_ );

	return changed_ == 1;
}

//////////////////////////////////////////////////////////////////////

bool GuiDisplayable::empty() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return allChildren_.empty();
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::filledRectangle( const Gui::Box& rel, const Gui::Colour& c )
{
	GuiPainter::instance().filledRectangle( translate( rel ), c );
}

void GuiDisplayable::hollowRectangle( const Gui::Box& rel,
								const Gui::Colour& c,
								unsigned thickness )
{
	GuiPainter::instance().hollowRectangle( translate( rel ), c, thickness );
}

void GuiDisplayable::fill( const Gui::Colour& c )
{
	GuiPainter::instance().filledRectangle( absoluteBoundary(), c );
}

//////////////////////////////////////////////////////////////////////

Gui::Coord
GuiDisplayable::translate( Gui::XCoord x, Gui::YCoord y ) const
{
	return Gui::Coord( absoluteCoord().x() + x, absoluteCoord().y() + y );
}

Gui::Coord
GuiDisplayable::translate( const Gui::Coord& r ) const
{
	return Gui::Coord( absoluteCoord().x() + r.x(), absoluteCoord().y() + r.y() );
}

Gui::Box
GuiDisplayable::translate( const Gui::Box& b ) const
{
	return GuiDisplayable::translateBox( b, absoluteCoord() );
}

//////////////////////////////////////////////////////////////////////

void GuiDisplayable::line( const Gui::Coord& rel1,
								const Gui::Coord& rel2,
								const Gui::Colour& c,
								unsigned thickness )
{
//	PRE( absoluteBoundary().contains( translate( rel1 ) ) );
//	PRE( absoluteBoundary().contains( translate( rel2 ) ) );

	GuiPainter::instance().line( translate( rel1 ),
									translate( rel2 ),
									c, thickness );
}

void GuiDisplayable::horizontalLine( const Gui::Coord& rel1,
											unsigned length,
											const Gui::Colour& c,
											unsigned thickness )
{
//	PRE( absoluteBoundary().contains( translate( rel1 ) ) );
//	PRE( absoluteBoundary().contains( translate( rel1.x() + length, rel1.y() ) ) );

	GuiPainter::instance().horizontalLine( translate( rel1 ),
												length, c, thickness );
}

void GuiDisplayable::verticalLine( const Gui::Coord& rel1,
										unsigned height,
										const Gui::Colour& c,
										unsigned thickness )
{
//	PRE( absoluteBoundary().contains( translate( rel1 ) ) );
//	PRE( absoluteBoundary().contains( translate( rel1.x(), rel1.y() + height ) ) );

	GuiPainter::instance().verticalLine( translate( rel1 ),
												height, c, thickness );

}

void GuiDisplayable::bevel( const Gui::Box& rel,
				   				unsigned thickness,
								const Gui::Colour& hiCol,
								const Gui::Colour& loCol )
{
	GuiPainter::instance().bevel( translate( rel ),
									thickness,
									hiCol,
									loCol );
}

void GuiDisplayable::text( const Gui::Coord& c, const string& theText, const Gui::Colour& col )
{
	GuiPainter::instance().text( translate( c ), theText, col );
}

void GuiDisplayable::rightAlignText( const Gui::Coord& c, const string& theText, const Gui::Colour& col )
{
	GuiPainter::instance().rightAlignText( translate( c ), theText, col );
}

//////////////////////////////////////////////////////////////////////

GuiDisplayable& GuiDisplayable::parent()
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( not isRoot() );
	return *pParent_;
}

const GuiDisplayable& GuiDisplayable::parent() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	PRE( not isRoot() );
	return *pParent_;
}

//////////////////////////////////////////////////////////////////////

const Gui::Boundary& GuiDisplayable::absoluteBoundary() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

 	return absoluteBox_;
}

const Gui::Boundary& GuiDisplayable::relativeBoundary() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return relativeBox_;
}

Gui::Boundary GuiDisplayable::relativeBoundary( const GuiDisplayable& ancestor ) const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	Gui::Coord coord = relativeCoord( ancestor );

	Gui::Box box( coord, width(), height() );

	return box;
}

Gui::Coord GuiDisplayable::relativeCoord( const GuiDisplayable& ancestor ) const
{
	PRE( not isRoot() );

	Gui::Coord coord = relativeCoord();

	if ( &parent() != &ancestor )
	{
		Gui::Coord parentCoord = parent().relativeCoord( ancestor );
		coord.x( coord.x() + parentCoord.x() );
		coord.y( coord.y() + parentCoord.y() );
	}

	return coord;
}

unsigned GuiDisplayable::width() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return relativeBox_.maxCorner().x()
			- relativeBox_.minCorner().x();
}

unsigned GuiDisplayable::height() const
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	return relativeBox_.maxCorner().y()
			- relativeBox_.minCorner().y();
}

//////////////////////////////////////////////////////////////////////

bool GuiDisplayable::contains( Gui::XCoord x, Gui::YCoord y ) const
{
	return contains( Gui::Coord( x, y ) );
}

// virtual
bool GuiDisplayable::contains( const Gui::Coord& c ) const
{
	if (c.x() >= absoluteBoundary().minCorner().x() + width() or
		c.y() >= absoluteBoundary().minCorner().y() + height() )
		return false;

	return absoluteBoundary().contains( c );
}

//////////////////////////////////////////////////////////////////////

bool GuiDisplayable::doHandleKeyEvent( const GuiKeyEvent& gke )
{
	PRE( gke.buttonEvent().isKeyEvent() );

	// Default implementation does not "use" the GuiKeyEvent therefore false is returned.
	return false;
}

bool GuiDisplayable::doHandleCharEvent( const GuiCharEvent& gce )
{
	PRE( gce.isCharEvent() );

	// Default implementation does not "use" the GuiCharEvent therefore false is returned.
	return false;
}

void GuiDisplayable::doHandleMouseClickEvent( const GuiMouseEvent& )
{
	// Intentionally Empty
}

void GuiDisplayable::doHandleMouseEnterEvent( const GuiMouseEvent& )
{
	// Intentionally Empty
}

void GuiDisplayable::doHandleMouseExitEvent( const GuiMouseEvent& )
{
	// Intentionally Empty
}

void GuiDisplayable::doHandleMouseScrollEvent( const GuiMouseEvent& )
{
    // Intentionally Empty
}

void GuiDisplayable::doHandleContainsMouseEvent( const GuiMouseEvent& )
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////

// static
Gui::Box GuiDisplayable::translateBox( const Gui::Box& b, const Gui::Coord& c )
{
	MexVec2 minC = b.minCorner();
	minC += c;
	MexVec2 maxC = b.maxCorner();
	maxC += c;
	return Gui::Box( minC, maxC );
}

bool GuiDisplayable::innermostContaining( const Gui::Coord& c, GuiDisplayable** ppResult )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool found = false;

	// If we are visible and the point is contained in the boundary then we have found
	// a gui displayable that contains the mouse.
	if ( isVisible() and absoluteBoundary().contains( c ) )
	{
		for ( Layer layer = NUMLAYERS; layer != LAYER1 and not found; )
		{
			--((int&)layer);

			// Check to see if any of the children contain the mouse pointer
			for ( Children::iterator i = children_[layer].begin(); not found and i != children_[layer].end(); ++i )
			{
				found = (*i)->innermostContaining( c, ppResult );
			}
		}

		// No children contain coord therefore we are most derived displayable containing coord.
		if ( not found )
		{
			*ppResult = this;
			found = true;
		}
	}

	return found;
}

void GuiDisplayable::changed()
{
	changed( true );
}

void GuiDisplayable::setLayer( Layer layer )
{
	PRE( not isRoot() );

	parent().removeChild( this );
	parent().addChild( this, layer );
}

bool GuiDisplayable::redrawEveryFrame()	const
{
	CB_DEPIMPL( bool, redrawEveryFrame_ );

	return redrawEveryFrame_;
}

void GuiDisplayable::redrawEveryFrame( bool redraw )
{
	CB_DEPIMPL( bool, redrawEveryFrame_ );

	redrawEveryFrame_ = redraw;
}

bool GuiDisplayable::useFastSecondDisplay() const
{
	CB_DEPIMPL( bool, useFastSecondDisplay_ );

	return useFastSecondDisplay_;
}

void GuiDisplayable::useFastSecondDisplay( bool fast )
{
	CB_DEPIMPL( bool, useFastSecondDisplay_ );

	useFastSecondDisplay_ = fast;
}


#ifndef _PRODUCTION_RELEASE
//virtual
const char* GuiDisplayable::description() const
{
	return "GuiDisplayable";
}
#endif


bool GuiDisplayable::innermostContainingCheckProcessesMouseEvents( const Gui::Coord& c, GuiDisplayable** ppResult )
{
	CB_GUIDISPLAYABLE_DEPIMPL();

	bool found = false;

	// If we are visible and the point is contained in the boundary then we have found
	// a gui displayable that contains the mouse.
	if ( isVisible() and absoluteBoundary().contains( c ) and processesMouseEvents() )
	{
		for ( Layer layer = NUMLAYERS; layer != LAYER1 and not found; )
		{
			--((int&)layer);

			// Check to see if any of the children contain the mouse pointer
			for ( Children::iterator i = children_[layer].begin(); not found and i != children_[layer].end(); ++i )
			{
				found = (*i)->innermostContainingCheckProcessesMouseEvents( c, ppResult );
			}
		}

		// No children contain coord therefore we are most derived displayable containing coord.
		if ( not found )
		{
			*ppResult = this;
			found = true;
		}
	}

	return found;
}

//virtual
bool GuiDisplayable::processesMouseEvents()	const
{
	return true;
}

//static
bool& GuiDisplayable::useFourTimesRender()
{
	static bool useFourTimesRenderBool = false;
	return useFourTimesRenderBool;
}

//////////////////////////////////////////////////////////////////////

/* End **************************************************************/
