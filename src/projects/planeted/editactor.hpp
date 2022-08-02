/*
 * E D I T A C T O R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedActorEditor

    A brief description of the class should go in here
*/

#ifndef _EDITACTOR_HPP
#define _EDITACTOR_HPP

//#include "base/base.hpp"
#include "device/butevent.hpp"
#include "planeted/editmode.hpp"
#include "machphys/machphys.hpp"
#include "world4d/entity.hpp"

class PedVertexMarker;
class W4dEntity;
class UtlLineTokeniser;
class wdebug;
class MachPhysMarker;
//class DevButtonEvent;
class SysPathName;
class W4dComposite;
class W4dSceneManager;
class MachLogPlanet;
class MexPoint3d;
class MexTransform3d;


class PedActorEditor : public PedEditorMode
// Canonical form revoked
{
public:
    PedActorEditor( const string& actorType );
    virtual ~PedActorEditor();

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayKeyboardCtrls();

	virtual void preRenderUpdate();

	virtual void displayModeInfo();

	virtual void changingMode();

	virtual void activateMode();

	virtual void initialise( W4dSceneManager* pSceneManager, MachLogPlanet* pPlanet );

	static void changeRace( MachPhys::Race* race );

protected:

	enum CycleDir { NEXT, PREV, CURRENT };

	enum MoveDir { UP, DOWN, RIGHT, LEFT };

	enum ActorType { CONSTRUCTION = 10000, MACHINE, ARTEFACT };

	virtual W4dEntity* currentActor() = 0;
	virtual void processCycle( CycleDir dir ) = 0;
	virtual void processSelection() = 0;
	virtual void processDelete() = 0;
	virtual void processRace();
	virtual void createEntity( W4dEntity&, const MexTransform3d& ) = 0;
	virtual void rotateAfterMove() = 0;
	virtual void processHide( bool );
 	virtual void changeAllSolidities( W4dEntity::Solidity ) = 0;
    virtual void processMove( MoveDir dir );
	virtual void processDrag();

	const MexTransform3d localTransformAtMouse();
	const MexTransform3d localTransformAtPoint( W4dEntity** ppDomain, const MexPoint3d& point );

	bool actorSelected( W4dEntity** ppActor );
	void highlightSelected();
//	void changeRace( MachPhys::Race* race );	// TBD Put in namespace
	size_t vertexSpacingX() const;
	size_t vertexSpacingY() const;
	void  parseRace( const string& );			// TBD Put in namespace
	MachPhys::WeaponCombo weaponCombo( const string& type );

	// Data..
	string actorType_;
	MachPhys::Race race_;
	MachPhysMarker* pMarker_;
	UtlLineTokeniser* pParser_;

	bool mouseDrag_;
	bool hidden_;
	bool alreadySelected_;

	wdebug* errors;

private:
	void processDrop();
	void processReleaseSelection();

	bool domainAtMousePoint( W4dEntity** ppEntity );
	W4dEntity* domain( const MexPoint3d& point );
	bool mouseVertexPoint( MexPoint3d* intersectPoint );
    friend ostream& operator <<( ostream& o, const PedActorEditor& t );

    PedActorEditor( const PedActorEditor& );
    PedActorEditor& operator =( const PedActorEditor& );

	void highlightVertex();
	// PRE( pSceneManager_ != NULL )
 	void displayVertexCoords();
	// PRE( pSceneManager_ != NULL )

    // Data...
    PedVertexMarker* pVertexMarker_;
};


#endif

/* End EDITACTOR.HPP ************************************************/
