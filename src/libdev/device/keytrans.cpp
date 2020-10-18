/*
 * K E Y T R A N S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "device/keytrans.hpp"
#include "device/keycomnd.hpp"
#include "device/butevent.hpp"
#include "device/eventq.hpp"

const size_t LEFT_ALT = 56;
const size_t RIGHT_ALT = 29;

bool DevKeyToCommandTranslator::Command::on() const
{
	return on_;
}

void DevKeyToCommandTranslator::Command::forceOn()
{
	if ( not on_ )
	{
		on_ = true;
		reset_ = true;
	}
}

DevKeyToCommandTranslator::Command::Command()
: 	on_( false ),
	pressed_( false ),
	reset_( false )
{}

DevKeyToCommandTranslator::DevKeyToCommandTranslator()
{
    commandTranslations_.reserve( 256 );
    TEST_INVARIANT;
}

DevKeyToCommandTranslator::~DevKeyToCommandTranslator()
{
    TEST_INVARIANT;

	for ( size_t i = 0; i < commandTranslations_.size(); ++i )
	{
		_DELETE( commandTranslations_[i] );
	}
}

void DevKeyToCommandTranslator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void DevKeyToCommandTranslator::addTranslation( const DevKeyToCommand& newTrans )
{
	commandTranslations_.push_back( _NEW( DevKeyToCommand( newTrans ) ) );
}

bool DevKeyToCommandTranslator::needsReleasedState( DevKeyToCommandTranslator::Command& command, DevKeyToCommand::Modifier modifier )
{
	bool processed = false;

	if ( modifier == DevKeyToCommand::RELEASED )
	{
		processed = true;
		command.reset_ = true;
	}
	else if ( modifier == DevKeyToCommand::PRESSED )
	{
		if ( command.pressed_ == true )
		{
			command.on_ = true;
			processed = true;
		}
	}

	return processed;
}

bool DevKeyToCommandTranslator::needsPressedState( DevKeyToCommandTranslator::Command& command, DevKeyToCommand::Modifier modifier )
{
	bool processed = false;

	if ( modifier == DevKeyToCommand::PRESSED )
	{
		processed = true;
		command.reset_ = true;
	}
	else if ( modifier == DevKeyToCommand::RELEASED )
	{
		if ( command.pressed_ == true )
		{
			command.on_ = true;
			processed = true;
		}
	}

	return processed;
}

bool DevKeyToCommandTranslator::translate( const DevButtonEvent& buttonEvent,
										   CommandId* pCommandId )
{
	PRE( pCommandId );

	bool found = false;	// Returned to caller to indicate that a command using the
						// buttonEvent was found.

	for ( size_t i = 0; i < commandTranslations_.size() && not found; ++i )
	{
		DevKeyToCommand* pEvent = commandTranslations_[i];
		if ( pEvent->scanCode() == buttonEvent.scanCode() )
		{
			bool ctrlCorrect = 	( pEvent->ctrlReq() == DevKeyToCommand::EITHER ) or
				   				( pEvent->ctrlReq() == DevKeyToCommand::PRESSED and buttonEvent.wasCtrlPressed() ) or
								( pEvent->ctrlReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasCtrlPressed() );
			bool shiftCorrect =	( pEvent->shiftReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->shiftReq() == DevKeyToCommand::PRESSED and buttonEvent.wasShiftPressed() ) or
								( pEvent->shiftReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasShiftPressed() );
			bool altCorrect =	( pEvent->altReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->altReq() == DevKeyToCommand::PRESSED and buttonEvent.wasAltPressed() ) or
								( pEvent->altReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasAltPressed() );

			if ( ctrlCorrect and shiftCorrect and altCorrect )
			{
				*pCommandId = pEvent->commandId();
				found = true;
			}
		}
	}

	return found;
}

bool DevKeyToCommandTranslator::translate( const DevButtonEvent& buttonEvent,
										   CommandList* pCommandList )
{
	PRE( pCommandList );

	CommandList& commandList = *pCommandList;
	bool found = false;	// Returned to caller to indicate that a command using the
						// buttonEvent was found.

	// Process key presses...
  	if ( buttonEvent.action() == DevButtonEvent::PRESS )
	{
		for ( size_t i = 0; i < commandTranslations_.size(); ++i )
		{
			DevKeyToCommand* pEvent = commandTranslations_[i];

			ASSERT( pEvent->commandId() < commandList.size(), "command list does not contain commandId" )

			// Find out if modifier keys are in correct state for this command
			bool ctrlCorrect = 	( pEvent->ctrlReq() == DevKeyToCommand::EITHER ) or
				   				( pEvent->ctrlReq() == DevKeyToCommand::PRESSED and buttonEvent.wasCtrlPressed() ) or
								( pEvent->ctrlReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasCtrlPressed() );
			bool shiftCorrect =	( pEvent->shiftReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->shiftReq() == DevKeyToCommand::PRESSED and buttonEvent.wasShiftPressed() ) or
								( pEvent->shiftReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasShiftPressed() );
			bool altCorrect =	( pEvent->altReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->altReq() == DevKeyToCommand::PRESSED and buttonEvent.wasAltPressed() ) or
								( pEvent->altReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasAltPressed() );

			if ( pEvent->scanCode() == buttonEvent.scanCode() )
			{
				found = true;

				// The button has been pressed but the command isn't necessarily turned on. This is
				// dependant on whether or not the shift, control and alt keys are in the correct
				// state.
				commandList[ pEvent->commandId() ].pressed_ = true;
			}

			if ( ctrlCorrect and shiftCorrect and altCorrect )
			{
				if ( commandList[ pEvent->commandId() ].pressed_ )
				{
					commandList[ pEvent->commandId() ].on_ = true;
					commandList[ pEvent->commandId() ].reset_ = false;
				}
			}
			else
			{
				commandList[ pEvent->commandId() ].reset_ = true;
			}
		}
	}
	else // Key released...
	{
		for ( size_t i = 0; i < commandTranslations_.size(); ++i )
		{
			DevKeyToCommand* pEvent = commandTranslations_[i];

			ASSERT( pEvent->commandId() < commandList.size(), "command list does not contain commandId" );

			// Find out if modifier keys are in correct state for this command
			bool ctrlCorrect = 	( pEvent->ctrlReq() == DevKeyToCommand::EITHER ) or
				   				( pEvent->ctrlReq() == DevKeyToCommand::PRESSED and buttonEvent.wasCtrlPressed() ) or
								( pEvent->ctrlReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasCtrlPressed() );
			bool shiftCorrect =	( pEvent->shiftReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->shiftReq() == DevKeyToCommand::PRESSED and buttonEvent.wasShiftPressed() ) or
								( pEvent->shiftReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasShiftPressed() );
			bool altCorrect =	( pEvent->altReq() == DevKeyToCommand::EITHER ) or
							   	( pEvent->altReq() == DevKeyToCommand::PRESSED and buttonEvent.wasAltPressed() ) or
								( pEvent->altReq() == DevKeyToCommand::RELEASED and not buttonEvent.wasAltPressed() );

			if ( pEvent->scanCode() == buttonEvent.scanCode() )
			{
				commandList[ pEvent->commandId() ].reset_ = true;
				commandList[ pEvent->commandId() ].pressed_ = false;
				found = true;
			}
			else
			{
				if ( ctrlCorrect and shiftCorrect and altCorrect )
				{
					if ( commandList[ pEvent->commandId() ].pressed_ )
					{
						commandList[ pEvent->commandId() ].on_ = true;
						commandList[ pEvent->commandId() ].reset_ = false;
					}
				}
				else
				{
					commandList[ pEvent->commandId() ].reset_ = true;
				}
			}
		}
	}

	return found;
}

void DevKeyToCommandTranslator::initEventQueue()
{
	for ( size_t i = 0; i < commandTranslations_.size(); ++i )
	{
		DevKeyToCommand* pEvent = commandTranslations_[i];
		DevEventQueue::instance().queueEvents( pEvent->scanCode() );
	}
	DevEventQueue::instance().queueEvents( DevKey::LEFT_SHIFT );
	DevEventQueue::instance().queueEvents( DevKey::RIGHT_SHIFT );
	DevEventQueue::instance().queueEvents( DevKey::LEFT_CONTROL );
	DevEventQueue::instance().queueEvents( DevKey::RIGHT_CONTROL );
	DevEventQueue::instance().queueEvents( (DevKey::ScanCode)LEFT_ALT );
	DevEventQueue::instance().queueEvents( (DevKey::ScanCode)RIGHT_ALT );
}

void DevKeyToCommandTranslator::resetCommands( CommandList* pCommandList, bool forceReset /* = false*/ )
{
	CommandList& commandList = *pCommandList;

	for ( size_t i = 0; i < commandList.size(); ++i )
	{
		if ( forceReset )
		{
			commandList[i].on_ = false;
			commandList[i].pressed_ = false;
		}
		else
			commandList[i].on_ = commandList[i].on_ and not commandList[i].reset_;

		commandList[i].reset_ = false;
	}
}

ostream& operator <<( ostream& o, const DevKeyToCommandTranslator& t )
{

    o << "DevKeyToCommandTranslator " << (void*)&t << " start" << std::endl;
    o << "DevKeyToCommandTranslator " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MCKEYBRD.CPP *************************************************/
