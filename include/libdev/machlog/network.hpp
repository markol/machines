/*
 * N E T W O R K . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNetwork

    MachLog network messaging controlling class
*/

#ifndef _MACHLOG_NETWORK_HPP
#define _MACHLOG_NETWORK_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "machphys/machphys.hpp"

class NetNode;
class MachLogMessageBroker;
class MachLogNetworkDataImpl;

class MachLogNetwork
// Canonical form revoked
{
public:
	enum Remote { LOCAL_PROCESS, REMOTE_PROCESS };
	enum InitialiseConnection { INITIALISE_CONNECTION, DO_NOT_INITIALISE_CONNECTION };
    //  Singleton class
    static MachLogNetwork& instance( void );
    ~MachLogNetwork( void );

	void processNetworkIniSettings( const string& fileName );
	void terminateAndReset();

	MachPhys::Race	localRace() const;
	void localRace( MachPhys::Race );

	bool			isNetworkGame() const;
	bool			isNodeLogicalHost() const;
	void			isNodeLogicalHost( bool );
	Remote			remoteStatus( MachPhys::Race ) const;
	void			remoteStatus( MachPhys::Race, Remote );
	void			setBroker( MachLogMessageBroker* );
	void 			update();
	int				expectedPlayers() const;
	bool			syncSimCycles() const;
	bool			simUpdateNeeded( MachPhys::Race ) const;
	void			simUpdateNeeded( MachPhys::Race, bool );
	bool			simUpdateReceived( MachPhys::Race ) const;
	void			simUpdateReceived( MachPhys::Race, bool );

	NetNode&		node();
	MachLogMessageBroker&	messageBroker();

	void ready( MachPhys::Race, bool newValue );
	bool ready( MachPhys::Race ) const;

	const string&					desiredProtocol() const;
	bool							desiredProtocol( const string&, InitialiseConnection = INITIALISE_CONNECTION );
	void							initialiseConnection();

	const string&					sessionId() const;
	bool							hostWithSessionId( const string& gameName, const string& playerName  );
	bool							joinWithSessionId( const string& gamename, const string& playerName );
	bool 							launchFromLobbyInfo();
	void							setNodeCompoundStatus( bool );


    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogNetwork& t );

private:
    MachLogNetwork( const MachLogNetwork& );
    MachLogNetwork& operator =( const MachLogNetwork& );
    bool operator ==( const MachLogNetwork& );

    MachLogNetwork( void );

	MachLogNetworkDataImpl*			pImpl_;

};


#endif

/* End NETWORK.HPP **************************************************/
