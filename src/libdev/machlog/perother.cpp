/*
 * P E R M A C H . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "ctl/list.hpp"

#include "utility/linetok.hpp"
#include "mathex/point2d.hpp"

#include "sim/action.hpp"
#include "sim/conditio.hpp"
#include "sim/conditim.hpp"
#include "sim/manager.hpp"

#include "machlog/perother.hpp"

#include "machlog/debris.hpp"
#include "machlog/lmine.hpp"
#include "machlog/minesite.hpp"

#include "machlog/actmaker.hpp"  
#include "machlog/artfacts.hpp"
#include "machlog/artefact.hpp"
#include "machlog/squad.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/resitem.hpp"
#include "machlog/restree.hpp"
#include "machlog/consitem.hpp"
#include "machlog/constree.hpp"

#include "machlog/actions.hpp"

PER_DEFINE_PERSISTENT( MachLogOtherPersistence );

// static
MachLogOtherPersistence& MachLogOtherPersistence::instance()
{
    static MachLogOtherPersistence instance_;
    return instance_;
}

MachLogOtherPersistence::MachLogOtherPersistence()
{
	registerDerivedClasses();
	MachLogActions::registerDerivedClasses();
    TEST_INVARIANT;
}

MachLogOtherPersistence::~MachLogOtherPersistence()
{
    TEST_INVARIANT;

}

void MachLogOtherPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogOtherPersistence& t )
{

    o << "MachLogOtherPersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogOtherPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogOtherPersistence::registerDerivedClasses()
{
	//ore holographs are not persisted - they are recreated from the mineral sites.
	PER_REGISTER_DERIVED_CLASS( MachLogDebris );
	PER_REGISTER_DERIVED_CLASS( MachLogLandMine );
	PER_REGISTER_DERIVED_CLASS( MachLogArtefact );
	PER_REGISTER_DERIVED_CLASS( MachLogSquadron );
}

void MachLogOtherPersistence::setUpLocalVectorFromViews() const
{
	//clear down collection of machines before we begin.
	MachLogOtherPersistence& nonConstPer = *( _CONST_CAST( MachLogOtherPersistence*, this ) );
	nonConstPer.others_.erase( nonConstPer.others_.begin(), nonConstPer.others_.end() );
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin(); i != MachLogRaces::instance().objects().end(); ++i )
	{
		switch( (*i)->objectType() )
		{
			case MachLog::DEBRIS:
			case MachLog::LAND_MINE:
			case MachLog::ARTEFACT:
				nonConstPer.others_.push_back( (*i) );
		}
	}

	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		if( MachLogRaces::instance().raceObjects( i ).size() > 0 )
			for( MachLogRaces::Squadrons::iterator j = MachLogRaces::instance().squadrons( i ).begin(); j != MachLogRaces::instance().squadrons( i ).end(); ++j )
				nonConstPer.others_.push_back( (*j) );
	}

	nonConstPer.sites_.erase( nonConstPer.sites_.begin(), nonConstPer.sites_.end() );
	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin(); i!= MachLogPlanet::instance().sites().end(); ++i )
	{
		// only persist the sites out if there's minerals left
		if( (*i)->amountOfOre() > 0 )
			nonConstPer.sites_.push_back( (*i) );
	}
		

	nonConstPer.researchItems_.erase( nonConstPer.researchItems_.begin(), nonConstPer.researchItems_.end() );
	for( MachLogResearchTree::ResearchItems::iterator i = MachLogRaces::instance().researchTree().researchItems().begin(); i!= MachLogRaces::instance().researchTree().researchItems().end(); ++i )
		nonConstPer.researchItems_.push_back( (*i) );

	nonConstPer.constructionItems_.erase( nonConstPer.constructionItems_.begin(), nonConstPer.constructionItems_.end() );
	for( MachLogConstructionTree::ConstructionItems::iterator i = MachLogRaces::instance().constructionTree().constructionItems().begin(); i!= MachLogRaces::instance().constructionTree().constructionItems().end(); ++i )
		nonConstPer.constructionItems_.push_back( (*i) );

	nonConstPer.actions_.erase( nonConstPer.actions_.begin(), nonConstPer.actions_.end() );
	const SimConditionsManager::Actions& actions = SimManager::instance().conditionsManager().actions();
	for( SimConditionsManager::Actions::const_iterator i = actions.begin(); i != actions.end(); ++i )
		nonConstPer.actions_.push_back( *i );
//	nonConstPer.conditions_.erase( nonConstPer.conditions_.begin(), nonConstPer.conditions_.end() );
}

void MachLogOtherPersistence::setUpViewsFromLocalVector()
{
	//put all the actors back into the collections.
	//this should update everything accordingly.
	for( Others::iterator i = others_.begin(); i != others_.end(); ++i )
	{
		if( (*i)->objectType() == MachLog::SQUADRON )
			MachLogRaces::instance().addToSquadrons( (*i)->asSquadron() );
		else
			MachLogRaces::instance().addToAllCollections( (*i) );
	}

	//put the mineral sites back onto the planet - recreate oreholographs as well.
	for( Sites::iterator i = sites_.begin(); i != sites_.end(); ++i )
	{
		MachLogPlanet::instance().newSite( (*i) );		
//		if( (*i)->hasBeenDiscovered() )
//			MachLogActorMaker::newLogOreHolograph( (*i)->discoveredByRace(), 0, 0, (*i)->position() );
	}

	MachLogConstructionTree& conTree = MachLogRaces::instance().constructionTree();
	conTree.constructionItems().erase( conTree.constructionItems().begin(), conTree.constructionItems().end() );
	for( ConstructionItems::iterator i = constructionItems_.begin(); i != constructionItems_.end(); ++i )
		MachLogRaces::instance().constructionTree().constructionItems().push_back( (*i) );

	for( Actions::iterator i = actions_.begin(); i != actions_.end(); ++i )
		SimManager::instance().addAction( *i );
}

void perWrite( PerOstream& ostr, const MachLogOtherPersistence& per )
{
	per.setUpLocalVectorFromViews();
	ostr << per.sites_;
	ostr << per.others_;
	ostr << per.researchItems_;
	ostr << per.constructionItems_;
	ostr << per.actions_;
}

void perRead( PerIstream& istr, MachLogOtherPersistence& per )
{
	istr >> per.sites_;
	istr >> per.others_;
	istr >> per.researchItems_;

	//clear down machLogRaces researchTree and put our vector of items onto the end of it.
	//this must be done before the actions as there is an order dependancy with research_complete conditions.
	MachLogResearchTree& resTree = MachLogRaces::instance().researchTree();
	while( resTree.researchItems().size() )
	{
		_DELETE( resTree.researchItems().front() );
		resTree.researchItems().erase( resTree.researchItems().begin() );
	}

	for( MachLogOtherPersistence::ResearchItems::iterator i = per.researchItems_.begin(); i != per.researchItems_.end(); ++i )
		MachLogRaces::instance().researchTree().researchItems().push_back( (*i) );

	istr >> per.constructionItems_;
	istr >> per.actions_;
	per.setUpViewsFromLocalVector();
	//the artefacts (if any) subtype data is now finished with so call the clearing method.
	MachLogRaces::instance().artefacts().finish();
}

/* End PERMACH.CPP **************************************************/
