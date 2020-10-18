/*
 * K E Y T R A N S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    DevKeyToCommandTranslator

    Takes a key press and updates a list of commands indicating whether or not the command
    is on. Enables different keyboard layouts to be applied to one command set.
*/

#ifndef _DEV_KEYTRANS_HPP
#define _DEV_KEYTRANS_HPP

#include "base/base.hpp"
#include "device/keycomnd.hpp"
#include "ctl/pvector.hpp"
#include "ctl/vector.hpp"

class DevButtonEvent;

class DevKeyToCommandTranslator
// Canonical form revoked
{
public:
	class Command
	{
	public:
		Command();
		bool on() const;
		void forceOn();

	private:
		// data members
		bool pressed_;	// true if commands main key is pressed (this doesn't mean that the command is on)
		bool on_;	 	// Indicates that the command is on.
		bool reset_;	// Command should be reset after use.

		friend class DevKeyToCommandTranslator; 
	};
	typedef ctl_vector< Command > CommandList; // Index pos is commandId

    DevKeyToCommandTranslator( void );
    ~DevKeyToCommandTranslator( void );

	void addTranslation( const DevKeyToCommand& );

	// Must be called before KeyTranslator is used to ensure that the correct
	// keyboard events are queued in DevEventQueue.
	void initEventQueue();

	// Should be called whenever there is a button event. Updates the clients
	// list of commands to indicate which commands are on. Returns true if
	// command using buttonEvent was found.
	bool translate( const DevButtonEvent&, CommandList* );
	// PRE( pCommandList );

	// Should be called after command list has been used (e.g. once every frame ).
	// Set forceReset to true if "this" is no longer going to be receiving buttonEvents
	// for translation. 
	void resetCommands( CommandList* pCommandList, bool forceReset = false );

	typedef size_t CommandId;
	// Should be called whenever there is a button event. Returns the CommandId and true
	// if a command using buttonEvent was found. It is up to the client to query the
	// buttonEvent.action() to decide if the button is pressed or not (i.e. the CommandId*
	// will be updated regardless of whether the button was pressed or released)
	// Alternative method to translate function	above, used when we are not interested 
	// in multiple concurrently active commands.
	bool translate( const DevButtonEvent&, CommandId* );
	// PRE( pCommandId );

	// Currently not implemented. Will be replaced by persistence when ready.
	void save();
	void load();

    void CLASS_INVARIANT;

private:
	// Operations revoked
    DevKeyToCommandTranslator( const DevKeyToCommandTranslator& );
    DevKeyToCommandTranslator& operator =( const DevKeyToCommandTranslator& );
    bool operator ==( const DevKeyToCommandTranslator& );

	friend ostream& operator <<( ostream& o, const DevKeyToCommandTranslator& t );

	bool needsReleasedState( Command& , DevKeyToCommand::Modifier modifier );
	bool needsPressedState( Command& , DevKeyToCommand::Modifier modifier );

	// data members
	ctl_pvector< DevKeyToCommand > commandTranslations_; 
};


#endif

/* End KEYTRANS.HPP *************************************************/
