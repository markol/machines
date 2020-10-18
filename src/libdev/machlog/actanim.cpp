/*
 * A C T A N I M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "ctl/list.hpp"
#include "ctl/countptr.hpp"

#include "stdlib/string.hpp"

#include "utility/linetok.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"

#include "sim/manager.hpp"

#include "machlog/actanim.hpp"
#include "machlog/scenario.hpp"
#include "machlog/machine.hpp"
#include "machlog/constron.hpp"
#include "machlog/canattac.hpp"
#include "machlog/races.hpp"


PER_DEFINE_PERSISTENT( MachLogPlayAnimationAction );

MachLogPlayAnimationAction::MachLogPlayAnimationAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled ),
	subTypeSet_( false ),
	hwLevelSet_( false ),
	weaponComboSet_( false ),
	anyTypeWillDo_( false ),
	repetitions_( 0 ),
	animation_( "" )
{
    TEST_INVARIANT;
}

//virtual
MachLogPlayAnimationAction::~MachLogPlayAnimationAction()
{
    TEST_INVARIANT;
}

void MachLogPlayAnimationAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPlayAnimationAction& t )
{

    o << "MachLogPlayAnimationAction " << (void*)&t << " start" << std::endl;
    o << "MachLogPlayAnimationAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachLogPlayAnimationAction::doAction()
{
	HAL_STREAM( "MachLogPlayAnimationAction::doAction()\n" );
	HAL_STREAM( race_ << " anyWillDo_ " << anyTypeWillDo_ << std::endl );
	MachLogRaces::Objects& objects = MachLogRaces::instance().raceObjects( race_ );
	MachLogRaces::Objects matchingObjects;
	for( MachLogRaces::Objects::iterator i = objects.begin(); i != objects.end(); ++i )
	{
		//do the quick checks first - these are all the numerical based checks.
		bool objectOk = true;
		if( (*i)->race() != race_ )
			objectOk = false;
		else if( anyTypeWillDo_ )	//take out object type selection if any is set
			objectOk = true;
		else if( (*i)->objectType() == objectType_ )
		{
			if( subTypeSet_ and (*i)->subType() != subType_ )
				objectOk = false;
			if( objectOk and hwLevelSet_ )
			{
				if( (*i)->objectIsMachine() )
				{
					MachLogMachine& mlm = (*i)->asMachine();
					if( mlm.hwLevel() != hwLevel_ )
						objectOk = false;
				}
				else if( (*i)->objectIsConstruction() )
				{
					MachLogConstruction& mlc = (*i)->asConstruction();
					if( mlc.level() != hwLevel_ )
						objectOk = false;
				}
				else	//if the object is not a machine or construction then matching on level is impossible
				{
					objectOk = false;
				}
			}
			if( objectOk and weaponComboSet_ )
			{
				if( (*i)->objectIsCanAttack() )
				{
					if( (*i)->asCanAttack().weaponCombo() != weaponCombo_ )
						objectOk = false;
				}
				else
					objectOk = false;
			}
		}
		else
			objectOk = false;
		if( objectOk and point_.sqrEuclidianDistance( (*i)->position() ) < sqrRange_ )
			matchingObjects.push_back( *i );
	}

	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	HAL_STREAM(" managed to match on " << matchingObjects.size() << " objects\n" );
	HAL_STREAM(" looking for animation " << animation_ << std::endl );
	for( MachLogRaces::Objects::iterator i = matchingObjects.begin(); i != matchingObjects.end(); ++i )
	{
		HAL_STREAM("  " << (*i)->id() << " " << (*i)->objectType() << std::endl );
		HAL_STREAM(" dereferencing the physObject of the object\n" );
		W4dEntity& entity = (*i)->physObject();
		HAL_STREAM(" asking the thingy if it is a composite\n" );
		if( entity.isComposite() )
		{
			HAL_STREAM(" object is a composite..get as composite\n" );
			W4dComposite& composite = entity.asComposite();
			W4dCompositePlanPtr pPlan;
			HAL_STREAM(" issuing findCompositePlan\n" );
			if( composite.findCompositePlan( animation_, &pPlan ) )
			{
				HAL_STREAM(" object " << (*i)->id() << " has matched on the animation named " << animation_ << std::endl );
				composite.plan( *pPlan, now, repetitions_ );
			}
		}
	}
}

//static
MachLogPlayAnimationAction* MachLogPlayAnimationAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	MachLogPlayAnimationAction* pResult = NULL;
	pResult = _NEW( MachLogPlayAnimationAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "RACE" )
			pResult->race_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( token == "ANY" )
			pResult->anyTypeWillDo_ = true;
		else if( token == "AT" )
			pResult->point_ = MexPoint2d( atof( pParser->tokens()[i+1].c_str() ), atof( pParser->tokens()[i+1].c_str() ) );
		else if( token == "ANIMATION" )
			pResult->animation_ = pParser->tokens()[i+1];
		else if( token == "REPETITIONS" )
			pResult->repetitions_ = atol( pParser->tokens()[i+1].c_str() );
		else if( token == "RANGE" )
		{
			pResult->sqrRange_ = atof( pParser->tokens()[i+1].c_str() );
			pResult->sqrRange_ *= pResult->sqrRange_;
		}
		else if( token == "OBJECT" )
		{
			//objectType must be set
			pResult->objectType_ = MachLogScenario::objectType( pParser->tokens()[i+1] );
			//All other switch parameters are optional.
			if( pParser->tokens().size() >= i+3 )
			{
				pResult->hwLevelSet_ = true;
				pResult->hwLevel_ = atol( pParser->tokens()[i+2].c_str() );
				if( pParser->tokens().size() >= i+4 )
				{
					pResult->subTypeSet_ = true;
					pResult->subType_ = MachLogScenario::objectSubType( pResult->objectType_, pParser->tokens()[i+3] );
					if( pParser->tokens().size() >= i+5 )
					{
						pResult->weaponComboSet_ = true;
						pResult->weaponCombo_ = MachLogScenario::weaponCombo( pParser->tokens()[i+4] );
					}
				}
			}
		}
	}
	return pResult;
}

//virtual
void MachLogPlayAnimationAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Race " << race_ << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogPlayAnimationAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.race_;
	ostr << action.point_;
	ostr << action.sqrRange_;
	ostr << action.objectType_;
	ostr << action.subTypeSet_;
	ostr << action.subType_;
	ostr << action.hwLevelSet_;
	ostr << action.hwLevel_;
	ostr << action.weaponComboSet_;
	ostr << action.weaponCombo_;
	ostr << action.anyTypeWillDo_;
	ostr << action.animation_;
	ostr << action.repetitions_;

}

void perRead( PerIstream& istr, MachLogPlayAnimationAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.race_;
	istr >> action.point_;
	istr >> action.sqrRange_;
	istr >> action.objectType_;
	istr >> action.subTypeSet_;
	istr >> action.subType_;
	istr >> action.hwLevelSet_;
	istr >> action.hwLevel_;
	istr >> action.weaponComboSet_;
	istr >> action.weaponCombo_;
	istr >> action.anyTypeWillDo_;
	istr >> action.animation_;
	istr >> action.repetitions_;
}

MachLogPlayAnimationAction::MachLogPlayAnimationAction( PerConstructor con )
:	SimAction( con )
{
}

/* End ACTREINF.CPP *************************************************/
