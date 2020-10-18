/*
 * S H R K E Y B D . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

#include "base/base.hpp"

#ifdef _DOSAPP
  #include <i86.h>
#endif
#include "device/keyboard.hpp"

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"

DevKeyboard::DevKeyboard()
//: pressedCount_(0)
// Don't initialise pressedCount_ here, because it could cause a
// race condition: pressedCount_ and keyMap_ must be initialised
// together, both within the same critical section.
{
	// Rely on this to initilaise pressedCount_.
	allKeysReleased();
}

DevKeyboard::~DevKeyboard()
{
}


bool DevKeyboard::keyCode( ScanCode sCode ) const
{
	PRE( sCode < N_KEYS );
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackKeyCode();
    else
    {
        result = keyCodeNoRecord(sCode);

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordKeyCode( result );
    }

    return result;
}

DevKeyboard::KeyState DevKeyboard::deltaKeyCode( ScanCode sCode ) const
{
	PRE( sCode < N_KEYS );
	TEST_INVARIANT;

    DevKeyboard::KeyState result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackDeltaKeyCode();
    else
    {
		result = deltaKeyCodeNoRecord(sCode);

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordDeltaKeyCode( result );
    }

	return result;
}

bool DevKeyboard::key( unsigned char character ) const
{
	PRE( character < N_KEYS );
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackKeyCode();
    else
    {
		result = keyNoRecord(character);

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordKeyCode( result );
    }

    return result;
}

DevKeyboard::KeyState DevKeyboard::deltaKey( unsigned char character ) const
{
	PRE( character < N_KEYS );
	TEST_INVARIANT;
	return deltaKeyCode(getCharacterIndex( character ));
}

DevKeyboard::ScanCode DevKeyboard::getCharacterIndex( unsigned char c ) const
{
	// for a given character (it's ASCII value), this will return the
	// corresponding scan code
    static unsigned char characterMap[] =
    {
        /*   0 */    0,     0,     0,     0,     0,     0,     0,     15,
        /*   8 */    14,    0,     0,     0,     0,    28,     0,     0,
        /*  16 */    0,     0,     0,     0,     0,     0,     0,     0,
        /*  24 */    0,     0,     0,     1,     0,     0,     0,     0,
        /*  32 */    57,    0,    40,     0,     0,     0,     0,     40,
        /*  40 */    0,     0,    53,    13,    51,    12,     52,    53,
        /*  48 */    11,    2,     3,     4,     5,     6,     7,     8,
        /*  56 */    9,     10,   39,    39,    51,    13,     52,    53,
        /*  64 */    0,     30,   48,    46,    32,    18,     33,    34,
        /*  72 */    35,    23,   36,    37,    38,    50,     49,    24,
        /*  80 */    25,    16,   19,    31,    20,    22,     47,    17,
        /*  88 */    45,    21,   44,    26,     0,    27,     0,     12,
		/*  96 */	 0,	    30,   48,    46,    32,    18,     33,    34,
        /* 104 */    35,    23,   36,    37,    38,    50,     49,    24,
        /* 112 */    25,    16,   19,    31,    20,    22,     47,    17,
        /* 120 */    45,    21,   44,    26, 	 0,    27, 	   41, 	  0,
        /* 128 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 136 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 144 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 152 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 160 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 168 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 176 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 184 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 192 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 200 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 208 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 216 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 224 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 232 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 240 */    0,     0,     0,     0,     0,     0,     0,     0,
        /* 248 */    0,     0,     0,     0,     0,     0,     0,     0
    };

	return (ScanCode) characterMap[ c ];
}

bool DevKeyboard::anyKey() const
{
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackAnyKey();
    else
    {
        result = anyKeyNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordAnyKey( result );
    }

    return result;
}

bool DevKeyboard::shiftPressed() const
{
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackShiftPressed();
    else
    {
        result = shiftPressedNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordShiftPressed( result );
    }

    return result;
}

