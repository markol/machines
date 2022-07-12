/*
 * C T X O P T N S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxoptions

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXOPTNS_HPP
#define _MACHGUI_CTXOPTNS_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "ctl/countptr.hpp"
#include "render/driv.hpp"
#include "machgui/startctx.hpp"
#include "machgui/startup.hpp"
#include "machgui/animatio.hpp"

#define GAMMA_REG_MULTIPLIER 0x19999999
#define GAMMA_LOWER_LIMIT 0.1
#define GAMMA_UPPER_LIMIT 5.0

class RenDriverSelector;
class MachGuiSlideBar;
class MachGuiMusicVolumeSlideBar;
class MachGuiSoundVolumeSlideBar;
class MachGuiDropDownListBoxCreator;
class MachGuiDDrawDropDownListBoxCreator;
class MachGuiCheckBox;
class RenDisplay;
class MachGuiGammaCorrectionSlideBar;

class MachGuiCtxOptions : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxOptions( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxOptions();

    void CLASS_INVARIANT;

	virtual void update();
	virtual bool okayToSwitchContext();
	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

	void exitFromOptions();

	static void load3dSoundFiles( bool enabled );

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxOptions& t );

	void getOptions();
	void setOptions();
	void showDirect3DDrivers();

    MachGuiCtxOptions( const MachGuiCtxOptions& );
    MachGuiCtxOptions& operator =( const MachGuiCtxOptions& );

	typedef ctl_pvector< MachGuiCheckBox > BooleanOptimisations;
	typedef ctl_pvector< MachGuiDropDownListBoxCreator > ChoicesOptimisations;

	friend class MachGuiDDrawDropDownCallback;

	// Data members...
	MachGuiStartupScreens::Context exitContext_;
	MachGuiAnimations animations_;

	MachGuiMusicVolumeSlideBar* pMusicVolume_;
	MachGuiSoundVolumeSlideBar* pSoundVolume_;
	MachGuiCheckBox* pSound3d_;
	MachGuiCheckBox* pTransitions_;
	MachGuiCheckBox* pScreenResolutionLock_;
	MachGuiCheckBox* pCursorType_;
	MachGuiCheckBox* pReverseKeys_;
	MachGuiCheckBox* pReverseMouse_;
	MachGuiDropDownListBoxCreator* pScreenSize_;
	BooleanOptimisations booleanOptimisations_;
	ChoicesOptimisations choicesOptimisations_;
	float musicVolume_, soundVolume_, gammaCorrection_;
	RenDriverSelector* pDriverSelector_;
	MachGuiDDrawDropDownListBoxCreator* pDirectDrawDrivers_;
	MachGuiDropDownListBoxCreator* pDirect3DDrivers_;
	bool exitFromOptions_;
	RenDriverPtr initialDDrawDriver_;
	bool cursorType2d_;
	MachGuiGammaCorrectionSlideBar* pGammaCorrection_;
};


#endif

/* End CTXOPTNS.HPP *************************************************/
