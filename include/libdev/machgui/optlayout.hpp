/*
 * O P T L A Y O U T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiOptionsLayout

    // Loads gui layout details from a file to specify where items will appear on screen
*/

#ifndef _OPTLAYOUT_HPP
#define _OPTLAYOUT_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "utility/linetok.hpp"
#include "mathex/point2d.hpp"


class MachGuiOptionsLayout
// Canonical form revoked
{
public:

	struct SlidebarInfo {
		SlidebarInfo( MexPoint2d tl, uint rng )
		:  topLeft( tl ), range( rng )
		{};
		MexPoint2d	topLeft;
		uint 		range;
	};

	struct MenuTextInfo{
		MenuTextInfo( uint stringId, MexPoint2d tl, MexPoint2d br, const string& fnt )
		:  idsStringId( stringId ), topLeft( tl ), bottomRight( br ), font( fnt )
		{};
		uint 		idsStringId;
	    MexPoint2d	topLeft;
	    MexPoint2d	bottomRight;
	    string		font;
	};

	struct CheckBoxInfo{
		CheckBoxInfo( MexPoint2d tl, uint id )
		:  topLeft( tl ), stringId( id )
		{};
		MexPoint2d topLeft;
		uint stringId;
	};

    MachGuiOptionsLayout( const SysPathName& );
    ~MachGuiOptionsLayout();

	const MachGuiOptionsLayout::SlidebarInfo& slidebarInfo( uint index ) const;
	// PRE( index < nScrollBars() )

	const MachGuiOptionsLayout::MenuTextInfo& menuTextInfo( uint index ) const;
	// PRE( index < nMenuTexts() )

	const MachGuiOptionsLayout::CheckBoxInfo& checkBoxInfo( uint index ) const;
	// PRE( index < nCheckBoxes() )

	uint nSlidebars() const { return slidebars_.size(); }
	uint nMenuTexts() const { return menuTexts_.size(); }
	uint nCheckBoxes() const { return checkBoxes_.size(); }

    void CLASS_INVARIANT;

private:

	void parse();
	void parseSlidebar( const UtlLineTokeniser::Tokens& tokens );
	void parseMenuText( const UtlLineTokeniser::Tokens& tokens );
	void parseCheckBox( const UtlLineTokeniser::Tokens& tokens );

    friend ostream& operator <<( ostream& o, const MachGuiOptionsLayout& t );

    MachGuiOptionsLayout( const MachGuiOptionsLayout& );
    MachGuiOptionsLayout& operator =( const MachGuiOptionsLayout& );

	typedef ctl_pvector< SlidebarInfo > Slidebars;
	typedef ctl_pvector< MenuTextInfo > MenuTexts;
	typedef ctl_pvector< CheckBoxInfo > CheckBoxes;

	Slidebars slidebars_;
	MenuTexts menuTexts_;
	CheckBoxes checkBoxes_;
	UtlLineTokeniser parser_;
};


#endif

/* End OPTLAYOUT.HPP ************************************************/
