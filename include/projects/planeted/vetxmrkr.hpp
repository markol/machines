/*
 * V E T X M R K R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedVertexMarker

    A brief description of the class should go in here
*/

#ifndef _PLANETED_VETXMRKR_HPP
#define _PLANETED_VETXMRKR_HPP

#include "base/base.hpp"

#include "world4d/custom.hpp"

#include "mathex/point2d.hpp"

class MexPoint3d;

class PedVertexMarker : public W4dCustom
{
public:
    PedVertexMarker( size_t x, size_t y, bool highlight );
    PedVertexMarker( const MexPoint3d& point, bool highlight );
    ~PedVertexMarker( void );

    void CLASS_INVARIANT;

	const MexPoint2d& position() const;

    friend ostream& operator <<( ostream& o, const PedVertexMarker& t );

private:
    // Operations deliberately revoked
    PedVertexMarker( const PedVertexMarker& );
    PedVertexMarker& operator =( const PedVertexMarker& );
    bool operator ==( const PedVertexMarker& );

	void addSmallBox( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, bool highlight );

	// Data...
	MexPoint2d position_;
};


#endif

/* End VETXMRKR.HPP *************************************************/
