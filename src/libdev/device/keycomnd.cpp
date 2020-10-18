/*
 * K E Y C O M N D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "device/keycomnd.hpp"

DevKeyToCommand::DevKeyToCommand( 	DevKeyToCommand::ScanCode sc,
									DevKeyToCommand::CommandId comId,
									DevKeyToCommand::Modifier ctrl,
									DevKeyToCommand::Modifier shift,
									DevKeyToCommand::Modifier alt )
{
	commandId_ 	= comId;
	scanCode_ 	= sc;
	ctrlReq_ 	= ctrl;
	shiftReq_ 	= shift;
	altReq_ 	= alt;

    TEST_INVARIANT;
}

DevKeyToCommand::DevKeyToCommand(ScanCode sc,
								 CommandId comId,
								 CtrlModifier ctrl /*= CTRL_EITHER*/,
								 ShiftModifier shift /*= SHIFT_EITHER*/,
								 AltModifier alt /*= ALT_EITHER*/ )
{
	commandId_ = comId;
	scanCode_ = sc;
	switch ( ctrl )
	{
		case CTRLKEY_EITHER:
			ctrlReq_ = EITHER;
			break;
		case CTRLKEY_PRESSED:
			ctrlReq_ = PRESSED;
			break;
		case CTRLKEY_RELEASED:
			ctrlReq_ = RELEASED;
			break;
		DEFAULT_ASSERT_BAD_CASE( ctrl );
	}
	switch ( shift )
	{
		case SHIFTKEY_EITHER:
			shiftReq_ = EITHER;
			break;
		case SHIFTKEY_PRESSED:
			shiftReq_ = PRESSED;
			break;
		case SHIFTKEY_RELEASED:
			shiftReq_ = RELEASED;
			break;
		DEFAULT_ASSERT_BAD_CASE( shift );
	}
	switch ( alt )
	{
		case ALTKEY_EITHER:
			altReq_ = EITHER;
			break;
		case ALTKEY_PRESSED:
			altReq_ = PRESSED;
			break;
		case ALTKEY_RELEASED:
			altReq_ = RELEASED;
			break;
		DEFAULT_ASSERT_BAD_CASE( alt );
	}
}


DevKeyToCommand::~DevKeyToCommand()
{
    TEST_INVARIANT;

}

DevKeyToCommand::Modifier DevKeyToCommand::ctrlReq() const
{
	return ctrlReq_;
}

DevKeyToCommand::Modifier DevKeyToCommand::shiftReq() const
{
	return shiftReq_;
}

DevKeyToCommand::Modifier DevKeyToCommand::altReq() const
{
	return altReq_;
}

DevKeyToCommand::ScanCode DevKeyToCommand::scanCode() const
{
	return scanCode_;
}

DevKeyToCommand::CommandId DevKeyToCommand::commandId() const
{
	return commandId_;
}

void DevKeyToCommand::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const DevKeyToCommand& t )
{

    o << "DevKeyToCommand " << (void*)&t << " start" << std::endl;
    o << "DevKeyToCommand " << (void*)&t << " end" << std::endl;

    return o;
}

/* End KEYCOMND.CPP **************************************************/
