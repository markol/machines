/*
 * I G A M E I M P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachInGameScreenImpl

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_IGAMEIMP_HPP
#define _MACHGUI_IGAMEIMP_HPP

#include "base/base.hpp"
#include "gui/gui.hpp"
#include "ctl/pvector.hpp"
#include "world4d/domain.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"

// Foward refs...
class W4dSceneManager;
class W4dRoot;
class MachWorldViewWindow;
class MachGuiBmuButton;
class MachGuiCommand;
class MachActor;
class MachContinentMap;
class MachMachinesIcon;
class MachConstructionsIcon;
class MachSquadronIcon;
class MachPromptText;
class MachGuiCorralSingleIcon;
class MachGuiCorral;
class MachGuiMachineNavigator;
class MachGuiConstructionNavigator;
class MachCommandIcons;
class MachSmallCommandIcons;
class MachConstructMenu;
class MachBuildMenu;
class MachHWResearchMenu;
class MachProductionBank;
class MachHWResearchBank;
class MachGuiSquadronBank;
class AfxResourceLib;
class MachCameras;
class MachInGameCursors2d;
class MachGuiDefconCommand;
class MachGuiSelfDestructCommand;
class MachGuiFirstPerson;
class MachGuiControlPanelAddOn;
class MachGuiControlPanel;
class MachGuiMapArea;
class MachGuiCameraScrollArea;
class MachGuiInGameChatMessagesDisplay;

class MachInGameScreenImpl
{
public:
	MachInGameScreenImpl();

    //Data members...
	typedef ctl_pvector< MachActor > Actors;    	
    typedef ctl_pvector< MachGuiCommand > Commands;

    W4dSceneManager* 				pSceneManager_; 	//The 3d window scene maanger
	W4dRoot*		 				pRoot_;
	MachWorldViewWindow* 			pWorldViewWindow_;
	MachGuiBmuButton* 				pBmuButton_;
    Actors 							selectedActors_; 	//The collection of currently selected actors
    Commands 						allCommands_; 		//The collection of all the potential commands
    MachGuiCommand* 				pDefaultCommand_; 	//The command that is active when no other command is active
    MachGuiCommand* 				pActiveCommand_; 	//The active command. Null => use pDefaultCommand_.
    W4dDomain::EntityFilter 		cursorFilter_; 		//The filter to be used for findNearerEntity
    bool 							commandBankNeedsUpdating_; //True when the visibility of command icons needs to be updated
    bool 							cancelActiveCommand_;
	bool							applyCommandToSquadron_;
	bool							switchToMenus_;			// Indicates to MachGuiStartupScreens that they should become root
	bool							switchGuiRoot_;
	bool							inFirstPerson_;
	MachActor*						pHighlightedActor_;
	MachContinentMap* 				pContinentMap_;
	MachMachinesIcon*				pMachinesIcon_;
	MachConstructionsIcon*			pConstructionsIcon_;
	MachSquadronIcon*				pSquadronIcon_;
    MachPromptText* 				pPromptText_; 			//The displayable displaying the prompt text
	MachGuiCorralSingleIcon*		pCorralSingleIcon_;
	MachGuiCorral*					pCorral_;
	MachGuiMachineNavigator*		pMachineNavigation_;
	MachGuiConstructionNavigator*	pConstructionNavigation_;
	MachCommandIcons*				pCommandIcons_;
	MachSmallCommandIcons*			pSmallCommandIcons_;
    MachConstructMenu* 				pConstructMenu_;
    MachBuildMenu* 					pBuildMenu_;
    MachHWResearchMenu* 			pHWResearchMenu_;
    MachProductionBank* 			pProductionBank_; 		//A factory production bank
    MachHWResearchBank* 			pHWResearchBank_; 		//A hardware lab research bank
    MachGuiSquadronBank* 			pSquadronBank_;	  		//The squadron bank
    AfxResourceLib* 				pStringResourceLib_; 	//The lib containing the app's string table
	MachCameras* 					pCameras_;
    MachGui::ControlPanelContext 	controlPanelContext_; 	//Menu context
    MachInGameCursors2d* 			pCursors2d_; 			//The class enabling switching of cursor	
	MachInGameScreen::GameState 	gameState_;
	PhysAbsoluteTime 				gameStateTimer_;
	MachGuiDefconCommand* 			pDefconCommand_;
	MachGuiSelfDestructCommand*		pSelfDestructCommand_;
	MachGuiIonAttackCommand* 		pIonAttackCommand_;
	MachGuiNukeAttackCommand*		pNukeAttackCommand_;
	MachGuiFirstPerson* 			pFirstPerson_;
	Gui::Coord						rightClickMousePos_;	// When right click happens the coord is stored in this var.
	MachActor*						pPromptTextActor_;		// Actor whos info is on the prompt text
	MachGuiControlPanelAddOn*		pControlPanelAddOn_;
	bool							controlPanelOn_;
	int								controlPanelXPos_;
	MachGuiControlPanel*			pControlPanel_;
	MachGuiMapArea*					pMapArea_;
	MachGuiCameraScrollArea*		pTopCameraScrollArea_;
	MachGuiCameraScrollArea*		pBottomCameraScrollArea_;
	MachGuiCameraScrollArea*		pLeftCameraScrollArea_;
	MachGuiCameraScrollArea*		pRightCameraScrollArea_;
	bool							resolutionChanged_;
	bool							renderingScreenShot_;
	MachInGameScreen::CorralStateBitfield	corralState_;
	MachGuiInGameChatMessagesDisplay* pChatMessageDisplay_;
	int								redrawMapCounter_;		// When ingame becomes GuiRoot the map needs to be redrawn every frame for 1st 4 frames to remove graphic glich (hack!! Oh well.)
	double							networkStuffedStartTime_;
	bool							disableFirstPerson_;
	bool							instantExit_;

	#ifndef PRODUCTION
	bool showCurrentMachine_;
	bool showNetworkPing_;
	bool showNetworkStuffed_;
	#endif
};

#endif

/* End IGAMEIMP.HPP *************************************************/
