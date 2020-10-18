/*
 * C A M E R A . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/camera.hpp"

RenCamera::RenCamera():
	hither_(1),
	yon_(500),
	angle_(80.0 * Mathex::PI / 180.0),
    tanHalfAngle_( tan( 0.5 * angle_ ) ),
	colourFilter_(RenColour::white())
{
}

RenCamera::~RenCamera()
{
}

void RenCamera::transform(const MexTransform3d& xform)
{
	transform_ = xform;
}

const MexTransform3d& RenCamera::transform() const
{
	return transform_;
}	

void RenCamera::hitherClipDistance(double dist)
{
	PRE(dist > 0 );	
	hither_ = dist;
}

double RenCamera::hitherClipDistance() const
{
	return hither_;
}	

void RenCamera::yonClipDistance(double dist)
{
	PRE(dist > 0 );	
	yon_ = dist;
}

double RenCamera::yonClipDistance() const
{
	return yon_;
}	

// Only the vertical angle can be set.  The horizontal angle can be read.
void RenCamera::verticalFOVAngle(double angle)
{
	PRE(angle > 0);
	angle_ = angle;
    tanHalfAngle_ = tan( 0.5 * angle );
}

double RenCamera::verticalFOVAngle() const
{
	return angle_;
}	

double RenCamera::horizontalFOVAngle(double ratio) const
{
	return 2 * atan(ratio * tanHalfAngle_);
}

double RenCamera::projectionPlaneHalfHeight() const
{
	return hither_ * tanHalfAngle_;
}

double RenCamera::tanHalfVerticalFOVAngle() const
{
    return tanHalfAngle_;
}

const RenColour& RenCamera::colourFilter() const
{
	return colourFilter_;
}

void RenCamera::colourFilter(const RenColour& f)
{
	colourFilter_ = f;
}

