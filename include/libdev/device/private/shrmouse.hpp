/*
 * S H R M O U S E . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved.
 */

#ifndef _DEV_SHARED_MOUSE_HPP
#define _DEV_SHARED_MOUSE_HPP

#ifndef DEVICE_MOUSE
        #error Do not include shrmouse.hpp file directly, include mouse.hpp
#endif

//////////////////////////////////////////////////////////////////////

#include "base/base.hpp"
#include "ctl/utility.hpp"

//////////////////////////////////////////////////////////////////////
// All the bits common to all mouse implementations.
class DevSharedMouse
{
public:
    typedef int32 XCoord;
    typedef int32 YCoord;

    typedef std::pair< XCoord, YCoord >  Position;

    enum ButtonState { NO_CHANGE, PRESSED, RELEASED };

    const Position& minRange( void ) const;
    const Position& maxRange( void ) const;

        // Clip the mouse coordinates to be within the given range.  If the mouse
        // is moved outside this area, then coordinates on the edge are returned.
    void range( const Position& min, const Position& max );

protected:
        DevSharedMouse();
    void clipToRange(Position&) const;

    bool    lastLeftButtonState_;
    bool    lastRightButtonState_;

private:
    Position    minRange_;
    Position    maxRange_;
};

#endif     /*    #ifndef  _DEV_SHARED_MOUSE_HPP     */

/* End SHRMOUSE.HPP ****************************************************/
