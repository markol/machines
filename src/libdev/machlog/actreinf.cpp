/*
 * A C T R E I N F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include <functional>
#include <algorithm>
#include "mathex/point3d.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machphys/machine.hpp"
#include "machlog/actreinf.hpp"
#include "machlog/produnit.hpp"
#include "machlog/scenario.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/machine.hpp"
#include "machlog/spacial.hpp"
#include "machlog/races.hpp"
#include "machlog/dbhandlr.hpp"
#include "machlog/squad.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"


PER_DEFINE_PERSISTENT( MachLogReinforcementsAction );

MachLogReinforcementsAction::MachLogReinforcementsAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled ),
	assignToSquadron_( false ),
	assignToSquadronIndex_( 1 ),
	giveVoiceMail_( true ),
	displayAsRace_( MachPhys::N_RACES )
{
    TEST_INVARIANT;
}

//virtual
MachLogReinforcementsAction::~MachLogReinforcementsAction()
{
    TEST_INVARIANT;
	while( productionList_.size() )
	{
		_DELETE( productionList_.front() );
		productionList_.erase( productionList_.begin() );
	}
}

void MachLogReinforcementsAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogReinforcementsAction& t )
{

    o << "MachLogReinforcementsAction " << (void*)&t << " start" << std::endl;
    o << "MachLogReinforcementsAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachLogReinforcementsAction::doAction()
{
	HAL_STREAM("MachLogReinforcementsAction::doAction\n" );
	HAL_INDENT( 2 );
	HAL_STREAM( *this << std::endl );
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	for( ProductionList::iterator i = productionList_.begin(); i != productionList_.end(); ++i )
	{
		MachLogProductionUnit& newMachine = (**i);
		MexPoint2d pos;
		MexTransform3d deployPointAsTransform( dest_ );
		MATHEX_SCALAR radius = 10;
		while( not MachLogSpacialManipulation::getNearestFreeSpacePoint( deployPointAsTransform, radius , MachLogMachine::maxHighClearance(), &pos ) )
			radius += 20;

		MachLogMachine* pMach = NULL;
		pMach = MachLogActorMaker::newLogMachine( newMachine.type(), newMachine.subType(),newMachine.hwLevel(), newMachine.swLevel(), race_ , pos, newMachine.weaponCombo() );
		if( displayAsRace_ != MachPhys::N_RACES )
		{
			pMach->displayAsRace( displayAsRace_ );
		}
		pMach->teleportIntoWorld();
		if( assignToSquadron_ )
		{
			HAL_STREAM("Looking to add directly into a squadron...\n" );
			HAL_STREAM("assign to Index is " << assignToSquadronIndex_ << std::endl );
			for( MachLogRaces::Squadrons::iterator i = MachLogRaces::instance().squadrons( race_ ).begin();
				i != MachLogRaces::instance().squadrons( race_ ).end(); ++i )
			{
				HAL_STREAM( (*i)->squadronId() << " " );
				if( (*i)->squadronId() == assignToSquadronIndex_ )
				{
					HAL_STREAM(" FOUND A MATCH ADDING...\n" );
					pMach->squadron( (*i) );
				}
			}
			HAL_STREAM( std::endl );
		}
	}
	HAL_INDENT( -2 );
	HAL_STREAM("MachLogReinforcementsAction::doAction DONE\n" );

	if( giveVoiceMail_ )
	{
		// give voicemail announcing arrival into game world
		MachLogVoiceMailManager::instance().postNewMail( VID_POD_REINFORCEMENTS_ONLINE, race_ );
	}
}

//static
MachLogReinforcementsAction* MachLogReinforcementsAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	HAL_STREAM("MachLogReinforcementsAction::newFromParser\n" );
	HAL_INDENT( 2 );
	MachLogReinforcementsAction* pResult = NULL;
	pResult = _NEW( MachLogReinforcementsAction( pCondition, enabled ) );
	bool finished = false;
	//read initial line for race.
	for( int i = 0; i < pParser->tokens().size(); ++i )
		if( pParser->tokens()[i] == "RACE" )
			pResult->race_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( pParser->tokens()[i] == "NO_VOICEMAIL" )
			pResult->giveVoiceMail_ = false;
	while( not finished )
	{
		if( pParser->tokens()[0] == "MACHINE" )
		{
			HAL_STREAM(" detected MACHINE line processing..." );
			MachLog::ObjectType ot = MachLogScenario::objectType( pParser->tokens()[1] );
			HAL_STREAM(" type " << ot );
			int hwLevel = atol( pParser->tokens()[2].c_str() );
			HAL_STREAM(" hwLevel " << hwLevel );
			int subType = 0;
			if( pParser->tokens().size() >= 4 )
			{
				HAL_STREAM( std::endl << " Token size is >= 4 so getting subtype from tokens[4]\n" );
				subType = MachLogScenario::objectSubType( ot, pParser->tokens()[3] );
			}
			MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
			if( pParser->tokens().size() >= 5 )
				wc = MachLogScenario::weaponCombo( pParser->tokens()[4] );
			//TBD: go and get swLevel from research item.
			int swLevel = 1;
			MachLogProductionUnit* pProd = _NEW( MachLogProductionUnit( ot, subType, hwLevel, swLevel, 0/*priority*/ ) );
			pProd->weaponCombo( wc );
			HAL_STREAM(" adding a desired machine to reinforcements list:\n" << *pProd << std::endl );
			pResult->addDesiredMachine( pProd );
		}
		else if( pParser->tokens()[0] == "END" )
			finished = true;
		else if( pParser->tokens()[0] == "POINT" )
			pResult->destination( MexPoint2d( atof( pParser->tokens()[1].c_str() ), atof( pParser->tokens()[2].c_str() ) ) );
        else if( pParser->tokens()[0] == "FROM_SCENARIO" )
            pResult->parseFromScenario( pResult->race_, pParser );
        else if( pParser->tokens()[0] == "DISPLAY" )
		{
			ASSERT( pParser->tokens().size() == 3,"Must have three tokens in DISPLAY AS line" );
			ASSERT( pParser->tokens()[1] == "AS","Must have second token as 'AS' in DISPLAY AS line" );
            pResult->displayAsRace_ = MachLogScenario::machPhysRace( pParser->tokens()[2] );
		}
        else if( pParser->tokens()[0] == "ASSIGN" )
		{
			HAL_STREAM("Detected ASSIGN line\n" );
            pResult->assignToSquadron_ = true;
			pResult->assignToSquadronIndex_ = atol( pParser->tokens()[1].c_str() );
		}

		if( not finished )
			pParser->parseNextLine();
	}
	HAL_STREAM( *pResult << std::endl );
	HAL_INDENT( -2 );
	HAL_STREAM("MachLogReinforcementsAction::newFromParser DONE\n" );
	return pResult;
}

