/*
 * E D I T D O M A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedDomainEditor

    A brief description of the class should go in here
*/

#ifndef _PLANETED_EDITDOMA_HPP
#define _PLANETED_EDITDOMA_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "planeted/editpoly.hpp"

class PedDomainEditor	: public PedPolygonEditor
// Canonical form revoked
{
public:
    PedDomainEditor( void );
    ~PedDomainEditor( void );

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayKeyboardCtrls( void );

	virtual void displayModeInfo( void );

	virtual void writeCspFile( std::ofstream& );

	virtual void readCspFile( const SysPathName& );
	// PRE( cspFileName.existsAsFile() );

	virtual void validate();

	void processInsertVertex();
	void processDeleteVertex();

protected:
	virtual PedPolygon* createPolygon( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected ) const;
	virtual PedPolygon* createDefaultPolygon() const;
	virtual void processVertexRight( void );
	virtual void processVertexLeft( void );
	virtual void processVertexUp( void );
	virtual void processVertexDown( void );
	virtual void updatePolygon( void );

private:
	// Operations deliberately revoked
    PedDomainEditor( const PedDomainEditor& );
    PedDomainEditor& operator =( const PedDomainEditor& );
    bool operator ==( const PedDomainEditor& );

    friend ostream& operator <<( ostream& o, const PedDomainEditor& t );

	void changeX( size_t fromX, size_t toX);
	void changeY( size_t fromY, size_t toY);
};

#endif

/* End EDITDOMA.HPP *************************************************/
