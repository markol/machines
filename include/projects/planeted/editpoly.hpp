/*
 * E D I T P O L Y . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedPolygonEditor

    A brief description of the class should go in here
*/

#ifndef _PLANETED_EDITPOLY_HPP
#define _PLANETED_EDITPOLY_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "planeted/editmode.hpp"

class PedVertexMarker;
class PedPolygon;

class PedPolygonEditor : public PedEditorMode
// Canonical form revoked
{
public:
    PedPolygonEditor();
    ~PedPolygonEditor();

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void preRenderUpdate();

	virtual void writeCspFile( std::ofstream& );

	virtual void changingMode();

	virtual void activateMode();

	typedef ctl_pvector< PedPolygon > Polygons;
	typedef ctl_vector< MexPoint2d > PolyVerticies;


	const Polygons& polygons() const;

	bool polygonsHidden() const;

protected:
	void highlightVertex();
	// PRE( pSceneManager_ != NULL );

	void clearSelectedPolygon();
	void selectFirstPolygon();
	void displayVertexCoords();
	void processSelectPolygon();
	void processDropPolygon();
	void processDeletePolygon();
	void processPrevPolygon();
	void processPrevVertex();
	void processNextPolygon();
	void processNextVertex();
	void processPolygonRight();
	void processPolygonLeft();
	void processPolygonUp();
	void processPolygonDown();
	void processFlattenPolygons();
	void processPaste( bool tempPaste );
	void hidePolygons( bool );
	void numberPolygons();
	virtual void processVertexRight() = 0;
	virtual void processVertexLeft() = 0;
	virtual void processVertexUp() = 0;
	virtual void processVertexDown() = 0;
	virtual PedPolygon* createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected) const = 0;
	virtual PedPolygon* createDefaultPolygon() const = 0;
	virtual void updatePolygon() = 0;
	size_t vertexSpacingX() const;
	size_t vertexSpacingY() const;

	// Data...
	Polygons polygons_;
	PedVertexMarker* pHighlightVertex_;
	PedVertexMarker* pSelectedVertex_;
	PedPolygon* pSelectedPolygon_;
	PedPolygon* pPastePolygon_;
	MexPoint2d* pSelectedPoint_;
	bool mouseDrag_;
	bool hidePolygons_;
	bool flattenPolygons_;
	PolyVerticies copyVerticies_;

private:
	// Operations deliberately revoked
    PedPolygonEditor( const PedPolygonEditor& );
    PedPolygonEditor& operator =( const PedPolygonEditor& );
    bool operator ==( const PedPolygonEditor& );

	friend ostream& operator <<( ostream& o, const PedPolygonEditor& t );
};


#endif

/* End EDITPOLY.HPP *************************************************/
