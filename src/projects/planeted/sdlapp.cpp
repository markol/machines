#include "planeted/sdlapp.hpp"

#include "base/base.hpp"
#include "base/istrrep.hpp"
#include "base/error.hpp"
#include "sound/snd.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndparam.hpp"
#include "device/butevent.hpp"
#include "profiler/profiler.hpp"
#include "afx/castfns.hpp"
#include "mathex/point2d.hpp"
#include "phys/phys.hpp"
#include "phys/cspace2.hpp"
#include "device/time.hpp"
#include "device/mouse.hpp"
#include "render/display.hpp"
#include "render/device.hpp"
#include "render/surface.hpp"
#include "render/capable.hpp"
#include "mathex/eulerang.hpp"
#include "world4d/root.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "world4d/soundman.hpp"
#include "world4d/shadow.hpp"
#include "world4d/camera.hpp"
#include "machlog/actor.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"
//#include "machlog/dbgstuff.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/preload.hpp"
#include "envirnmt/planet.hpp"
#include "planeted/errorhnd.hpp"
#include "planeted/leaktrak.hpp"
#include "planeted/availods.hpp"
#include "planeted/planeted.hpp"
#include "planeted/cameras.hpp"
#include "sim/manager.hpp"
#include "mathex/coordsys.hpp"
#include "system/metafile.hpp"
#include "system/winapi.hpp"

#include "machphys/machphys.hpp"
#include "machphys/levels.hpp"
#include "machphys/persist.hpp"

class ProgressIndicator : public BaseProgressReporter
{
public:
	ProgressIndicator()
	{
		lowerLimit_ = 0.0;
		upperLimit_ = 1.0;
        lastDone_ = 0;
	}

    virtual size_t report( size_t done, size_t maxDone )
	{
		if( done == lastDone_ ) return 0;
        const double minx = 98;
		const double maxx = 538;
		const double miny = 362;
		const double maxy = 366;
		const double width = maxx - minx + 1;
		const double height = maxy-miny+1;
		const double limitRange = upperLimit_ - lowerLimit_;
		const double percentComplete = (((double)done/(double)maxDone)*limitRange)+lowerLimit_;
		const double displayWidth = std::min((percentComplete*width)+5,width);

		const double red 	= 255.0/255.0;
		const double green 	= 250.0/255.0;
		const double blue 	= 142.0/255.0;

		RenDevice::current()->frontSurface().filledRectangle( Ren::Rect(minx,miny,displayWidth,height), RenColour(red,green,blue) );
		RenDevice::current()->display()->flipBuffers();
		RenDevice::current()->frontSurface().filledRectangle( Ren::Rect(minx,miny,displayWidth,height), RenColour(red,green,blue) );
        lastDone_ = done;
		return (double)maxDone/50.0;
	}

	void setLimits( double lower, double upper )
	{
		lowerLimit_ = lower;
		upperLimit_ = upper;
	}

private:
	double 	lowerLimit_;
	double 	upperLimit_;
    size_t  lastDone_;
};

void debugTiming( const char* text, bool startTiming )
{
	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( startTiming )
	{
		//FAST_DEBUG( text << std::endl );
		startTime = Phys::time();
	}
	else
	{
		//FAST_DEBUG( text << std::endl );
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_LIONEL, text << " , " << ( endTime - startTime ) << std::endl );
	}

}

SDLApp::SDLApp():
	winWidth_(400),
	winHeight_(300),
    pPlanet_( NULL ),
    pDisplay_( NULL ),
    pRoot_( NULL ),
    manager_( NULL ),
    loadScenario_("NONE"),
	loadArtefact_(""),
    showMemory_( false ),
	showCurrentMachine_( false ),
	initialised_(false),
    pEnvironment_( NULL )
{
    for( int i = 0; i < 4; ++i )
        aShowRace_[i] = false;
}

SDLApp::~SDLApp()
{
}

inline double degToRad(double degs)
{
	return degs / 180 * Mathex::PI;
}

