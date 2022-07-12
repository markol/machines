/*
 * A N I M A T I O . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiAnimation

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_ANIMATIO_HPP
#define _MACHGUI_ANIMATIO_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/displaya.hpp"
#include "gui/gui.hpp"
#include "ctl/pvector.hpp"

class SysPathName;

class MachGuiAnimation : public GuiDisplayable
// Canonical form revoked
{
public:
	struct Cell
	{
		Cell();

		enum CellTime { STATIC, RANDOM };
		GuiBitmap cell_;
		double finishTime_;
		double duration_;
		double randomDurationMinTime_;
		double randomDurationMaxTime_;
		CellTime cellTimeType_;
		bool hasSound_;
		string wavFile_;
	};

	typedef ctl_pvector<Cell> Cells;

	// Create animation from file describing animation
	static MachGuiAnimation* createAnimation( GuiDisplayable* pParent, const SysPathName& animPath );

	// Create animation from file describing animation. colourKey bool is used to switch colourKeying on for
	// each frame of the animation.
	static MachGuiAnimation* createAnimation( GuiDisplayable* pParent, const SysPathName& animPath, bool colourKey );

	// Create animation from file describing animation. colourKey bool is used to switch colourKeying on for
	// each frame of the animation. xOffset and yOffset will change the x/y values specified in the .anm file.
	static MachGuiAnimation* createAnimation( GuiDisplayable* pParent, const SysPathName& animPath, bool colourKey, int xOffset, int yOffset );

	// MachGuiAnimationCells* must be newed and will be deleted by this class.
    MachGuiAnimation( GuiDisplayable* pParent, const Gui::Box&, Cells* );
    ~MachGuiAnimation();

    void CLASS_INVARIANT;

	void update();

	// Force the animation to jump to a particular cell (frame).
	void jumpToCell( size_t cellIndex );
	// PRE( cellIndex < pCells_->size() );

	// Return the cell that the animation is currently on.
	size_t cellIndex() const;

	// Get the number of cells in the animation.
	size_t numCells() const;

protected:
	virtual void doDisplay();

	void updateCellTimes();
	void playSound( const string& wavFile );

private:
    friend ostream& operator <<( ostream& o, const MachGuiAnimation& t );

    MachGuiAnimation( const MachGuiAnimation& );
    MachGuiAnimation& operator =( const MachGuiAnimation& );

	// Data members...
	Cells* pCells_;
	double startTime_;
	size_t cellIndex_;
	size_t loopBack_;
};

class MachGuiAnimations
{
public:
	MachGuiAnimations( GuiDisplayable*, const SysPathName& pathName );
	~MachGuiAnimations();

	void update();

private:
	ctl_pvector<MachGuiAnimation> animations_;
};


#endif

/* End ANIMATIO.HPP *************************************************/
