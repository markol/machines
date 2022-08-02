/*
 * C T X B R I E F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
	 
/*
    MachGuiCtxBriefing

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXBRIEF_HPP
#define _MACHGUI_CTXBRIEF_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"
#include "sound/soundmix.hpp"

class MachGuiScrollableText;
class MachGuiMenuButton;
class GuiImage;

class MachGuiCtxBriefing : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxBriefing( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxBriefing();

    void CLASS_INVARIANT;

	virtual void update();

	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

	virtual bool okayToSwitchContext();

	void autoLoadGame();

	void restartVoiceMailAndAnimation();
	
protected:
	void displayObjectives();
	void displayHints();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxBriefing& t );

    MachGuiCtxBriefing( const MachGuiCtxBriefing& );
    MachGuiCtxBriefing& operator =( const MachGuiCtxBriefing& );

	void displayBriefingImage();
	void playBriefingVoicemail();
	void stopPlayingBriefingVoicemail();

	// Data members...
	MachGuiAnimations animations_;
	MachGuiScrollableText* pTextInfo_;
	MachGuiMenuButton* pHintsBtn_;
	MachGuiMenuButton* pObjectiveBtn_;
	MachGuiMenuButton* pOkBtn_;
	GuiImage* pBriefImage_;
	SndSampleHandle briefVoicemail_;
	//Has this mail been played at all
	bool playedMail_;
	bool autoLoadGame_;
};


#endif

/* End CTXBRIEF.HPP *************************************************/