static void usage()
{
	static string usageString;
	usageString = "Command line parameters are:\n";
	usageString += "\t<planet name>\n";
	usageString += "or\n";
	usageString += "\t<planet name> -f [<screen mode number>]\n";
	usageString += "or\n";
	usageString += "\t<planet name> -w <window width> <window height>\n";
	usageString += "\n";
	usageString += "Where:\n";
	usageString += "\t-w means run in window mode (the default)\n";
	usageString += "\t-f means run in full-screen mode\n";
	usageString += "\t<screen mode number> allows you to chose the screen resolution.\n";
	usageString += "\n";
	usageString += "Additional [optional] parameters are:\n";
	usageString += "\t-s < scenario file > ( .scn suffix is ADDED to filename)\n";
	usageString += "\t-a < artefact file > ( .arf suffix is ADDED to filename)";
	usageString += "\n";
	std::cout << usageString;
	std::cerr << usageString;
	SysWindowsAPI::messageBox( usageString.c_str(),"PlanetEditor v352.01" );
}

static  void    newHandler()
{
    WHERE_STREAM( "Aborting in new handler " << std::endl );
	//FAST_DEBUG("Aborting in new handler" << std::endl );
    ASSERT_FAIL( "Aborting in new handler" );
}

// Temporary timing stuff.
#ifndef PRODUCTION
#include "utility/time.hpp"
#endif
//#include <env.h>