bool DevKeyboard::ctrlPressed() const
{
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackCtrlPressed();
    else
    {
        result = ctrlPressedNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordCtrlPressed( result );
    }

    return result;
}

bool DevKeyboard::altPressed() const
{
	TEST_INVARIANT;

    bool result;
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackAltPressed();
    else
    {
        result = altPressedNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordAltPressed( result );
    }

    return result;
}

void DevKeyboard::pressed(ScanCode code)
{
	// Although this is an OS independent piece of code, we must be
	// aware that it can be called from within a DOS interrupt
	// handler.  It appears that we are quite limited in the system
	// calls that can be made here.  Hence, no TEST_INVARIANT.
	PRE(code < N_KEYS);

	if (!keyMap_[code])
	{
		// An interrupt between the next two statements could cause this
		// object's state to become inconsistent.  We rely on the caller
		// having disabled interrupts under DOS!!
		keyMap_[code] = true;
		++pressedCount_;
	}
}

void DevKeyboard::released(ScanCode code)
{
	// Don't test invariant, see DevKeyboard::pressed.
	PRE(code < N_KEYS);

	if (keyMap_[code])
	{
		// Interrupts must be disabled, see DevKeyboard::pressed.
		keyMap_[code] = false;
		--pressedCount_;
	}
}

void DevKeyboard::allKeysReleased()
{
	TEST_INVARIANT;

	// Disable interrupts because pressedCount_ and keyMap_ can change
	// asyncronously.
	#ifdef _DOSAPP
	_disable();
	#endif

	for( int i = 0; i < N_KEYS; ++i )
	{
		keyMap_    [ i ] = false;
		lastKeyMap_[ i ] = false;
	}

	// An interrupt occuring here, between these statements, could set an
	// element of keyMap_ and increment pressedCount_, hence the disable.
	pressedCount_ = 0;

	#ifdef _DOSAPP
	_enable();
	#endif

	TEST_INVARIANT;
}

bool DevKeyboard::keyNoRecord(unsigned char character) const
{
	return keyMap_[getCharacterIndex(character)];
}

bool DevKeyboard::keyCodeNoRecord(ScanCode code) const
{
	return keyMap_[code];
}

bool DevKeyboard::anyKeyNoRecord() const
{
	return pressedCount_ > 0;
}

bool DevKeyboard::shiftPressedNoRecord() const
{
	return keyMap_[DevKey::RIGHT_SHIFT] or keyMap_[DevKey::LEFT_SHIFT];
}

bool DevKeyboard::ctrlPressedNoRecord() const
{
	return keyMap_[DevKey::RIGHT_CONTROL] or keyMap_[DevKey::LEFT_CONTROL];
}

bool DevKeyboard::altPressedNoRecord() const
{
	return keyMap_[DevKey::RIGHT_ALT] or keyMap_[DevKey::LEFT_ALT];
}

DevKeyboard::KeyState DevKeyboard::deltaKeyCodeNoRecord(ScanCode code) const
{
	KeyState result;

   	// An interrupt in the middle of the tests below could produce an incorrect
   	// result.  keyMap_[sCode] was previously evaluated *within* each if
   	// statement.  However, an interrupt could occur between the two if
   	// tests, so that the second one could see a different value for the same
   	// expression.  By sampling to a local variable, each if statement sees the
   	// same value.
   	const bool current =     keyMap_[code];
   	const bool last    = lastKeyMap_[code];

   	// keyMap_[sCode] may have changed since the last evaluation, so we must
   	// use current.
   	DevKeyboard* crufty = _CONST_CAST(DevKeyboard*, this);
   	crufty->lastKeyMap_[code] = current;

   	if( current == last )
   		result = NO_CHANGE;
   	else if( current and not last )
   		result = PRESSED;
   	else
   	{
   		// This assertion could actually fail if in the presence of
   		// race conditions (if keyMap_ were used rather than current).
   		ASSERT(not current and last, logic_error());
   		result = RELEASED;
   	}

	return result;
}


