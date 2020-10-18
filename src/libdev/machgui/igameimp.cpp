/*
 * I G A M E I M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/igameimp.hpp"
#include "machgui/gui.hpp"

MachInGameScreenImpl::MachInGameScreenImpl()
:	pSceneManager_( NULL ),
	pRoot_( NULL ),
	pWorldViewWindow_( NULL ),
	pDefaultCommand_( NULL ),
	pActiveCommand_( NULL ),
	pContinentMap_( NULL ),
	pMachinesIcon_( NULL ),
	pConstructionsIcon_( NULL ),
	pSquadronIcon_( NULL ),
	pMachineNavigation_( NULL ),
	pConstructionNavigation_( NULL ),
	pCorral_( NULL ),
	pPromptText_( NULL ),
	pCommandIcons_( NULL ),
	pConstructMenu_( NULL ),
	pBuildMenu_( NULL ),
	pProductionBank_( NULL ),
	pHWResearchMenu_( NULL ),
	pHWResearchBank_( NULL ),
	pSquadronBank_( NULL ),
	pCursors2d_( NULL ),
	pStringResourceLib_( NULL ),
	pHighlightedActor_( NULL ),
	pDefconCommand_( NULL ),
	pSelfDestructCommand_( NULL ),
	pIonAttackCommand_( NULL ),
	pNukeAttackCommand_( NULL ),
	commandBankNeedsUpdating_( true ),
	cancelActiveCommand_( false ),
	switchToMenus_( false ),
	switchGuiRoot_( false ),
	inFirstPerson_( false ),
	controlPanelContext_( MachGui::MAIN_MENU ),
	cursorFilter_( W4dDomain::EXCLUDE_NOT_SOLID ),
	pPromptTextActor_( NULL ),
	pControlPanelAddOn_( NULL ),
	controlPanelOn_( true ),							 // Control panel should be out to start with
	controlPanelXPos_( MachGui::controlPanelInXPos() ),	 // Control panel starts in hidden position	( slides out when you first enter the game )
	pControlPanel_( NULL ),
	pMapArea_( NULL ),
	pTopCameraScrollArea_( NULL ),
	pBottomCameraScrollArea_( NULL ),
	pLeftCameraScrollArea_( NULL ),
	pRightCameraScrollArea_( NULL ),
	resolutionChanged_( true ),
	renderingScreenShot_( false ),
	corralState_( MachInGameScreen::CORRAL_EMPTY ),
	pChatMessageDisplay_( NULL ),
	networkStuffedStartTime_( 0 ),
	disableFirstPerson_( false ),
	instantExit_( false ),
	#ifndef PRODUCTION
	showCurrentMachine_(false),
	showNetworkPing_(false),
	showNetworkStuffed_(false),
	#endif
	redrawMapCounter_( 0 )
{
	// Intentionally empty
}

/* End IGAMEIMP.CPP *************************************************/
