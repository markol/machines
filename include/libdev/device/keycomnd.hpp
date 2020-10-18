/*
 * K E Y C O M N D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    DevKeyToCommand

	Used in conjunction with DevKeyToCommandTranslator.
    Indicates what key presses are required to turn a command on. For example, you
	might want Ctrl+Alt+Delete to indicate a Delete command. To do this construct
	using : 
	DevKeyToCommand( DevKey::KEY_DELETE, MYDELETECOMMANDID, Modifier::PRESSED, Modifier::EITHER, Modifier::PRESSED );
*/

#ifndef _DEV_KEYCOMND_HPP
#define _DEV_KEYCOMND_HPP

#include "base/base.hpp"
#include "utility/property.hpp"
#include "device/butevent.hpp"
#include "device/keyboard.hpp"

class DevKeyToCommand
// Canonical form
{
public:
 	typedef DevKey::ScanCode ScanCode;
	typedef size_t CommandId;

	enum Modifier { PRESSED, RELEASED, EITHER };
	enum CtrlModifier { CTRLKEY_PRESSED, CTRLKEY_RELEASED, CTRLKEY_EITHER };
	enum ShiftModifier { SHIFTKEY_PRESSED, SHIFTKEY_RELEASED, SHIFTKEY_EITHER }; // 
	enum AltModifier { ALTKEY_PRESSED, ALTKEY_RELEASED, ALTKEY_EITHER };
	
	// Old constructor. Kept in for now so that client code doesn't have to change.
    DevKeyToCommand( ScanCode, CommandId, Modifier ctrl, Modifier shift, Modifier alt );

    DevKeyToCommand( ScanCode, CommandId, CtrlModifier = CTRLKEY_EITHER, ShiftModifier = SHIFTKEY_EITHER, AltModifier = ALTKEY_EITHER );

    ~DevKeyToCommand( void );

	Modifier ctrlReq() const;
	Modifier shiftReq() const;
	Modifier altReq() const;
	ScanCode scanCode() const;
	CommandId commandId() const;

    void CLASS_INVARIANT;

private:
	// Revoked
    bool operator ==( const DevKeyToCommand& );

    friend ostream& operator <<( ostream& o, const DevKeyToCommand& t );

	// data members
	CommandId commandId_;
	ScanCode scanCode_;
	Modifier ctrlReq_;
	Modifier shiftReq_;
	Modifier altReq_;
};


#endif

/* End MCEVENT.HPP **************************************************/