#ifdef _TEST_INVARIANTS
#include <sstream>
#endif

// We appear to get a crash if this is called from within a DOS interrupt
// handler.  Probably due to complicated non-reentrant things (like malloc).
#ifdef _TEST_INVARIANTS
void DevKeyboard::keys_invariant(const char* file, const char* line) const
#else
void DevKeyboard::keys_invariant(const char*, const char*) const
#endif
{
	// Disable interrupts because pressedCount_ and keyMap_ can change
	// asyncronously.
	#ifdef _DOSAPP
	_disable();
	#endif

	// pressedCount_ should always reflect the number of keys depressed
	// in keyMap_.
	int count = 0;
	for (int i=0; i!=N_KEYS; ++i)
		if (keyMap_[i])
			++count;

	// Re-enable interrupts before testing any invariant which may fail.
	const int pressed_count = pressedCount_;
	#ifdef _DOSAPP
	_enable();
	#endif

	if (pressed_count != count)
	{
		std::ostringstream ostr;
		ostr << "KeyMap is:\n";
		for (int j=0; j!=N_KEYS; ++j)
		{
			ostr << ((keyMap_[j])? "1 ": "0 ");

			if (!(j % 32))
				ostr << "\n";
		}

		ostr << std::ends;
		INVARIANT_INFO(ostr.str());
		//ostr.rdbuf()->freeze(0);
	}

	INVARIANT_INFO(pressed_count);
	INVARIANT_INFO(count);
	INVARIANT(pressed_count == count);
}

