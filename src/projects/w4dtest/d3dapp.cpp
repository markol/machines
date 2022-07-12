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


PER_DEFINE_PERSISTENT( ModelData );
PER_DEFINE_PERSISTENT( D3DAppPersistence );

void persistenceImplementationReadWriteTimingData( ostream& ostr );

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

bool D3DApp::readModels(const SysPathName& pathname, MexTransform3d* cameraXform)
{
    if( not pathname.existsAsFile() )
	{
		std::cout << "Couldn't read file " << pathname << std::endl;
		return false;
	}

    UtlLineTokeniser    parser( pathname );

	while( not parser.finished() )
	{
        const UtlLineTokeniser::Tokens&    tokens = parser.tokens();

		if (tokens[0] == "camera" && cameraXform)
		{
			// Expect x,y,z position and a heading in degrees.
			MATHEX_SCALAR x,y,z, h;
            x = atof( tokens[1].c_str() );
            y = atof( tokens[2].c_str() );
            z = atof( tokens[3].c_str() );
            h = atof( tokens[4].c_str() );

			MexDegrees heading(h);
			MexQuaternion quat(MexVec3(0,0,1), heading);

			cameraXform->position(MexPoint3d(x,y,z));
			cameraXform->rotation(quat);
		}
		else
		{
			SysPathName meshFile = tokens[ 0 ];

			if( meshFile.existsAsFile() )
			{
	            size_t  positionIndex = 1;
				// Some file types require a model name. Skip over this to find the position and scale data
	       		if (!meshFile.hasExtension() || meshFile.extension() == "x" || meshFile.extension() == "agt")
	                positionIndex = 2;

				MATHEX_SCALAR x,y,z, scale;

	            x = atof( tokens[ positionIndex ].c_str() );
	            y = atof( tokens[ positionIndex + 1 ].c_str() );
	            z = atof( tokens[ positionIndex + 2 ].c_str() );
	            scale = atof( tokens[ positionIndex + 3 ].c_str() );

				MexTransform3d xform(MexPoint3d(x,y,z));
				ModelData* model = _NEW(ModelData);

				if (!meshFile.hasExtension() || meshFile.extension() == "x" || meshFile.extension() == "agt")
				{
	    			string modelName = tokens[ 1 ];

					ASSERT(modelName.length() != 0, "No model name specified");
					W4dGeneric* gen = _NEW(W4dGeneric(root_, xform));
					gen->loadSingleMesh(meshFile, modelName, scale);
					model->entity_ = gen;
					model->composite_ = NULL;
				}
				else if (meshFile.extension() == "lod")
				{
					W4dGeneric* gen = _NEW(W4dGeneric(root_, xform));
					gen->loadLODFile(meshFile);
					model->entity_ = gen;
					model->composite_ = NULL;
				}
				else if (meshFile.extension() == "cdf")
				{
					model->composite_ = _NEW(W4dGenericComposite(root_, xform, meshFile ));
					model->entity_ = model->composite_;
					model->entity_->name(meshFile.filename());
				}
				else if (meshFile.extension() == "odf")
				{
					model->composite_ = _NEW(W4dGenericComposite(root_, xform, meshFile ));
					model->entity_ = model->composite_;
					model->entity_->name(meshFile.filename());
				}

				model->control_ = _NEW( PhysFlyControl( _NEW(W4dMotionControlledEntity(model->entity_)), MexVec2(0,-1) ) );
				model->control_->initEventQueue();
				model->control_->degreesPerSecond(8);
				model->control_->metresPerSecond(0.6);
				model->control_->setDefaultPosition(xform);
				model->control_->resetPosition();
				model->control_->useKeypadMapping();

				models_.push_back(model);
			}
		}

        parser.parseNextLine();
	}

	POST(models_.size() >= 1);
	return true;
}

