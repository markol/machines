/*
 * D B T E X D A T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbTextData

    Collates all the paragraph type text info for a single database object.
*/

#ifndef _MACHGUI_DBTEXDAT_HPP
#define _MACHGUI_DBTEXDAT_HPP

#include "base/base.hpp"

//forward refs
class MachGuiDbITextData;
class SysPathName;

class MachGuiDbTextData
// Canonical form revoked
{
public:
    MachGuiDbTextData();
    ~MachGuiDbTextData();

    //Construct and return a new text data object derived from filename.
    //This looks first in pdata/... for a .bin persistent version. If not found
    //reads the source file from data/... and creates the persistent file.
    //Client must delete the returned object.
    static MachGuiDbTextData* pNewTextData( const string& filename );

    //The various strings of text
    const string& campaignText() const; //Campaign screen description
    const string& hintText() const; //Briefing screen hints
    const string& winText() const; //DeBriefing screen win text
    const string& loseText() const; //DeBriefing screen lose text

	const string& winVoicemail() const; // DeBriefing screen win voicemail filename
	const string& loseVoicemail() const; // DeBriefing screen lose voicemail filename

    uint nObjectives() const; //Number of objectives
    const string& objectiveText( uint index ) const; //Briefing text for index'th objective
    //PRE( index < nObjectives() );

	const string& objectiveVoicemail( uint index ) const; //Briefing voicemail for index'th objective
    uint nTasks() const; //Number of tasks

    const string& taskText( uint index ) const; //DeBriefing lose text for index'th task
    //PRE( index < nTasks() );

    //True iff the task beings in the OPEN state at start of scenario
    bool taskStartsAvailable( uint index ) const;
    //PRE( index < nTasks() );

	bool hasWinVoicemail() const;	// true if voicemail file is present for win
	bool hasLoseVoicemail() const;	// true if voicemail file is present for lose
	bool hasobjectiveVoicemail( uint index ) const;	 // true if voicemail file is present for index'th objective

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbTextData& t );

    MachGuiDbTextData( const MachGuiDbTextData& );
    MachGuiDbTextData& operator =( const MachGuiDbTextData& );

    //Parse the text version of the text file
    void parseFile( const SysPathName& filename );

    //data members
    MachGuiDbITextData* pData_; //implementation data object
};

#endif

/* End DBTEXDAT.HPP *************************************************/
