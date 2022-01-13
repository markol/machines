/*
    DevMouse

    Provide an interface to the mouse on a SDL2.0 system.
*/

#ifndef _SDL_MOUSE_HPP
#define _SDL_MOUSE_HPP

//////////////////////////////////////////////////////////////////////
#include "base/base.hpp"
#include "device/private/shrmouse.hpp"
#include "device/butevent.hpp"
#include "device/SdlDelegate.hpp"

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"
#include "device/time.hpp"
#include "device/eventq.hpp"
#include "utility/DependencyProvider.hpp"

//////////////////////////////////////////////////////////////////////
// Mouse support under the SDL2.0 system.  This class
// should share the same protocol as other classes for differnt OSs.
template<typename RecRecorderDep = RecRecorder, typename RecRecorderPrivDep = RecRecorderPrivate, typename DevTimeDep = DevTime, typename DEQDep = DevEventQueue>
class DevMouseT : public DevSharedMouse
{
public:
    // No dependency provider for: DevTimeDep
    using DevButtonEventType = DevButtonEventT<DevTimeDep>;

    static DevMouseT& instance();

    //  Return the absolute mouse position
    //  ( clipped by the range limits )
	const Position& position() const;

    bool  leftButton() const;
    bool  rightButton() const;
    bool  wheelScrollUp() const;
    bool  wheelScrollDown() const;

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

protected:
    //  Singleton
    DevMouseT();
    explicit DevMouseT(SdlDelegate* useInstead);
    ~DevMouseT();

    DependencyProvider<RecRecorderDep>         recorderDependency_;
    DependencyProvider<RecRecorderPrivDep>     recorderPrivDependency_;
    DependencyProvider<DEQDep>                 eventQueueDependency_;

    void wm_button(const DevButtonEventType&);

private:
	friend class AfxSdlApp;

	void resetPosition();

    SdlDelegate  sdlDelegate_;
    SdlDelegate* pSdl_;        // <-- Use me

    mutable Position    position_;
    Position            lastPosition_;
    int                 cursorVisible_;
    bool                lButtonPressed_;
    bool                rButtonPressed_;
    Position            maxPosition_;
    double              scaleX_, scaleY_;
    mutable bool        scrolledUp_, scrolledDown_;
};

// !!!!!!!! CONCRETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!
using DevMouse = DevMouseT<RecRecorder, RecRecorderPrivate, DevTime, DevEventQueue>;
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#endif
