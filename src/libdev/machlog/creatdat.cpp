/*
 * C R E A T D A T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/creatdat.hpp"
#include "ctl/vector.hpp"

class MachLogGameCreationDataImpl
{
public:
	~MachLogGameCreationDataImpl() {};
private:
	friend class MachLogGameCreationData;
	friend ostream& operator<<( ostream&, const MachLogGameCreationDataImpl& );

	MachLogGameCreationDataImpl() {};
	MachLog::RandomStarts							randomStarts_;
    MachLogGameCreationData::PlayersCreationData 	playersCreationData_;
	MachLog::ResourcesAvailable						resourcesAvailable_;
	MachLog::StartingResources 						startingResources_;
	MachLog::VictoryCondition						victoryCondition_;
	MachLog::TechnologyLevel						technologyLevel_;
	PhysAbsoluteTime								timerTickAt_;

};

ostream& operator <<( ostream& o, const MachLogGameCreationDataImpl& t )
{
	o << " RandomStarts	" << t.randomStarts_ << std::endl;
    o << " PlayersCreationData " << t.playersCreationData_ << std::endl;
	o << " ResourcesAvailable " << t.resourcesAvailable_ << std::endl;
	o << " StartingResources " << t.startingResources_ << std::endl;
	o << " VictoryCondition	" << t.victoryCondition_ << std::endl;
	o << " TechnologyLevel " << t.technologyLevel_ << std::endl;
	o << " TimerTickAt " << t.timerTickAt_ << std::endl;

    return o;
}

MachLogGameCreationData::MachLogGameCreationData()
:	pImpl_( _NEW( MachLogGameCreationDataImpl ) )
{
	pImpl_->randomStarts_ = MachLog::FIXED_START_LOCATIONS;
	pImpl_->resourcesAvailable_ = MachLog::RES_DEFAULT;
	pImpl_->victoryCondition_ = MachLog::VICTORY_DEFAULT;
	pImpl_->technologyLevel_ = MachLog::TECH_LEVEL_DEFAULT;
	pImpl_->startingResources_ = MachLog::STARTING_RESOURCES_DEFAULT;
	pImpl_->timerTickAt_ = 0;
    TEST_INVARIANT;
}

MachLogGameCreationData::MachLogGameCreationData
(
	MachLog::RandomStarts randomStarts,  
	const MachLogGameCreationData::PlayersCreationData& playersCreationData,
	MachLog::ResourcesAvailable resourcesAvailable,
	MachLog::StartingResources startingResources, 
	MachLog::VictoryCondition victoryCondition, 
	MachLog::TechnologyLevel technologyLevel,
	const PhysAbsoluteTime& timerTickAt
)
:	pImpl_( _NEW( MachLogGameCreationDataImpl ) )
{
	pImpl_->randomStarts_ = randomStarts;
	pImpl_->resourcesAvailable_ = resourcesAvailable;
	pImpl_->victoryCondition_ = victoryCondition;
	pImpl_->technologyLevel_ = technologyLevel;
	pImpl_->startingResources_ = startingResources;
	pImpl_->playersCreationData_ = playersCreationData;
	pImpl_->timerTickAt_ = timerTickAt;
}

MachLogGameCreationData::MachLogGameCreationData( const MachLogGameCreationData& rhs )
:	pImpl_( _NEW( MachLogGameCreationDataImpl ) )
{
	pImpl_->randomStarts_ = rhs.randomStarts();
	pImpl_->resourcesAvailable_ = rhs.resourcesAvailable();
	pImpl_->victoryCondition_ = rhs.victoryCondition();
	pImpl_->technologyLevel_ = rhs.technologyLevel();
	pImpl_->startingResources_ = rhs.startingResources();
	pImpl_->playersCreationData_ = rhs.playersCreationData();
	pImpl_->timerTickAt_ = rhs.timerTickAt();
}


MachLogGameCreationData::~MachLogGameCreationData()
{	
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void MachLogGameCreationData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogGameCreationData& t )
{

    o << "MachLogGameCreationData " << (void*)&t << " start" << std::endl;
	o << " Impl. " << *(t.pImpl_);
    o << "MachLogGameCreationData " << (void*)&t << " end" << std::endl;

    return o;
}

MachLog::RandomStarts MachLogGameCreationData::randomStarts() const
{
	return pImpl_->randomStarts_;
}

void MachLogGameCreationData::randomStarts( MachLog::RandomStarts random )
{
	pImpl_->randomStarts_ = random;
}


const MachLogGameCreationData::PlayersCreationData&
MachLogGameCreationData::playersCreationData() const
{
	return pImpl_->playersCreationData_;
}


void MachLogGameCreationData::playersCreationData( const MachLogGameCreationData::PlayersCreationData& players )
{
	pImpl_->playersCreationData_ = players;
}

MachLog::ResourcesAvailable MachLogGameCreationData::resourcesAvailable() const
{
	return pImpl_->resourcesAvailable_;
}

void MachLogGameCreationData::resourcesAvailable( MachLog::ResourcesAvailable resources )
{
	pImpl_->resourcesAvailable_ = resources;
}

MachLog::StartingResources MachLogGameCreationData::startingResources() const
{
	return pImpl_->startingResources_;
}

void MachLogGameCreationData::startingResources( MachLog::StartingResources starting )
{
	pImpl_->startingResources_ = starting;
}

MachLog::VictoryCondition MachLogGameCreationData::victoryCondition() const
{
	return pImpl_->victoryCondition_;
}

void MachLogGameCreationData::victoryCondition( MachLog::VictoryCondition victory )
{
	pImpl_->victoryCondition_ = victory;
}

MachLog::TechnologyLevel MachLogGameCreationData::technologyLevel() const
{
	return pImpl_->technologyLevel_;
}

void MachLogGameCreationData::technologyLevel( MachLog::TechnologyLevel techLevel )
{
	pImpl_->technologyLevel_ = techLevel;
}

const PhysAbsoluteTime& MachLogGameCreationData::timerTickAt() const
{
	return pImpl_->timerTickAt_;
}

void MachLogGameCreationData::timerTickAt( const PhysAbsoluteTime& timer )
{
	pImpl_->timerTickAt_ = timer;
}

ostream& operator<<( ostream& o, const MachLogGameCreationData::PlayerCreationData& data )
{
	o << "MachLogGameCreationData::PlayerCreationData " << (void*)&data << std::endl;
	o << " type_ " << data.type_ << " colour " << data.colour_ << std::endl;
	return o;
}


/* End CREATDAT.CPP *************************************************/