// static
void DevKey::printScanCode(ostream& o, ScanCode code)
{
	switch (code)
	{
		case DevKey::ENTER_PAD:			o << "ENTER_PAD"; break;
		case DevKey::ASTERISK_PAD:		o << "ASTERISK_PAD"; break;
		case DevKey::FORWARD_SLASH_PAD:	o << "FORWARD_SLASH_PAD"; break;
		case DevKey::PLUS_PAD:			o << "PLUS_PAD"; break;
		case DevKey::MINUS_PAD:			o << "MINUS_PAD"; break;
		case DevKey::PAD_0:				o << "PAD_0"; break;
		case DevKey::PAD_1:				o << "PAD_1"; break;
		case DevKey::PAD_2:				o << "PAD_2"; break;
		case DevKey::PAD_3:				o << "PAD_3"; break;
		case DevKey::PAD_4:				o << "PAD_4"; break;
		case DevKey::PAD_5:				o << "PAD_5"; break;
		case DevKey::PAD_6:				o << "PAD_6"; break;
		case DevKey::PAD_7:				o << "PAD_7"; break;
		case DevKey::PAD_8:				o << "PAD_8"; break;
		case DevKey::PAD_9:				o << "PAD_9"; break;
		case DevKey::SCROLL_LOCK:		o << "SCROLL_LOCK"; break;
		case DevKey::RIGHT_CONTROL:		o << "RIGHT_CONTROL"; break;
		case DevKey::LEFT_CONTROL:		o << "LEFT_CONTROL"; break;
		case DevKey::UP_ARROW:			o << "UP_ARROW"; break;
		case DevKey::DOWN_ARROW:		o << "DOWN_ARROW"; break;
		case DevKey::LEFT_ARROW:		o << "LEFT_ARROW"; break;
		case DevKey::RIGHT_ARROW:		o << "RIGHT_ARROW"; break;
		case DevKey::INSERT:			o << "INSERT"; break;
		case DevKey::KEY_DELETE:		o << "KEY_DELETE"; break;
		case DevKey::HOME:				o << "HOME"; break;
		case DevKey::END:				o << "END"; break;
		case DevKey::BREAK:				o << "BREAK"; break;
		case DevKey::PAGE_UP:			o << "PAGE_UP"; break;
		case DevKey::PAGE_DOWN:			o << "PAGE_DOWN"; break;
		case DevKey::ESCAPE:			o << "ESCAPE"; break;
		case DevKey::ENTER:				o << "ENTER"; break;
		case DevKey::SPACE:				o << "SPACE"; break;
		case DevKey::BACK_SPACE:		o << "BACK_SPACE"; break;
		case DevKey::TAB:				o << "TAB"; break;
		case DevKey::LEFT_SHIFT:		o << "LEFT_SHIFT"; break;
		case DevKey::RIGHT_SHIFT:		o << "RIGHT_SHIFT"; break;
		case DevKey::CAPS_LOCK:			o << "CAPS_LOCK"; break;
		case DevKey::F1:				o << "F1"; break;
		case DevKey::F2:				o << "F2"; break;
		case DevKey::F3:				o << "F3"; break;
		case DevKey::F4:				o << "F4"; break;
		case DevKey::F5:				o << "F5"; break;
		case DevKey::F6:				o << "F6"; break;
		case DevKey::F7:				o << "F7"; break;
		case DevKey::F8:				o << "F8"; break;
		case DevKey::F9:				o << "F9"; break;
		case DevKey::F10:				o << "F10"; break;
		case DevKey::F11:				o << "F11"; break;
		case DevKey::F12:				o << "F12"; break;
		case DevKey::KEY_A:				o << "KEY_A"; break;
		case DevKey::KEY_B:				o << "KEY_B"; break;
		case DevKey::KEY_C:				o << "KEY_C"; break;
		case DevKey::KEY_D:				o << "KEY_D"; break;
		case DevKey::KEY_E:				o << "KEY_E"; break;
		case DevKey::KEY_F:				o << "KEY_F"; break;
		case DevKey::KEY_G:				o << "KEY_G"; break;
		case DevKey::KEY_H:				o << "KEY_H"; break;
		case DevKey::KEY_I:				o << "KEY_I"; break;
		case DevKey::KEY_J:				o << "KEY_J"; break;
		case DevKey::KEY_K:				o << "KEY_K"; break;
		case DevKey::KEY_L:				o << "KEY_L"; break;
		case DevKey::KEY_M:				o << "KEY_M"; break;
		case DevKey::KEY_N:				o << "KEY_N"; break;
		case DevKey::KEY_O:				o << "KEY_O"; break;
		case DevKey::KEY_P:				o << "KEY_P"; break;
		case DevKey::KEY_Q:				o << "KEY_Q"; break;
		case DevKey::KEY_R:				o << "KEY_R"; break;
		case DevKey::KEY_S:				o << "KEY_S"; break;
		case DevKey::KEY_T:				o << "KEY_T"; break;
		case DevKey::KEY_U:				o << "KEY_U"; break;
		case DevKey::KEY_V:				o << "KEY_V"; break;
		case DevKey::KEY_W:				o << "KEY_W"; break;
		case DevKey::KEY_X:				o << "KEY_X"; break;
		case DevKey::KEY_Y:				o << "KEY_Y"; break;
		case DevKey::KEY_Z:				o << "KEY_Z"; break;
		case DevKey::KEY_0:				o << "KEY_0"; break;
		case DevKey::KEY_1:				o << "KEY_1"; break;
		case DevKey::KEY_2:				o << "KEY_2"; break;
		case DevKey::KEY_3:				o << "KEY_3"; break;
		case DevKey::KEY_4:				o << "KEY_4"; break;
		case DevKey::KEY_5:				o << "KEY_5"; break;
		case DevKey::KEY_6:				o << "KEY_6"; break;
		case DevKey::KEY_7:				o << "KEY_7"; break;
		case DevKey::KEY_8:				o << "KEY_8"; break;
		case DevKey::KEY_9:				o << "KEY_9"; break;
		case DevKey::LEFT_MOUSE:		o << "LEFT_MOUSE"; break;
		case DevKey::RIGHT_MOUSE:		o << "RIGHT_MOUSE"; break;

		// NB: don't use ASSERT_BAD_CASE because it is non-extensible.  New scan
		// codes can be added and will work even if this fn doesn't print them.
		default:
			o << "Unknown scan code, numeric value=" << code;
			break;
	}
}
