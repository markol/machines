/*
 * C N T R L A I I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/cntrlaii.hpp"
#include "machlog/produnit.hpp"

PER_DEFINE_PERSISTENT( MachLogAIStrategicProductionUnit );
PER_DEFINE_PERSISTENT( MachLogAIControllerImpl );

MachLogAIControllerImpl::MachLogAIControllerImpl( const string& AIStrategicRules )
:	rules_( AIStrategicRules ),
    researchInterest_( MachLogAIController::HARDWARE )
{

    TEST_INVARIANT;
}

MachLogAIControllerImpl::~MachLogAIControllerImpl()
{
    TEST_INVARIANT;

}

void MachLogAIControllerImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAIControllerImpl& t )
{

    o << "MachLogAIControllerImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAIControllerImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAIControllerImpl& actorImpl )
{
	ostr << actorImpl.rules_;
	ostr << actorImpl.strategicProductionList_;
	ostr << actorImpl.constructionProductionList_;
	ostr << actorImpl.desiredMachineList_;
	ostr << actorImpl.checkForDynamicAllies_;
	ostr << actorImpl.nextAllyUpdateTime_;	
}

void perRead( PerIstream& istr, MachLogAIControllerImpl& actorImpl )
{
	istr >> actorImpl.rules_;
	istr >> actorImpl.strategicProductionList_;
	istr >> actorImpl.constructionProductionList_;
	istr >> actorImpl.desiredMachineList_;
	istr >> actorImpl.checkForDynamicAllies_;
	istr >> actorImpl.nextAllyUpdateTime_;	
	actorImpl.researchInterest_ = MachLogAIController::HARDWARE;
}

MachLogAIControllerImpl::MachLogAIControllerImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachLogAIStrategicProductionUnit& aiProdUnit )
{
	ostr << aiProdUnit.type_;
	ostr << aiProdUnit.number_;
	ostr << aiProdUnit.priority_;
}

void perRead( PerIstream& istr, MachLogAIStrategicProductionUnit& aiProdUnit )
{
	istr >> aiProdUnit.type_;
	istr >> aiProdUnit.number_;
	istr >> aiProdUnit.priority_;
}

MachLogAIStrategicProductionUnit::MachLogAIStrategicProductionUnit( PerConstructor )
{
}

/* End CNTRLAII.CPP *************************************************/
