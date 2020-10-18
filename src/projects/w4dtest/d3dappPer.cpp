#include "phys/rampacce.hpp"
#include "w4dtest/d3dapp.hpp"

#include <stdio.h>
#include <math.h>
//#include <env.h>					// for setenv
#include <limits.h>					// for UINT_MAX

#include "system/pathname.hpp"
#include "device/time.hpp"
#include "device/timer.hpp"
#include "device/mouse.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/quatern.hpp"
#include "mathex/coordsys.hpp"

#include "render/display.hpp"
#include "render/device.hpp"
#include "render/texmgr.hpp"
#include "render/mesh.hpp"
#include "render/stats.hpp"
#include "render/colour.hpp"

#include "phys/timeangl.hpp"
#include "phys/linetrav.hpp"
#include "phys/mcfly.hpp"
#include "phys/mcground.hpp"

#include "world4d/root.hpp"
#include "world4d/generic.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/light.hpp"
#include "world4d/camera.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "world4d/backgrnd.hpp"
#include "world4d/shadow.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/uvtrans.hpp"
#include "world4d/mcentity.hpp"
#include "world4d/persist.hpp"

#include "utility/time.hpp"
#include "utility/linetok.hpp"

#include "afx/castfns.hpp"

#include "machphys/persist.hpp"
        #include "machphys/preload.hpp"
        #include "base/istrrep.hpp"
        #include "base/diag.hpp"
        #include "machphys/mphydata.hpp"
        #include "system/metafile.hpp"
        #include "world4d/soundman.hpp"
        #include "sound/sndparam.hpp"
        #include "sound/soundmix.hpp"

D3DApp::D3DApp():
	device_(NULL),
	manager_(NULL),
	root_(NULL),
	light_(NULL),
	dummyLight_(NULL),
	camera_(NULL),
    pGroundCamera_( NULL ),
	eyeControl_(NULL),
    pGroundControl_( NULL ),
	lightControl_(NULL),
	controledModel_(0),
	doFog_(false),
	nvgOn_(false),
	showInfo_(false),
	initialised_(false),
    displayHelp_( false ),
    persistence_( this )
{
}

D3DApp::~D3DApp()
{
}


static void usage()
{
	std::cout << "Command line parameters are:" << std::endl;
	std::cout << "     -w|-f <mesh file name> <screen width> <screen height> [<bitmap name>]" << std::endl;
	std::cout << "Where:" << std::endl;
	std::cout << "     -w means run in window mode (the default)" << std::endl;
	std::cout << "     -f means run in full-screen mode" << std::endl;
	std::cout << std::endl;
}

void D3DApp::pickDisplayMode(bool windowMode, int width, int height)
{
	display_ = _NEW(RenDisplay(window()));

	if (!windowMode)
	{
		// Initially, pick the lowest-res 16-bit mode.
		display_->useFullScreen();

		std::cout << "Trying to select display mode " << width << "x" << height << "x" << 16 << std::endl;
		if (!display_->useMode(width, height, 16))
		{
			std::cout << "Failed to select that mode -- the nearest alternative will be chosen." << std::endl;
			display_->useNearestMode(width * height, 16);
		}

		// If there are no 16-bit modes, then mode should default to the
		// first mode in the list, regardless of depth.
		std::cout << "Initially, setting mode: " << display_->currentMode() << std::endl;
	}

	std::cout << *display_ << std::endl;
}


