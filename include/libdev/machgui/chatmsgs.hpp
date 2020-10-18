/*
 * C H A T M S G S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiInGameChatMessages

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CHATMSGS_HPP
#define _MACHGUI_CHATMSGS_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "stdlib/strfwd.hpp"

template < class T > class ctl_list;
template < class T > class ctl_vector;

struct MachGuiInGameChatMessagesImpl;
class MachGuiMessageBroker;
class MachGuiStartupScreens;

class MachGuiInGameChatMessages
// Canonical form revoked
{
public:
    //  Singleton class
    static MachGuiInGameChatMessages& instance( void );
    ~MachGuiInGameChatMessages( void );

	void initialise( MachGuiMessageBroker*, MachGuiStartupScreens* );

    void CLASS_INVARIANT;

	// Call every frame. Returns true if it has been updated.
	bool update();

	void addMessage( const string& );

	void clearAllMessages();

	const ctl_list< string >& messages() const;

	void sendMessage( const string& message, MachPhys::Race intendedRace );
	// PRE( pMessageBroker_ );

	const string& playerName() const;
	MachPhys::Race playerRace() const;

	bool opponentExists( int index ) const;
	string opponentName( int index ) const;
	MachPhys::Race opponentRace( int index ) const;
		
	static int reqWidth();
	static int reqHeight();

	// Get a list of standard messages that can be sent. This saves typing
	const ctl_vector< string >& standardMessages() const;

private:
    friend ostream& operator <<( ostream& o, const MachGuiInGameChatMessages& t );

    MachGuiInGameChatMessages( const MachGuiInGameChatMessages& );
    MachGuiInGameChatMessages& operator =( const MachGuiInGameChatMessages& );

    MachGuiInGameChatMessages( void );

	// Data members...
	MachGuiInGameChatMessagesImpl* pImpl_;
};


#endif

/* End CHATMSGS.HPP *************************************************/