// Read the environment parameters from a file.  These are: sky colour; light
// intensity; light direction; and ambient light level.
void D3DApp::readEnvironment(const SysPathName& planetName)
{
	PRE(device_);
	PRE(light_);

	SysPathName withExt = planetName;
	withExt.extension("env");
	std::ifstream is(withExt.pathname().c_str());

	// If there is no file, pick sensible default values for everything.
	MexDegrees lightZ=225, lightY=-47;
	RenColour skyCol(0.29, 0.51, 0.87), ambient(0.3), lightCol(0.7);

	if (is)
	{
		string key;

		istream& tmpStream = is;
		tmpStream >> key;
		ASSERT(key == "ambient:", "Failed to find \"ambient:\" keyword in environment file.");
		is >> ambient;

		tmpStream >> key;
		ASSERT(key == "light_y:", "Failed to find \"light_y:\" keyword in environment file.");
		MATHEX_SCALAR tmp1;
		is >> tmp1;
		lightY = tmp1;

		tmpStream >> key;
		ASSERT(key == "light_z:", "Failed to find \"light_z:\" keyword in environment file.");
		MATHEX_SCALAR tmp2;
		is >> tmp2;
		lightZ = tmp2;

		tmpStream >> key;
		ASSERT(key == "light_colour:", "Failed to find \"light_colour:\" keyword in environment file.");
		is >> lightCol;

		tmpStream >> key;
		ASSERT(key == "sky_colour:", "Failed to find \"sky_colour:\" keyword in environment file.");
		is >> skyCol;


		tmpStream >> key;
		ASSERT(key == "fog:", "Failed to find \"fog:\" keyword in environment file.");
		bool fogOn;
		is >> fogOn;

		if (fogOn)
		{
			tmpStream >> key;
			ASSERT(key == "fog_start:", "Failed to find \"fog_start:\" keyword in environment file.");
			is >> fogStart_;

			tmpStream >> key;
			ASSERT(key == "fog_end:", "Failed to find \"fog_end:\" keyword in environment file.");
			is >> fogEnd_;

			tmpStream >> key;
			ASSERT(key == "fog_density:", "Failed to find \"fog_density:\" keyword in environment file.");
			is >> fogDensity_;

			tmpStream >> key;
			ASSERT(key == "fog_colour:", "Failed to find \"fog_colour:\" keyword in environment file.");
			RenColour fogCol;
			is >> fogCol;

			device_->fogOn(fogStart_, fogEnd_, fogDensity_);
			device_->fogColour(fogCol);
		}
	}

	const MexEulerAngles lightAngles(lightZ, lightY, 0);
	MexTransform3d lightXform(lightAngles);
    MexVec3 dvec = lightXform.xBasis();
    dvec *= -1.0;
    MexPoint3d tempPoint( dvec );
    MexTransform3d newTransform( tempPoint );
	light_->localTransform(newTransform);
	light_->colour(lightCol);

    device_->backgroundColour(skyCol);
	device_->ambient(ambient);
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

bool D3DApp::readBackground
(
	const SysPathName& pathname,
	W4dEntity*  root,			// the root for unanimated objects
	W4dEntity** primaryLight,	// an object representing the main light source
	W4dEntity*  cloudRoot,		// root for objects animated as clouds
	W4dEntity*  celestialRoot,	// root for objects animated as suns, moons, etc.
	MATHEX_SCALAR* cloudSpeed,	// max translation speed of clouds per sec
	MexDegrees* rotSpeed		// max rotation speed of celestials per sec
)
{
	PRE(root && cloudRoot && celestialRoot);
	PRE(primaryLight && !(*primaryLight));

	std::ifstream ifs(pathname.pathname().c_str());
	std::istream& is = ifs;

	if (!is)
	{
		std::cout << "Couldn't read file " << pathname << std::endl;
		return false;
	}

	// Initially, new objects are added to the unanimated root.
	W4dEntity* addTo = root;

	bool readingSun = false;
	while (is)
	{
		string tmp;
		is >> tmp;

		if (tmp == "clouds")
		{
			addTo = cloudRoot;
			is >> *cloudSpeed;
		}
		else if (tmp == "celestial")
		{
			addTo = celestialRoot;
			MATHEX_SCALAR degrees;
			is >> degrees;
			*rotSpeed = degrees;
		}
		else if (tmp == "sun")
		{
			readingSun = true;
		}
		else if (tmp.length() > 0)
		{
			// If the string isn't either of those keywords, treat it as a filename.
			SysPathName meshFile = tmp;

			if (!meshFile.existsAsFile())
			{
				std::cout << "Couldn't read file " << meshFile << std::endl;
				return false;
			}
			else
			{
				// Some file types require a model name.
				string modelName;
				if (!meshFile.hasExtension() || meshFile.extension() == "x")
					is >> modelName;

				MATHEX_SCALAR x,y,z, az, el, scale;
				is >> x >> y >> z >> az >> el >> scale;

				MexEulerAngles angles(MexDegrees(az), MexDegrees(el), 0);
				MexTransform3d xform(angles, MexPoint3d(x,y,z));

				W4dGeneric* gen = NULL;
				if (!meshFile.hasExtension() || meshFile.extension() == "x")
				{
					ASSERT(modelName.length() != 0, "No model name specified");
					gen = _NEW(W4dGeneric(addTo, xform));
					gen->loadSingleMesh(meshFile, modelName, scale);
				}
				else if (meshFile.extension() == "lod")
				{
					gen = _NEW(W4dGeneric(addTo, xform));
					gen->loadLODFile(meshFile);
				}
				else
				{
					std::cout << "Unsupported background extension " << meshFile.extension() << std::endl;
					return false;
				}

				ASSERT(gen, logic_error("Failed to read background object"));

				// If the sun keyword is encountered, assign the current object
				// to the primary light pointer and then cancel the keyword.
				if (readingSun)
				{
					if (*primaryLight)
					{
						std::cout << "Encountered two \"sun\" keywords in background file." << std::endl;
						return false;
					}

					*primaryLight  = gen;
					readingSun = false;
				}
			}
		}
	}

	return true;
}

bool D3DApp::createBackground()
{
	// Create a separate W4d tree for the background sky items.
	W4dRoot*    bgRoot        = _NEW(W4dRoot(W4dRoot::W4dRootId()));
	W4dGeneric* domeRoot      = _NEW(W4dGeneric(bgRoot,  MexTransform3d()));
	W4dGeneric* celestialRoot = _NEW(W4dGeneric(bgRoot,  MexTransform3d()));
	W4dGeneric* cloudRoot     = _NEW(W4dGeneric(bgRoot,  MexTransform3d()));
	sun_ = NULL;

	MATHEX_SCALAR cloudSpeed;
	MexDegrees    rotSpeed;
	if (!::W4dReadBackground("dales.bg", domeRoot, &sun_, cloudRoot, celestialRoot, &cloudSpeed, &rotSpeed))
	{
		_DELETE(bgRoot);
		return false;
	}

	manager_->useBackground(bgRoot);

	// Set up motion plans for the clouds and celestial bodies.  The celestials
	// rotate around a slightly off-vertical axis.
	PhysMotionPlan::AnglesPtr angles = _NEW(PhysMotionPlan::Angles);
	angles->push_back(MexDegrees(0));
	angles->push_back(MexDegrees(120));
	angles->push_back(MexDegrees(240));
	angles->push_back(MexDegrees(360));

	PhysMotionPlan::TimesPtr times = _NEW(PhysMotionPlan::Times);
	times->push_back(120.0 / rotSpeed.asScalar());
	times->push_back(240.0 / rotSpeed.asScalar());
	times->push_back(360.0 / rotSpeed.asScalar());

	MexVec3 axis(0, 0.2, 0.98);
	PhysMotionPlanPtr sunPlan = _NEW(PhysTimedAnglePlan(angles, times, axis, MexPoint3d()));

	celestialRoot->entityPlanForEdit().absoluteMotion(sunPlan, 0, 1000000);

	// These positions define an octagonal path for the clouds.
	PhysMotionPlan::TransformsPtr xforms = _NEW(PhysMotionPlan::Transforms);
	xforms->push_back(MexPoint3d(-100, -100, 0));
	xforms->push_back(MexPoint3d(-141,    0, 0));
	xforms->push_back(MexPoint3d(-100,  100, 0));
	xforms->push_back(MexPoint3d(   0,  141, 0));
	xforms->push_back(MexPoint3d( 100,  100, 0));
	xforms->push_back(MexPoint3d( 141,    0, 0));
	xforms->push_back(MexPoint3d( 100, -100, 0));
	xforms->push_back(MexPoint3d(   0, -141, 0));
	xforms->push_back(MexPoint3d(-100, -100, 0));

	// With the acceleration is will take the clouds 7 seconds to arrive at
	// their top speed.  The speed is expected to be quite slow, relative to
	// the path.  (Each path segment is 108 meters long.)
	MATHEX_SCALAR cloudAcel = cloudSpeed / 7;
	PhysMotionPlanPtr cldPlan = _NEW(PhysLinearTravelPlan(xforms, 0, cloudSpeed, cloudAcel,
														  cloudAcel, 1, 1, 1, 1, true));
	cloudRoot->entityPlanForEdit().absoluteMotion(cldPlan, 0, 1000000);

	return true;
}

bool D3DApp::clientStartup()
{
    W4dPersistence::instance();

	// The debug timer doesn't work if this environment var isn't set.
	//setenv("CHARYBDIS", "x", false);
	putenv("CHARYBDIS");
	UtlDebugTimer::startCalibration();
	gameTimer_.time(0);
	queueEvents();

    SysPathName::rootEnvironmentVariable( "WORLD4D_ROOT" );

    if (invokeArgs().size() != 4 and invokeArgs().size() != 5)
    {
        usage();
        return false;
    }

    bool    usePersistence = false;

    if (invokeArgs().size() == 5 )
        usePersistence = true;

	bool windowMode = true;

	const string flag = invokeArgs()[0];
	if (flag == "-w")
		windowMode = true;
	else if (flag == "-f")
		windowMode = false;

	SysPathName meshListFile = invokeArgs()[1];

	const int modeW = atoi(invokeArgs()[2].c_str());
	const int modeH = atoi(invokeArgs()[3].c_str());

	ASSERT(meshListFile.filename().length() > 0, logic_error());

	// We must pick a co-orindate system: left- or right-handed.
	MexCoordSystem::instance().setFromEnvironment();

	// This must be called before any render library object is created.  (It
	// ensures a correct destruction order for render library Singletons.)
	Ren::initialise();

    //  This is necessary to get around the persistence reading
    //  in code. We need a root before we start the persistence,
    //  however the persistence will also set up its own root.

    W4dRoot*    pRoot = _NEW(W4dRoot( W4dRoot::W4dRootId() ));
	root_  = pRoot;

    RenTexManager::PathNames    searchList = RenTexManager::instance().searchList();

    searchList.push_back( SysPathName( "models/texture4" ) );
    //searchList.push_back( SysPathName( "models/texture4/exp" ) );
    //searchList.push_back( SysPathName( "models/texture4/fire" ) );
    //searchList.push_back( SysPathName( "models/texture4/smoke" ) );
/*
    searchList.push_back( SysPathName( "dummy" ) );
    searchList.push_back( SysPathName( "models/textures" ) );
    searchList.push_back( SysPathName( "models/planet/dales/textures" ) );
    searchList.push_back( SysPathName( "models/explode/fball1" ) );
    searchList.push_back( SysPathName( "models/explode/fball2" ) );
    searchList.push_back( SysPathName( "models/explode/fball3" ) );
    searchList.push_back( SysPathName( "models/explode/fball4" ) );
    searchList.push_back( SysPathName( "models/explode/smoke" ) );
*/
    RenTexManager::instance().searchList( searchList );

	pickDisplayMode(windowMode, modeW, modeH);
	manager_ = _NEW(W4dSceneManager(display_, root_));
	device_  = manager_->pDevice();
	device_->displayImage("wait.bmp");

    // Register the scene manager with the render libary manager
    W4dManager::instance().sceneManager( manager_ );

	UtlDebugTimer::finishCalibration();

    UtlDebugTimer   t2;

	// This is the default camera transform.  readModels can change it.
	const MexVec3 eyePos1(20, 0, 5);
	MexTransform3d eyeXform(eyePos1);

    if( usePersistence )
    {
//    	readOK = persistentReadModels( "models.bin" );

#ifndef NDEBUG
//        PER_LOG_READ( true );
#endif

        //std::ifstream fstr( "models.bin", std::ios::binary );
        //PerIstream  istr( fstr );
        //istr >> persistence_;

        std::cout << "Time to read models = " << t2.time() << "s" << std::endl;

        manager_->useRoot( root_ );

        //  Delete our dummy root as the proper root will have been
        //  set up from the persistence mechanism
        _DELETE( pRoot );

        for( size_t i = 0; i < models_.size(); ++i )
        {
    		ModelData* pModel = models_[ i ];

    		MexTransform3d xform;

			pModel->control_ = _NEW( PhysFlyControl( _NEW(W4dMotionControlledEntity(pModel->entity_)), MexVec2(0,-1) ) );
			pModel->control_->initEventQueue();
			pModel->control_->degreesPerSecond(8);
			pModel->control_->metresPerSecond(0.6);
			pModel->control_->setDefaultPosition(pModel->entity_->localTransform());
			pModel->control_->resetPosition();
			pModel->control_->useKeypadMapping();
        }
    }
    else
    {
        readModels(meshListFile, &eyeXform);

        std::cout << "Time to read models = " << t2.time() << "s" << std::endl;

#ifndef NDEBUG
//        PER_LOG_WRITE( true );
#endif
        //std::ofstream fstr( "models.bin", std::ios::binary );
        //PerOstream  ostr( fstr );
        //ostr << persistence_;

    }

    persistenceImplementationReadWriteTimingData( std::cout );

	light_ = _NEW(W4dDirectionalLight(root_, MexVec3(1,0,0)));

	readEnvironment(meshListFile);

	// The light control actually moves this object and we copy it's orientation
	// into the true light's position.
	dummyLight_ = _NEW(W4dGeneric(root_, light_->localTransform()));

	if (!createBackground())
		std::cout << "Couldn't read background meshes." << std::endl;

	MexRadians a1 = MexDegrees(40);
	camera_ = _NEW(W4dCamera(manager_, root_, eyeXform));
    camera_->verticalFOVAngle(a1.asScalar());
	camera_->yonClipDistance(2000);

	pGroundCamera_ = _NEW(W4dCamera(manager_, root_, eyeXform));
    pGroundCamera_->verticalFOVAngle(a1.asScalar());
	pGroundCamera_->yonClipDistance(2000);

	manager_->useCamera(pGroundCamera_);

	// Use a motion constraint which allows you to ascend to effectively
	// infinity (the default max height is 10).
	PhysGroundMotionConstraint* constraint = _NEW(PhysGroundMotionConstraint());
	constraint->maxHeight(100000);

 	pGroundControl_ = _NEW( PhysGroundFlyControl(_NEW( W4dMotionControlledEntity( pGroundCamera_ ) ),
												 constraint) );
    pGroundControl_->enableInput();
	pGroundControl_->initEventQueue();

	// Set-up a flight sim like flying eyepoint.
 	eyeControl_ = _NEW( PhysFlyControl( _NEW( W4dMotionControlledEntity( camera_ ) ) ) );
	eyeControl_->metresPerSecond(2);
	eyeControl_->degreesPerSecond(10);
	eyeControl_->initEventQueue();
	eyeControl_->useKeypadMapping();

	lightControl_ = _NEW( PhysFlyControl( _NEW( W4dMotionControlledEntity( dummyLight_ ) ) ) );
	lightControl_->degreesPerSecond(10);
   	lightControl_->initEventQueue();
	lightControl_->useKeypadMapping();

	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();
	UtlDebugTimer::finishCalibration();

	// Set the projection plane to z=2cm for all projected shadows.
	W4dShadowProjected2d::shadowPlaneZ(0.05);

    //Hide the cursor
    if( not windowMode )
        DevMouse::instance().hide();

	initialised_ = true;

	std::cout << "Light xform at end of startup "  << light_->localTransform() << std::endl;

	return true;
}

void D3DApp::clientShutdown()
{
	_DELETE(eyeControl_);
	_DELETE(pGroundControl_);
	_DELETE(lightControl_);

	for (int i=0; i!=models_.size(); ++i)
	{
		ModelData* model = models_[i];

		if (model)
		{
			_DELETE(model->control_);
			_DELETE(model);
		}
	}

	_DELETE(root_);
    _DELETE( manager_ );
	_DELETE(display_);
}

static accumulateSpeed(UtlAverager<double>& average, const MexPoint3d position)
{
	static MexPoint3d oldPosition(0,0,0);
	const double distance = position.euclidianDistance(oldPosition);
	oldPosition = position;

	average.accumulate(distance);
}

static void displaySpeed(ostream& ostr, double speed)
{
    const std::ios::fmtflags oldFlags = ostr.flags();
	const int oldPrecision       = ostr.precision();

	ostr.precision(2);
	ostr.setf(std::ios::fixed, std::ios::floatfield);

	const double kph = speed * 60 * 60 / 1000;
	const double mph = kph * 0.621;
	ostr << "Speed: " << speed << " (mps) " << mph << " (mph) " << kph << " (kph)";

	ostr.precision(oldPrecision);
	ostr.setf(oldFlags);
}

// Update the display.
void D3DApp::loopCycle()
{
	processInput();

	// Say what is being controlled by the keyboard.
    RenStats* pStats = device_->statistics();
	if (!showInfo_)
	{
        if( pStats)
    		pStats->hide();
	}
	else
	{
        if( pStats)
    		pStats->show();

		if (manager_->autoAdjustFrameRate())
		{
			char buf[1000];
			sprintf(buf, "Minimum frame rate: %.1f; Maximum frame rate: %.1f;\n",
					manager_->requestedMinFrameRate(),
					manager_->highEnoughFrameRate());
			Ren::out() << buf;
		}

		ModelData* model = models_[controledModel_];
		if (model && model->control_->inputEnabled())
		{
			const MexVec3 pos2 = model->entity_->globalTransform().position();
			Ren::out() << "Controlling model: " << model->entity_->name();
			Ren::out() << " at " << pos2 << std::endl;
		}
		else if (lightControl_ && lightControl_->inputEnabled())
		{
			Ren::out() << "Controlling light" << std::endl;
		}
		else if (eyeControl_->inputEnabled())
		{
			Ren::out() << "Controlling ";
		}

		// Always print the eye's position.

        W4dCamera* pCamera = manager_->currentCamera();

		const MexPoint3d pos = pCamera->globalTransform().position();
		MexEulerAngles params;
	    pCamera->globalTransform().rotation(&params);
		Ren::out() << "Eye at " << pos << ", " << params << std::endl;

		Ren::out() << "Time is " << gameTimer_.time() << "\t";
		Ren::out() << "Max fog vis=" << fogEnd_ << std::endl;

		// Work out a speed for the eyepoint.
		accumulateSpeed(speedo_, pos);
		const double speed = speedo_.average();
		displaySpeed(Ren::out(), speed);

		W4dCompositePlanPtr tplan = models_[controledModel_]->plan_;
		if (tplan.isDefined())
		{
			Ren::out() << std::endl;
			Ren::out() << "Animation Sequences:" << std::endl;
			Ren::out() << tplan->name() << std::endl;
		}
	}

    if( displayHelp_ )
        displayHelp();

	// Get the sun's position and use that to set the light's z rotation.
	if (sun_)
	{
		const MexTransform3d& lightXform = light_->localTransform();
		const MexTransform3d& sunXform   = sun_  ->globalTransform();

		// Leave all the light angles alone, except azimuth.
		MexEulerAngles lightAngles, sunAngles;
		lightXform.rotation(&lightAngles);
		sunXform  .rotation(&sunAngles);
		lightAngles.azimuth(sunAngles.azimuth());

		MexTransform3d newXform(lightAngles);
		light_->localTransform(newXform);
	}
	else if (dummyLight_)
	{
		const MexVec3 dir = dummyLight_->globalTransform().xBasis();

		MexTransform3d xform;
		xform.position(dir);
		light_->globalTransform(xform);
	}

//	device_->fogOn(fogStart_, fogEnd_, fogDensity_);

 	W4dShadow::setLight(light_);

    W4dManager::instance().time(gameTimer_.time());

	if (device_->startFrame())
	{
	    W4dManager::instance().render();
		device_->endFrame();
	}
}

void D3DApp::displayHelp()
{
    Ren::out() << std::endl;
    Ren::out() << "        ==== VIEWER HELP SCREEN ====" << std::endl;
    Ren::out() << "       Right-handed co-ordinate system." << std::endl;
    Ren::out() << "   ----------------------------------------" << std::endl;
    Ren::out() << "       h     Toggle this screen on/off" << std::endl;
    Ren::out() << "      ESC    Quit the program" << std::endl;
    Ren::out() << "       i     Toggle display of eyepoint etc." << std::endl;
    Ren::out() << "       f     Toggle fogging on/off" << std::endl;
    Ren::out() << "       u     Toggle frame rate regulator on/off" << std::endl;
    Ren::out() << "       d     Increase fog range" << std::endl;
    Ren::out() << "    Shift-d  Decrease fog range" << std::endl;
    Ren::out() << "     Pause   Pause/resume" << std::endl;
    Ren::out() << "       F1    Normal flight controls" << std::endl;
    Ren::out() << "       F2    Undamped flight sim constrols" << std::endl;
	Ren::out() << "       m     Cycle forwards  thru all objects" << std::endl;
	Ren::out() << "    Shift-m  Cycle backwards thru all objects" << std::endl;
	Ren::out() << "       a     Cycle thru animations of a composite" << std::endl;
	Ren::out() << "       z     Play the animation selected with the 'a' key" << std::endl;
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

void perWrite( PerOstream& ostr, const ModelData& data )
{
    ostr << data.entity_;
    ostr << data.composite_;
}

void perRead( PerIstream& istr, ModelData& data )
{
    istr >> data.entity_;
    istr >> data.composite_;
    data.plan_ = NULL;
}

D3DAppPersistence::D3DAppPersistence( D3DApp* pApp )
: pD3DApp_( pApp )
{
}

D3DAppPersistence::D3DAppPersistence( PerConstructor )
{
    ASSERT_FAIL( "This should never be called" );
}

void perWrite( PerOstream& ostr, const D3DAppPersistence& per )
{
    ostr << per.pD3DApp_->root_;
    ostr << per.pD3DApp_->models_;
}

void perRead( PerIstream& istr, D3DAppPersistence& per )
{
    istr >> per.pD3DApp_->root_;
    istr >> per.pD3DApp_->models_;
}
