/*
 * M A P B M P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedMapCreator

    A brief description of the class should go in here
*/

#ifndef _PLANETED_MAPBMP_HPP
#define _PLANETED_MAPBMP_HPP

#include "base/base.hpp"

class PedPolygonEditor;

class PedMapCreator
// Canonical form revoked
{
public:
    PedMapCreator( 	const PedPolygonEditor& domainEditor,
    				const PedPolygonEditor& portalEditor, 
					const PedPolygonEditor& obstacleEditor );
    ~PedMapCreator( void );

	void createBmp();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PedMapCreator& t );

private:
    PedMapCreator( const PedMapCreator& );
    PedMapCreator& operator =( const PedMapCreator& );
    bool operator ==( const PedMapCreator& );

	const PedPolygonEditor& domainEditor_;
	const PedPolygonEditor& portalEditor_; 
	const PedPolygonEditor& obstacleEditor_; 
};


#endif

/* End MAPBMP.HPP ***************************************************/
