/*
 * C T X S K I R M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxskirm.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/scrltext.hpp"
#include "machgui/scrolbar.hpp"
#include "machgui/gui.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbtexdat.hpp"
#include "machgui/database.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/text.hpp"
#include "machgui/dropdwnc.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/numpdrop.hpp"
#include "gui/scrolbut.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "machgui/internal/strings.hpp"
#include "system/registry.hpp"
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiTerrainTypeListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiTerrainTypeListBoxItem( 	MachGuiStartupScreens* pStartupScreens,
	    							MachGuiSingleSelectionListBox* pListBox,
	    							size_t width,
	    							MachGuiDbPlanet& planet,
	    							MachGuiCtxSkirmish* pSkirmishCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, displayString( planet ) ),
		planet_( planet ),
		pSkirmishCtx_( pSkirmishCtx )
	{}

    ~MachGuiTerrainTypeListBoxItem( void )
	{}

	static bool allScenariosCompleted( MachGuiDbPlanet& planet )
	{
		// Work out if all the scenarios on this planet have been played and won
		// Iterate through scenarios
		uint numScenarios = planet.nScenarios();

		bool allComplete = true;

		for ( uint loop = 0; loop < numScenarios and allComplete; ++loop )
		{
			MachGuiDbScenario& scenario = planet.scenario( loop );

			allComplete = scenario.hasBeenWon();
		}

		return allComplete;
	}

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pSkirmishCtx_->updateScenarioList( planet_ );
	}

	static string displayString( MachGuiDbPlanet& planet )
	{
		string retValue;

		if ( allScenariosCompleted( planet ) )
		{
			retValue = MachGui::wonSymbolText();
		}

		retValue += planet.menuString();

		return retValue;
	}

private:
    MachGuiTerrainTypeListBoxItem( const MachGuiTerrainTypeListBoxItem& );
    MachGuiTerrainTypeListBoxItem& operator =( const MachGuiTerrainTypeListBoxItem& );

	// Data members...
	MachGuiDbPlanet& planet_;
	MachGuiCtxSkirmish* pSkirmishCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiMapSizeListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiMapSizeListBoxItem( 	MachGuiStartupScreens* pStartupScreens,
    							MachGuiSingleSelectionListBox* pListBox,
    							size_t width,
    							MachGuiDbSystem& system,
    							MachGuiCtxSkirmish* pSkirmishCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, displayString( system ) ),
		system_( system ),
		pSkirmishCtx_( pSkirmishCtx )
	{}

    ~MachGuiMapSizeListBoxItem( void )
	{}

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pSkirmishCtx_->updateTerrainTypeList( system_ );
	}

	static string displayString( MachGuiDbSystem& system )
	{
		string retValue;

		if ( allPlanetsCompleted( system ) )
		{
			retValue = MachGui::wonSymbolText();
		}

		retValue += system.menuString();

		return retValue;
	}

	// Work out if all the planets in this system have been played and won
	static bool allPlanetsCompleted( MachGuiDbSystem& system )
	{
		uint numPlanets = system.nPlanets();

		bool allComplete = true;

		for ( uint loop = 0; loop < numPlanets and allComplete; ++loop )
		{
    		MachGuiDbPlanet& planet = system.planet( loop );

			allComplete = MachGuiTerrainTypeListBoxItem::allScenariosCompleted( planet );
		}

		return allComplete;
	}

private:
    MachGuiMapSizeListBoxItem( const MachGuiMapSizeListBoxItem& );
    MachGuiMapSizeListBoxItem& operator =( const MachGuiMapSizeListBoxItem& );

	// Data members...
	MachGuiDbSystem& system_;
	MachGuiCtxSkirmish* pSkirmishCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiSkirmScenarioListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiSkirmScenarioListBoxItem(MachGuiStartupScreens* pStartupScreens,
	    							MachGuiSingleSelectionListBox* pListBox,
	    							size_t width,
	    							MachGuiDbScenario& scenario,
	    							MachGuiCtxSkirmish* pSkirmishCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, displayString( scenario ) ),
		scenario_( scenario ),
		pSkirmishCtx_( pSkirmishCtx )
	{}

    ~MachGuiSkirmScenarioListBoxItem( void )
	{}

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pSkirmishCtx_->updateSelectedScenario( scenario_ );
	}

	static string displayString( MachGuiDbScenario& scenario )
	{
		string retValue;

		if ( scenario.hasBeenWon() )
		{
			retValue = MachGui::wonSymbolText();
		}

		retValue += scenario.menuString();

		return retValue;
	 }

private:
    MachGuiSkirmScenarioListBoxItem( const MachGuiSkirmScenarioListBoxItem& );
    MachGuiSkirmScenarioListBoxItem& operator =( const MachGuiSkirmScenarioListBoxItem& );

	// Data members...
	MachGuiDbScenario& scenario_;
	MachGuiCtxSkirmish* pSkirmishCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////
#define LB_MINX 61
#define LB_MAXX 266
#define MAPSIZE_LB_MINY 15
#define MAPSIZE_LB_MAXY 84
#define TERRAINTYPE_LB_MINY 127
#define TERRAINTYPE_LB_MAXY 280
#define SCENARIO_LB_MINY 305
#define SCENARIO_LB_MAXY 457
#define SETTINGS_LB_MINY 19
#define SETTINGS_LB_MAXY 205
#define SETTINGS_LB_MINX 369
#define SETTINGS_LB_MAXX 601

MachGuiCtxSkirmish::MachGuiCtxSkirmish( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sn_anims.anm")	),
	pNumPlayersSelector_( NULL ),
	pNumPlayersText_( NULL ),
	autoLoadGame_( false )
{
	// Display backdrop, play correct music, switch cursor on.
	pStartupScreens->changeBackdrop( "gui/menu/sn.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	// Regular menu buttons...
  	MachGuiMenuButton* pOkBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 302, 420, 444, 451 ), IDS_MENUBTN_OK, MachGuiStartupScreens::BE_DUMMY_OK ) );
  	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 472, 420, 607, 451 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );

	pCancelBtn->escapeControl( true );
	pOkBtn->defaultControl( true );

	// Scenario description
  	pScenarioDesc_ = _NEW( MachGuiScrollableText( pStartupScreens, Gui::Box( 353, 288, 576, 385 ) ) );
 	MachGuiVerticalScrollBar::createWholeBar( pStartupScreens, Gui::Coord( 576, 288 ), 96, pScenarioDesc_ );

	// Display mapsize list box heading
	GuiResourceString mapsizeHeading( IDS_MENULB_MAPSIZE );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	MachGuiMenuText* pMapSizeText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( LB_MINX, MAPSIZE_LB_MINY,
							   			 LB_MINX + font.textWidth( mapsizeHeading.asString() ),
							   			 MAPSIZE_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_MAPSIZE, "gui/menu/largefnt.bmp" ) );

	// Display planet list box heading
	GuiResourceString terrainHeading( IDS_MENULB_TERRAINTYPE );
	MachGuiMenuText* pTerrainText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( LB_MINX, TERRAINTYPE_LB_MINY,
							   			 LB_MINX + font.textWidth( terrainHeading.asString() ),
							   			 TERRAINTYPE_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_TERRAINTYPE, "gui/menu/largefnt.bmp" ) );

	// Display scenario list box heading
	GuiResourceString scenarioHeading( IDS_MENULB_SCENARIO );
	MachGuiMenuText* pScenarioText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( LB_MINX, SCENARIO_LB_MINY,
							   			 LB_MINX + font.textWidth( scenarioHeading.asString() ),
							   			 SCENARIO_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_SCENARIO, "gui/menu/largefnt.bmp" ) );

	// Display settings list box heading
	GuiResourceString settingsHeading( IDS_MENULB_SETTINGS );
	MachGuiMenuText* pSettingsText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( SETTINGS_LB_MINX, SETTINGS_LB_MINY,
							   			 SETTINGS_LB_MINX + font.textWidth( settingsHeading.asString() ),
							   			 SETTINGS_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_SETTINGS, "gui/menu/largefnt.bmp" ) );

	// Create system list box
	pMapSizeList_ = _NEW( MachGuiSingleSelectionListBox(  pStartupScreens,
														 Gui::Box( LB_MINX, pMapSizeText->absoluteBoundary().maxCorner().y() - pStartupScreens->yMenuOffset(), LB_MAXX, MAPSIZE_LB_MAXY),
														 1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );
	// Create planet list box
	pTerrainTypeList_ = _NEW( MachGuiSingleSelectionListBox( pStartupScreens,
															 Gui::Box( LB_MINX, pTerrainText->absoluteBoundary().maxCorner().y() - pStartupScreens->yMenuOffset(), LB_MAXX, TERRAINTYPE_LB_MAXY),
															 1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );
	// Create scenario list box
	pScenarioList_ = _NEW( MachGuiSingleSelectionListBox(pStartupScreens,
														 Gui::Box( LB_MINX, pScenarioText->absoluteBoundary().maxCorner().y() - pStartupScreens->yMenuOffset(), LB_MAXX, SCENARIO_LB_MAXY),
														 1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );

	// Create settings list box
	pSettingsList_ = _NEW( GuiSimpleScrollableList( pStartupScreens,
													Gui::Box( SETTINGS_LB_MINX, pSettingsText->absoluteBoundary().maxCorner().y() - pStartupScreens->yMenuOffset(), SETTINGS_LB_MAXX, SETTINGS_LB_MAXY),
													(SETTINGS_LB_MAXX-SETTINGS_LB_MINX)/2,
													 MachGuiDropDownListBoxCreator::reqHeight() + 1, 1 ) );

	initSettings();
	updateMapSizeList();

	TEST_INVARIANT;
}

MachGuiCtxSkirmish::~MachGuiCtxSkirmish()
{
    TEST_INVARIANT;

}

void MachGuiCtxSkirmish::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxSkirmish& t )
{

    o << "MachGuiCtxSkirmish " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxSkirmish " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxSkirmish::update()
{
	if ( autoLoadGame_ )
	{
		pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
	}
	else
	{
		animations_.update();
	}
}

void MachGuiCtxSkirmish::updateMapSizeList()
{
	bool firstItem = true;
    // There will be no CUSTOM skirmishSystem if there is no customscen.dat file
    const MachGuiDatabase::TerrainSize maxMapSize = MachGuiDatabase::doCustomScenariosExist() ? MachGuiDatabase::CUSTOM : MachGuiDatabase::LARGE;

    for ( MachGuiDatabase::TerrainSize loop = MachGuiDatabase::SMALL; loop <= maxMapSize; loop = _STATIC_CAST(MachGuiDatabase::TerrainSize, _STATIC_CAST(uint, loop)+1 ))
	{
    	MachGuiDbSystem& system = MachGuiDatabase::instance().skirmishSystem( loop );

		if( not system.nPlanets() ) // skip user custom scenarios if empty
			continue;
		MachGuiMapSizeListBoxItem* pItem = _NEW( MachGuiMapSizeListBoxItem( pStartupScreens_, pMapSizeList_, LB_MAXX - LB_MINX, system, this ) );
		// Select first item in list
		if ( firstItem )
		{
			firstItem = false;
			pItem->selectThisItem();
		}
	}

	pMapSizeList_->childrenUpdated();
}

void MachGuiCtxSkirmish::updateTerrainTypeList( MachGuiDbSystem& system )
{
	// Delete all items from list
	pTerrainTypeList_->deleteAllItems();

	// Redraw backdrop to list
	pStartupScreens_->blitBackdrop( pTerrainTypeList_->absoluteBoundary(),
									pTerrainTypeList_->absoluteBoundary().minCorner() );


	// Insert new items into list
	uint numPlanets = system.nPlanets();
	bool firstItem = true;

	for ( uint loop = 0; loop < numPlanets; ++loop )
	{
    	MachGuiDbPlanet& planet = system.planet( loop );

		MachGuiTerrainTypeListBoxItem* pItem = _NEW( MachGuiTerrainTypeListBoxItem( pStartupScreens_, pTerrainTypeList_, LB_MAXX - LB_MINX, planet, this ) );
		// Select first item in list
		if ( firstItem )
		{
			firstItem = false;
			pItem->selectThisItem();
		}
	}

	pTerrainTypeList_->childrenUpdated();
}

void MachGuiCtxSkirmish::updateScenarioList( MachGuiDbPlanet& planet )
{
	// Delete all items from list
	pScenarioList_->deleteAllItems();

	// Redraw backdrop to list
	pStartupScreens_->blitBackdrop( pScenarioList_->absoluteBoundary(),
									pScenarioList_->absoluteBoundary().minCorner() );


	// Insert new items into list
	uint numScenarios = planet.nScenarios();
	bool firstItem = true;

	for ( uint loop = 0; loop < numScenarios; ++loop )
	{
    	MachGuiDbScenario& scenario = planet.scenario( loop );

		MachGuiSkirmScenarioListBoxItem* pItem = _NEW( MachGuiSkirmScenarioListBoxItem( pStartupScreens_, pScenarioList_, LB_MAXX - LB_MINX, scenario, this ) );
		// Select first item in list
		if ( firstItem )
		{
			firstItem = false;
			pItem->selectThisItem();
		}
	}

	pScenarioList_->childrenUpdated();
}

void MachGuiCtxSkirmish::updateSelectedScenario( MachGuiDbScenario& scenario )
{
	pStartupScreens_->startupData()->scenario( &scenario );
	pScenarioDesc_->setText( scenario.textData().campaignText() );
	updateMaxPlayersList( scenario );
}

void MachGuiCtxSkirmish::initSettings()
{
	// Delete all items from list
	pSettingsList_->deleteAllChildren();

	// Redraw backdrop to list
	pStartupScreens_->blitBackdrop( pSettingsList_->absoluteBoundary(),
									pSettingsList_->absoluteBoundary().minCorner() );


	// Add "fog of war" setting...
	addSetting( false, pFogOfWarSelector_, IDS_MENU_FOGOFWAR, 2, IDS_MENU_ON, IDS_MENU_OFF );
	pFogOfWarSelector_->text( pStartupScreens_->startupData()->fogOfWarStr() );

	// Add "resource" setting...
	addSetting( false, pResourcesSelector_, IDS_MENU_RESOURCES, 3, IDS_MENU_HIGH, IDS_MENU_MEDIUM, IDS_MENU_LOW );
	pResourcesSelector_->text( pStartupScreens_->startupData()->resourcesStr() );

	// Add "starting resource" setting...
   	addSetting( false, pStartingResourcesSelector_, IDS_MENU_STARTINGRESOURCES, 6, IDS_MENU_DEFAULT, IDS_MENU_RESOURCESLEVEL1, IDS_MENU_RESOURCESLEVEL2, IDS_MENU_RESOURCESLEVEL3, IDS_MENU_RESOURCESLEVEL4 ,IDS_MENU_RESOURCESLEVEL5 );
	pStartingResourcesSelector_->text( pStartupScreens_->startupData()->startingResourcesStr() );

	// Add "starting pos" setting...
	addSetting( false, pStartingPosSelector_, IDS_MENU_STARTPOS, 2, IDS_MENU_STARTFIXED, IDS_MENU_STARTRANDOM );
	pStartingPosSelector_->text( pStartupScreens_->startupData()->startingPositionStr() );

	// Add "victory conditions" setting...
	addSetting( false, pVictorySelector_, IDS_MENU_VICTORYCONDITION, 12, IDS_MENU_DEFAULT, IDS_MENU_VICTORYTANNIHILATION, IDS_MENU_VICTORYDESTROYPOD,
				IDS_MENU_VICTORYTIMED5, IDS_MENU_VICTORYTIMED10, IDS_MENU_VICTORYTIMED15, IDS_MENU_VICTORYTIMED30, IDS_MENU_VICTORYTIMED45,
				IDS_MENU_VICTORYTIMED60, IDS_MENU_VICTORYTIMED90, IDS_MENU_VICTORYTIMED120, IDS_MENU_VICTORYTIMED180 );
	pVictorySelector_->text( pStartupScreens_->startupData()->victoryConditionStr() );

	// Add "tech level" setting...
	addSetting( false, pTechLevelSelector_, IDS_MENU_STARTINGTECHLEVEL, 4, IDS_MENU_DEFAULT, IDS_MENU_TECHLEVELLOW, IDS_MENU_TECHLEVELMED, IDS_MENU_TECHLEVELHIGH );
	pTechLevelSelector_->text( pStartupScreens_->startupData()->techLevelStr() );

	// Add "player colour" setting...
	addSetting( false, pColourSelector_, IDS_MENU_PLAYERCOLOUR, 4, IDS_MENU_PLAYERCOLOURRED, IDS_MENU_PLAYERCOLOURGREEN, IDS_MENU_PLAYERCOLOURBLUE, IDS_MENU_PLAYERCOLOURYELLOW );
	pColourSelector_->text( pStartupScreens_->startupData()->playerRaceStr() );

	pSettingsList_->childrenUpdated();
}

//virtual
bool MachGuiCtxSkirmish::okayToSwitchContext()
{
	// Update settings...
	pStartupScreens_->startupData()->resources( pResourcesSelector_->text() );
	pStartupScreens_->startupData()->fogOfWar( pFogOfWarSelector_->text() );
	pStartupScreens_->startupData()->resources( pResourcesSelector_->text() );
	pStartupScreens_->startupData()->startingResources( pStartingResourcesSelector_->text() );
	pStartupScreens_->startupData()->startingPosition( pStartingPosSelector_->text() );
	pStartupScreens_->startupData()->victoryCondition( pVictorySelector_->text() );
    pStartupScreens_->startupData()->numPlayers( pNumPlayersSelector_->text() );
	pStartupScreens_->startupData()->techLevel( pTechLevelSelector_->text() );
	pStartupScreens_->startupData()->playerRace( pColourSelector_->text() );

	return true;
}

MachGuiText* MachGuiCtxSkirmish::addSetting( bool numPlayersDropDown, MachGuiDropDownListBoxCreator*& pCreator, uint labelStrId, int numStrs, ... )
{
	const int width = (SETTINGS_LB_MAXX-SETTINGS_LB_MINX)/2;

	va_list marker;

   	va_start( marker, numStrs ); // Initialize variable arguments.

	GuiResourceString labelStr( labelStrId );
	GuiStrings strings;
    strings.reserve( numStrs + 1 );
	MachGuiText* pText = _NEW( MachGuiText( pSettingsList_, pStartupScreens_, width, labelStr.asString() ) );
	pText->textOffset( 1, 1 );

   	while( numStrs )
   	{
		--numStrs;

    	int listBoxStrId = va_arg( marker, int);

		GuiResourceString listBoxStr( listBoxStrId );
		strings.push_back( listBoxStr.asString() );
	}

	if ( numPlayersDropDown )
	{
		pCreator = _NEW( MachGuiNumPlayersDropDownListBoxCreator( pSettingsList_, pStartupScreens_, width, strings, true ) );
	}
	else
	{
		pCreator = _NEW( MachGuiDropDownListBoxCreator( pSettingsList_, pStartupScreens_, width, strings, true ) );
	}

	va_end( marker ); // Reset variable arguments.

	return pText;
}

void MachGuiCtxSkirmish::updateMaxPlayersList( MachGuiDbScenario& scenario )
{
	// Update "num players" setting so that, for example, you can't select a four player game when
	// the scenario doesn't support four players
	bool hasKeyboardFocus = false;

	if ( pNumPlayersSelector_ )
	{
		ASSERT( pNumPlayersText_, "this should be created at the same time as the pNumPlayersSelector_" );

		if ( pNumPlayersSelector_->hasFocusSet() )
		{
			hasKeyboardFocus = true;
		}

		_DELETE( pNumPlayersSelector_ );
		_DELETE( pNumPlayersText_ );
	}

	GuiResourceString str4( IDS_MENU_NUMPLAYERS4 );
	GuiResourceString str3( IDS_MENU_NUMPLAYERS3 );
	GuiResourceString str2( IDS_MENU_NUMPLAYERS2 );

	switch ( scenario.maxPlayers() )
	{
		case 4:
			{
				pNumPlayersText_ = addSetting( true, pNumPlayersSelector_, IDS_MENU_NUMPLAYERS, 3, IDS_MENU_NUMPLAYERS2, IDS_MENU_NUMPLAYERS3, IDS_MENU_NUMPLAYERS4 );

				if ( chosenNumPlayers() == 2 )
				{
					pNumPlayersSelector_->text( str2.asString() );
				}
				else if ( chosenNumPlayers() == 3 )
				{
					pNumPlayersSelector_->text( str3.asString() );
				}
				else
				{
					pNumPlayersSelector_->text( str4.asString() );
				}
			}
			break;
		case 3:
			{
				pNumPlayersText_ = addSetting( true, pNumPlayersSelector_, IDS_MENU_NUMPLAYERS, 2, IDS_MENU_NUMPLAYERS2, IDS_MENU_NUMPLAYERS3 );

				if ( chosenNumPlayers() == 2 )
				{
					pNumPlayersSelector_->text( str2.asString() );
				}
				else
				{
					pNumPlayersSelector_->text( str3.asString() );
				}
			}
			break;
		case 2:
			{
				pNumPlayersText_ = addSetting( true, pNumPlayersSelector_, IDS_MENU_NUMPLAYERS, 1, IDS_MENU_NUMPLAYERS2 );

				pNumPlayersSelector_->text( str2.asString() );
			}
			break;
		DEFAULT_ASSERT_BAD_CASE( scenario.maxPlayers() );
	}

	if ( hasKeyboardFocus )
	{
		pNumPlayersSelector_->hasFocus( true );
	}

	pSettingsList_->childrenUpdated();
}

///////////////////////////////////////////////////////////////////////////////////////////

class MachGuiStartSkirmishMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiStartSkirmishMessageBoxResponder( MachGuiCtxSkirmish* pSkirmishCtx )
	:	pSkirmishCtx_( pSkirmishCtx )
	{}

	virtual bool okPressed()
	{
		//if ( MachGui::machinesCDIsAvailable( 2 ) )
		{
		 	pSkirmishCtx_->autoLoadGame();
			return true;
		}
		//return false;
	}

private:
	MachGuiCtxSkirmish* pSkirmishCtx_;
};

///////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachGuiCtxSkirmish::buttonEvent( MachGuiStartupScreens::ButtonEvent buttonEvent )
{
	if ( buttonEvent == MachGuiStartupScreens::BE_DUMMY_OK )
	{
		#ifndef DEMO
		//if ( MachGui::machinesCDIsAvailable( 2 )
			#ifndef PRODUCTION
		//	 or getenv("cb_dontcheckcd") )
	   		#else
	   	//	)
			#endif
		{
		 	pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
		}
		//else
		//{
		//	pStartupScreens_->displayMsgBox( IDS_MENUMSG_INSERTCD, _NEW( MachGuiStartSkirmishMessageBoxResponder(this) ) );
		//}
		#else	// DEMO defined.
		 	pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
		#endif
	}
}

void MachGuiCtxSkirmish::autoLoadGame()
{
	autoLoadGame_ = true;
}

//static
int& MachGuiCtxSkirmish::chosenNumPlayers()
{
	static int chosenNumPlayersVal = SysRegistry::instance().queryIntegerValue( "Game Settings\\Num Players", "Value", SysRegistry::CURRENT_USER );

	if ( chosenNumPlayersVal == 0 )
	{
		chosenNumPlayersVal = 4;
	}

	return chosenNumPlayersVal;
}

/* End CTXSKIRM.CPP *************************************************/
