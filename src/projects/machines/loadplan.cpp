#include "base/istrrep.hpp"
#include "machines/sdlapp.hpp"
#include "system/filedata.hpp"
#include "system/fileenum.hpp"
#include "mathex/point2d.hpp"
#include "render/texture.hpp"
#include "render/texset.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/database.hpp"
#include "machlog/races.hpp"
#include "machlog/artfacts.hpp"

class PlanetLoadDummyProgressIndicator : public BaseProgressReporter
{
public:
	PlanetLoadDummyProgressIndicator() {};

    virtual size_t report( size_t , size_t  )
	{
		return 0;
	}

private:
};



static void loading( MachGuiDbSystem& system, W4dSceneManager* pSceneManager )
{
	PlanetLoadDummyProgressIndicator	reporter;
	const uint nPlanets = system.nPlanets();
	for( uint jPla=0; jPla<nPlanets; ++jPla)
	{
	    MachGuiDbPlanet& planet = system.planet( jPla );
        planet.textData();

		const uint nScenarios = planet.nScenarios();
		for( uint kSce=0; kSce<nScenarios; ++kSce )
		{
			const string& planetName = planet.scenario( kSce ).planetFile();
		    MachLogRaces::instance().loadPlanet( pSceneManager, planetName, &reporter );
		    MachLogRaces::instance().unloadGame();

            planet.scenario( kSce ).textData();
		}
	}
}

void SDLApp::loadPlanets()
{
    typedef ctl_vector< SysFileData > FileDatas;

    //Construct a file enumerator for all the models\planet\...\*.arf files
    SysFileEnumerator arfEnum(  SysPathName( "models/planet" ), SysPathName( "*.arf" ) );
    arfEnum.examineSubdirectories( true );
    arfEnum.find();

    const FileDatas& fileDatas = arfEnum.files();
    DANIEL_INSPECT( fileDatas.size() );

    //Handles parsing and creation of artefacts
    MachLogArtefacts artefacts;

    //Create the artefact persistent files for each one
    for( FileDatas::const_iterator cit = fileDatas.begin(); cit != fileDatas.end(); ++cit )
    {
        SysPathName arfPath = (*cit).pathName();
        DANIEL_INSPECT( arfPath );

        //Preload the textures for this planet (2MB versions will do)
        string textureDirectory = arfPath.directory();
        textureDirectory += "/texture2";
        RenTextureSet textureSet( textureDirectory );

        artefacts.load( arfPath );
        artefacts.unload();
        artefacts.finish();
    }

	//load all the planets

	//campaigns
	const uint nSystems = MachGuiDatabase::instance().nCampaignSystems();

	for(uint iSys=0; iSys<nSystems; ++iSys)
	{
	    MachGuiDbSystem& system = MachGuiDatabase::instance().campaignSystem( iSys );
        system.textData(); //Loads text bin file
		loading( system, manager_ );
	}

	//skirmishes
	const uint nTerrains = 3;
	for(uint iTer=0; iTer<nTerrains; ++iTer)
	{
	    MachGuiDbSystem& system = MachGuiDatabase::instance().skirmishSystem( MachGuiDatabase::TerrainSize(iTer) );
		loading( system, manager_ );
	}

	//multiPlaySystems
	for(uint iTer=0; iTer<nTerrains; ++iTer)
	{
	    MachGuiDbSystem& system = MachGuiDatabase::instance().multiPlayerSystem( MachGuiDatabase::TerrainSize(iTer) );
		loading( system, manager_ );
	}

}

