#ifndef _GUI_ICONSEI
#define _GUI_ICONSEI

#include "ctl/vector.hpp"
#include "ctl/bitvec.hpp"

#include "gui/iconseq.hpp"

class GuiIconSequenceImpl
{
	private:
		GuiIconSequence::ActiveChildren	activeChildren_;
		GuiIconSequence::FixedChildren  fixedChildren_;
		unsigned						nFixedChildren_;
		ctl_bit_vector					allocatedPositions_;
		GuiIconSequence::Coords			coords_;
		bool							isDoingDisplay_;

		GuiIconSequenceImpl( const GuiIconSequence::Coords& c )
		:
		activeChildren_( c.size() ),
		fixedChildren_( c.size() ),
		allocatedPositions_( c.size() )
		{}

		friend class GuiIconSequence;
};

class GuiScrollableIconSequenceImpl
{
	private:
		GuiDisplayable * pLeftScroller_;
		GuiDisplayable * pRightScroller_;
		GuiDisplayable * pTwoWayScroller_;

		GuiIconSequence::Coords::size_type leftScrollerIndex_;
		GuiIconSequence::Coords::size_type rightScrollerIndex_;
		GuiIconSequence::Coords::size_type twoWayScrollerIndex_;

		bool canScrollLeft_;
		bool canScrollRight_;
		unsigned offset_;
		unsigned scrollIncrement_;
	
		friend class GuiScrollableIconSequence;		
};

#endif 	// _GUI_ICONSEI