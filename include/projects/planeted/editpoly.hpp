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
    PedPolygonEditor( void );
    ~PedPolygonEditor( void );

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void preRenderUpdate( void );

	virtual void writeCspFile( std::ofstream& );

	virtual void changingMode( void );

	virtual void activateMode( void );

	typedef ctl_pvector< PedPolygon > Polygons;
	typedef ctl_vector< MexPoint2d > PolyVerticies;


	const Polygons& polygons() const;

	bool polygonsHidden() const;

protected:
	void highlightVertex( void );
	// PRE( pSceneManager_ != NULL );

	void clearSelectedPolygon( void );
	void selectFirstPolygon( void );
	void displayVertexCoords( void );
	void processSelectPolygon( void );
	void processDropPolygon( void );
	void processDeletePolygon( void );
	void processPrevPolygon( void );
	void processPrevVertex( void );
	void processNextPolygon( void );
	void processNextVertex( void );
	void processPolygonRight( void );
	void processPolygonLeft( void );
	void processPolygonUp( void );
	void processPolygonDown( void );
	void processFlattenPolygons( void );
	void processPaste( bool tempPaste );
	void hidePolygons( bool );
	void numberPolygons();
	virtual void processVertexRight( void ) = 0;
	virtual void processVertexLeft( void ) = 0;
	virtual void processVertexUp( void ) = 0;
	virtual void processVertexDown( void ) = 0;
	virtual PedPolygon* createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected) const = 0;
	virtual PedPolygon* createDefaultPolygon() const = 0;
	virtual void updatePolygon( void ) = 0;
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