bool D3DApp::clientStartup()
{
    W4dPersistence::instance();
	queueEvents();

    SysPathName::rootEnvironmentVariable( "WORLD4D_ROOT" );

    if (invokeArgs().size() != 4 and invokeArgs().size() != 5)
    {
        usage();
        return false;
    }

	bool windowMode = true;

	const string flag = invokeArgs()[0];
	if (flag == "-w")
		windowMode = true;
	else if (flag == "-f")
		windowMode = false;

	SysPathName meshListFile = invokeArgs()[1];

	const int modeW = atoi(invokeArgs()[2].c_str());
	const int modeH = atoi(invokeArgs()[3].c_str());

	SysMetaFile::encryptionType(SysMetaFile::ENCRYPTION_1);

	// We must pick a co-orindate system: left- or right-handed.
	MexCoordSystem::instance().setFromEnvironment();

	// This must be called before any render library object is created.  (It
	// ensures a correct destruction order for render library Singletons.)
	Ren::initialise();

    //  This is necessary to get around the persistence reading
    //  in code. We need a root before we start the persistence,
    //  however the persistence will also set up its own root.

    //W4dRoot*    pRoot = _NEW(W4dRoot( W4dRoot::W4dRootId() ));
	//root_  = pRoot;

    RenTexManager::PathNames    searchList = RenTexManager::instance().searchList();

    searchList.push_back( SysPathName( "models/texture4" ) );
    searchList.push_back( SysPathName( "models/texture4/exp" ) );
    searchList.push_back( SysPathName( "models/texture4/fire" ) );
    searchList.push_back( SysPathName( "models/texture4/smoke" ) );

    RenTexManager::instance().searchList( searchList );

    W4dRoot*    pRoot = _NEW(W4dRoot( W4dRoot::W4dRootId() ));
	root_  = pRoot;

	pickDisplayMode(windowMode, modeW, modeH);
	manager_ = _NEW(W4dSceneManager(display_, root_));
	device_  = manager_->pDevice();
    W4dManager::instance().sceneManager( manager_ );

        SndMixerParameters soundParams
    (
    	window(), // Where this is the HWND of your application window
     	Snd::Polyphony(5), // The Mixers polyphony
    	SndMixerParameters::DIRECTSOUND,// You must be using DIRECTSOUND
    	Snd::ELEVEN_THOUSAND_HZ, // The playback sample rate
    	Snd::SIXTEEN_BIT,// The playback "bit-ness" ?!
    	Snd::STEREO,// Speaker setup
    	Snd::THREE_D,// THREE_D or TWO_D
    	30 // Number of samples that can exist at one time, an assertion will occur if this limit is overstepped.
    );

    SndMixer::initialise(soundParams);


    MachPhysData::instance();
    MachPhysPreload::registerSounds();
    W4dSoundManager::instance().currentSoundDimensions(Snd::TWO_D);
    W4dSoundManager::instance().readSoundDefinitionFile("sounds/sd2d64.dat");

	PER_LOG_READ( true );
	PER_PRIVATE_LOG_READ(true);
	PER_PRIVATE_LOG_ADDRESSES(true);

    {
        //TEST
        //DiagInternal::enableStream(DiagStreamType::DIAG_PER_READ);
        //Diag::instance().perReadStream().rdbuf(std::cout.rdbuf());
        //Persistence::instance().debugReadStream().rdbuf(std::cout.rdbuf());

        //DiagInternal::stream().rdbuf();
        class MyReporter : public PerIstreamReporter
        {
            public:
            MyReporter(){};
            ~MyReporter(){};
            size_t report( size_t bytesRead, size_t fileSize )
            {
                return 0;
            }
        };

        MyReporter reporter;
        if( MachPhysPreload::persistentFileName().existsAsFile() )
        {
            std::cout << "Preload start" << std::endl;
            MachPhysPreload::persistentPreload( MachPhysPreload::QUIET_PROGRESS, &reporter );
            std::cout << "Preload weapons and effects" << std::endl;
            MachPhysPreload::preloadWeaponsAndEffects(MachPhysPreload::QUIET_PROGRESS );
            std::cout << "Preload extras" << std::endl;
            MachPhysPreload::extrasPreload(MachPhysPreload::QUIET_PROGRESS );
            //MachPhysPreload::persistentSave( MachPhysPreload::QUIET_PROGRESS );
            std::cout << "Stuff loaded!" << std::endl;
        }
        else
        {
            std::cout << "Unable to open persistence file!" << std::endl;
        }
        //TEST



        //std::ofstream fstr( "models.bin", std::ios::binary );
        //PerOstream  ostr( fstr );
        //ostr << persistence_;
        //MachPhysPreload::persistentSave( MachPhysPreload::QUIET_PROGRESS );
    }
}

// Update the display.
void D3DApp::loopCycle()
{

}

// virtual
bool D3DApp::activate(WORD /*wParam*/)
{
	return true;
}

// virtual
void D3DApp::updateDisplay()
{
}

// virtual
void D3DApp::getWindowSize(int&, int&, int& w, int& h)
{
	w = 400;
	h = 300;
}

// virtual
const string& D3DApp::name() const
{
	static string n = "Charybdis 3D viewer";
	return n;
}

void D3DApp::clientShutdown()
{}

D3DAppPersistence::D3DAppPersistence(D3DApp*)
{

}
