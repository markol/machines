/*
 * A C T R E I N F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "utility/linetok.hpp"
#include "sim/manager.hpp"
#include "machlog/actvmail.hpp"
#include "machlog/vmman.hpp"
#include "machlog/scenario.hpp"

PER_DEFINE_PERSISTENT( MachLogVoiceMailAction );

MachLogVoiceMailAction::MachLogVoiceMailAction( SimCondition* pCondition, bool enabled )
:	SimAction( pCondition, enabled )
{
    TEST_INVARIANT;
}

//virtual
MachLogVoiceMailAction::~MachLogVoiceMailAction()
{
    TEST_INVARIANT;
}

void MachLogVoiceMailAction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogVoiceMailAction& t )
{

    o << "MachLogVoiceMailAction " << (void*)&t << " start" << std::endl;
    o << "MachLogVoiceMailAction " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachLogVoiceMailAction::doAction()
{
	MachLogVoiceMailManager::instance().postNewMail( mailID_, race_ );
}

//static
MachLogVoiceMailAction* MachLogVoiceMailAction::newFromParser( SimCondition* pCondition, bool enabled, UtlLineTokeniser* pParser )
{
	MachLogVoiceMailAction* pResult = NULL;
	pResult = _NEW( MachLogVoiceMailAction( pCondition, enabled ) );
	for( int i = 0; i < pParser->tokens().size(); ++i )
	{
		const string& token = pParser->tokens()[i];
		if( token == "RACE" )
			pResult->race_ = MachLogScenario::machPhysRace( pParser->tokens()[i+1] );
		else if( token == "VID" )
//			pResult->mailID_ = MachLogVoiceMailManager::instance().veMailIDMap()[ pParser->tokens()[i+1] ];
			pResult->mailID_ = _CONST_CAST(MachLogVoiceMailManager::VEmailIDMap&,
                               MachLogVoiceMailManager::instance().veMailIDMap())[ pParser->tokens()[i+1] ];
	}
	return pResult;
}

//virtual
void MachLogVoiceMailAction::doOutputOperator( ostream& o ) const
{
	SimAction::doOutputOperator( o );
	o << "Race " << race_;
	o << " MailId " << (int)mailID_ << std::endl;
}

void perWrite( PerOstream& ostr, const MachLogVoiceMailAction& action )
{
	const SimAction& base1 = action;

	ostr << base1;
	ostr << action.mailID_;
	ostr << action.race_;

}

void perRead( PerIstream& istr, MachLogVoiceMailAction& action )
{
	SimAction& base1 = action;

	istr >> base1;
	istr >> action.mailID_;
	istr >> action.race_;
}

MachLogVoiceMailAction::MachLogVoiceMailAction( PerConstructor con )
:	SimAction( con )
{
}

/* End ACTREINF.CPP *************************************************/