bool SDLApp::clientStartup()
{
	// Call early so no files are read in incorrectly.
	SysMetaFile::encryptionType( SysMetaFile::ENCRYPTION_1 );

    // Set coord system
	MexCoordSystem::instance().set(MexCoordSystem::LEFT_HANDED);
	ProgressIndicator progressIndicator;

    //Initialise profiling
    double profileInterval = 50.0;
    char* pMs = getenv( "PROFILE_RATE" );
    if( pMs )
        profileInterval = atof( pMs );
    ProProfiler::instance().traceInterval( profileInterval / 1000.0 );

	// The debug timer doesn't work if this environment var isn't set.
	//setenv("CHARYBDIS", "x", false);
	//UtlDebugTimer::startCalibration();
#ifndef PRODUCTION
	UtlDebugTimer::calibrate();
#endif

	HAL_STREAM("SDLApp::clientStartup\n" );
    char* pRate = getenv( "MACH_RATE" );

    if( pRate )
    {
        MATHEX_SCALAR   rate = atof( pRate );
        DevTime::instance().rate( rate );

        std::cout << "Rate is " << rate << std::endl;
    }

    char* pRunTime = getenv( "MACH_RUN_TIME" );

    if( pRunTime )
    {
        runTime_ = atof( pRunTime );

        std::cout << "Run time is " << runTime_ << std::endl;
    }

//    set_new_handler( newHandler );

    //Set the diretory to look for all files
    SysPathName::rootEnvironmentVariable( "MACH_ROOT" );

    MachPhysData::instance();

	string planetName;
	bool windowMode = true;
    size_t  displayMode = 0;
    int modeW = 320;
    int modeH = 200;

	switch (invokeArgs().size())
	{
		case 8:	 // <planet file> -w <width> <height> -s <scenario.scn>	-a <artefact.arf>
				  //<planet file> -f <width> <height> -s <scenario.scn> -a <artefact.arf>
		{
			loadArtefact_ = invokeArgs()[7].c_str();
		}
		//Deliberate Drop through!!!
		case 6:	  // <planet file> -w <width> <height> -s <scenario.scn>
				  // <planet file> -f <width> <height> -s <scenario.scn>
		{
        	loadScenario_ = invokeArgs()[5].c_str();
		}
		//Deliberate Drop through!!!
		case 4:		// <planet file> -w <width> <height>
		     		// <planet file> -f <width> <height>
		{
			const string flag = invokeArgs()[1];
            windowMode = (flag == "-w");
        	modeW = atoi(invokeArgs()[2].c_str());
	        modeH = atoi(invokeArgs()[3].c_str());
		}

		case 1:
			planetName = invokeArgs()[0];
			break;
		default:
			usage();
			return false;
	}

	ASSERT(planetName.length() > 0, logic_error());

	// This must be called before any render library object is created.  (It
	// ensures a correct destruction order for render library Singletons.)
	Ren::initialise();

	// Initialise

    //Ensure correct order of destruction of static singletons.
    //Destruction order is reverse of creation
    //DevSound::instance();
    int nMaxSamples = 20;
    SndMixerParameters soundParams
    (
//    	window(), // Where this is the HWND of your application window
     	Snd::Polyphony(5), // The Mixers polyphony
    	SndMixerParameters::DIRECTSOUND,// You must be using DIRECTSOUND
    	Snd::ELEVEN_THOUSAND_HZ, // The playback sample rate
    	Snd::SIXTEEN_BIT,// The playback "bit-ness" ?!
    	Snd::STEREO,// Speaker setup
    	Snd::THREE_D,// THREE_D or TWO_D
    	nMaxSamples // Number of samples that can exist at one time, an assertion will occur if this limit is overstepped.
    );

    SndMixer::initialise(soundParams);

	MachPhysPreload::registerSounds();

	W4dSoundManager::instance().readSoundDefinitionFile("sounds/snddef64.dat");

	W4dRoot* root = pRoot_ = _NEW(W4dRoot( W4dRoot::W4dRootId() ));

	pDisplay_ = _NEW(RenDisplay(window()));

    ErrorHandler::instance().pDisplay( pDisplay_ );
    pDisplay_->buildDisplayModesList();

	if (!windowMode)
	{
		// Initially, pick the lowest-res 16-bit mode.
		pDisplay_->useFullScreen();

		std::cout << "Trying to select display mode " << modeW << "x" << modeH << "x" << 16 << std::endl;
		if (!pDisplay_->useMode(modeW, modeH, 0))
		{
			std::cout << "Failed to select that mode -- the nearest alternative will be chosen." << std::endl;
			pDisplay_->useNearestMode(modeW * modeH, 16);
		}

		// If there are no 16-bit modes, then mode should default to the
		// first mode in the list, regardless of depth.
		std::cout << "Initially, setting mode: " << pDisplay_->currentMode() << std::endl;

        // Ask the mouse to give us coordinates scale to the current Direct3D
	    // resolution (which doesn't necessarilly match the Windows resolution).
    	const RenDisplay::Mode& mode = pDisplay_->currentMode();
	    DevMouse::instance().scaleCoordinates(mode.width(), mode.height());
	}
	else
        pDisplay_->useMode(modeW, modeH, 0);

	manager_ = _NEW(W4dSceneManager(pDisplay_, root));
    W4dManager::instance().sceneManager( manager_ );

    //Set the viewport boundary to the entire screen.
    RenDevice& device = *manager_->pDevice();
    const int w = device.windowWidth();
    const int h = device.windowHeight();
	device.setViewport(0, 0, w, h);


    SysPathName planetFileName;
    // Set up the texture search path.
    RenTexManager::PathNames    searchList = RenTexManager::instance().searchList();

	// We set different search paths depending on the amount
	// of video memory availbable after the display mode has been set
	const bool canSupport4MegTexture = manager_->pDevice()->capabilities().supports4MBytesTextureSet();
	if(not canSupport4MegTexture)
	{
       searchList.push_back( SysPathName( "models/texture2" ) );
       searchList.push_back( SysPathName( "models/texture2/exp" ) );
       searchList.push_back( SysPathName( "models/texture2/fire" ) );
       searchList.push_back( SysPathName( "models/texture2/smoke" ) );
	   planetFileName = planetPath( planetName, "texture2", &searchList );
	}
	else
	{
       searchList.push_back( SysPathName( "models/texture4" ) );
       searchList.push_back( SysPathName( "models/texture4/exp" ) );
       searchList.push_back( SysPathName( "models/texture4/fire" ) );
       searchList.push_back( SysPathName( "models/texture4/smoke" ) );
	   planetFileName = planetPath( planetName, "texture4", &searchList );
    }

    RenTexManager::instance().searchList( searchList );
	// Used by PedTileEditor to cycle through the available *.lod file (tiles)
	SysPathName::Components lodPathComps;
	lodPathComps.push_back( "models/planet" );
	lodPathComps.push_back( planetName );
	lodPathComps.push_back( "lod" );
	SysPathName lodPath;
	lodPath.createFromComponents( lodPathComps );

	PedAvailableTileLods::instance().initialise( lodPath );

	// Display a loading screen.
	manager_->pDevice()->displayImage("gui/menu/wait.bmp");

	MachPhysPersistence::instance();

    //Construct the planet, surface and race
    pPlanet_ = &MachLogPlanet::instance();
    pPlanet_->surface( manager_, planetFileName, &progressIndicator );
    //Reuse loaded tiles lods
    pPlanet_->surface()->registerLoadedTiles();
    manager_->useLevelOfDetail(false);

	// If this environment variable is set, then don't load any Machines.
	// This provids a quick load when tuning sky colour, fog etc.
	if (!getenv("PLANET_ONLY"))
	{
	    //size_t memBefore = DbgMemChkAllocationData().maxMemoryNewed();
	    //memprobos << " Before redscar = " << memBefore << std::endl;
	    MachLogRaces::instance();
	}
	HAL_STREAM("SDLApp::clientStartup readEnvironment\n" );
    readEnvironment(planetName);

    manager_->hideStats();

    // Register the scene manager with the render libary manager
	HAL_STREAM("SDLApp::clientStartup sceneManager\n" );

    W4dShadowProjected2d::shadowPlaneZ(0.025);

	SysPathName::Components cspPathComps;
	cspPathComps.push_back( "models/planet" );
	cspPathComps.push_back( planetName );
	cspPathComps.push_back( planetName + ".csp" );
	SysPathName cspPath;
	cspPath.createFromComponents( cspPathComps );

	// Create csp file if it doesn't exist
	if ( not cspPath.existsAsFile() )
	{
		std::ofstream cspFile( cspPath.pathname().c_str() );
	}
	MachCameras::instance().initialise( manager_, pRoot_ );
	PedPlanetEditor::instance().initialise( manager_, pPlanet_ );
	PedPlanetEditor::instance().readCspFile( cspPath );

	// Load artefact data ( if present )
	if ( loadArtefact_ != "" )
	{
		SysPathName::Components scnPathComps;
		scnPathComps.push_back( "data" );
	  	scnPathComps.push_back( loadArtefact_ + ".arf" );
	 	SysPathName scnPath;
		scnPath.createFromComponents( scnPathComps );

		//	Abort if .arf doesn't exist
		if ( not scnPath.existsAsFile() )
		{
			string usageString = scnPath.c_str();
			usageString += "\t does not exist - terminating program.\n";
			std::cout << usageString;
			std::cerr << usageString;
			SysWindowsAPI::messageBox( usageString.c_str(),"PlanetEditor v0.01" );
			exit(0);
		}
		else
		{
			PedPlanetEditor::instance().readArfFile( scnPath );
		}
	}

	// Load model data for constructions/machines
	if( !getenv("CB_NOPRELOAD" ) )
	{
		MachPhysPreload::persistentFileName( "pemodels.bin" );
	    if( MachPhysPreload::persistentFileName().existsAsFile() )
	    {
	        MachPhysPreload::persistentPreload( MachPhysPreload::ECHO_PROGRESS, &progressIndicator );
	    }
	    else
	    {
	        MachPhysPreload::swPreload( MachPhysPreload::ECHO_PROGRESS );
	        MachPhysPreload::persistentSave( MachPhysPreload::ECHO_PROGRESS );
	    }
	}

	if ( loadScenario_ != "" )
	{
		SysPathName::Components scnPathComps;
		scnPathComps.push_back( "data" );
	  	scnPathComps.push_back( loadScenario_ + ".scn" );
	 	SysPathName scnPath;
		scnPath.createFromComponents( scnPathComps );

		// Create scn file if it doesn't exist
		if ( not scnPath.existsAsFile() )
		{
			std::ofstream scnFile( scnPath.pathname().c_str() );
		}

		PedPlanetEditor::instance().readScnFile( scnPath );
	}

    initialised_ = true;
    finishTimer_.time( 0 );

	//UtlDebugTimer::finishCalibration();

	return true;
}

