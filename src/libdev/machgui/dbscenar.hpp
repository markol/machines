/*
 * D B S C E N A R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbScenario

    Represents a scenario in a campaign, skirmish or multi-player game.
*/

#ifndef _MACHGUI_DBSCENAR_HPP
#define _MACHGUI_DBSCENAR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machgui/dbelemen.hpp"

//forward refs
class MachGuiDbIScenario;
class MachGuiDbPlanet;

class MachGuiDbScenario : public MachGuiDbElement
// Canonical form revoked
{
public:
    //The scenario is known by identifier name which is used to construct the scenario filename.
    //The relative path for the planet files is planetFile.
    //The id of the string used in the menu list boxes is menuStringId.
   // MachGuiDbScenario( const string& name, const string& planetFile, uint menuStringId );
    MachGuiDbScenario( const string& name, const string& planetFile, uint menuStringId, uint maxPlayers );

    ~MachGuiDbScenario();

    //set/get the logical planet on which the scenario occurs
    void planet( MachGuiDbPlanet* pPlanet );
    MachGuiDbPlanet& planet() const;

    //set/get the planet file name for the scenario
    void planetFile( const string& filename );
    const string& planetFile() const;

    //set/get the flic or bmp filename displayed in the campaign menu
    //string is empty if undefined.
    void campaignPicture( const string& filename );
    const string& campaignPicture() const;

    //set/get the flic/bmp filename displayed in the briefing menu
    //string is empty if undefined.
    void briefingPicture( const string& filename );
    const string& briefingPicture() const;

    //set/get the flic/bmp filename displayed in the debriefing menu
    //string is empty if undefined.
    void debriefingPicture( const string& filename );
    const string& debriefingPicture() const;
	void debriefingLosePicture( const string& filename );
	const string& debriefingLosePicture() const;

    //set/get the fullscreen flic to be displayed upon starting a scenario
    //string is empty if undefined.
    void entryFlic( const string& filename );
    const string& entryFlic() const;

    //set/get the fullscreen flic to be displayed upon starting a scenario
    //string is empty if undefined.
    void winFlic( const string& filename );
    const string& winFlic() const;

    //set/get the fullscreen flic to be displayed upon starting a scenario
    //string is empty if undefined.
    void loseFlic( const string& filename );
    const string& loseFlic() const;

	// get the maximum number of players that play a scenario
	uint maxPlayers() const;
    void CLASS_INVARIANT;

    //Number of races for which we need save a list of the surviving machines
    //on successful completion
    uint nRacesToHaveSurvivingMachinesSaved() const;

    //The index'th such race
    MachPhys::Race raceToHaveSurvivingMachinesSaved( uint index );
    //PRE( index < nRacesToHaveSurvivingMachinesSaved() );

    //Add another race to have its machines saved
    void raceToHaveSurvivingMachinesSaved( MachPhys::Race );

    //Music track to be played
    uint musicTrack() const;
    void musicTrack( uint track);

	//Check if this scenario is one of the training levels. This is implemented via a bit of a hack but
	//with only a few days to go...
	bool isTrainingScenario() const;

	// Indicate that scenario has been won by someone.
	void hasBeenWon( bool );
	bool hasBeenWon() const;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachGuiDbScenario );
    PER_FRIEND_READ_WRITE( MachGuiDbScenario );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbScenario& t );

    MachGuiDbScenario( const MachGuiDbScenario& );
    MachGuiDbScenario& operator =( const MachGuiDbScenario& );
	
	void initialise( const string& scenarioName, const string& planetName  );
    
    //data members
    MachGuiDbIScenario *pData_; //implementation data object
};

PER_DECLARE_PERSISTENT( MachGuiDbScenario );

#endif

/* End DBSCENAR.HPP *************************************************/
