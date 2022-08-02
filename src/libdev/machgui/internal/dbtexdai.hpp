/*
 * D B T E X D A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbITextData

    Implementation data obejct for MachGuiDbTextData
*/

#ifndef _MACHGUI_DBTEXDAI_HPP
#define _MACHGUI_DBTEXDAI_HPP

#include "base/base.hpp"				   
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

class MachGuiDbITextData
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbITextData );
    PER_FRIEND_READ_WRITE( MachGuiDbITextData );

private:
    friend class MachGuiDbTextData;

    friend ostream& operator <<( ostream& o, const MachGuiDbITextData& t );

    MachGuiDbITextData();
    ~MachGuiDbITextData();
    MachGuiDbITextData( const MachGuiDbITextData& );
    MachGuiDbITextData& operator =( const MachGuiDbITextData& );

    typedef ctl_vector< string > Strings;
    typedef ctl_vector< bool > TaskFlags;

    //data members
    string campaignText_; //Appears in campaign menu
    Strings objectives_; //Used in briefing menu 
    Strings objectiveVoicemails_; // Used in briefing menu
    string hintText_; //Used in briefing menu
    string winText_; //used in debriefing menu
    string loseText_; //used in debriefing menu
    Strings tasks_; //used in debriefing menu when tasks not completed
    TaskFlags tasksStartAvailable_; //True if task starts in available state
	string winVoicemail_;
	string loseVoicemail_;
};

PER_DECLARE_PERSISTENT( MachGuiDbITextData );

#endif

/* End DBTEXDAI.HPP *************************************************/
