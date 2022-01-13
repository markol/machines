/*
 * S H R K E Y B D . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

#ifndef DEVICE_SHARED_KEYBOARD_HPP
#define DEVICE_SHARED_KEYBOARD_HPP

//#ifndef DEVICE_KEYBOARD
//	#error Do not include shrkeybd.hpp file directly, include keyboard.hpp
//#endif

#include "base/base.hpp"
//class ostream;

class DevKey
{
public:
	enum ScanCode {
		// keypad keys
		UP_ARROW_PAD = 72,
		DOWN_ARROW_PAD = 80,
		LEFT_ARROW_PAD = 75,
		RIGHT_ARROW_PAD = 77,
		ENTER_PAD = 156,
		INSERT_PAD = 82,
		DELETE_PAD = 83,
		HOME_PAD = 71,
		END_PAD = 79,
		PAGE_UP_PAD = 73,
		PAGE_DOWN_PAD = 81,
		FIVE_PAD = 76,
		ASTERISK_PAD = 55,
		FORWARD_SLASH_PAD = 181,
		PLUS_PAD = 78,
		MINUS_PAD = 74,
		PAD_0 = 82,	PAD_1 = 79,	PAD_2 = 80,	PAD_3 = 81,	PAD_4 = 75,
		PAD_5 = 76, PAD_6 = 77, PAD_7 = 71, PAD_8 = 72, PAD_9 = 73,

		// non-keypad keys
		FORWARD_SLASH = 53,
		NUM_LOCK = 197,
		SCROLL_LOCK = 70,
		RIGHT_CONTROL = 157,
		LEFT_CONTROL = 29,
		UP_ARROW = 200,
		DOWN_ARROW = 208,
		LEFT_ARROW = 203,
		RIGHT_ARROW = 205,
		INSERT = 210,
		KEY_DELETE = 211,		// Unfortunately Windows has a #define for DELETE.
		HOME = 199,
		END = 207,
		PAGE_UP = 201,
		PAGE_DOWN = 209,
		BREAK = 69,
		ESCAPE = 1,
		ENTER = 28,
		SPACE = 57,
		BACK_SPACE = 14,
		TAB = 15,
		LEFT_SHIFT = 42,
		RIGHT_SHIFT = 54,
		CAPS_LOCK = 58,
		F1 = 59, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		F11 = 87,
		F12 = 88,
		KEY_A = 30, KEY_B = 48, KEY_C = 46, KEY_D = 32, KEY_E = 18, KEY_F = 33,
		KEY_G = 34, KEY_H = 35,	KEY_I = 23,	KEY_J = 36,	KEY_K = 37,	KEY_L = 38,
		KEY_M = 50,	KEY_N = 49,	KEY_O = 24,	KEY_P = 25,	KEY_Q = 16,	KEY_R = 19,
		KEY_S = 31,	KEY_T = 20,	KEY_U = 22,	KEY_V = 47,	KEY_W = 17,	KEY_X = 45,
		KEY_Y = 21,	KEY_Z = 44,
		KEY_0 = 11,	KEY_1 = 2,	KEY_2 = 3,	KEY_3 = 4,	KEY_4 = 5,	KEY_5 = 6,
		KEY_6 = 7,	KEY_7 = 8,	KEY_8 = 9,	KEY_9 = 10,

    	LEFT_ALT = 56,
        RIGHT_ALT = 184,

		// These codes represent the mouse buttons, numbered left-to-right.
		// They allow the mouse to be processed in the saame manner as
		// keyboard keys.
		LEFT_MOUSE = 256, RIGHT_MOUSE = 257,

        // Not trusting that these guys didn't go LEFT_MOUSE+1 or something janky somewhere
        MIDDLE_MOUSE = 258,

		// This *must* be the highest code in this enumeration plus 1.
        MAX_CODE = 259
	};

	static void printScanCode(ostream&, ScanCode);

private:
	// Alt key support is not part of the public interface.
	friend class DevKeyboard;
};

//////////////////////////////////////////////////////////////////////
// This class maintains a table of which keys are currently pressed
// on a keyboard.  Derived classes provide the means of determining
// when a key is pressed under a particular OS.
//
// Getting this object's internal state to match the real world is quite
// tricky. In fact, it's impossible in a Windows environment where press
// and release events don't necessarily come in matched pairs (because
// your window may not always have the input focus).
class DevKeyboard
{
public:
	static DevKeyboard& instance();

	typedef DevKey::ScanCode ScanCode;

	// Is the key corresponding to a given character or key code pressed?
	bool key( unsigned char ) const;
	bool keyCode( ScanCode ) const;

	// Is any key depressed?
	bool anyKey() const;

	// Are either of these modifier keys pressed?
	bool shiftPressed() const;
	bool  ctrlPressed() const;
	bool   altPressed() const;

	// Has the given key changed its state since the last time it was queried?
	enum KeyState { NO_CHANGE, PRESSED, RELEASED };
	KeyState deltaKey( unsigned char ) const;
	KeyState deltaKeyCode( ScanCode ) const;

	// These are **not** for public use as they subvert the recording mechanism.
	bool keyNoRecord(unsigned char) const;
	bool keyCodeNoRecord(ScanCode) const;
	bool anyKeyNoRecord() const;
	bool shiftPressedNoRecord() const;
	bool ctrlPressedNoRecord() const;
	bool altPressedNoRecord() const;
	KeyState deltaKeyCodeNoRecord(ScanCode) const;

	// For testing only: to be removed.
	int pressedCount() const { return pressedCount_; }

	void CLASS_INVARIANT
	{
	    #ifdef _TEST_INVARIANTS
		keys_invariant(file, line);
		#endif
	}

protected:
	DevKeyboard();
	~DevKeyboard();

	// Derived classes inform us of key-state changes with these msgs.
	void pressed (ScanCode);
	void released(ScanCode);
	void allKeysReleased();

private:
	ScanCode getCharacterIndex( unsigned char ) const;

	enum    { N_KEYS = 256 };
	bool	keyMap_    [ N_KEYS ];
	bool	lastKeyMap_[ N_KEYS ];

	// This is the number of keys currently depressed, this should
	// equal the number of true elements in keyMap_.  Keeping these
	// consistent, especially in the presence of interrupts and races,
	// is tricky.  Is there a better way to implement anyKey()?
	int		pressedCount_;

	void keys_invariant(const char* file, const char* line) const;

	// Singleton => ops revoked.
	DevKeyboard( const DevKeyboard& );
	const DevKeyboard& operator =( const DevKeyboard& );
	bool operator ==( const DevKeyboard& ) const;
};

#endif

///////////////////////////////////
