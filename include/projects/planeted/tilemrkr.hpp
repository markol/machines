/*
 * T I L E M R K R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedTileMarker

    A brief description of the class should go in here
*/

#ifndef _PLANETED_TILEMRKR_HPP
#define _PLANETED_TILEMRKR_HPP

#include "base/base.hpp"
#include "world4d/custom.hpp"

class PedTileMarker	: public W4dCustom
{
public:
    PedTileMarker(  W4dEntity* pParent, 
    				const W4dTransform3d& localTransform,
					const MexAlignedBox3d& boundary, 
					bool permanent );

    ~PedTileMarker();

    void CLASS_INVARIANT;

	bool intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const;
	
private:
    // Operation deliberately revoked
    PedTileMarker( const PedTileMarker& );
    PedTileMarker& operator =( const PedTileMarker& );
    bool operator ==( const PedTileMarker& );

    friend ostream& operator <<( ostream& o, const PedTileMarker& t );
};


#endif

/* End TILEMRKR.HPP *************************************************/
