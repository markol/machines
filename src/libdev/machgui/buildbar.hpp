/*
 * B U I L D B A R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiBuildProgressBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_BUILDBAR_HPP
#define _MACHGUI_BUILDBAR_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "machphys/machphys.hpp"

class MachGuiBuildProgressBar : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiBuildProgressBar(GuiDisplayable * pParent,
    						const Gui::Coord& rel,
							size_t width );
    ~MachGuiBuildProgressBar();

    void CLASS_INVARIANT;

	static size_t height();

	void depress( bool doDepress );

	void percentageComplete( float );
	float percentageComplete() const;

protected:
	virtual void doDisplay();

private:
	// Operations revoked
    MachGuiBuildProgressBar( const MachGuiBuildProgressBar& );
    MachGuiBuildProgressBar& operator =( const MachGuiBuildProgressBar& );
    bool operator ==( const MachGuiBuildProgressBar& );

	Gui::Colour barColour() const;

    friend ostream& operator <<( ostream& o, const MachGuiBuildProgressBar& t );

	enum { BORDER_THICKNESS = 1 };
	enum { BAR_THICKNESS = 2 };
	enum { SHADOW_THICKNESS = 1 };

	float percentageComplete_;
};


#endif

/* End BUILDBAR.HPP *************************************************/
