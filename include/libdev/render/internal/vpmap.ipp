/*
 * V P M A P . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

// Note: The input co-ordinate space has y increasing up the
// screen.  The output space has y increasing down the screen.
// Hence, the y scale needs a negation which is absent in the x.
/*_CODE_INLINE
RenIViewportMapping::RenIViewportMapping(const D3DVIEWPORT2& d):
	left_(d.dwX),
	top_ (d.dwY),
	width_ (d.dwWidth),
	height_(d.dwHeight),
	scaleX(d.dwWidth  / d.dvClipWidth), // dvClipWidth = ratio * viewData.dvClipHeight
	scaleY(d.dwHeight / d.dvClipHeight), // dvClipHeight = 2.0
	offsetX(d.dwX - scaleX * d.dvClipX),
	offsetY(d.dwY + scaleY * d.dvClipY)		// negated see note
{
}*/

_CODE_INLINE
RenIViewportMapping::RenIViewportMapping(const int width, const int height, const int left, const int top):
	left_(left),
	top_ (top),
	width_ (width),
	height_(height),
	scaleX((float)width / ( ((float)width / height) * 2.0f) ),
    scaleY( (float)height / 2.0f),
	offsetX(0),
	offsetY(0)
{
}

/*_CODE_INLINE
void RenIViewportMapping::mapPoint(D3DTLVERTEX& vtx) const
{
	vtx.sx = vtx.sx *  scaleX + offsetX;
	vtx.sy = vtx.sy * -scaleY + offsetY;	// negated see note
}
*/

_CODE_INLINE
void RenIViewportMapping::mapPoint(RenIVertex& vtx) const
{
	vtx.x = vtx.x *  scaleX + offsetX;
	vtx.y = vtx.y * scaleY + offsetY;	// negated see note
}

_CODE_INLINE
int RenIViewportMapping::screenLeft() const
{
	return left_;
}

_CODE_INLINE
int RenIViewportMapping::screenTop() const
{
	return top_;
}

_CODE_INLINE
int RenIViewportMapping::screenWidth() const
{
	return width_;
}

_CODE_INLINE
int RenIViewportMapping::screenHeight() const
{
	return height_;
}

/* End VPMAP.IPP ****************************************************/