void SDLApp::clientShutdown()
{
    _DELETE( pEnvironment_ );

    DevMouse::instance().unhide();
	_DELETE( pRoot_ );
    _DELETE( manager_ );
    _DELETE( pDisplay_ );
}

// Update the display.
void SDLApp::loopCycle()
{
    //Prevent processing before clientStartup() call
    if( not initialised_ )
        return;

	SimManager::instance().cycle();

	processInput();

    //Update world4d
    W4dManager& w4dManager = W4dManager::instance();
    w4dManager.update();

	MachCameras::instance().updateCameras();

	// If time is suspended for profiling, make the general position ID
	// change, so that W4dEntity evaluates transforms is if time were advancing.
	static const bool timeSuspended = getenv("SUSPEND_TIME") != NULL;
	if (timeSuspended)
		W4dManager::instance().generateGeneralPositionId();

 	MexTransform3d xform = manager_->currentCamera()->globalTransform();

	PedPlanetEditor::instance().preRenderUpdate();

	if (manager_->pDevice()->startFrame())
	{
	    //Render the scene
	  	debugTiming("SDLApp:: render", true );
	    w4dManager.render();
		debugTiming("SDLApp:: render complete", false );

		manager_->pDevice()->endFrame();
	}

	MexPoint3d pos = xform.position();
}

