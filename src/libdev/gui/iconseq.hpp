/*
 * I C O N S E Q . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_ICONSEQ_HPP
#define _GUI_ICONSEQ_HPP

#include "gui/displaya.hpp"
#include "ctl/vector.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class GuiIconSequenceImpl;

class GuiIconSequence : public GuiDisplayable
// cannonical from revoked
{
public:

	typedef ctl_vector< Gui::Coord > Coords;

	GuiIconSequence( GuiDisplayable * pParent,
						const Gui::Box& rel,
						const Coords& coords );

	///////////////////////////////

	// dimensions of a regular array of icons
	static Gui::Box arrayDimensions( unsigned iconWidth, unsigned iconHeight,
										unsigned nRows, unsigned nColumns );

	// coords of a regular array of icons
	static Coords arrayCoords( unsigned iconWidth, unsigned iconHeight,
										unsigned nRows, unsigned nColumns );

	// bounding box for a seqeunce of coords
	static Gui::Box coordBoundary( const Coords&, unsigned iconWidth, unsigned iconHeight );
	// PRE( not c.empty() );

	// return a sequence of coords containing as many icon positions
	// as will fit in the given box dimensions
	static Coords
	fitCoordsToBox( unsigned boxWidth, unsigned boxHeight,
						unsigned iconWidth, unsigned iconHeight );

	// return c translated by rel
	static Coords translateCoords( const Coords& c,
									const Gui::Coord& rel );

protected:

	virtual ~GuiIconSequence();

	virtual bool canScroll() const;

	void update();
	void updateMetrics();
	virtual void doUpdateMetrics( unsigned nFixedChildren, unsigned nActiveChildren );

	const Coords& coords() const;

	virtual void doDisplay();
	virtual bool isPositionOfScroller( Coords::size_type coordIndex ) const;

	virtual bool isScroller( GuiDisplayable * pChild ) const;
	// PRE( hasChild( pChild ) );

	///////////////////////////////

	virtual unsigned offset() const;

	bool isFixedChild( GuiDisplayable *pChild, Coords::size_type * pResult ) const;
	// PRE( hasChild( pChild ) );

	bool isPositionOfFixedChild( Coords::size_type coordIndex ) const;
	// PRE( coordIndex < coords().size() );

	bool isFixedPosition( Coords::size_type coordIndex ) const;
	// PRE( coordIndex < coords().size() );
	// POST( iff( result, isPositionOfScroller( coordIndex ) or isPositionOfFixedChild( coordIndex ) ) );

	///////////////////////////////

	void fixChild( GuiDisplayable *pChild, Coords::size_type coordIndex );
	// PRE( hasChild( pChild ) );
	// PRE( not isScroller( pChild ) );
	// PRE( coordIndex < coords().size() );
	// PRE( not isFixedPosition( coordIndex ) );
	// POST_DATA( Coords::size_type i );
	// POST( isFixedChild( pChild, &i ) and coordIndex == i );

	void unfixChild( GuiDisplayable *pChild );
	// PRE( hasChild( pChild ) );
	// POST( not isFixedChild( pChild ) );

	void fixScroller( GuiDisplayable *pChild, Coords::size_type coordIndex );
	// PRE( isScroller( pChild ) );
	// PRE( coordIndex < coords().size() );
	// PRE( not isPositionOfFixedChild( coordIndex ) );

	///////////////////////////////

	virtual void doRemoveChild( GuiDisplayable * pChild );
	// PRE( hasChild( pChild ) );

	virtual void doOutputOperator( ostream& ) const;

	// determines if changed() should be called after update
	bool isDoingDisplay() const;

private:
	GuiIconSequence( const GuiIconSequence& );
	GuiIconSequence& operator =( const GuiIconSequence& );
	bool operator ==( const GuiIconSequence& ) const;

	void repositionChildren();

	virtual void doUpdate( unsigned nFixedChildren, unsigned nActiveChildren );

	typedef ctl_vector< GuiDisplayable * > ActiveChildren;
	typedef ctl_vector< GuiDisplayable * > FixedChildren;

	GuiIconSequenceImpl * pImpl_;

	friend class GuiIconSequenceImpl;
	friend ostream& operator <<( ostream&, const GuiIconSequence& );
};

/* //////////////////////////////////////////////////////////////// */

