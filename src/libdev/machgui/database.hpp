/*
 * D A T A B A S E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDatabase

    Collates all the information about which scenarios are available for campaigns,
    skirmishes and multi-player games, and the current status of all registered players.
*/

#ifndef _MACHGUI_DATABASE_HPP
#define _MACHGUI_DATABASE_HPP

#include "base/base.hpp"

//Forward refs
class UtlLineTokeniser;
class SysPathName;
class MachGuiIDatabase;
class MachGuiDbSystem;
class MachGuiDbPlanet;
class MachGuiDbPlayer;
class MachGuiDbSavedGame;
class MachGuiDbScenario;
class MachGuiDatabaseHandler;

class MachGuiDatabase
// Canonical form revoked
{
public:
    //Constructs a new database, when called for the first time, by reading the appropriate file.
    //Uses one of the following in priority order if exists:
    //  ..savegame/playinfo.bin (persistence file including all player data)
    //  ..pdata/sceninfo.bin (persistence file with no player info)
    //  ..data/sceninfo.dat (text file defining scenario hierarchy)
    static MachGuiDatabase& instance();

    //Writes this database out to ..savegame/playinfo.bin
    void writeDatabase() const;

    void CLASS_INVARIANT;

    //Export the database handler used for interfacing with machlog
    MachGuiDatabaseHandler& handler();

    //used to indicate the sizes of terrain for skirmishes and multi-player games.
    //Explicit values allow indexing into array on type, and will not change.
    enum TerrainSize{ SMALL = 0, MEDIUM = 1, LARGE = 2, CUSTOM = 3 };

    //the number of campaign systems
    uint nCampaignSystems() const;

    //The index'th campaign system
    MachGuiDbSystem& campaignSystem( uint index );
    //PRE( index < nCampaignSystems() );

    //Search for a campaign scenario identified by name, returning true if found, and its pointer
    //in pDbScenario.
    bool campaignScenario( const string& name, MachGuiDbScenario** pDbScenario );

    //The system object for skirmishes of specified size
    MachGuiDbSystem& skirmishSystem( TerrainSize size );

    //The system object for the multi-player games of specified size
    MachGuiDbSystem& multiPlayerSystem( TerrainSize size );

    //Unload all loaded text data objects. Should be called at intervals.
    void clearAllTextData();

    //Are there even any custom scenarios to begin with?
    static bool doCustomScenariosExist();
    ///////////////////////////////////////////
    //  Players

    //Sort the collection of players into name/time order
    void sortPlayersByName();
    void sortPlayersByTime();

    //Add a new player using name, returning the player object ref
    MachGuiDbPlayer& addPlayer( const string& name );

    //remove pDbPlayer from the database. Any saved games referencing this player
    //will have the player field set to null.
    void removePlayer( MachGuiDbPlayer* pDbPlayer );

    //The number of registered players
    uint nPlayers() const;

    //The index'th player
    MachGuiDbPlayer& player( uint index ) const;
    //PRE( index < nPlayers() );

    //Set/get current player. This affects the availability flags for teh campaign scenarios.
    MachGuiDbPlayer& currentPlayer() const;
    void currentPlayer( MachGuiDbPlayer* pDbPlayer );
	// Has current player been set?
	bool hasCurrentPlayer() const;
	// Reset the current player to NULL.
	void clearCurrentPlayer();

    //Set/get the currently playing scenario
    void currentScenario( MachGuiDbScenario* pDbScenario );
    MachGuiDbScenario& currentScenario() const;
    //PRE( hasCurrentScenario() );

    //True if there is a current scenario
    bool hasCurrentScenario() const;

    ///////////////////////////////////////////
    // Saved games

    //the number of saved games
    uint nSavedGames() const;

    //the index'th saved game
    MachGuiDbSavedGame& savedGame( uint index ) const;
    //PRE( index < nSavedGames() );

    //Add a new saved game
    void addSavedGame( MachGuiDbSavedGame* pDbSavedGame );

    //Remove an old saved game. Does not delete the saved game object, or the disk file.
    void removeSavedGame( MachGuiDbSavedGame* pDbSavedGame );

    ///////////////////////////////////////////

private:
    friend ostream& operator <<( ostream& o, const MachGuiDatabase& t );

    MachGuiDatabase();
    ~MachGuiDatabase();

    void readDatabase();

    //register the persistence for hierarchical classes (only does it once)
    static void registerDerivedClasses();

    //Construct the database by parsing the campaign file data/sceninfo.dat
    void parseCampaignFile();
    //Construct the user scenarios database
    void parseUserCampaignFile();

    enum Context{ CAMPAIGNS, SKIRMISHES, MULTI_PLAYER };

    //Parses a CAMPAIGNS, SKIRMISHES or MULTI_PLAYER section
    void parseCategory( UtlLineTokeniser& parser, Context context, bool custom = false );

    //Parse a SYSTEM in the CAMPAIGNS section
    void parseCampaignSystem( UtlLineTokeniser& parser );

    //Parse a SMALL, MEDIUM or LARGE section in the SKIRMISHES or MULTI_PLAYER section.
    //pSystem is the appropriate size-based system.
    void parseNonCampaignSystem( UtlLineTokeniser& parser, MachGuiDbSystem* pSystem, Context context );

    //Parse a planet section
    void parsePlanet( UtlLineTokeniser& parser, MachGuiDbSystem* pSystem, Context context );

    //Parse a scenario definition
    void parseScenario( UtlLineTokeniser& parser, MachGuiDbPlanet* pPlanet, Context context );

    static const SysPathName& customDatabaseSourcePath();
    static const SysPathName& virginDatabaseSourcePath();
    static const SysPathName& virginDatabasePersistentPath();
    static const SysPathName& playerDatabasePath();

    //Set all campaign systems, planets and scenarios to not complete
    void clearAllCampaignCompleteFlags();

    //Set complete flags in all the campaign scenarios player has won
    void setPlayerScenarioFlags( const MachGuiDbPlayer& player );

    //Set up the entire campaign system's complete flags for player
    void setCampaignCompleteFlags( const MachGuiDbPlayer& player );

    MachGuiDatabase( const MachGuiDatabase& );
    MachGuiDatabase& operator =( const MachGuiDatabase& );

    //data members
    MachGuiIDatabase* pData_; //the implementation class
};

//Added missing operator
 inline MachGuiDatabase::TerrainSize operator++(MachGuiDatabase::TerrainSize& ts)
 {
     return _STATIC_CAST(MachGuiDatabase::TerrainSize, _STATIC_CAST(unsigned char, ts)+1);
 }

#endif

/* End DATABASE.HPP *************************************************/
