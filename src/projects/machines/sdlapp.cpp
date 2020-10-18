//#include <env>

#include "machines/sdlapp.hpp"
#include "afx/castfns.hpp"
#include "system/pathname.hpp"
#include "system/winapi.hpp"
#include "system/registry.hpp"
#include "mathex/point2d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/coordsys.hpp"
#include "device/time.hpp"
#include "device/mouse.hpp"
#include "device/cd.hpp"
#include "network/netnet.hpp"
#include "network/node.hpp"
#include "profiler/profiler.hpp"
#include "phys/phys.hpp"
#include "render/display.hpp"
#include "render/device.hpp"
#include "render/capable.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/texset.hpp"
#include "world4d/root.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "world4d/shadow.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/persist.hpp"
#include "machphys/preload.hpp"
#include "machlog/races.hpp"
#include "machlog/actor.hpp"
#include "machlog/network.hpp"
#include "machines/errorhnd.hpp"
#include "machines/leaktrak.hpp"
#include "render/colour.hpp"
#include "system/metafile.hpp"
#include "machgui/ctxoptns.hpp"
#include "machgui/gui.hpp"

void debugTiming( const char* text, bool startTiming )
{
	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( startTiming )
	{
		startTime = Phys::time();
	}
	else
	{
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_LIONEL, text << " , " << ( endTime - startTime ) << std::endl );
	}

}

SDLApp::SDLApp():
	winWidth_(400),
	winHeight_(300),
    pDisplay_( NULL ),
    pRoot_( NULL ),
	pKeyboardFocus_( NULL ),
    manager_( NULL ),
    showMemory_( false ),
	initialised_(false),
	pTextureSet_( NULL ),
	savedSampleVolume_( 0 ),
	savedCDVolume_( 0 )
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
	static string usage = "";
	usage += "Command line parameters are:\n";
	usage += "\t-?\tDisplay this help.\n";
	usage += "or\n";
	usage += "\t-f <xRes> <yRes>";
	usage += "or\n";
	usage += "\t-w <window width> <window height>\n";
	usage += "Where:\n";
	usage += "\t-w means run in window mode\n";
	usage += "\t-f means run in full-screen mode (default)\n";
	usage += "Leaving the command line blank will ask machines to default the resolution.";
	SysWindowsAPI::messageBox( usage.c_str(),"Machines" );
}

static  void    newHandler( void )
{
    WHERE_STREAM( "Aborting in new handler " << std::endl );
    ASSERT_FAIL( "Aborting in new handler" );
}

class ProgressIndicator : public BaseProgressReporter
{
public:
	ProgressIndicator( bool lobbyStart, int xOffset, int yOffset )
	: 	lobbyStart_( lobbyStart ),
		xOffset_( xOffset ),
		yOffset_( yOffset ),
		lowerLimit_( 0.0 ),
		upperLimit_( 1.0 )
	{}

