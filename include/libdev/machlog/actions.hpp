/*
 * A C T I O N S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogActions

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_ACTIONS_HPP
#define _MACHLOG_ACTIONS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogActions
// Canonical form revoked
{
public:
    ~MachLogActions();

    //Parse the ACTIONS section of a scenario file
    static void parseActionsSection( UtlLineTokeniser* pParser );
	static void registerDerivedClasses();
	static void createDynamically( MachLog::VictoryCondition, const PhysRelativeTime& timerFireAt );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogActions& t );

    MachLogActions();
    MachLogActions( const MachLogActions& );
    MachLogActions& operator =( const MachLogActions& );

};


#endif

/* End ACTIONS.HPP **************************************************/
