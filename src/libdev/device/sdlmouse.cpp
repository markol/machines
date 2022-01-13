#include "device/mouse.hpp"

// My eyes can't do it
#define DEV_MOUSE_CLASS DevMouseT<RecRecorderDep, RecRecorderPrivDep, DevTimeDep, DEQDep>

// static
template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
DEV_MOUSE_CLASS& DEV_MOUSE_CLASS::instance()
{
    static DevMouse instance_;
    return instance_;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
DEV_MOUSE_CLASS::DevMouseT()
  :
    sdlDelegate_(),
    pSdl_(&sdlDelegate_),
    position_(0,0),
    lastPosition_(0,0),
    lButtonPressed_(false),
    rButtonPressed_(false),
    scaleX_(1),
    scaleY_(1),
    scrolledUp_(false),
    scrolledDown_(false)
{
    resetPosition();
    lastPosition_ = position_;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
DEV_MOUSE_CLASS::DevMouseT(SdlDelegate* useInstead)
  :
    DevMouseT()
{
    pSdl_ = useInstead;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
DEV_MOUSE_CLASS::~DevMouseT()
{
}

// Windows actually implements the counter which gives the
// nested behaviour specified by the DevMouseT interface.
template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::hide()
{
    // SDL_ShowCursor
    pSdl_->showCursor(SDL_DISABLE);
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::unhide()
{
    // SDL_ShowCursor
    pSdl_->showCursor(SDL_ENABLE);
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
bool DEV_MOUSE_CLASS::isHidden() const
{
	return cursorVisible_ < 0;
}

extern int mouseSleepTime;

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::position( XCoord new_x, YCoord new_y )
{
    position_.first  = static_cast<XCoord>( new_x * scaleX_ );
    position_.second = static_cast<YCoord>( new_y * scaleY_ );
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::changePosition( XCoord new_x, YCoord new_y )
{
    position( new_x / scaleX_, new_y / scaleY_ );

    // SDL call to move the mouse cursor to the new position
    // SDL_WarpMouseInWindow
    pSdl_->moveCursorToPosition(nullptr, new_x / scaleX_, new_y / scaleY_ );
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
const typename DEV_MOUSE_CLASS::Position&  // RETURN TYPE. Method below:
DEV_MOUSE_CLASS::position() const
{
    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        position_ = recorderPrivDependency_.get().playbackMousePosition();
    }
    else
    {
        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            recorderPrivDependency_.get().recordMousePosition( position_ );
        }
    }

    return position_;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
const typename DEV_MOUSE_CLASS::Position  // RETURN TYPE. Method below:
DEV_MOUSE_CLASS::deltaPosition() const
{
	Position retval = position();
	retval.first  -= lastPosition_.first;
	retval.second -= lastPosition_.second;
	return retval;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
typename DEV_MOUSE_CLASS::ButtonState  // RETURN TYPE. Method below:
DEV_MOUSE_CLASS::deltaLeftButton() const
{
    bool    currentLeftButtonState = leftButton();

    ButtonState result;

    if( currentLeftButtonState == lastLeftButtonState_ )
        result = NO_CHANGE;
    else if ( currentLeftButtonState )
        result = PRESSED;
    else
        result = RELEASED;

    lastLeftButtonState_ = currentLeftButtonState;

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
typename DEV_MOUSE_CLASS::ButtonState  // RETURN TYPE. Method below:
DEV_MOUSE_CLASS::deltaRightButton() const
{
    bool    currentRightButtonState = rightButton();

    ButtonState result;

    if( currentRightButtonState == lastRightButtonState_ )
        result = NO_CHANGE;
    else if ( currentRightButtonState )
        result = PRESSED;
    else
        result = RELEASED;

    lastRightButtonState_ = currentRightButtonState;

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::wm_button(const DevButtonEventType& ev)
{
    PRE(ev.scanCode() == DevKey::LEFT_MOUSE || ev.scanCode() == DevKey::RIGHT_MOUSE || ev.scanCode() == DevKey::MIDDLE_MOUSE);

    // Decode the message and set this object's internal state.
    switch (ev.scanCode())
    {
        case DevKey::LEFT_MOUSE:
            lButtonPressed_ = ev.action() == DevButtonEventType::PRESS;
        break;
        case DevKey::RIGHT_MOUSE:
            rButtonPressed_ = ev.action() == DevButtonEventType::PRESS;
        break;
        case DevKey::MIDDLE_MOUSE:
            scrolledUp_   = ev.action() == DevButtonEventType::SCROLL_UP;
            scrolledDown_ = ev.action() == DevButtonEventType::SCROLL_DOWN;
        break;
    }

    // The coords in the event should already be scaled correctly, so bypass
    // the position set method because it also applies a scale.
    position_.first  = ev.cursorCoords().x();
    position_.second = ev.cursorCoords().y();

    // Pass the message onto the event queue.
    eventQueueDependency_.get().queueEvent(ev);
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
bool DEV_MOUSE_CLASS::leftButton() const
{
    bool result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        result = recorderPrivDependency_.get().playbackLeftButton();
    }
    else
    {
        result = lButtonPressed_;

        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            recorderPrivDependency_.get().recordLeftButton( result );
        }
    }

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
bool DEV_MOUSE_CLASS::rightButton() const
{
    bool result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        result = recorderPrivDependency_.get().playbackRightButton();
    }
    else
    {
        result = rButtonPressed_;

        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            recorderPrivDependency_.get().recordRightButton( result );
        }
    }

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
bool DEV_MOUSE_CLASS::wheelScrollUp() const
{
    bool result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        std::cerr << "WARN: Recording/Playback NOT SUPPORTED for wheelScrollUp()" << std::endl;
        result = false;
    }
    else
    {
        result = scrolledUp_;

        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            std::cerr << "WARN: Recording/Playback NOT SUPPORTED for wheelScrollUp()" << std::endl;
        }
    }

    // We shall say the mouse is no longer scrolling until the next scroll event flips it back to true
    scrolledUp_ = false;
    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
bool DEV_MOUSE_CLASS::wheelScrollDown() const
{
    bool result;

    if( recorderDependency_.get().state() == RecRecorder::PLAYING )
    {
        std::cerr << "WARN: Recording/Playback NOT SUPPORTED for wheelScrollDown()" << std::endl;
        result = false;
    }
    else
    {
        result = scrolledDown_;

        if( recorderDependency_.get().state() == RecRecorder::RECORDING )
        {
            std::cerr << "WARN: Recording/Playback NOT SUPPORTED for wheelScrollDown()" << std::endl;
        }
    }

    // We shall say the mouse is no longer scrolling until the next scroll event flips it back to true
    scrolledDown_ = false;
    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::scaleCoordinates(XCoord xmax, YCoord ymax)
{
	maxPosition_.first  = xmax;
	maxPosition_.second = ymax;
	//const double screenX = GetSystemMetrics(SM_CXSCREEN);
	//const double screenY = GetSystemMetrics(SM_CYSCREEN);

	//scaleX_ = xmax / screenX;
	//scaleY_ = ymax / screenY;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
typename DEV_MOUSE_CLASS::Position //  RETURN TYPE. Method below:
DEV_MOUSE_CLASS::getMessagePos() const
{
    // SDL_GetMouseState
    const auto& unscaledXY = pSdl_->getCursorPosition();

    Position result;
    result.first  = static_cast<XCoord>( unscaledXY.first  * scaleX_ );
    result.second = static_cast<YCoord>( unscaledXY.second * scaleY_ );

    return result;
}

template<typename RecRecorderDep, typename RecRecorderPrivDep, typename DevTimeDep, typename DEQDep>
void DEV_MOUSE_CLASS::resetPosition()
{
    const XCoord x = (minRange().first  + maxRange().first ) / 2;
    const YCoord y = (minRange().second + maxRange().second) / 2;
    position(x,y);
}


//Instantiate the template identified by DevMouse alias
template class DevMouseT<RecRecorder, RecRecorderPrivate, DevTime, DevEventQueue>;
