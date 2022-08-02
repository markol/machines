/*
 * C A M R M A R K . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedCameraMarker

    A brief description of the class should go in here
*/

#ifndef _PLANETED_CAMRMARK_HPP
#define _PLANETED_CAMRMARK_HPP

#include "base/base.hpp"
#include "world4d/custom.hpp"
#include "render/colour.hpp"
#include "machlog/camera.hpp"


class W4dEntity;
class MexTransform3d;
class RenColour;

class PedCameraMarker : public W4dCustom
// Canonical form revoked
{
public:
    PedCameraMarker( W4dEntity*, const MexTransform3d&, MachLogCamera::Type, const RenColour& );
    ~PedCameraMarker();

    void CLASS_INVARIANT;

	void draw();

	MachLogCamera::Type cameraType() const { return cameraType_; }

private:
    friend ostream& operator <<( ostream& o, const PedCameraMarker& t );

    PedCameraMarker( const PedCameraMarker& );
    PedCameraMarker& operator =( const PedCameraMarker& );

	MachLogCamera::Type cameraType_;
	RenColour colour_;
};


#endif

/* End CAMRMARK.HPP *************************************************/
