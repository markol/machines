/*
 * D I S P L A Y A B L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

// Note on GuiDisplayables::absoluteBoundary()... 
//
//  +-+
//  | | = one pixel
//  +-+
//
//	A = Coord(0,0)   minCorner
//  B = Coord(4,4)   maxCorner
//
//         	+-+-+-+-+-+
//			|A|#|#|#| |
//			+-+-+-+-+-+
//			|#|#|#|#| |
//			+-+-+-+-+-+
//			|#|#|#|#| |
//			+-+-+-+-+-+
//			| | | | |B|
//			+-+-+-+-+-+
//
// The GuiDisplayable depicted above is 4 pixels wide and 4 pixels high. As you can
// see on the diagram the absoluteBoundary().maxCorner() is positioned at (4,4). Note
// that you should not render anything in the last column or row ( as depicted above ).
// This enables you to, for whatever reason, set up a GuiDisplayable that is of zero width
// and zero height. The coords for this GuiDiplayable would be minCorner (0,0) and 
// maxCorner (0,0). It would not be correct to render anything in the screen pixel (0,0).
// Note also that mouse hit detection does not occur in the last column and last row.
// 
// When constructing a GuiDisplayable the easiest way to remember what coords to set it
// up with is by using width and height info, for example :
//
// GuiDisplayable( parent, Gui::Boundary( topLeftPixel.x(), topLeftPixel.y(), topLeftPixel.x() + width, topLeftPixel.y() + height ) );
//
			
#ifndef _GUI_DISPLAYA_HPP
#define _GUI_DISPLAYA_HPP

#include "stdlib/strfwd.hpp"
#include "gui/gui.hpp"

class GuiDisplayableImpl;
class GuiKeyEvent;
class GuiMouseEvent;
class GuiEvent;
class DevButtonEvent;

typedef DevButtonEvent GuiCharEvent;

template <class T> class ctl_vector;

// GuiDisplayable is the root of the class heirarchy of screen objects. 
// It defines framework protocols for display and responding to
// user input events.

class GuiDisplayable
// cannonical form revoked
{
public:
	enum Layer { LAYER1 = 0, LAYER2 = 1, LAYER3 = 2, LAYER4 = 3, LAYER5 = 4, NUMLAYERS = 5 };
	
	// since subclasses of GuiDisplayable are owned by their parent
	// GuiDisplayable, they should _only_ be allocated on the heap.

	// Layer defaults to 1 ( i.e. background )
	GuiDisplayable( GuiDisplayable* pParent, const Gui::Boundary& relativeBoundary );
	// PRE( pParent != NULL );
	// PRE( pParent->boundary().contains( boundary() ) );
	// POST( not isRoot() );
	// POST( pParent->hasChild( this ) );
	// POST ( useFastSecondDisplay() );

	GuiDisplayable( GuiDisplayable* pParent, const Gui::Boundary& relativeBoundary, Layer );
	// PRE( pParent != NULL );
	// PRE( pParent->boundary().contains( boundary() ) );
	// POST( not isRoot() );
	// POST( pParent->hasChild( this ) );
	// POST ( useFastSecondDisplay() );

	// Render the gui displayable if it is visible and has changed. Should be called on GuiRoot only
	// as the function is recursive.
	void display();
					 
	// True if top level gui displayable
	bool isRoot() const;

	// Get the immediate parent gui displayable
	const GuiDisplayable& parent() const;
	// PRE( not isRoot() );

	// true iff the component has not been marked invisible 
	// by some entity _and_ it deems itself eligible for 
	// visualisation and user interaction
	bool isVisible() const;
	// POST( implies( result, isEligableForVisibility() );
	
	// true iff the component deems itself
	// eligible for visualisation and user interaction,
	// default implementation returns true
	virtual bool isEligableForVisibility() const;

	void isVisible( bool visible );
	// POST( iff( visible and isEligableForVisibility(), isVisible() ) );

	// Returns true is GuiKeyEvent was "used" by this GuiDisplayable
	virtual bool doHandleKeyEvent( const GuiKeyEvent& );
	
	// Returns true is DevButtonEvent was "used" by this GuiDisplayable
	virtual bool doHandleCharEvent( const GuiCharEvent& );
	
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );

	unsigned width() const;
	unsigned height() const;
	
	Gui::Coord relativeCoord() const;
	Gui::Coord absoluteCoord() const;
	Gui::Coord relativeCoord( const GuiDisplayable& ancestor ) const;
	Gui::Boundary relativeBoundary( const GuiDisplayable& ancestor ) const;

	const Gui::Boundary& absoluteBoundary() const;
	
	bool contains( Gui::XCoord x, Gui::YCoord y ) const;
	// POST( iff( result, boundary().contains( Gui::Coord( x, y ) ) ) );

	virtual bool contains( const Gui::Coord& c ) const;
	// POST( iff( result, boundary().contains( c ) ) );

	static Gui::Box translateBox( const Gui::Box&, const Gui::Coord& c );
	// translate b by c
		
	void addChild( GuiDisplayable * pNewChild, Layer );
	// PRE( not hasChild( pNewChild ) );
	// POST( hasChild( pNewChild ) );

	// delete pChild, removing it from the list of children		
	void deleteChild( GuiDisplayable * pChild );
	// PRE( hasChild( pNewChild ) );
	// POST( not hasChild( pChild ) );

	void deleteAllChildren();

	bool hasChild( const GuiDisplayable * ) const;
	bool recursivelyHasChild( const GuiDisplayable * ) const;
	
	typedef ctl_vector< GuiDisplayable * > Children;

	GuiDisplayable& parent();
	// PRE( not isRoot() );

	// call this method when a subclass undergoes a state
	// change which alters the display of the object
	void changed();
	bool hasChanged() const;

	// Indicates that display should be called every frame for this gui displayable
	bool redrawEveryFrame() const;
	void redrawEveryFrame( bool );

	// Does this gui displayable have any children.
	bool empty() const;

	// Iterate through children until most derived child containing coord is found.
	bool innermostContaining( const Gui::Coord& c, GuiDisplayable** ppResult );
   	bool innermostContainingCheckProcessesMouseEvents( const Gui::Coord& c, GuiDisplayable** ppResult );

	// Change the layer that the gui displayable is in.
	void setLayer( Layer );
	// PRE( hasParent() );

	// True if fast second display turned on. This causes the second call to doDisplay ( for the double buffering )
	// to be by-passed, instead the area covered by this GuiDisplayable is copied from the front buffer to the
	// back buffer.
	bool useFastSecondDisplay() const;
	void useFastSecondDisplay( bool );

	// Does this GuiDisplayable want to receive mouse enter, exit and contains messages
	virtual bool processesMouseEvents() const;
	
	#ifndef _PRODUCTION_RELEASE
	virtual const char* description() const;
	#endif

	// Fixes problem with screen going white when resolution is changed on certain cards
	static bool& useFourTimesRender();
		 
protected:
	void absoluteCoord( const Gui::Coord& absCoord );
	// PRE( implies( not isRoot(), parent().contains( Gui::Box( absCoord, width(), height() ) ) );
	// POST( absoluteCoord() == absCoord );
	// POST( boundary() == Gui::Box( absCoord, width(), height() ) ) );

	void relativeCoord( const Gui::Coord& c );
	// PRE_DATA( GuiVec diff = relativeCoord() - c );
	// PRE( implies( not isRoot(), parent().contains( translateBox( boundary(), diff ) ) ) );
	// POST( relativeCoord() == c );
	// POST( boundary() == translateBox( old( boundary() ), diff ) );

	void positionChildAbsolute( GuiDisplayable * pChild, const Gui::Coord& absCoord );
	// PRE( hasChild( pChild ) );
	// POST( pChild->absoluteCoord() == c );

	void positionChildRelative( GuiDisplayable * pChild, const Gui::Coord& relCoord );
	// PRE( hasChild( pChild ) );
	// POST( pChild->relativeCoord() == c );

	// Function that actually does the rendering
	virtual void doDisplay() = 0;
		
	virtual ~GuiDisplayable();

	Children& children( Layer );
	const Children& children( Layer ) const;
	Children& children();
	const Children& children() const;
	
	const Gui::Boundary& relativeBoundary() const;
	// POST( translate( result ) == boundary() );

	Gui::Coord	translate( Gui::XCoord x, Gui::YCoord ) const;
	Gui::Coord	translate( const Gui::Coord& ) const;
	Gui::Box	translate( const Gui::Box& ) const;

	void fill( const Gui::Colour& colour );

	void filledRectangle( const Gui::Box& rel, const Gui::Colour& colour );
	// PRE( boundary().contains( translate( rel ) ) );

	void hollowRectangle( const Gui::Box& rel, 
								const Gui::Colour& colour, 
								unsigned thickness );
	// PRE( boundary().contains( translate( rel ) ) );
	
	///////////////////////////////

	void line( const Gui::Coord& rel1, 
				const Gui::Coord& rel2, 
				const Gui::Colour&, 
				unsigned thickness );
	// PRE( boundary().contains( translate( rel1 ) ) );
	// PRE( boundary().contains( translate( rel2 ) ) );

	void horizontalLine( const Gui::Coord& rel1,
							unsigned length,
							const Gui::Colour&, 
							unsigned thickness );
	// PRE( boundary().contains( translate( rel1 ) ) );
	// PRE( boundary().contains( translate( rel1.x() + length, rel1.y() ) ) );

	void verticalLine( const Gui::Coord& rel1,
							unsigned height,
							const Gui::Colour&, 
							unsigned thickness );
	// PRE( boundary().contains( translate( rel1 ) ) );
	// PRE( boundary().contains( translate( rel1.x(), rel1.y() + height ) ) );
	
	///////////////////////////////

	void bevel( const Gui::Box& rel,
					unsigned thickness, 
					const Gui::Colour& hiCol, 
					const Gui::Colour& loCol );
	// PRE( boundary().contains( translate( rel ) ) );
		
	void text( const Gui::Coord& c, const string& text, const Gui::Colour& );
	// PRE( boundary().contains( translate( GuiText::extent( text ) ) ) );

	// c.x() is right hand side of text. i.e. text will draw to the left of c.x()
	void rightAlignText( const Gui::Coord& c, const string& text, const Gui::Colour& );

	void removeChild( GuiDisplayable* pChild );
	// PRE( hasChild( pChild ) );

	virtual void doRemoveChild( GuiDisplayable* pChild );
	// PRE( hasChild( pChild ) );

private: 
	GuiDisplayable( const GuiDisplayable& );
	bool operator ==( const GuiDisplayable& ) const;
	GuiDisplayable& operator =( const GuiDisplayable& );

	void changed( bool );

	void normalDisplay();
	void fastDisplay();
	void fastDisplayChildren();
	bool firstDisplay() const;
	bool secondDisplay() const;

	// private ctor for GuiRoot
	friend class GuiRoot;
	GuiDisplayable( const Gui::Boundary& );

	// Data members
	GuiDisplayableImpl* pImpl_;
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _GUI_DISPLAYA_HPP