class GuiScrollableIconSequenceImpl;

class GuiScrollableIconSequence
: public GuiIconSequence
// cannonical from revoked
{
public:

	GuiScrollableIconSequence( GuiDisplayable * pParent,
						const Gui::Box& rel,
						const Coords& coords,
						unsigned scrollIncrement );
	// PRE( scrollIncrement < coords.size() );

	virtual ~GuiScrollableIconSequence();

	virtual bool canScroll() const;

	bool canScrollLeft() const;
	bool canScrollRight() const;

	void scrollLeft();
	// PRE( canScrollLeft() );

	void scrollRight();
	// PRE( canScrollRight() );

	///////////////////////////////

protected:

	virtual bool isScroller( GuiDisplayable * pChild ) const;
	// PRE( hasChild( pChild ) );

	///////////////////////////////

	bool isPositionOfScroller( Coords::size_type coordIndex ) const;
	// PRE( coordIndex < coords().size() );

	///////////////////////////////

	bool hasLeftScroller() const;
	bool hasRightScroller() const;

	void leftScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex );
	// PRE( hasChild( pNewScroller ) );
	// PRE( not isScroller( pNewScroller ) );
	// PRE( not isFixedChild( pNewScroller ) );
	// PRE( not hasLeftScroller() );
	// PRE( coordIndex < coords().size() );
	// PRE( not isFixedPosition( coordIndex ) );
	// POST( isPositionOfScroller( coordIndex ) );
	// POST( hasLeftScroller() );
	// POST( isScroller( pNewScroller ) );

	void rightScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex );
	// PRE( hasChild( pNewScroller ) );
	// PRE( not isScroller( pNewScroller ) );
	// PRE( not isFixedChild( pNewScroller ) );
	// PRE( not hasRightScroller() );
	// PRE( coordIndex < coords().size() );
	// PRE( not isFixedPosition( coordIndex ) );
	// POST( isPositionOfScroller( coordIndex ) );
	// POST( hasRightScroller() );
	// POST( isScroller( pNewScroller ) );

	void twoWayScroller( GuiDisplayable * pNewScroller, Coords::size_type coordIndex );
	// PRE( hasChild( pNewScroller ) );
	// PRE( not isScroller( pNewScroller ) );
	// PRE( not isFixedChild( pNewScroller ) );
	// PRE( not hasLeftScroller() and not hasRightScroller() );
	// PRE( coordIndex < coords().size() );
	// PRE( not isFixedPosition( coordIndex ) );
	// POST( isPositionOfScroller( coordIndex ) );
	// POST( hasLeftScroller() and hasRightScroller() );
	// POST( isScroller( pNewScroller ) );

	void positionScrollers();

	///////////////////////////////

	unsigned scrollIncrement() const;

	///////////////////////////////

	virtual unsigned offset() const;

	virtual void doUpdate( unsigned nFixedChildren, unsigned nActiveChildren );
	virtual void doUpdateMetrics( unsigned nFixedChildren, unsigned nActiveChildren );
	virtual void doRemoveChild( GuiDisplayable * pChild );
	// PRE( hasChild( pChild ) );

	virtual void doOutputOperator( ostream& ) const;

private:
	GuiScrollableIconSequence( const GuiScrollableIconSequence& );
	GuiScrollableIconSequence& operator =( const GuiScrollableIconSequence& );
	bool operator ==( const GuiScrollableIconSequence& ) const;

	GuiScrollableIconSequenceImpl * pImpl_;
};

/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _GUI_ICONSEQ_HPP

/* End ICONSEQ.HPP **************************************************/