    virtual size_t report( size_t done, size_t maxDone )
	{
		const double minx = 98 + xOffset_;
		const double maxx = 538 + xOffset_;
		const double miny = 362 + yOffset_;
		const double maxy = 366 + yOffset_;
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
		// For double buffering do it twice to prevent bar from blinking
		RenDevice::current()->frontSurface().filledRectangle( Ren::Rect(minx,miny,displayWidth,height), RenColour(red,green,blue) );
		if( lobbyStart_ )
		{
			//If the game session has come out prematurely then the network connection may have been reset.
			if( MachLogNetwork::instance().isNetworkGame() )
			{
				MachLogNetwork::instance().update();
				SysWindowsAPI::sleep( 0 );
				SysWindowsAPI::peekMessage();
				//network game _may_ have come out on update above.
/*				if( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().node().lastPingAllTime() > 1 )
				{
					MachLogNetwork::instance().node().pingAll();
				}*/
			}
		}
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
	bool	lobbyStart_;
	int 	xOffset_;
	int 	yOffset_;
};

bool SDLApp::clientStartup()
{
	double time = DevTime::instance().time();
	double loadPos = 0;

    //Hide the cursor
    DevMouse::instance().hide();

	// must be called before the display is created
	//and the CD volume registry is used
	#ifndef DEMO
	SysRegistry::instance().currentStubKey( "SOFTWARE\\Acclaim Entertainment\\Machines" );
	#else
	SysRegistry::instance().currentStubKey( "SOFTWARE\\Acclaim Entertainment\\Machines Demo" );
	#endif

	initSound();
	// Start to play a CD to give the poor user something to listen to whilst
	// the models load...
	if ( DevCD::instance().isAudioCDPresent() )
	{
		//This is not a particularly nice place to do this initialisation
		//but if we want the music to start playing at the right volume
		//this is where it's going to happen
		SysRegistry::KeyHandle handle;
		if(SysRegistry::instance().onlyOpenKey( "Options\\CD", &handle, SysRegistry::CURRENT_USER )==SysRegistry::SUCCESS)
		{
			size_t theVol = SysRegistry::instance().queryIntegerValue( "Options\\CD", "Volume", SysRegistry::CURRENT_USER);
			DevCD::instance().volume(theVol);
		}
		// Play loading music. It's in a different position depending upon which CD is in.
		if ( MachGui::machinesCDIsAvailable( 1 ) )
		{
 			DevCD::instance().play( 2 ); // Data 0, Menu Music 1, Loading Music 2
		}
		else if ( MachGui::machinesCDIsAvailable( 2 ) )
		{
			DevCD::instance().play( 4 ); // Data 0, Menu Music 1, Victory Music 2, Defeat Music 3, Loading Music 4
		}
 	}

#ifdef DEMO
    SysMetaFile::encryptionType( SysMetaFile::ENCRYPTION_2 );
#else
    SysMetaFile::encryptionType( SysMetaFile::ENCRYPTION_1 );
#endif

    //Initialise profiling
    double profileInterval = 50.0;
    char* pMs = getenv( "PROFILE_RATE" );
    if( pMs )
        profileInterval = atof( pMs );
    ProProfiler::instance().traceInterval( profileInterval / 1000.0 );

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

    std::set_new_handler( newHandler );

    //Set the diretory to look for all files
    SysPathName::rootEnvironmentVariable( "MACH_ROOT" );

    MachPhysData::instance();

	bool lobbyFlag = false;

	for( size_t i = 0; i < invokeArgs().size(); ++i )
	{
		const string& token = invokeArgs()[i];

		if( token == "-lobby" )
			lobbyFlag = true;
		else if( token == "-?" )
		{
//			usage();
//			return false;
		}
	}

    //Check other debugging environment variables
    bool noPreload = getenv("CB_NOPRELOAD" ) != NULL;

	bool noTexturePreload = getenv( "TEX_NOPRELOAD" ) != NULL;

    bool planetsPreload = getenv("CB_PLANETS_PRELOAD" ) != NULL;

	// We must pick a co-orindate system: left-handed for Machines.
	MexCoordSystem::instance().set(MexCoordSystem::LEFT_HANDED);

	// This must be called before any render library object is created.  (It
	// ensures a correct destruction order for render library Singletons.)
	Ren::initialise();

	W4dRoot* root = pRoot_ = _NEW(W4dRoot( W4dRoot::W4dRootId() ));

	pDisplay_ = _NEW(RenDisplay(window()));

    ErrorHandler::instance().pDisplay( pDisplay_ );

	// Initially, pick the lowest-res 16-bit mode.
	pDisplay_->buildDisplayModesList();

	// Check for windowed mode
	if ( ! SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Windowed", SysRegistry::CURRENT_USER ) )
        pDisplay_->useFullScreen();

    // Check for first time run - set config to desktop resolution then
    if( !SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Width", SysRegistry::CURRENT_USER ) )
    {
        const RenDisplay::Mode desktopMode = pDisplay_->getDesktopDisplayMode();
        SysRegistry::instance().setIntegerValue( "Screen Resolution", "Width", desktopMode.width(), SysRegistry::CURRENT_USER );
        SysRegistry::instance().setIntegerValue( "Screen Resolution", "Height", desktopMode.height(), SysRegistry::CURRENT_USER );
        SysRegistry::instance().setIntegerValue( "Screen Resolution", "Refresh Rate", desktopMode.refreshRate(), SysRegistry::CURRENT_USER );
    }

	//Set default resolution.
    int modeW = 640;
    int modeH = 480;
    int modeR = 0;

	if ( SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Lock Resolution", SysRegistry::CURRENT_USER ) )
	{
		modeW = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Width", SysRegistry::CURRENT_USER );
		modeH = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Height", SysRegistry::CURRENT_USER );

		if(pDisplay_->fullScreen())
            modeR = SysRegistry::instance().queryIntegerValue( "Screen Resolution", "Refresh Rate", SysRegistry::CURRENT_USER );
	}

	std::cout << "Trying to select display mode " << modeW << "x" << modeH << " at " << modeR << " Hz" << std::endl;
	if( not pDisplay_->useMode(modeW, modeH, modeR))
	{
		if ( modeW != 640 or modeH != 480 )
		{
			modeW = 640;
			modeH = 480;

			if( not pDisplay_->useMode(modeW, modeH, modeR))
			{
				SysWindowsAPI::messageBox( "Failed to select minimum resolution (640x480x32bit colour)","Machines" );
				return false;
			}
		}
		else
		{
			SysWindowsAPI::messageBox( "Failed to select minimum resolution (640x480x32bit colour)","Machines" );
			return false;
		}
	}
	// tell the display what is the lowest resolution mode it is allowed to use
	pDisplay_->lowestAllowedMode( 640, 480, 16 );

	// if current mode uses too much memory, switch to a mode fitting in video memory
//	bool modeSet = pDisplay_->setHighestAllowedMode();
//	ASSERT(modeSet, "Could not find a mode fitting in video memory");

    // Ask the mouse to give us coordinates scale to the current Direct3D
    // resolution (which doesn't necessarilly match the Windows resolution).
	const RenDisplay::Mode& mode = pDisplay_->currentMode();
    DevMouse::instance().scaleCoordinates(mode.width(), mode.height());

	manager_ = _NEW(W4dSceneManager(pDisplay_, root));
	manager_->pDevice()->debugTextCoords(204, 0);
	manager_->useLevelOfDetail(
        !SysRegistry::instance().queryIntegerValue( "Options\\Graphics Complexity\\LOD", "Value", SysRegistry::CURRENT_USER ));

	// set this after the device has been created: we need the capability class
	// to find out how much memory is available for display
//	bool highestModeSet = manager_->pDevice()->setHighestAllowedDisplayMode();
//	ASSERT(highestModeSet, "Could tot find a mode fitting in the amount of display memory available");

    // Register the scene manager with the render libary manager
	HAL_STREAM("D3dApp::clientStartup sceneManager\n" );
    W4dManager::instance().sceneManager( manager_ );

	// Restore users gamma correction setting
	if ( pDisplay_->supportsGammaCorrection() )
	{
		SysRegistry::KeyHandle handle;
		if(SysRegistry::instance().onlyOpenKey( "Options\\Gamma Correction", &handle, SysRegistry::CURRENT_USER )==SysRegistry::SUCCESS)
		{
			double gammaValue = SysRegistry::instance().queryIntegerValue( "Options\\Gamma Correction", "Value", SysRegistry::CURRENT_USER );
			gammaValue /= GAMMA_REG_MULTIPLIER;
			gammaValue = mexClamp( gammaValue, GAMMA_LOWER_LIMIT, GAMMA_UPPER_LIMIT );
			pDisplay_->gammaCorrection( gammaValue );
		}
	}

	// Get top left offset for images
	int xOffset = ( mode.width() - 640 ) / 2;
	int yOffset = ( mode.height() - 480 ) / 2;

	//moved the sequence of calls slightly - the init sound call must still be done first though
//    initSound();

	if( lobbyFlag )
	{
		// Display a loading screen.
		RenSurface frontBuffer = manager_->pDevice()->frontSurface();
		frontBuffer.filledRectangle( Ren::Rect(0,0,mode.width(),mode.height()), RenColour::black() );
		RenSurface waitBmp = RenSurface::createSharedSurface( "gui/menu/waitlobb.bmp", frontBuffer );
		frontBuffer.simpleBlit( waitBmp, xOffset, yOffset );

		// Initialise lobby code
		NetNetwork::instance().instantiateLobby();
		MachLogNetwork::instance().launchFromLobbyInfo();

		// Display progress loading screen.
		// Call it twice to ensure on both buffers.
		RenSurface waitBmp2 = RenSurface::createSharedSurface( "gui/menu/wait.bmp", frontBuffer );
		frontBuffer.simpleBlit( waitBmp2, xOffset, yOffset );
	}
	else
	{
		// Display a loading screen.
		RenSurface frontBuffer = manager_->pDevice()->frontSurface();
		frontBuffer.filledRectangle( Ren::Rect(0,0,mode.width(),mode.height()), RenColour::black() );
		RenSurface waitBmp = RenSurface::createSharedSurface( "gui/menu/wait.bmp", frontBuffer );
		frontBuffer.simpleBlit( waitBmp, xOffset, yOffset );
		// Call it twice to draw on both front and back buffers
		manager_->pDevice()->display()->flipBuffers();
		frontBuffer.simpleBlit( waitBmp, xOffset, yOffset );
	}

	if( lobbyFlag )
	{
		// Display progress loading screen.
		RenSurface frontBuffer = manager_->pDevice()->frontSurface();
		frontBuffer.filledRectangle( Ren::Rect(0,0,mode.width(),mode.height()), RenColour::black() );
		RenSurface waitBmp = RenSurface::createSharedSurface( "gui/menu/wait.bmp", frontBuffer );
		frontBuffer.simpleBlit( waitBmp, xOffset, yOffset );
		// Call it twice to draw on both front and back buffers
		manager_->pDevice()->display()->flipBuffers();
		frontBuffer.simpleBlit( waitBmp, xOffset, yOffset );
	}
    // Draw copyright note, store it in a way preventing from modification
	{
        const int chars[] = {'N'+255,'O'+260,'T'+265,' '+270,'f'+275,'o'+280,'r'+285,' '+290,'s'+295,'a'+300,'l'+305,'e'+310,','+315,
                            ' '+320,'f'+325,'o'+330,'r'+335,' '+340,'t'+345,'e'+350,'s'+355,'t'+360,'i'+365,'n'+370,'g'+375,' '+380,
                        'p'+385,'u'+390,'r'+395,'p'+400,'o'+405,'s'+410,'e'+415,'s'+420,' '+425,'o'+430,'n'+435,'l'+440,'y'+445,'.'+450 };
        std::string note; note.reserve(sizeof(chars)/sizeof(int));
        for(int i = 0; i < sizeof(chars)/sizeof(int); ++i) note.push_back(chars[i] - (255 + i*5) );
        //std::string note("NOT for sale, for testing purposes only.");
        RenSurface frontBuffer = manager_->pDevice()->frontSurface();
        frontBuffer.drawText(xOffset + 6, yOffset + 6, note, RenColour::yellow());
        // Call it twice to draw on both front and back buffers
        manager_->pDevice()->display()->flipBuffers();
        frontBuffer.drawText(xOffset + 6, yOffset + 6, note, RenColour::yellow());
    }

	ProgressIndicator progressIndicator( lobbyFlag, xOffset, yOffset );
	progressIndicator.setLimits( 0.00, 0.18 );

	//moved higher due to call sequence problems in lobbying when very slow machines host with very fast machines
	//in the game.
	HAL_STREAM("D3dApp::clientStartup initialiseGui\n" );
	StartedFromLobby startedType = NORMAL_START;
	if( lobbyFlag )
		startedType = LOBBY_START;
    initialiseGui( startedType, &progressIndicator );

	progressIndicator.report( 100, 100 );
	progressIndicator.setLimits( 0.18, 0.26 );

    MachPhysPreload::registerSounds();
	loadSounds();

	progressIndicator.report( 100, 100 );
	progressIndicator.setLimits( 0.26, 0.28 );

    // Set up the texture search path.
    RenTexManager::PathNames    searchList = RenTexManager::instance().searchList();

	// We set different search paths depending on the amount
	// of video memory available for texture
//	bool doLoad2MBytesTexture = not manager_->pDevice()->capabilities().supports4MBytesTextureSet();
	bool doLoad2MBytesTexture = false;
	// Check to see if texture4 directory even exists. The user may have done a minimum
	// install in which case texture4 directory will not be available
	if ( not doLoad2MBytesTexture )
	{
		SysPathName texture4Dir( "models/texture4" );
		if ( not texture4Dir.existsAsDirectory() )
		{
			doLoad2MBytesTexture = true;
		}
	}

	progressIndicator.report( 100, 100 );
	progressIndicator.setLimits( 0.28, 0.71 );

	if( noTexturePreload /*or lobbyFlag*/ )
	{

		// set path search list for run-time loading of the textures
	    LIONEL_STREAM("setting up search path for run-time models texture loading  " << std::endl);
	  	if( doLoad2MBytesTexture )
	  	{
	         searchList.push_back( SysPathName( "models/texture2" ) );
	         searchList.push_back( SysPathName( "models/texture2/exp" ) );
	         searchList.push_back( SysPathName( "models/texture2/fire" ) );
	         searchList.push_back( SysPathName( "models/texture2/smoke" ) );
	         RenTexManager::instance().searchList( searchList );
	  	}
	  	else
	  	{
	         searchList.push_back( SysPathName( "models/texture4" ) );
	         searchList.push_back( SysPathName( "models/texture4/exp" ) );
	         searchList.push_back( SysPathName( "models/texture4/fire" ) );
	         searchList.push_back( SysPathName( "models/texture4/smoke" ) );
	         RenTexManager::instance().searchList( searchList );
	    }
	    LIONEL_STREAM("searchList  " << searchList);
	    LIONEL_STREAM("done setting up search path for run-time models texture loading  " << std::endl);
	}
	else // do texture preload
	{
	    LIONEL_STREAM("preloading model and weapons texture bitmaps " << std::endl);
		if( doLoad2MBytesTexture )
		{
    		pTextureSet_ = _NEW( RenTextureSet( "models/texture2", &progressIndicator ) );
			// planet surface is going to use the search list as a set of flags
			// to decide 1) whether it should preload or not 2) whether it should
			// use texture2 or texture4... Isn't that pretty ?
        	searchList.push_back( SysPathName( "preload/texture2" ) );
	        RenTexManager::instance().searchList( searchList );
		}
		else
		{
       		pTextureSet_ = _NEW( RenTextureSet( "models/texture4", &progressIndicator ) );
			// planet surface is going to use the search list as a set of flags
			// to decide 1) whether it should preload or not 2) whether it should
			// use texture2 or texture4... Isn't that pretty ?
        	searchList.push_back( SysPathName( "preload/texture4" ) );
	        RenTexManager::instance().searchList( searchList );
    	}
	    LIONEL_STREAM("done preloading model and weapons texture bitmaps " << std::endl);
	}

	progressIndicator.setLimits( 0.71, 0.96 );

    //Preload the models if required
    if( not noPreload )
    {
        if( MachPhysPreload::persistentFileName().existsAsFile() )
        {
            /*if( not lobbyFlag )*/
			{
	            DevTimer    t1;
	            DevTimer    t2;
	            MachPhysPreload::persistentPreload( MachPhysPreload::ECHO_PROGRESS, &progressIndicator );
	            std::cout << "Time for persistent preload " << t1.time() << std::endl;

	            t1.time( 0.0 );
	            MachPhysPreload::preloadWeaponsAndEffects(MachPhysPreload::ECHO_PROGRESS );
	            MachPhysPreload::extrasPreload(MachPhysPreload::ECHO_PROGRESS );
	            std::cout << "Time for post-persistent preload " << t1.time() << std::endl;
	            std::cout << "Time for total preload " << t2.time() << std::endl;
			}
//			else
//			{
//				//register MDCs.
//				MachPhysPersistence::instance();
//			}
        }
        else
        {
            DevTimer    t1;
            MachPhysPreload::swPreload( MachPhysPreload::ECHO_PROGRESS );
            std::cout << "Time for total preload " << t1.time() << std::endl;
            t1.time( 0.0 );
            MachPhysPreload::persistentSave( MachPhysPreload::ECHO_PROGRESS );
            std::cout << "Time for persistent save " << t1.time() << std::endl;
		}
	}
	else
	{
		//if no preload then call the instance to ensure that sub classes have all been registered.
		MachPhysPersistence::instance();
	}

	if( planetsPreload )
		loadPlanets();

    // After loading, dump out the list of all the textures.
	// This helps determine exactly how much texture memory is used where.
    RENDER_STREAM(RenTexManager::instance());

    manager_->hideStats();

    W4dShadowProjected2d::shadowPlaneZ(0.025);

	HAL_STREAM("D3dApp::clientStartup checkSuspend SimManager\n" );

	checkSuspendSimManager();

	progressIndicator.setLimits( 0.0, 1.0 );
	progressIndicator.report( 100, 100 ); // 100& done

    initialised_ = true;
    finishTimer_.time( 0 );

	HAL_STREAM("D3dApp::clientStartup simResetTimer\n" );
	simResetTime();
	HAL_STREAM("D3dApp::clientStartup done\n" );

	return true;
}

void SDLApp::clientShutdown()
{
	// Move the window completely off-screen before reseting from fullscreen
	// mode.  Smacker appears to use the window as temporary space for
	// decompression.  Thus when we exit, it is partly covered in garbage.
	// This is the easiest way to hide it.  We can't permanently hide it
	// during normal execution because it screws up the mouse co-ordinates./
//	moveWindow(10000, 10000);

	cleanUpGui();

    DevMouse::instance().unhide();
	_DELETE( pRoot_ );
    _DELETE( manager_ );
	// Tell W4dManager about destruction of sceneManager
	W4dManager::instance().clearSceneManager();
    _DELETE( pDisplay_ );
	_DELETE( pTextureSet_ );
    cleanUpSound();
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
bool SDLApp::activate()
{
	if (!manager_)
		return false;

	if (manager_->pDevice())
		manager_->pDevice()->activate();

	activateGui();

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
		const string flag = invokeArgs()[0];
		if (flag == "-w" && invokeArgs().size() == 3)
		{
			winWidth_  = atoi(invokeArgs()[1].c_str());
			winHeight_ = atoi(invokeArgs()[2].c_str());
		}
	}

	w = winWidth_;
	h = winHeight_;
}

// virtual
const string& SDLApp::name() const
{
	static string n = "Machines";
	return n;
}

/* End D3DAPP.CPP ***************************************************/
