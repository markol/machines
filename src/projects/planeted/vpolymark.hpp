/*
 * V P O L Y M A R K . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedVerticalPolygonMarker

    A brief description of the class should go in here
*/

#ifndef _PLANETED_VPOLYMARK_HPP
#define _PLANETED_VPOLYMARK_HPP

#include "base/base.hpp"
#include "world4d/custom.hpp"
#include "mathex/point3d.hpp"

class W4dEntity;
class MexTransform3d;

class PedVerticalPolygonMarker : public W4dCustom
// Canonical form revoked
{
public:
    PedVerticalPolygonMarker( W4dEntity*, const MexTransform3d&, const MexPoint3d&, const MexPoint3d&, const MexPoint3d& );
    ~PedVerticalPolygonMarker();

	void display( bool display );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const PedVerticalPolygonMarker& t );

    PedVerticalPolygonMarker( const PedVerticalPolygonMarker& );
    PedVerticalPolygonMarker& operator =( const PedVerticalPolygonMarker& );

	MexPoint3d position_;
};


#endif

/* End VPOLYMARK.HPP ************************************************/
