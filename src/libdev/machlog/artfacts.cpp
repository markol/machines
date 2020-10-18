/*
 * A R T F A C T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "mathex/point2d.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/internal/artfctsi.hpp"
#include "system/pathname.hpp"
#include "utility/linetok.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/genrepos.hpp"
#include "world4d/generic.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/domain.hpp"
#include "world4d/soundman.hpp"
#include "machphys/artfdata.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/snddata.hpp"

#include "machlog/artefact.hpp"
#include "machlog/internal/artsdata.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/races.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

PER_DECLARE_PERSISTENT( MachLogArtefacts );

#define CB_MachLogArtefacts_DEPIMPL()	\
	PRE( pImpl_ )	\
    CB_DEPIMPL( W4dGenericRepository* 	,pExemplars_ ) \
    CB_DEPIMPL( MachLogArtefactsData* 	,pData_ ) \
	CB_DEPIMPL( SysPathName*			,pArtefactPathName_ ) \
	CB_DEPIMPL( bool 					,artefactsParsed_ );


MachLogArtefacts::MachLogArtefacts()
:	pImpl_( _NEW( MachLogArtefactsImpl ) )
{
    TEST_INVARIANT;
}

MachLogArtefacts::~MachLogArtefacts()
{
    TEST_INVARIANT;

	CB_MachLogArtefacts_DEPIMPL();
//    _DELETE( pExemplars_ ); // don't be a MERGE MORON - don't put this line back in
//    _DELETE( pData_ );	  // don't be a MERGE MORON - don't put this line back in
	_DELETE( pImpl_ );
}

void MachLogArtefacts::CLASS_INVARIANT
{
	CB_MachLogArtefacts_DEPIMPL();
    INVARIANT( this != NULL );
    INVARIANT( pExemplars_ != NULL );
    INVARIANT( pData_ != NULL );
}

ostream& operator <<( ostream& o, const MachLogArtefacts& t )
{

    o << "MachLogArtefacts " << (void*)&t << " start" << std::endl;
    o << "MachLogArtefacts " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogArtefacts::load( const SysPathName& pathname )
{
	CB_MachLogArtefacts_DEPIMPL();
	HAL_STREAM("MLArtefacts::load " << pathname << std::endl );

    //Make sure get loaded round network
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.isNodeLogicalHost() )
		network.messageBroker().sendLoadArtefactModelsMessage( pathname );

    //Set the filename for the persistent version
    string path = "pdata\\" + pathname.components().back();
    SysPathName persistentPathname( path );
    persistentPathname.extension( "arb" );

    //Use a temporary repository to read the data into
    W4dGenericRepository* pTemporaryRepository = _NEW( W4dGenericRepository );

    if( persistentPathname.existsAsFile() )
    {
    	std::ifstream ifstr( persistentPathname.c_str(), std::ios::binary );
    	PerIstream istr( ifstr );

    	istr >> (*pTemporaryRepository);
    }
    else
    {
        //Assume a text file and read into the repository
        pTemporaryRepository->readGenericFile( pathname );

        //Save this as a binary persistent file
    	std::ofstream ofstr( persistentPathname.c_str(), std::ios::binary );
    	PerOstream ostr( ofstr );

    	ostr << (*pTemporaryRepository);
    }

    //Move the models over to the exemplars, and delete the temporary one
    pExemplars_->takeAllFrom( pTemporaryRepository );
    _DELETE( pTemporaryRepository );
}

void MachLogArtefacts::finish()
{
	CB_MachLogArtefacts_DEPIMPL();
    //Clear the repository of exemplar models
    pExemplars_->clear();

    //Delete the subtypes implementation
    _DELETE( pData_->pSubTypes_ );
    pData_->pSubTypes_ = NULL;
}

int MachLogArtefacts::nSubTypes() const
{
	CB_MachLogArtefacts_DEPIMPL();
    return pData_->artefactDatas_.size();
}

void MachLogArtefacts::addSubType
(
    int subType, const string& name, MachPhys::BuildingMaterialUnits cost,
    MachPhys::ArmourUnits armour, MachPhys::HitPointUnits hitPoints, uint stringId,
    MATHEX_SCALAR height, const ctl_vector< MexPoint2d >& localBoundary
)
{
	CB_MachLogArtefacts_DEPIMPL();
	HAL_STREAM("MLArtefacts::adding complex sub type " << subType << " " << name << std::endl );
    PRE( nSubTypes() == subType );
    PRE( localBoundary.size() >= 3 );
    PRE( pExemplars_->type( name ) != W4dGenericRepository::UNKNOWN );

    //Construct a polygon for the boundary
    MexConvexPolygon2d* pBoundaryPolygon =
        _NEW( MexConvexPolygon2d( localBoundary ) );

    //Construct an artefact data object for this subType
    MachPhysArtefactData* pArtefactData =
        _NEW( MachPhysArtefactData( height, pBoundaryPolygon, armour, hitPoints, cost,
                                    subType, stringId ) );

    //Find the generic exemplar in the repository
    const W4dEntity* pExemplar = &pExemplars_->entity( name );

    //Ensure the vector of sub types exists
    if( pData_->pSubTypes_ == NULL )
    {
        pData_->pSubTypes_ = _NEW( ctl_vector< MachLogArtefactSubType > );
        pData_->pSubTypes_->reserve( 64 );
    }

    //Push a new subtype into the collection
    pData_->pSubTypes_->push_back( MachLogArtefactSubType( subType, pExemplar, name ) );
    pData_->artefactDatas_.push_back( pArtefactData );
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.isNodeLogicalHost() )
		network.messageBroker().sendAddNewArtefactSubTypeMessage( subType, name, cost, armour, hitPoints, stringId, height, &localBoundary );
}

void MachLogArtefacts::addSubType
(
    int subType, const string& name
)
{
	CB_MachLogArtefacts_DEPIMPL();
	HAL_STREAM("MLArtefacts::adding dumb sub type " << subType << " " << name << std::endl );
    PRE( nSubTypes() == subType );
    PRE( pExemplars_->type( name ) != W4dGenericRepository::UNKNOWN );

    //Find the generic exemplar in the repository
    const W4dEntity* pExemplar = &pExemplars_->entity( name );

    //Ensure the vector of sub types exists
    if( pData_->pSubTypes_ == NULL )
    {
        pData_->pSubTypes_ = _NEW( ctl_vector< MachLogArtefactSubType > );
        pData_->pSubTypes_->reserve( 64 );
    }

    //Push a new subtype into the collection
    pData_->pSubTypes_->push_back( MachLogArtefactSubType( subType, pExemplar, name ) );
    pData_->artefactDatas_.push_back( NULL );

	MachLogNetwork& network = MachLogNetwork::instance();
	//send message to add new subtype - DUMBs key from localboundary being NULL
	if( network.isNetworkGame() and network.isNodeLogicalHost() )
		network.messageBroker().sendAddNewArtefactSubTypeMessage( subType, name, 0, 0, 0, 0, 0, NULL );
}

W4dEntity* MachLogArtefacts::newPhysArtefact( int subType, const MexPoint3d& location, const MexRadians& angle ) const
{
	CB_MachLogArtefacts_DEPIMPL();
	HAL_STREAM("MLArtefacts::newPhysArtefact " << subType << std::endl );
    PRE( subType < nSubTypes() );

    //Find the domain and get the local transform at the specified location
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, angle, &localTransform );

    //Get the exemplar entity
	HAL_STREAM("MLArtefacts::newPhysArtefact dereferencing for the exemplar using subtype " << subType << std::endl );
    MachLogArtefactSubType& artefactSubType = (*pData_->pSubTypes_)[ subType ];
    const W4dEntity& exemplar = *(artefactSubType.pExemplar_);

    //Construct a simple or composite generic copy as appropriate
    W4dEntity* pResult;
    if( exemplar.isComposite() )
        pResult = _NEW( W4dGenericComposite( exemplar.asComposite(), pDomain, localTransform ) );
    else
        pResult = _NEW( W4dGeneric( exemplar, pDomain, localTransform ) );

    //Add any domain intersect relations
    MachLogPlanetDomains::addIntersectRelations( pResult );

    //Add any sound which should be associated with this model
    const string& artefactName = artefactSubType.name_;
    uint soundId = 0;
    if( artefactName == "cmain" )
        soundId = SID_CMAIN;
    else if( artefactName == "tile22" )
        soundId = SID_TILE22;
    else if( artefactName == "control" )
        soundId = SID_CONTROL;
    else if( artefactName == "cool1" )
        soundId = SID_COOL1;
    else if( artefactName == "power" )
        soundId = SID_POWER;
    else if( artefactName == "contrl" )
        soundId = SID_CONTRL;
    else if( artefactName == "shield" )
        soundId = SID_SHIELD;
    else if( artefactName == "cdom" )
        soundId = SID_CDOM;

    if( soundId != 0 )
        W4dSoundManager::instance().play( pResult, soundId, PhysAbsoluteTime( 0.0 ), W4dSoundManager::LOOP_CONTINUOUSLY );

    return pResult;
}

const MachPhysArtefactData& MachLogArtefacts::artefactData( int subType ) const
{
	CB_MachLogArtefacts_DEPIMPL();
    PRE( subType < nSubTypes() );

    return *(pData_->artefactDatas_[ subType ]);
}

void MachLogArtefacts::parseArtefactsSection
(
    UtlLineTokeniser* pParser, MachLogArtefacts::CreateArtefactInstances doCreate
)
{
	CB_MachLogArtefacts_DEPIMPL();
	//store the file that the artefacts are parsed from.
	_DELETE( pArtefactPathName_ );
	pArtefactPathName_ = _NEW( SysPathName( pParser->fileName() ) );
	artefactsParsed_ = true;

    bool done = false;
    int currentSubType = -1;
    bool currentSubTypeIsDumb = false;
    string currentArtefactTypeName;

    //Get the planet surface
    const MachPhysPlanetSurface& planetSurface = *(MachLogPlanet::instance().surface());

    //Set up collections of tag id used in the scenario file and corresponding artefact
    ctl_vector< uint > tagIds;
    ctl_pvector< MachLogArtefact > taggedArtefacts;
    tagIds.reserve( 256 );
    taggedArtefacts.reserve( 256 );

    while( not done )
    {
        //Check for end of section
        if( pParser->tokens()[0] == "ENDARTEFACTS" )
        {
            done = true;
			//if this is a scenario based parsing run then the instances will have been created and
			//we can throw away the lookup data.
			if( doCreate == CREATE_ARTEFACT_INSTANCES )
	            finish();
        }
		else if( pParser->tokens()[0] == "ARTEFACTS" )
		{
			//we can have that line in our jurisdiction if we are parsing via the persistence mechanism
		}
        else if( pParser->tokens()[0] == "MODELS" )
        {
            //Load the models file into the repository
            ASSERT( pParser->tokens().size() == 2, "" );
            load( SysPathName( pParser->tokens()[1] ) );
        }
        else if( pParser->tokens()[0] == "DUMB" )
        {
            //Advance subType index
            ++currentSubType;
            currentSubTypeIsDumb = true;

            //Define a new non-interacting sub type - example
            //DUMB bush
            ASSERT( pParser->tokens().size() == 2, "" );

            //register the subtype
            currentArtefactTypeName = pParser->tokens()[1];
            addSubType( currentSubType, pParser->tokens()[1].c_str() );
        }
        else if( pParser->tokens()[0] == "ACTIVE" )
        {
            //Advance subType index
            ++currentSubType;
            currentSubTypeIsDumb = false;

            //Define a new logical sub type - example
            //ACTIVE tree COST 1 HP 2 ARMOUR 2 IDS 7030 HEIGHT 2 BOUNDARY -1.5 0  1.5 1  1.5 -1
            ASSERT( pParser->tokens().size() >= 19, "" );

            //get the BMU cost of the artefact
            ASSERT( pParser->tokens()[2] == "COST", "" );
            MachPhys::BuildingMaterialUnits cost = atol(pParser->tokens()[3].c_str() );

            //get the hitpoint and armour counts
            ASSERT( pParser->tokens()[4] == "HP", "" );
            MachPhys::HitPointUnits hitPoints = atol(pParser->tokens()[5].c_str() );

            ASSERT( pParser->tokens()[6] == "ARMOUR", "" );
            MachPhys::ArmourUnits armour = atol(pParser->tokens()[7].c_str() );

            //get the resource string id
            ASSERT( pParser->tokens()[8] == "IDS", "" );
            uint stringId = atol(pParser->tokens()[9].c_str() );

            //get boundary height
            ASSERT( pParser->tokens()[10] == "HEIGHT", "" );
            MATHEX_SCALAR height = atof(pParser->tokens()[11].c_str() );

            //Now the boundary polygon coordinates
            ASSERT( pParser->tokens()[12] == "BOUNDARY", "" );
            uint nBoundaryCoords = pParser->tokens().size() - 13;
            ASSERT( (nBoundaryCoords & 1) == 0, "Odd number of coords" );
            uint nBoundaryPoints = nBoundaryCoords / 2;

            size_t index = 13;
            ctl_vector< MexPoint2d > boundaryPoints;
            boundaryPoints.reserve( nBoundaryPoints );

            for( uint i = 0; i != nBoundaryPoints; ++i )
            {
                MATHEX_SCALAR x = atof(pParser->tokens()[index++].c_str() );
                MATHEX_SCALAR y = atof(pParser->tokens()[index++].c_str() );
                boundaryPoints.push_back( MexPoint2d( x, y ) );
            }

            //register the subtype
            currentArtefactTypeName = pParser->tokens()[1];
            addSubType( currentSubType, pParser->tokens()[1].c_str(), cost, armour, hitPoints, stringId,
                        height, boundaryPoints );
        }
        else if( pParser->tokens()[0] == "AT" )
        {
            //Create an artefact instance - example line (ID clause optional)
            //AT 250 800 10 -90 ID 1
            size_t nTokens = pParser->tokens().size();
            ASSERT( nTokens == 5 or nTokens == 7, "" );
            ASSERT( currentSubType >= 0, "No artefact type defined" );

            //Get the x, y location
            MATHEX_SCALAR x = atof(pParser->tokens()[1].c_str() );
            MATHEX_SCALAR y = atof(pParser->tokens()[2].c_str() );

            //Get the terrain height at this location for the z coordinate
            MATHEX_SCALAR z = planetSurface.terrainHeight( x, y );

            //Get the z axis angle
            MexRadians zAngle( MexDegrees(atof( pParser->tokens()[4].c_str() ) ) );

            //Create an artefact
            if( currentSubTypeIsDumb )
            {
                MachLogActorMaker::newDumbArtefact( currentSubType, MexPoint3d( x, y, z ), zAngle );
            }
            else if( doCreate == CREATE_ARTEFACT_INSTANCES )
            {
                //Do a logical type of artefact
                MachLogArtefact* pArtefact =
                    MachLogActorMaker::newLogArtefact( currentSubType, MexPoint3d( x, y, z ), zAngle );

                //Check for a tag id clause
                if( nTokens == 7 )
                {
                    ASSERT( pParser->tokens()[5] == "ID", "" );
                    uint tagId = atol(pParser->tokens()[6].c_str() );

                    //Store the tag id and pointer in matching vectors
                    tagIds.push_back( tagId );
                    taggedArtefacts.push_back( pArtefact );
                }
            }
        }
        else if( pParser->tokens()[0] == "ONDESTROY" )
        {
			if( doCreate == CREATE_ARTEFACT_INSTANCES )
			{
	            //Set up a link to damage another artefact on destruction of one
	            //ONDESTROY 1 DAMAGE 3 HP 10 ARMOUR 1
	            ASSERT( pParser->tokens().size() == 8, "" );

	            //Get the dying artefact
	            uint tagId = atol( pParser->tokens()[1].c_str() );
	            MachLogArtefact* pDyingArtefact = NULL;
	            for( size_t i = 0; pDyingArtefact == NULL  and  i < tagIds.size(); ++i )
	                if( tagIds[i] == tagId )
	                    pDyingArtefact = taggedArtefacts[i];

	            ASSERT_INFO( tagId );
	            ASSERT( pDyingArtefact != NULL, "Dying artefact id not in use" );

	            //Get the damaged artefact
	            ASSERT( pParser->tokens()[2] == "DAMAGE", "" );
	            tagId = atol( pParser->tokens()[3].c_str() );
	            MachLogArtefact* pDamageArtefact = NULL;
	            for( size_t i = 0; pDamageArtefact == NULL  and  i < tagIds.size(); ++i )
	                if( tagIds[i] == tagId )
	                    pDamageArtefact = taggedArtefacts[i];

	            ASSERT_INFO( tagId );
	            ASSERT( pDamageArtefact != NULL, "Damage artefact id not in use" );

	            //get the hp and armour values
	            ASSERT( pParser->tokens()[4] == "HP", "" );
	            int hpDamage = atol( pParser->tokens()[5].c_str() );
	            ASSERT( pParser->tokens()[6] == "ARMOUR", "" );
	            int armourDamage = atol( pParser->tokens()[7].c_str() );

	            //Set up the damage relation
	            addDamageRelation( pDyingArtefact->id(), pDamageArtefact->id(), hpDamage, armourDamage );
			}
        }
        else
        {
            ASSERT_INFO( pParser->tokens()[0] );
            ASSERT( false, "invalid line in artefacts section" );
        }

        //Do the next line. The caller skips to next line after ENDARTEFACTS, so we don't
        if( not done )
            pParser->parseNextLine();
    }
}

void MachLogArtefacts::unload()
{
	CB_MachLogArtefacts_DEPIMPL();
    //Delete all the artefact data objects
    MachLogArtefactsData::ArtefactDatas& artefactDatas = pData_->artefactDatas_;
    for( MachLogArtefactsData::ArtefactDatas::iterator it = artefactDatas.begin(); it != artefactDatas.end(); ++it )
        _DELETE( (*it) );

    artefactDatas.erase( artefactDatas.begin(), artefactDatas.end() );
	artefactsParsed_ = false;
}

void MachLogArtefacts::addDamageRelation( UtlId dyingArtefactId, UtlId damageArtefactId, int hpDamage, int armourDamage )
{
	CB_MachLogArtefacts_DEPIMPL();
	//do the network stuff first as values of parameters can be changed inside the function.
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.isNodeLogicalHost() )
		network.messageBroker().sendAddArtefactDamageLinkMessage( dyingArtefactId, damageArtefactId, hpDamage, armourDamage );

    //Get the 2 artefacts from the ids
	MachLogRaces& races = MachLogRaces::instance();
    MachLogArtefact& dyingArtefact = races.actor( dyingArtefactId ).asArtefact();
    MachLogArtefact& damageArtefact = races.actor( damageArtefactId ).asArtefact();

    //set the values dependent on absolute/relative as indicated by sign of damage values
    MachLogArtefactLinkData::DamageType hpDamageType = MachLogArtefactLinkData::DAMAGE_ABSOLUTE;
    MachLogArtefactLinkData::DamageType armourDamageType = MachLogArtefactLinkData::DAMAGE_ABSOLUTE;

    if( hpDamage < 0 )
    {
        hpDamageType = MachLogArtefactLinkData::DAMAGE_RELATIVE;
        hpDamage = -hpDamage;
    }

    if( armourDamage < 0 )
    {
        armourDamageType = MachLogArtefactLinkData::DAMAGE_RELATIVE;
        armourDamage = -armourDamage;
    }

    //Add the link to the dying artefact
    dyingArtefact.damageOnDestroy( &damageArtefact, hpDamageType, hpDamage, armourDamageType, armourDamage );
}

const SysPathName& MachLogArtefacts::artefactPathName() const
{
	CB_MachLogArtefacts_DEPIMPL();
	return *pArtefactPathName_;
}

bool MachLogArtefacts::artefactsParsed() const
{
	CB_MachLogArtefacts_DEPIMPL();
	return artefactsParsed_;
}

void perWrite( PerOstream& ostr, const MachLogArtefacts& artefacts )
{
	ostr << artefacts.pImpl_;
}

void perRead( PerIstream& istr, MachLogArtefacts& artefacts )
{
	istr >> artefacts.pImpl_;
	UtlLineTokeniser parser( artefacts.artefactPathName() );
	artefacts.parseArtefactsSection( &parser, MachLogArtefacts::DO_NOT_CREATE_ARTEFACT_INSTANCES );
}

/* End ARTFACTS.CPP *************************************************/
