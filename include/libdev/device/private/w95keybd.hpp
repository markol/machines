/*
 * W 9 5 K E Y B D . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

#ifndef DEVICE_W95_KEYBOARD_HPP
#define DEVICE_W95_KEYBOARD_HPP

#include "device/private/shrkeybd.hpp"

class DevButtonEvent;

//////////////////////////////////////////////////////////////////////
// Implements the mechanism for determining key-presses under Windows
// 95.  The public interface is entirely supplied by DevKeyboard.
class DevWin95Keyboard : public DevKeyboard
{
private:
	friend DevKeyboard& DevKeyboard::instance();
	static DevWin95Keyboard& win95Instance();
	DevWin95Keyboard();
	~DevWin95Keyboard();

	// The Windows proceedure communicates key-press events to this
	// class using these messages.
	friend class AfxWin95App;
	void wm_key(const DevButtonEvent& ev);
	void wm_char(const DevButtonEvent& ev);
	void wm_killfocus();
};

#endif

///////////////////////////////////
