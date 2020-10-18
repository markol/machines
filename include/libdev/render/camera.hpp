/*
 * C A M E R A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _REN_CAMERA_HPP
#define _REN_CAMERA_HPP

#include "base/base.hpp"
#include "mathex/transf3d.hpp"
#include "render/colour.hpp"

//class ostream;

class RenCamera
{
public:
    RenCamera();
    virtual ~RenCamera();

	// The camera must be explicitly moved at the start of every frame.
	void transform(const MexTransform3d&);
	const MexTransform3d& transform() const;

	// Various camera properties.
	// PRE(dist > 0 );
	void   hitherClipDistance(double dist);		// default = 1
	double hitherClipDistance() const;

	// PRE(dist > 0 );
	void   yonClipDistance(double dist);		// default = 500
	double yonClipDistance() const;

	// Set the vertical field of view angle in radians.
	// Only the vertical angle can be set.  The horizontal angle can be read.
	// PRE(angle > 0 );
	void   verticalFOVAngle(double angle);		// default = 80 * PI / 180
	double verticalFOVAngle() const;

	double tanHalfVerticalFOVAngle() const; //tan( 0.5 * verticalFOVAngle() ), but faster

	// The camera will be set-up such that the horizontal angle will be
	// computed from the viewport's aspect ratio and the vertical angle.
	// Ratio is the viewport's or window's width / height.
	double horizontalFOVAngle(double ratio) const;

	// The dimensions of the projection plane are used for setting up the
	// viewport and projection matrices.  The projection plane is assumed
	// to be at the hither plane.
	double projectionPlaneHalfHeight() const;

	// A filter which is applied (by multiplication) to the entire image seen
	// by this camera.  This is analagous to the filters -- bits of coloured
	// plastic -- which you can buy for SLR cameras.
	void colourFilter(const RenColour&);
	const RenColour& colourFilter() const;

private:
	MexTransform3d	transform_;
	double			hither_, yon_, angle_;
    double 			tanHalfAngle_; 			// Caches the tan of 0.5*vertical FOV angle
	RenColour		colourFilter_;

    // Operations deliberately revoked.
    RenCamera( const RenCamera& );
    const RenCamera& operator =( const RenCamera& );
    bool operator ==( const RenCamera& ) const;
};

#endif
