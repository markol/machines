/*
 * D B H A N D L R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/dbhandlr.hpp"

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "ctl/vector.hpp"
#include "mathex/point2d.hpp" //required for instantiation only
#include "machlog/canattac.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"

struct MachLogDatabaseHandlerImpl
{
    typedef ctl_vector< string > Strings;
    Strings setFlags_;

    typedef ctl_vector< bool > TaskFlags;
    TaskFlags tasksAvailable_;
    TaskFlags tasksComplete_;
};

MachLogDatabaseHandler::MachLogDatabaseHandler()
{
    pImpl_ = _NEW( MachLogDatabaseHandlerImpl );
    pImpl_->setFlags_.reserve( 8 );

    TEST_INVARIANT;
}

MachLogDatabaseHandler::~MachLogDatabaseHandler()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

void MachLogDatabaseHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogDatabaseHandler& t )
{

    o << "MachLogDatabaseHandler " << (void*)&t << " start" << std::endl;
    o << "MachLogDatabaseHandler " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogDatabaseHandler::raceUnits( MachPhys::Race race, Units* pUnits ) const
{
    //Get the collection of actors for the race
    const MachLogRaces::Objects& actors = MachLogRaces::instance().raceObjects( race );

    //Check em all
    for( MachLogRaces::Objects::const_iterator cit = actors.begin(); cit != actors.end(); ++cit )
    {
        //Deal only with machines
        const MachActor& actor = *(*cit);
        if( actor.objectIsMachine() )
        {
            //Extract the data we need to make a production unit
            const MachLogMachine& machine = actor.asMachine();
            MachLog::ObjectType	type = machine.objectType();
            int subType = machine.subType();
            int hwLevel = machine.hwLevel();
            int swLevel = machine.swLevel();
            MachLogProductionUnit unit( type, subType, hwLevel, swLevel, MachLogProductionUnit::UNCHECKED_PRIORITY );

            if( machine.objectIsCanAttack() )
                unit.weaponCombo( machine.asCanAttack().weaponCombo() );

            //Add this unit to the collection
            pUnits->push_back( unit );
        }
    }
}

void MachLogDatabaseHandler::setScenarioFlag( const string& flag )
{
    CB_DEPIMPL( MachLogDatabaseHandlerImpl::Strings, setFlags_ );
    setFlags_.push_back( flag );
}

uint MachLogDatabaseHandler::nSetFlags() const
{
    CB_DEPIMPL( MachLogDatabaseHandlerImpl::Strings, setFlags_ );
    return setFlags_.size();
}

const string& MachLogDatabaseHandler::setFlag( uint index ) const
{
    PRE( index < nSetFlags() );
    CB_DEPIMPL( MachLogDatabaseHandlerImpl::Strings, setFlags_ );
    return setFlags_[ index ];
}

void MachLogDatabaseHandler::clearSetFlags()
{
    CB_DEPIMPL( MachLogDatabaseHandlerImpl::Strings, setFlags_ );
    setFlags_.erase( setFlags_.begin(), setFlags_.end() );
}

void MachLogDatabaseHandler::taskIsAvailable( uint index, bool isNow )
{
    PRE( index < nTasksInCurrentScenario() );
    pImpl_->tasksAvailable_[ index ] = isNow;
}

bool MachLogDatabaseHandler::taskIsAvailable( uint index ) const
{
    PRE( index < nTasksInCurrentScenario() );
    return pImpl_->tasksAvailable_[ index ];
}

void MachLogDatabaseHandler::taskIsComplete( uint index, bool isNow )
{
    PRE( index < nTasksInCurrentScenario() );
    pImpl_->tasksComplete_[ index ] = isNow;
}

bool MachLogDatabaseHandler::taskIsComplete( uint index ) const
{
    PRE( index < nTasksInCurrentScenario() );
    return pImpl_->tasksComplete_[ index ];
}

void MachLogDatabaseHandler::initialiseTaskFlags()
{
    //Ensure the vectors are cleared
    clearTaskFlags();

    //Get the number of tasks
    uint nTasks = nTasksInCurrentScenario();
    if( nTasks != 0 )
    {
        pImpl_->tasksAvailable_.reserve( nTasks );
        pImpl_->tasksComplete_.reserve( nTasks );
        for( uint i = 0; i != nTasks; ++i )
        {
            pImpl_->tasksAvailable_.push_back( taskStartsAvailable( i ) );
            pImpl_->tasksComplete_.push_back( false );
        }
    }
}

void MachLogDatabaseHandler::clearTaskFlags()
{
    pImpl_->tasksAvailable_.erase( pImpl_->tasksAvailable_.begin(), pImpl_->tasksAvailable_.end() );
    pImpl_->tasksComplete_.erase( pImpl_->tasksComplete_.begin(), pImpl_->tasksComplete_.end() );
}

void MachLogDatabaseHandler::writePersistenceData( PerOstream& ostr )
{
    //Write any scenario flags that have been set
    uint nFlags = nSetFlags();
    PER_WRITE_RAW_OBJECT( ostr, nFlags );

    for( uint i = 0; i != nFlags; ++i )
    {
        string tempString = setFlag( i );
        PER_WRITE_RAW_OBJECT( ostr, tempString );
    }

    //Write the task states
    uint nTaskStates = nTasksInCurrentScenario();
    PER_WRITE_RAW_OBJECT( ostr, nTaskStates );

    for( uint i = 0; i != nTaskStates; ++i )
    {
        bool isAvailable = taskIsAvailable( i );
        PER_WRITE_RAW_OBJECT( ostr, isAvailable );

        bool isComplete = taskIsComplete( i );
        PER_WRITE_RAW_OBJECT( ostr, isComplete );
    }
}

void MachLogDatabaseHandler::readPersistenceData( PerIstream& istr )
{
    //Read any scenario flags that have been set
    uint nFlags;
    PER_READ_RAW_OBJECT( istr, nFlags );
    for( uint i = 0; i != nFlags; ++i )
    {
        string tempString;
        PER_READ_RAW_OBJECT( istr, tempString );
        setScenarioFlag( tempString );
    }

    //Read the task states
    uint nTaskStates;
    PER_READ_RAW_OBJECT( istr, nTaskStates );

    for( uint i = 0; i != nTaskStates; ++i )
    {
        bool isAvailable;
        PER_READ_RAW_OBJECT( istr, isAvailable );
        taskIsAvailable( i, isAvailable );

        bool isComplete;
        PER_READ_RAW_OBJECT( istr, isComplete );
        taskIsComplete( i, isComplete );
    }
}

/* End DBHANDLR.CPP *************************************************/
