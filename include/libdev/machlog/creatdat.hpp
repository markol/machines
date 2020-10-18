/*
 * C R E A T D A T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGameCreationData

    This class has all the data elements necessary to fully describe the variable fields
	used in the consgruction of a new game.
*/

#ifndef _MACHLOG_CREATDAT_HPP
#define _MACHLOG_CREATDAT_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/machlog.hpp"

class MachLogGameCreationDataImpl;
template < class T > class ctl_vector;

class MachLogGameCreationData
// Canonical form revoked
{
public:

	struct PlayerCreationData {
		MachLog::PlayerType	type_;
		MachPhys::Race		colour_;
	};

	friend ostream& operator<<( ostream&, const PlayerCreationData& );

	typedef ctl_vector< PlayerCreationData >            PlayersCreationData;

	//default constructor sets appropriate default values.
    MachLogGameCreationData( void );
	//use this constructor in skirmish and multiplayer games.
    MachLogGameCreationData( MachLog::RandomStarts, 
    						const PlayersCreationData&, 
    						MachLog::ResourcesAvailable,
    						MachLog::StartingResources, 
    						MachLog::VictoryCondition, 
    						MachLog::TechnologyLevel,
    						const PhysAbsoluteTime& timerTickAt );

    MachLogGameCreationData( const MachLogGameCreationData& );

    ~MachLogGameCreationData( void );

    void CLASS_INVARIANT;

	MachLog::RandomStarts randomStarts() const;
	void randomStarts( MachLog::RandomStarts );

	const PlayersCreationData& playersCreationData() const;
	void playersCreationData( const PlayersCreationData& );

	MachLog::ResourcesAvailable resourcesAvailable() const;
	void resourcesAvailable( MachLog::ResourcesAvailable );

	MachLog::StartingResources startingResources() const;
	void startingResources( MachLog::StartingResources );

	MachLog::VictoryCondition victoryCondition() const;
	void victoryCondition( MachLog::VictoryCondition );

	MachLog::TechnologyLevel technologyLevel() const;
	void technologyLevel( MachLog::TechnologyLevel );

	const PhysAbsoluteTime& timerTickAt() const;
	void timerTickAt( const PhysAbsoluteTime& );

private:
    friend ostream& operator <<( ostream& o, const MachLogGameCreationData& t );

    MachLogGameCreationData& operator =( const MachLogGameCreationData& );

	MachLogGameCreationDataImpl* pImpl_;

};


#endif

/* End CREATDAT.HPP *************************************************/
