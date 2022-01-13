// Common bits of mouse source code.  Included from both
// the DOS and Windows95 mouse implementations.

#include "base/compiler.hpp"
#ifndef _SDLAPP

// static 
DevMouse& DevMouse::instance( void )
{
    static DevMouse instance;
    return instance;
}

///////////////////////////////////

DevMouse::ButtonState DevMouse::deltaLeftButton() const
{
    bool    currentLeftButtonState = leftButton();
    
    ButtonState result;
    
    if( currentLeftButtonState == lastLeftButtonState_ )
        result = NO_CHANGE;
    else if ( currentLeftButtonState )
        result = PRESSED;
    else
        result = RELEASED;

    DevMouse* nonConstThis = _CONST_CAST( DevMouse*, this );
    
    nonConstThis->lastLeftButtonState_ = currentLeftButtonState;
            
	return result;
}

///////////////////////////////////

DevMouse::ButtonState DevMouse::deltaRightButton() const
{
    bool    currentRightButtonState = rightButton();
    
    ButtonState result;
    
    if( currentRightButtonState == lastRightButtonState_ )
        result = NO_CHANGE;
    else if ( currentRightButtonState )
        result = PRESSED;
    else
        result = RELEASED;

    DevMouse* nonConstThis = _CONST_CAST( DevMouse*, this );
    
    nonConstThis->lastRightButtonState_ = currentRightButtonState;
            
	return result;
}

// Set the position to the middle of the range.
void DevMouse::resetPosition()
{
	const XCoord x = (minRange().first  + maxRange().first ) / 2;
	const YCoord y = (minRange().second + maxRange().second) / 2;
	position(x,y);
}

#endif
