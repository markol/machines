#include "device/mouse.hpp"
#include "device/eventq.hpp"

#include "commouse.cpp"

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"
#include <SDL2/SDL.h>

DevMouse::DevMouse():
	position_(0,0),
	lastPosition_(0,0),
	lButtonPressed_(false),
	rButtonPressed_(false),
	scaleX_(1),
	scaleY_(1)
{
	resetPosition();
	lastPosition_ = position_;
}

DevMouse::~DevMouse()
{
}

// Windows actually implements the counter which gives the
// nested behaviour specified by the DevMouse interface.
void DevMouse::hide()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void DevMouse::unhide()
{
	SDL_ShowCursor(SDL_ENABLE);
}

bool DevMouse::isHidden() const
{
	return cursorVisible_ < 0;
}

extern int mouseSleepTime;

void DevMouse::position( XCoord new_x, YCoord new_y )
{
	position_.first  = _STATIC_CAST(XCoord, new_x * scaleX_);
	position_.second = _STATIC_CAST(YCoord, new_y * scaleY_);
}

void DevMouse::changePosition( XCoord new_x, YCoord new_y )
{
	position( new_x / scaleX_, new_y / scaleY_ );

	// SDL call to move the mouse cursor to the new position
	SDL_WarpMouseInWindow(NULL, new_x / scaleX_, new_y / scaleY_ );
}

const DevMouse::Position& DevMouse::position() const
{
    DevMouse* nonConstThis = _CONST_CAST( DevMouse*, this );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        nonConstThis->position_ = RecRecorderPrivate::instance().playbackMousePosition();
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordMousePosition( position_ );
    }

    return position_;
}

const DevMouse::Position DevMouse::deltaPosition() const
{
	Position retval = position();
	retval.first  -= lastPosition_.first;
	retval.second -= lastPosition_.second;
	return retval;
}

void DevMouse::wm_button(const DevButtonEvent& ev)
{
	PRE(ev.scanCode() == DevKey::LEFT_MOUSE || ev.scanCode() == DevKey::RIGHT_MOUSE);

	// Decode the message and set this object's internal state.
	switch (ev.scanCode())
	{
		case DevKey::LEFT_MOUSE:
			lButtonPressed_ = ev.action() == DevButtonEvent::PRESS;
			break;
		case DevKey::RIGHT_MOUSE:
			rButtonPressed_ = ev.action() == DevButtonEvent::PRESS;
			break;
	}

	// The coords in the event should already be scaled correctly, so bypass
	// the position set method because it also applies a scale.
	position_.first  = ev.cursorCoords().x();
	position_.second = ev.cursorCoords().y();

	// Pass the message onto the event queue.
	DevEventQueue::instance().queueEvent(ev);
}

bool DevMouse::leftButton() const
{
    bool result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackLeftButton();
    else
    {
  		result = lButtonPressed_;

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordLeftButton( result );
    }

    return result;
}

bool DevMouse::rightButton() const
{
    bool result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackRightButton();
    else
    {
  		result = rButtonPressed_;

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordRightButton( result );
    }

    return result;
}

void DevMouse::scaleCoordinates(XCoord xmax, YCoord ymax)
{
	maxPosition_.first  = xmax;
	maxPosition_.second = ymax;
	//const double screenX = GetSystemMetrics(SM_CXSCREEN);
	//const double screenY = GetSystemMetrics(SM_CYSCREEN);

	//scaleX_ = xmax / screenX;
	//scaleY_ = ymax / screenY;
}

DevMouse::Position DevMouse::getMessagePos() const
{
	int x, y;
	SDL_GetMouseState( &x, &y );

	Position result;
	result.first  = _STATIC_CAST(XCoord, x * scaleX_);
	result.second = _STATIC_CAST(YCoord, y * scaleY_);

	return result;
}
