/*
 * O B S T E D I T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedObstacleEditor

    A brief description of the class should go in here
*/

#ifndef _PLANETED_OBSTEDIT_HPP
#define _PLANETED_OBSTEDIT_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "planeted/editpoly.hpp"
#include "planeted/obstacle.hpp"

class PedVerticalPolygonMarker;

class PedObstacleEditor	: public PedPolygonEditor
// Canonical form revoked
{
public:
    PedObstacleEditor();
    ~PedObstacleEditor();

    void CLASS_INVARIANT;

	virtual void initialise( W4dSceneManager*, MachLogPlanet* );
	// PRE( pPlanet_ != NULL );
	// PRE( pSceneManager_ != NULL );

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );
	
	virtual void displayKeyboardCtrls();

	virtual void displayModeInfo();

	virtual void readCspFile( const SysPathName& );
	// PRE( cspFileName.existsAsFile() );

	virtual void validate();

protected:
	virtual PedPolygon* createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected ) const;
	virtual PedPolygon* createDefaultPolygon() const;
	virtual void updatePolygon();
	virtual void processVertexRight();
	virtual void processVertexLeft();
	virtual void processVertexUp();
	virtual void processVertexDown();
	void processInsertVertex();
	void processDeleteVertex();
	void processDisplayVerticalPolygons( bool displayPolygons );
	void processComputeVerticalPolygons();
	void highlightAllObstacles();
	void processSelectPolygon();
	void processChangeObstacleType();

private:
	// Operations deliberately revoked
    PedObstacleEditor( const PedObstacleEditor& );
    PedObstacleEditor& operator =( const PedObstacleEditor& );
    bool operator ==( const PedObstacleEditor& );

    friend ostream& operator <<( ostream& o, const PedObstacleEditor& t );
	
	typedef ctl_pvector< PedVerticalPolygonMarker > VerticalPolygons;

	VerticalPolygons verticalPolygons_;
	bool displayVerticalPolygons_;   
};
#endif

/* End OBSTEDIT.HPP *************************************************/
