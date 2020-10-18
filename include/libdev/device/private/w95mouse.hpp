/*
 * W 9 5 M O U S E . H P P
 * (c) Charybdis Limited, 1995, 1996, 1997. All Rights Reserved.
 */

/*
    DevMouse
    
    Provide an interface to the mouse on a Windows 95 system.
*/

#ifndef _WIN_MOUSE_HPP
#define _WIN_MOUSE_HPP

//////////////////////////////////////////////////////////////////////
#include "base/base.hpp"
#include "device/private/shrmouse.hpp"

class DevButtonEvent;

//////////////////////////////////////////////////////////////////////
// Mouse support under the Windows 95 operating system.  This class 
// should share the same protocol as other classes for differnt OSs.
class DevMouse : public DevSharedMouse
{
public:
	static DevMouse& instance();
	
    //  Return the absolute mouse position
    //  ( clipped by the range limits )
	const Position& position() const;

	bool  leftButton() const;
	bool rightButton() const;

    // These all return the change in state since the last time 
	// the method was called.
	const Position deltaPosition() const;
	ButtonState deltaLeftButton() const;
	ButtonState deltaRightButton() const;

	// Logically hide/show cursor ( successive calls are nested )
    void   hide();
    void unhide();
	bool isHidden() const;

	// Scale the output coordinates to be in the range [0,xmax) and [0,ymax).
	void scaleCoordinates(XCoord xmax, YCoord ymax);

	// Set the mouse's position.  Does NOT move the on screen pointer. Called by Win95App when
	// mouse move message ( WM_MOUSEMOVE ) is recieved. It is undesirable to move the on screen pointer as
	// this will generate another WM_MOUSEMOVE message.
	void position( XCoord newX, YCoord newY );

	// Set the mouse's position. This function updates the on screen pointer position. Should not be
	// called in response to a WM_MOUSEMOVE event because this function generates another WM_MOUSEMOVE
	// event.
	void changePosition( XCoord new_x, YCoord new_y );
	
	// This does the same as the Windows fn. of the same name, however, the
	// return value use the coordinate system of this class's position fn.
	Position getMessagePos() const;

private:
	friend class AfxWin95App;
	void wm_button(const DevButtonEvent&);

	void resetPosition();

	Position	position_;
	Position	lastPosition_;
	int			cursorVisible_;
	bool		lButtonPressed_;
	bool		rButtonPressed_;
	Position	maxPosition_;
	double		scaleX_, scaleY_;

	//  Singleton
	DevMouse();
	~DevMouse();
};

#endif
