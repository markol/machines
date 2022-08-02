/*
 * S T A R T U P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//
// Implementation class for MachGuiStartupScreens
//
#ifndef _MACHGUI_STARTUI
#define _MACHGUI_STARTUI

#include "phys/phys.hpp"
#include "ctl/pvector.hpp"

class W4dSceneManager;
class RenAnimCursor2d;
class MachInGameScreen;
class W4dRoot;
class GuiKeyEvent;
class AniSmacker;
class MachGuiStartupData;
class MachGuiMessageBroker;
class MachGuiAutoDeleteDisplayable;
class MachGuiMessageBox;
class MachGuiMessageBoxResponder;
class MachGuiDbSavedGame;
class MachLogGameCreationData;
class MachGuiStartupScreenContext;
class MachGuiFocusCapableControl;
class MachGuiDispositionChangeNotifiable;

class MachGuiStartupScreensImpl
{
	private:
		typedef ctl_pvector< AniSmacker > SmackerAnims;
		typedef ctl_pvector< MachGuiFocusCapableControl > FocusCapableControls;

		// Data members...
		RenAnimCursor2d* 						pMenuCursor_;
		W4dSceneManager* 						pSceneManager_;
		MachGuiStartupScreens::Context 			context_;
		MachGuiStartupScreens::Context 			contextAfterFlic_;
		MachGuiStartupScreens::Context 			contextBeforeFlic_;
		GuiBitmap* 								pBackdrop_;
		MachInGameScreen*						pInGameScreen_;
		bool 									switchGuiRoot_;
		bool 									finishApp_;
		W4dRoot* 								pW4dRoot_;
		PhysAbsoluteTime 						contextTimer_;
	    AniSmacker* 							pPlayingSmacker_;// A currently playing smacker file. NULL if none
		MachGuiStartupScreens::Music 			playingCdTrack_; // Track number playing on audio cd. ( -1 for none )
		MachGuiStartupScreens::Music 			desiredCdTrack_; // Track number that should be playing on audio cd. ( -1 for none )
		PhysAbsoluteTime 						cdCheckTime_;    // Only check cd once every 60 seconds
		bool 									endGame_;
		MachGuiStartupScreens::ButtonEvent 		lastButtonEvent_;
		MachGuiMessageBox* 						pMsgBox_; // Displays a message box with OK or OK/Cancel buttons
		MachGuiMessageBoxResponder* 			pMsgBoxResponder_; // Is told about OK/Cancel button presses in a message box
		MachGuiStartupScreens::GameType			gameType_;
		MachGuiStartupScreenContext* 			pCurrContext_;
		MachGuiStartupData* 					pStartupData_;
		MachGuiMessageBroker* 					pMessageBroker_;
		MachGuiAutoDeleteDisplayable* 			pMustContainMouse_; // A gui displayable that is present only if the mouse if inside it's boundary ( generally drop-down list boxes )
	 	SmackerAnims 							smackerAnims_;     // list of smacker animations to play
		GuiDisplayable*							pCharFocus_; // Store the char focus whilst the message box is displayed
		int										inGameResolutionWidth_;
		int										inGameResolutionHeight_;
		int										inGameResolutionRate_;
		bool									isGamePaused_;
		FocusCapableControls					focusCapableControls_;
		MachGuiDispositionChangeNotifiable*		pDispositionNotifiable_;
		bool									ignoreHostLostSystemMessage_;

		friend class MachGuiStartupScreens;
};

#endif 	// _MACHGUI_STARTUI
