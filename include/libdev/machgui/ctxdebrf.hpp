/*
 * C T X D E B R F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxDeBriefing

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXDEBRF_HPP
#define _MACHGUI_CTXDEBRF_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"
#include "sound/soundmix.hpp"

class GuiImage;

class MachGuiCtxDeBriefing : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxDeBriefing( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxDeBriefing();

    void CLASS_INVARIANT;

	virtual void update();

	virtual bool okayToSwitchContext();

	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

protected:
	void playDeBriefingVoicemail();
	void stopPlayingDeBriefingVoicemail();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxDeBriefing& t );

    MachGuiCtxDeBriefing( const MachGuiCtxDeBriefing& );
    MachGuiCtxDeBriefing& operator =( const MachGuiCtxDeBriefing& );

	void displayDeBriefImage();

	// Data members...
	MachGuiAnimations animations_;
	GuiImage* pDebriefImage_;
	SndSampleHandle debriefVoicemail_;
	//Has this mail been played yet
	bool playedMail_;
};


#endif

/* End CTXDEBRF.HPP *************************************************/