void MachLogReinforcementsAction::addDesiredMachine( MachLogProductionUnit* pNewProdUnit )
{
	productionList_.push_back( pNewProdUnit );
}

void MachLogReinforcementsAction::destination( const MexPoint2d& newDest )
{
	dest_ = newDest;
}

const MexPoint2d& MachLogReinforcementsAction::destination() const
{
	return dest_;
}

MachPhys::Race	MachLogReinforcementsAction::race() const
{
	return race_;
}

//virtual
void MachLogReinforcementsAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Production list size " << productionList_.size() << std::endl;
	o << "Race " << race_ << " displayAsrace " << displayAsRace_;
	o << " Destination " << dest_ << std::endl;
	o << " assign to squad? " << assignToSquadron_;
	if( assignToSquadron_ )
	{
		o << " which " << assignToSquadronIndex_ << std::endl;
		o << " squadrons " << race_ << " .size " << MachLogRaces::instance().squadrons( race_ ).size() << std::endl;
	}
	else
		o << std::endl;

}

void perWrite( PerOstream& ostr, const MachLogReinforcementsAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.productionList_;
	ostr << action.dest_;
	ostr << action.race_;
	ostr << action.displayAsRace_;
	ostr << action.assignToSquadron_;
	ostr << action.assignToSquadronIndex_;

}

void perRead( PerIstream& istr, MachLogReinforcementsAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.productionList_;
	istr >> action.dest_;
	istr >> action.race_;
	istr >> action.displayAsRace_;
	istr >> action.assignToSquadron_;
	istr >> action.assignToSquadronIndex_;
}

MachLogReinforcementsAction::MachLogReinforcementsAction( PerConstructor con )
:	SimAction( con )
{
}

void MachLogReinforcementsAction::parseFromScenario
(
    MachPhys::Race race, UtlLineTokeniser* pParser
)
{
    //Get a copy of the list of units surviving from the scenario
    MachLogDatabaseHandler::Units units;
    units.reserve( 20 );
    const string& scenarioName = pParser->tokens()[1];
    units = MachLogRaces::instance().databaseHandler().survivingUnits( race, scenarioName );

    if( not units.empty() )
    {
        //We need to sort the units into descending order
        std::sort( units.begin(), units.end(), std::greater< MachLogProductionUnit >() );

        //Now work through the rest of the line, getting numbers of units of various types to add
        size_t index = 2;
        size_t nTokens = pParser->tokens().size();
        while( index + 1 < nTokens )
        {
            //Get the type of machine of interest
            MachLog::ObjectType type = MachLogScenario::objectType( pParser->tokens()[index++] );

            //Get the number we want
            uint nToLoad = atol( pParser->tokens()[index++].c_str() );

            //Iterate through our collection, add machines of this type to the reinforcement list,
            //up to the maximum number specified
            for( MachLogDatabaseHandler::Units::iterator it = units.begin(); nToLoad != 0  and  it != units.end(); ++it )
            {
                MachLogProductionUnit& unit = (*it);
                if( unit.type() == type )
                {
                    addDesiredMachine( _NEW( MachLogProductionUnit( unit ) ) );
                    --nToLoad;
                }
            }
        }
    }
}
/* End ACTREINF.CPP *************************************************/
