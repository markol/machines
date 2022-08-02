/*
 * P L A Y E R S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiPlayerNameList

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_PLAYERS_HPP
#define _MACHGUI_PLAYERS_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/gui.hpp"
#include "gui/displaya.hpp"

class MachGuiPlayerNameList	: public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiPlayerNameList( GuiDisplayable *pParent, const Gui::Coord& topLeft, const string& fontName, uint title );
    MachGuiPlayerNameList( GuiDisplayable *pParent, const Gui::Boundary& boundary, const string& fontName, uint title, bool isWhite );
    ~MachGuiPlayerNameList();

    void CLASS_INVARIANT;
	
	virtual void doDisplay();

	void names( const string& player1, const string& player2, const string& player3, const string& player4 );

private:
	// Truncate player's name if it will not fit in bounding box.
	static string truncate( const string& name, uint maxWidth );

    friend ostream& operator <<( ostream& o, const MachGuiPlayerNameList& t );

    MachGuiPlayerNameList( const MachGuiPlayerNameList& );
    MachGuiPlayerNameList& operator =( const MachGuiPlayerNameList& );

	bool isWhite_;
};


#endif

/* End PLAYERS.HPP **************************************************/
