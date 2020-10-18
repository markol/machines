/*
 * T I L E D T O R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedTileEditor

    A brief description of the class should go in here
*/

#ifndef _PLANETED_TILEDTOR_HPP
#define _PLANETED_TILEDTOR_HPP

#include "base/base.hpp"

#include "planeted/editmode.hpp"

#include "system/pathname.hpp"
#include "device/butevent.hpp"
#include "mathex/line3d.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

class PedArtefactEditor;

// Foward references
class W4dSceneManager;
class MexPoint2d;
class MachLogPlanet;
class MachPhysTerrainTile;
class PedTileMarker;

class PedTileEditor : public PedEditorMode
{
// Canonical form revoked
public:
   	PedTileEditor( PedArtefactEditor& artefactEditor );
	// POST( selectedTiles_.empty() );

	~PedTileEditor( void );

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayKeyboardCtrls( void );

	virtual void displayModeInfo( void );

	virtual void preRenderUpdate( void );

	virtual void readCspFile( const SysPathName& );
	// PRE( cspFileName.existsAsFile() );

	virtual void writeCspFile( std::ofstream& );

	virtual void changingMode( void );

private:
	enum CycleDir { NEXT, PREV, CURRENT };

	struct SelectedTile
	{
		SelectedTile()
		: 	xPos_(0),
			yPos_(0),
			pSelectedTile_(NULL),
			pSelIndicator_(NULL)
		{}

		size_t xPos_;
		size_t yPos_;
		MachPhysTerrainTile* pSelectedTile_;
		PedTileMarker* pSelIndicator_;
	};

	void processAttatchCeilingArtefact( bool attatch );

	typedef ctl_list< SelectedTile* > SelectedTiles;

	 // Operation deliberately revoked
    PedTileEditor( const PedTileEditor& );
	PedTileEditor& operator =( const PedTileEditor& );
	bool operator ==( const PedTileEditor& );

	void highlightTile( void );
	// PRE( pSceneManager_ != NULL );

	void processSelection( const DevButtonEvent& devButtonEvent );
	void processRotation( bool clockwise );
	void processHeightChange( bool up, bool shiftPressed );
	void processCycleTile( CycleDir dir );

	// Called by displayInfo
	void displayTileCoords( void );

	bool isSelected( MachPhysTerrainTile* );
	void clearAllSelectedTiles();

	friend ostream& operator <<( ostream& o, const PedTileEditor& t );

	// Data...
	PedTileMarker* pHighlightTile_;
	SelectedTiles selectedTiles_;
	PedArtefactEditor& artefactEditor_;
};


#endif

/* End TILEDTOR.HPP *************************************************/