void SDLApp::outputDebugInfo( const MexPoint2d& pos, const MexTransform3d& xform, MachActor* pActor )
{
    if( showPosition_ )
    {
	    MexEulerAngles angles;
	    xform.rotation( &angles );
	    int ez = (int)(angles.azimuth().asScalar() * 180.0 / Mathex::PI );
	    int ey = (int)(angles.elevation().asScalar() * 180.0 / Mathex::PI );
	    int ex = (int)(angles.roll().asScalar() * 180.0 / Mathex::PI );

        //Test the cursor to actor code
        if( pActor != NULL )
            manager_->out() << "Cursor: " << pActor->objectType() << " " << (void*)&((const MachActor*)pActor)->physObject() << std::endl;

    	manager_->out() << "Eye at: " << pos << "(" << ez << "," << ey << "," << ex << ")\n";
    }

 	#ifndef NDEBUG
    if( showMemory_ )
        manager_->out() << DbgMemChkAllocationData();
	#endif

    if( aShowRace_[0] )
	{
		MachLogRaces::instance().outputRace( manager_->out(), MachPhys::RED );
	}

    if( aShowRace_[1] )
	{
		MachLogRaces::instance().outputRace( manager_->out(), MachPhys::BLUE );
	}

	#ifndef NDEBUG
    //Update leak tracking
    LeakTracker::update( LeakTracker::INLOOP, LeakTracker::NONE );
	#endif
}

// virtual
bool SDLApp::activate(/*WORD wordArg*/)
{
	if (!manager_)
		return false;

	if (manager_->pDevice())
		manager_->pDevice()->activate(/*wordArg*/);

	return true;
}

// virtual
void SDLApp::updateDisplay()
{
	if (initialised_ && manager_)
		manager_->updateDisplay();
}

// virtual
void SDLApp::getWindowSize(int&, int&, int& w, int& h)
{
	if (invokeArgs().size() > 1)
	{
		const string flag = invokeArgs()[1];
		if (flag == "-w" && invokeArgs().size() == 4)
		{
			winWidth_  = atoi(invokeArgs()[2].c_str());
			winHeight_ = atoi(invokeArgs()[3].c_str());
		}
	}

	w = winWidth_;
	h = winHeight_;
}

SysPathName SDLApp::planetPath( const string& planetName, const string& texDirectory, RenTexManager::PathNames* pSearchList )
{
	string texturePath = "models/planet/";
    texturePath += planetName;
    texturePath += "/";
    texturePath += texDirectory;

	string psfPath = "models/planet/";
    psfPath += planetName;
    psfPath += "/";
    psfPath += planetName;
	psfPath += ".psf";

	pSearchList->push_back( SysPathName( texturePath ) );

    return SysPathName( psfPath );
}

void SDLApp::readEnvironment(const string& planetLeafName)
{
	PRE(manager_);

    string planetPath = "models/planet/";
    planetPath += planetLeafName;
    planetPath += "/";
    planetPath += planetLeafName;
    planetPath += ".env";
	SysPathName withExt = planetPath;
	ASSERT_INFO(withExt);
	ASSERT(withExt.existsAsFile(), "Environment file doesn't exist.");
	ALWAYS_ASSERT(withExt.existsAsFile(), ("Environment file doesn't exist." + withExt.filename()).c_str() );

	//Create an environment for the sky etc
	//pEnvironment_ = _NEW(EnvPlanetEnvironment(withExt, manager_));
	//manager_->environment(pEnvironment_);
}

// virtual
const string& SDLApp::name() const
{
	static string n = "Machines Planet Editor v0.00.01";
	return n;
}

void SDLApp::checkForQuit( const DevButtonEvent& devButtonEvent )
{
	if(devButtonEvent.scanCode() == DevKey::KEY_Q or ( devButtonEvent.scanCode() == DevKey::ESCAPE and devButtonEvent.wasShiftPressed() ) )
	{
		finish();
		PhysConfigSpace2d* pConfigSpace = &pPlanet_->configSpace();

        std::ofstream os( "pedconfig.log" );
        os << *pConfigSpace;
        pConfigSpace->traceObstacles( os );
	}
}

/* End SDLApp.CPP ***************************************************/
