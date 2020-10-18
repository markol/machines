/*
 * R E S T R E E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "base/error.hpp"
#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "machlog/restree.hpp"
#include "machlog/internal/restreei.hpp"

#include "utility/linetok.hpp"

#include "machlog/mlnotif.hpp"
#include "machlog/scenario.hpp"
#include "machlog/resitem.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>


//////////////////////////////////////////////////////////////////////////////

MachLogResearchTree::MachLogResearchTree()
:	pImpl_( _NEW( MachLogResearchTreeImpl ) )
{
	CB_MachLogResearchTree_DEPIMPL();

	nNotifiables_ = 0;
	researchItems_.reserve( 100 );
    notifiables_.reserve( 100 );
	SysPathName globalTree( "data/global.rst" );
	readAllItems( globalTree );
    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////

MachLogResearchTree::~MachLogResearchTree()
{
	CB_MachLogResearchTree_DEPIMPL();

	HAL_STREAM("MLResearchTree::DTOR removing items\n" );
	while( researchItems_.size() > 0 )
	{
		_DELETE( researchItems_.front() );
		researchItems_.erase( researchItems_.begin() );
	}

	_DELETE( pImpl_ );
}

//////////////////////////////////////////////////////////////////////////////

const MachLogResearchTree::ResearchItems&
MachLogResearchTree::researchItems() const
{
	CB_MachLogResearchTree_DEPIMPL();

	return researchItems_;
}

//////////////////////////////////////////////////////////////////////////////

MachLogResearchTree::ResearchItems& MachLogResearchTree::researchItems()
{
	CB_MachLogResearchTree_DEPIMPL();

	return researchItems_;
}

//////////////////////////////////////////////////////////////////////////////

MachLogResearchItem&
MachLogResearchTree::researchItem( MachLog::ObjectType objectType, int subType, MachLogMachine::Level hwLevel, MachPhys::WeaponCombo wc )
{
	CB_MachLogResearchTree_DEPIMPL();

	//HAL_STREAM(" MLResearchTree::researchItem " << objectType << " sub " << subType << " hw " << hwLevel << std::endl );
	//if the hardware level is zero then just match on the first one that you find.
	//this is designed for te sw data which is stored only in the first instance of a RI for
	//obtype/sub type and not on hwLevel.
	bool found = false;
	MachLogResearchItem* pRI = NULL;
	ResearchItems::iterator i = researchItems_.begin();
	for( ; i != researchItems_.end() and not found; ++i )
	{
		if( (*i)->objectType() == objectType and
			(*i)->subType() == subType and
			(*i)->weaponCombo() == wc )
		{
			if( hwLevel == 0 )
			{
				pRI = *i;
				found = true;
			}
			else if( (*i)->hwLevel() == hwLevel )
			{
				pRI = *i;
				found = true;
			}
		}
	}
	ASSERT_INFO( objectType );
	ASSERT_INFO( subType );
	ASSERT_INFO( hwLevel );
	ASSERT_INFO( wc );
	ASSERT( found,"Didn't find combination in research tree.\n" );
	std::ostringstream ss;
	ss << "Machine combination not found in research tree: " << objectType << " " << subType << " " << hwLevel << " " << wc;
	ALWAYS_ASSERT( found, ss.str().c_str());
	return *pRI;
}

//////////////////////////////////////////////////////////////////////////////

void MachLogResearchTree::readAllItems( const SysPathName& treePath )
{
	CB_MachLogResearchTree_DEPIMPL();

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, treePath, ios::text ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, treePath, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( treePath.c_str() );
        //pIstream = _NEW( ifstream( treePath.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( treePath.c_str(), std::ios::in ) ));
    }

	UtlLineTokeniser parser( *pIstream, treePath );
    while( not parser.finished() )
    {
    	size_t lineSize = parser.tokens().size();
		MachLog::ObjectType obType;
		int subType = 0;
		int hwLevel = 0;
		int swLevel = 0;
		int techLevel = 0;
		int researchCost = 0;
		int buildingCost = 0;
		if( lineSize > 0 )
		{
			if( parser.tokens()[0] == "MACHINE" or parser.tokens()[0] == "CONSTRUCTION" )
			{
				bool doingMachine = true;
				obType = MachLogScenario::objectType( parser.tokens()[ 1 ] );
				if( lineSize == 3 )
					subType = MachLogScenario::objectSubType( obType, parser.tokens()[2].c_str() );
				parser.parseNextLine();
				MachLog::ObjectType parentObType;
				MachPhys::HardwareLabSubType hardwareLabSubType = MachPhys::LAB_MILITARY;
				MachPhys::WeaponCombo weaponCombo = MachPhys::N_WEAPON_COMBOS;
				MachPhys::WeaponCombo parentWeaponCombo = MachPhys::N_WEAPON_COMBOS;
				MachPhys::BuildingMaterialUnits newFactoryCost = 0;
				bool factoryCostAdjustmentRelative = true;
				int parentSubType = 0;
				int parentHwLevel = 0;
				int swTechLevel[6];
				//most RI will not have a valid set of swdata
				//set the techlevl outside the scope of the technicians
				for( int i = 0; i <5; ++i )
					swTechLevel[i] = 10;
				int swResearchCost[6];
				int swBuildingCost[6];
				while( doingMachine )
				{
					lineSize = parser.tokens().size();
					if( lineSize > 0 )
					{
						if( parser.tokens()[0] == "HWLEVEL" )
							hwLevel = atoi( parser.tokens()[1].c_str() );
						else if( parser.tokens()[0] == "SWLEVEL" )
							swLevel = atoi( parser.tokens()[1].c_str() );
						else if( parser.tokens()[0] == "TECHLEVEL" )
							techLevel = atoi( parser.tokens()[1].c_str() );
						else if( parser.tokens()[0] == "RESEARCH_COST" )
							researchCost = atoi( parser.tokens()[1].c_str() );
						else if( parser.tokens()[0] == "BUILDING_COST" )
							buildingCost = atoi( parser.tokens()[1].c_str() );
						else if( parser.tokens()[0] == "MILITARY" )
							hardwareLabSubType = MachPhys::LAB_MILITARY;
						else if( parser.tokens()[0] == "CIVILIAN" )
							hardwareLabSubType = MachPhys::LAB_CIVILIAN;
						else if( parser.tokens()[0] == "WCOMBO" )
							weaponCombo = MachLogScenario::weaponCombo( parser.tokens()[1] );
						else if( parser.tokens()[0] == "FACTORY_COST" )
						{
							ASSERT( parser.tokens().size() == 3,"Factory_cost line must be 3 tokens (absolute/relative and amount)" );
							factoryCostAdjustmentRelative = ( parser.tokens()[1] == "RELATIVE" );
							newFactoryCost = atol( parser.tokens()[2].c_str() );
						}
						else if( parser.tokens()[0] == "PARENT" )
						{
							//HAL_STREAM(" reading a parent line\n" );
							parentObType = MachLogScenario::objectType( parser.tokens()[ 1 ] );
							parentHwLevel = atoi( parser.tokens()[2].c_str() );
							if( lineSize >= 4 )
								parentSubType = MachLogScenario::objectSubType( parentObType, parser.tokens()[3].c_str() );
							if( lineSize >= 5 )
								parentWeaponCombo = MachLogScenario::weaponCombo( parser.tokens()[4] );
						}
						else if( parser.tokens()[0] == "SWTECHLEVEL" )
						{
							ASSERT_INFO( lineSize );
							ASSERT( lineSize == 6,"Wrong number of tokens on SWTECHLEVEL line\n" );
							for( int i = 1; i < 6; ++i )
								swTechLevel[ i ] = atoi( parser.tokens()[i].c_str() );
						}
						else if( parser.tokens()[0] == "SW_RESEARCH_COST" )
						{
							ASSERT_INFO( lineSize );
							ASSERT( lineSize == 6,"Wrong number of tokens on SW_RESEARCH_COST line\n" );
							for( int i = 1; i < 6; ++i )
								swResearchCost[ i ] = atoi( parser.tokens()[i].c_str() );
						}
						else if( parser.tokens()[0] == "SW_BUILDING_COST" )
						{
							ASSERT_INFO( lineSize );
							ASSERT( lineSize == 6,"Wrong number of tokens on SW_BUILDING_COST line\n" );
							for( int i = 1; i < 6; ++i )
								swBuildingCost[ i ] = atoi( parser.tokens()[i].c_str() );
						}
						else if( parser.tokens()[0] == "END" )
							doingMachine = false;
					}
					if( doingMachine )
						parser.parseNextLine();
				}
				MachLogResearchItem* pParentRI = NULL;
				if( parentHwLevel )
					pParentRI = &researchItem( parentObType, parentSubType, parentHwLevel, parentWeaponCombo );
				HAL_STREAM("MLResearchTree gonna create a new MLresearchItem with " << obType << "," << subType << "," << hwLevel << "," << swLevel << std::endl );
				MachLogResearchItem* pRI = _NEW( MachLogResearchItem( obType, subType, hwLevel, swLevel, techLevel, researchCost, buildingCost, pParentRI, hardwareLabSubType, weaponCombo ) );
				if( newFactoryCost > 0 )
				{
					if( factoryCostAdjustmentRelative )
						pRI->factoryInstanceCost( pRI->factoryInstanceCost() + newFactoryCost );
					else
						pRI->factoryInstanceCost( newFactoryCost );
				}
//				ASSERT_INFO( *pRI );
				researchItems_.push_back( pRI );
				for( int i = 1; i < 6; ++i )
				{
					pRI->swTechnologyLevel( i, swTechLevel[i] );
					pRI->swResearchCost( i, swResearchCost[i] );
					pRI->swBuildingCost( i, swBuildingCost[i] );
				}

				if( obType == MachLog::AGGRESSOR and hwLevel == 4 )
				{
					MachLogResearchItem* pRI2 = _NEW( MachLogResearchItem( obType, subType, hwLevel, swLevel, 1, researchCost, buildingCost, NULL, hardwareLabSubType, MachPhys::LR_LARGE_MISSILE_X2 ) );
					pRI2->factoryInstanceCost( 100 );
					for( int i = 1; i < 6; ++i )
					{
						pRI2->swTechnologyLevel( i, swTechLevel[i] );
						pRI2->swResearchCost( i, swResearchCost[i] );
						pRI2->swBuildingCost( i, swBuildingCost[i] );
					}
					researchItems_.push_back( pRI2 );
				}
			}
		}
		parser.parseNextLine();
	}
}

//////////////////////////////////////////////////////////////////////////////

void MachLogResearchTree::addMe( MachLogNotifiable* pNewRTreeNot )
{
	CB_MachLogResearchTree_DEPIMPL();

	notifiables_.push_back( pNewRTreeNot );
	++nNotifiables_;

	TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////

void MachLogResearchTree::removeMe( MachLogNotifiable* pCurrentRTreeNot )
{
	CB_MachLogResearchTree_DEPIMPL();

	Notifiables::iterator i = notifiables_.begin();
	bool found = false;
	while ( not found && i != notifiables_.end() )
	{
		if ( (*i) == pCurrentRTreeNot )
		{
 			notifiables_.erase (i);
 			found = true;
			--nNotifiables_;
		}
		else
			++i;
	}

	ASSERT ( found, "MachLogResearchTree::removeMe : couldn't find a match for pCurrentRTreeNot in notifiables_");

   	TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////

void MachLogResearchTree::updated( MachPhys::Race r )
{
	CB_MachLogResearchTree_DEPIMPL();

	if (nNotifiables_ > 0 )
	{
		// Looks like someone potentially cares that we've changed...........

		for (Notifiables::iterator i = notifiables_.begin(); i != notifiables_.end(); ++i)
		{
			// only notify an observer of the same race type as that for which the tree is being updated.
			if ( (*i)->race() == r )
			{
				(*i)->notifiableBeNotified();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

bool MachLogResearchTree::activated( MachPhys::Race r, MachLog::ObjectType objectType, int subType, size_t hwLevel, MachPhys::WeaponCombo wc )
{
	CB_MachLogResearchTree_DEPIMPL();

	bool isActivated = false;
	ResearchItems::iterator i = researchItems_.begin();
	for( ; i != researchItems_.end() and not isActivated; ++i )
	{
		if( objectType == (*i)->objectType() and (*i)->researched( r ) )
		{
			if ( subType == MachLog::DONTCARE )
			{
				ASSERT( hwLevel == MachLog::DONTCARE and wc == MachPhys::N_WEAPON_COMBOS, "No further parameters should be passed after a DONTCARE parameter." );
				isActivated = true;
			}
			else
			  	if ( subType == (*i)->subType() )
			  	{
	 		  		if( hwLevel == MachLog::DONTCARE )
	 		  		{
						ASSERT( wc == MachPhys::N_WEAPON_COMBOS, "No further parameters should be passed after a DONTCARE parameter." );
	 		  			isActivated = true;
	 		  		}
					else
				    	if ( hwLevel == (*i)->hwLevel() )

						{
 						if ( wc == MachPhys::N_WEAPON_COMBOS or wc == (*i)->weaponCombo() )
								isActivated = true;
						}
			  	}
		}
	}

	TEST_INVARIANT;

	return isActivated;

}

//////////////////////////////////////////////////////////////////////////////

void MachLogResearchTree::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

//////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogResearchTree& t )
{

    o << "MachLogResearchTree " << (void*)&t << " start" << std::endl;
	o << " ResearchItems.size() " << t.researchItems().size() << std::endl;
    o << "MachLogResearchTree " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////

/* End RESTREE.CPP **************************************************/
