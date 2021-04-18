/*
 * D E V I C E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//TODO: This god class needs refactoring. 

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/device.hpp"

#include <cstdio>
#include <iostream>
#include <iomanip>

#include <string>
#include "ctl/vector.hpp"
#include "system/pathname.hpp"

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/coordsys.hpp"
#include "mathex/random.hpp"

#include "render/colour.hpp"
#include "render/camera.hpp"
#include "render/display.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/surface.hpp"
//#include "render/renerror.hpp"
#include "render/capable.hpp"
#include "render/stats.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "render/drivsel.hpp"

#include "render/internal/devicei.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/internal.hpp"
#include "render/internal/debug.hpp"
#include "render/internal/driver.hpp"
#include "render/internal/glmath.hpp"
#include "render/internal/vpmap.hpp"
#include "render/internal/polysord.hpp"
#include "render/internal/polysorp.hpp"
#include "render/internal/matbody.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/illumine.hpp"
#include "render/internal/nonmmx.hpp"
#include "render/internal/mmx.hpp"
#include "render/internal/displayi.hpp"
#include "render/internal/surfmgri.hpp"
#include "render/internal/linediag.hpp"
#include "render/internal/capablei.hpp"
#include "render/internal/drivi.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/colpack.hpp"
#include "render/render.hpp"
#include "render/internal/trigroup.hpp"
#include "render/internal/surfbody.hpp"
#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include "system/winapi.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>

////////////////////////////////////////////////////////////
#define CB_REN_DEVICE_DEPIMPL                                       \
			CB_DEPIMPL(RenCamera*, currentCamera_);                 \
			CB_DEPIMPL(RenSurface*, surfBackBuf_);                  \
			CB_DEPIMPL(RenSurface*, surfFrontBuf_);                 \
			CB_DEPIMPL(RenDisplay*, display_);                      \
			CB_DEPIMPL(bool, rendering_);                           \
			CB_DEPIMPL(bool, rendering3D_);                         \
			CB_DEPIMPL(bool, rendering2D_);                         \
			CB_DEPIMPL(RenColour, background_);                     \
			CB_DEPIMPL(RenCapabilities*, caps_);                    \
			CB_DEPIMPL(RenStats*, stats_);                          \
			CB_DEPIMPL(RenIIlluminator*, illuminator_);             \
			CB_DEPIMPL(int, surfacesMayBeLost_);                    \
			CB_DEPIMPL(bool, shouldBeginScene_);                    \
			CB_DEPIMPL(glm::mat4*, projViewMatrix_);                \
			CB_DEPIMPL(RenIViewportMapping*, vpMapping_);           \
            CB_DEPIMPL(RenIPriorityPostSorter*, coplanarSorter_);   \
			CB_DEPIMPL(RenIDepthPostSorter*, alphaSorter_);         \
			CB_DEPIMPL(bool, doingBackground_);                     \
			CB_DEPIMPL(RenIDepthPostSorter*, normalAlphaSorter_);   \
			CB_DEPIMPL(BaseLogBuffer, extOut_);                     \
			CB_DEPIMPL(bool, fogOn_);                               \
			CB_DEPIMPL(RenColour, fogColour_);                      \
			CB_DEPIMPL(float, fogStart_);                           \
			CB_DEPIMPL(float, fogEnd_);                             \
			CB_DEPIMPL(float, fogDensity_);                         \
			CB_DEPIMPL(bool, staticOn_);                            \
			CB_DEPIMPL(bool, antiAliasingOn_);                      \
			CB_DEPIMPL(RenDriverSelector*, driverSelector_);       \
			CB_DEPIMPL(double, interference_);

#define CB_PEER_PTR_DEPIMPL(objectPtr, name) objectPtr->pImpl_->name
#define CB_PEER_REF_DEPIMPL(objectRef, name) objectRef.pImpl_->name

#pragma GCC diagnostic ignored "-Wunused-variable"
////////////////////////////////////////////////////////////

RenDevice::RenDevice(RenDisplay* display)

	:pImpl_(_NEW(RenIDeviceImpl(display, this)))
{
	PRE(display);
	CB_REN_DEVICE_DEPIMPL;

	display_ = display;
	//projViewMatrix_ = _NEW(D3DMATRIX);
	projViewMatrix_ = _NEW(glm::mat4);
	coplanarSorter_ = _NEW(RenIPriorityPostSorter);
	normalAlphaSorter_ = _NEW(RenIDepthPostSorter);
	alphaSorter_ = NULL;
	doingBackground_ = false;
	caps_ = NULL;
	stats_ = _NEW(RenStats);
	fogOn_ = false;
	fogStart_ = 1;
	fogEnd_ = 500;
	fogDensity_ = 0.05;
	interference_ = 0;
	staticOn_ = false;
	currentCamera_ = NULL;
	vpMapping_ = NULL;
	rendering_ = false;
	rendering3D_ = false;
	rendering2D_ = false;
	illuminator_ = NULL;		// viewport must be created 1st, see below
	surfBackBuf_ = NULL;
	surfFrontBuf_ = 0;
	surfacesMayBeLost_ = 0;
	shouldBeginScene_ = true;
	antiAliasingOn_ = false;

	PRE(Ren::initialised());
	PRE(MexCoordSystem::instance().isSet());

	// There are two alpha sorters.  Only one is in use at a time.  The switch
	// is made by chaning this pointer.
	alphaSorter_ = normalAlphaSorter_;

	fogColour(RenColour::white());

	if( not fitToDisplay(display_) )
	{
		// TBD: What shall i do then ?
		ASSERT(false, "could not create surfaces " );
	}

    // Create and compile our GLSL programs from the shaders
	glProgramID_GIU2D_ = loadShaders( "2DShading.vxgls", "2DShading.fggls" );

	// Initialize VBO
	glGenBuffers(1, &gl2DVertexBufferID_);
	// Get a handle for our buffers
	glVertexPosition_screenspaceID_ = glGetAttribLocation(glProgramID_GIU2D_, "vertexPosition_screenspace");
	glVertexUVID_ = glGetAttribLocation(glProgramID_GIU2D_, "vertexUV");
	glVertexColour_screenspaceID_ = glGetAttribLocation(glProgramID_GIU2D_, "vertexColor");
	glScreenspaceID_ = glGetUniformLocation(glProgramID_GIU2D_, "uScreenspace");
	// Initialize uniforms' IDs
	gl2DUniformID_ = glGetUniformLocation( glProgramID_GIU2D_, "uTextureSampler" );

	glProgramID_Standard_ = loadShaders( "StandardShading.vxgls", "StandardShading.fggls" );
	// Get a handle for our "MVP" uniform
	glModelMatrixID_ = glGetUniformLocation(glProgramID_Standard_, "uM");
	glViewMatrixID_ = glGetUniformLocation(glProgramID_Standard_, "uV");
	glProjectionMatrixID_ = glGetUniformLocation(glProgramID_Standard_, "uP");
	glFogColourID_ = glGetUniformLocation(glProgramID_Standard_, "uFogColour");
	glFogParamsID_ = glGetUniformLocation(glProgramID_Standard_, "uFogParams");
	// VBO
	glGenBuffers(1, &glVertexDataBufferID_);
	glGenBuffers(1, &glElementBufferID_);
	// Get a handle for our buffers
	glVertexPosition_modelspaceID_ = glGetAttribLocation(glProgramID_Standard_, "vertexPosition_modelspace");
	glVertexColour_modelspaceID_ = glGetAttribLocation(glProgramID_Standard_, "vertexColor");
    glVertex_modelspaceUVID_ = glGetAttribLocation(glProgramID_Standard_, "vertexUV");
    glTextureSamplerID_ = glGetUniformLocation( glProgramID_Standard_, "uTextureSampler2" );

    glProgramID_Billboard_ = loadShaders( "BillboardShading.vxgls", "2DShading.fggls" );
	// VBO
	glGenBuffers(1, &glVertexDataBufferBillboardID_);
	glGenBuffers(1, &glElementBufferBillboardID_);
	// Get a handle for our buffers
	glViewProjMatrix_BillboardID_ = glGetUniformLocation(glProgramID_Billboard_, "uVP");
	glVertexPosition_BillboardID_ = glGetAttribLocation(glProgramID_Billboard_, "vertexPosition_Billboard");
	glVertexColour_BillboardID_ = glGetAttribLocation(glProgramID_Billboard_, "vertexColor");
    glVertex_BillboardUVID_ = glGetAttribLocation(glProgramID_Billboard_, "vertexUV");
    glTextureSamplerBillboardID_ = glGetUniformLocation( glProgramID_Billboard_, "uTextureSampler" );

    // Create empty texture
    glGenTextures(1, &glTextureEmptyID_);
	glBindTexture(GL_TEXTURE_2D, glTextureEmptyID_);
	const uint data = 0xFFFFFFFF;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1,  1,  0, GL_RGBA, GL_UNSIGNED_BYTE, &data);

	// Prepare framebuffer for offscreen rendering
	glGenFramebuffers(1, &glOffscreenFrameBuffID_);

	const char* disableMMX = getenv("CB_DISABLE_MMX");

	// The illuminator comes in two flavors: MMX optimised or not.
	ASSERT(caps_, "No capabilites object created: can't query presence of MMX.");
	if (disableMMX || !caps_->supportsMMX())
		illuminator_ = _NEW(RenINonMMXIlluminator(pImpl_));
	else
		illuminator_ = _NEW(RenIMMXIlluminator(pImpl_));

	RenStats* pStats = statistics();
	bool statsShown = true;

	if(pStats == 0)
		statsShown = false;
	else
	{
		statsShown = pStats->shown();
		pStats->hide();
	}

	// Test the line drawing capabilities of the system.
	RenSurface lineSurf = RenSurface::createAnonymousSurface(32,32, backSurface());
	if(startFrame())
	{

		start2D();
		{
			RenILinesDiagnostic::instance().drawLines(&lineSurf);
		}
		end2D();

		endFrame();
	}
	RenILinesDiagnostic::instance().testLines(lineSurf);

    // No luck, try another one test (TODO: why isn't test above working all the times?)
    if (RenILinesDiagnostic::instance().verticalResult() == RenILinesDiagnostic::UNKNOWN and
        RenILinesDiagnostic::instance().horizontalResult() == RenILinesDiagnostic::UNKNOWN)
    {
        RenSurface backSurf = backSurface();
        RenILinesDiagnostic::instance().setTestType(RenILinesDiagnostic::TEST2);
        if(startFrame())
        {

            start2D();
            {
                RenILinesDiagnostic::instance().drawLines(&backSurf);
            }
            end2D();

            endFrame();
        }
        RenILinesDiagnostic::instance().testLines(backSurf);
    }

	if(statsShown)
		pStats->show();

    TEST_INVARIANT;
}

// Question: do all the COM interfaces need to be released in a certain
// order?  Some of Micro$oft's samples release them in reverse order of
// creation.
RenDevice::~RenDevice()
{
	CB_REN_DEVICE_DEPIMPL;

    TEST_INVARIANT;

	// Note: the alphaSorter_ variable is set to point to either NULL or to
	// normalAlphaSorter_, depending on what's being rendered.  It should
	// *not* be deleted.
	_DELETE(coplanarSorter_);
    _DELETE(normalAlphaSorter_);

	_DELETE(caps_);
	_DELETE(stats_);
	_DELETE(vpMapping_);
	_DELETE(projViewMatrix_);
	_DELETE(illuminator_);
    _DELETE(surfBackBuf_);
	_DELETE(surfFrontBuf_);
	_DELETE(pImpl_);

	pImpl_ = 0;

	// Delete shaders
	glDeleteProgram(glProgramID_Standard_);
	glDeleteProgram(glProgramID_GIU2D_);
	glDeleteRenderbuffers(1, &glOffscreenFrameBuffID_);
	//glDeleteBuffers(1, &uvbuffer);
}

void RenDevice::reset()
{
	CB_REN_DEVICE_DEPIMPL;
}

const GLuint RenDevice::loadShaders(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	SysMetaFile metaFile( "shaders.met" );

	if( SysMetaFile::useMetaFile() )
    {
        SysMetaFile::encryptionType(SysMetaFile::ENCRYPTION_2);
        SysMetaFileIstream vertexShaderStream( metaFile, vertexShaderPath, std::ios::in );
        std::string line = "";
        while(getline(vertexShaderStream, line))
            vertexShaderCode += "\n" + line;

        SysMetaFileIstream fragmentShaderStream( metaFile, fragmentShaderPath, std::ios::in );
        line = "";
        while(getline(fragmentShaderStream, line))
            fragmentShaderCode += "\n" + line;
        SysMetaFile::encryptionType(SysMetaFile::ENCRYPTION_1);
    }
    else
    {
        // Read the Vertex Shader code from the file
        std::string shadersDir("data/shaders/");
        // Check if GLSL is too old
        if(glGetString(GL_SHADING_LANGUAGE_VERSION)[0] < '3')
            shadersDir.append("120/");
        std::ifstream vertexShaderStream(shadersDir+vertexShaderPath, std::ios::in);
        if(vertexShaderStream.is_open()){
            std::string line = "";
            while(getline(vertexShaderStream, line))
                vertexShaderCode += "\n" + line;
            vertexShaderStream.close();
        }
        else
        {
            std::cerr << "Impossible to open vertex shader file from: " << shadersDir << std::endl;
            return 0;
        }

        // Read the Fragment Shader code from the file
        std::ifstream fragmentShaderStream(shadersDir+fragmentShaderPath, std::ios::in);
        if(fragmentShaderStream.is_open()){
            std::string line = "";
            while(getline(fragmentShaderStream, line))
                fragmentShaderCode += "\n" + line;
            fragmentShaderStream.close();
        }
        else
        {
            std::cerr << "Impossible to open fragment shader file from: " << shadersDir << std::endl;
            return 0;
        }
    }

	GLint result = GL_FALSE;
	int infoLogLength;


	// Compile Vertex Shader
	RENDER_STREAM("Compiling vx shader : " << vertexShaderPath << std::endl);
	char const * vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
	glCompileShader(vertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 2 )
	{
		std::vector<char> vertexShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		std::cerr << "Error: " << &vertexShaderErrorMessage[0] << std::endl;
	}


	// Compile Fragment Shader
	RENDER_STREAM("Compiling fg shader : " << fragmentShaderPath << std::endl);
	char const * fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
	glCompileShader(fragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 2 )
	{
		std::vector<char> fragmentShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		std::cerr << "Error: " << &fragmentShaderErrorMessage[0] << std::endl;
	}

	// Link the program
	RENDER_STREAM("Linking program" << std::endl);
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 2 )
	{
		std::vector<char> programErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
		std::cerr << "Error: " << &programErrorMessage[0] << std::endl;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

void RenDevice::clearAllSurfaces(const RenColour& colour)
{
	PRE(!rendering3D() && !rendering2D());
	CB_REN_DEVICE_DEPIMPL;

	// Temporarily set, then reset the background colour.
    glClearColor(colour.r(), colour.g(), colour.b(), colour.a());
	clearAllSurfaces();
	glClearColor(colour.r(), background_.g(), background_.b(), background_.a());
}

void RenDevice::clearAllSurfaces()
{
	CB_REN_DEVICE_DEPIMPL;
	PRE(!rendering3D() && !rendering2D());
	PRE(display_);

	const RenDisplay::Mode mode = display_->currentMode();

	// As in startFrame, if the client hasn't set a viewport, then default
	// to full-screen.
	if (!vpMapping_)
		setViewport(0,0, mode.width(), mode.height());


	ASSERT(vpMapping_, "No viewport set; there should be a default.");

	// Having 100% reset the screen, any areas saved under the cursor
	// are now irrelevant.
	display_->displayImpl().discardCursorSaves();
}

bool RenDevice::fitToDisplay(RenDisplay* display)
{
	PRE(display);
	CB_REN_DEVICE_DEPIMPL;

	display_ = display;

	// Check the capabilities of the device, so that we can selectively enable
	// special effects such as alpha textures.
	if( caps_ )
		_DELETE( caps_ );
    caps_ = _NEW(RenCapabilities(this, true));
	RENDER_STREAM( *caps_ );

	// we have to copy the gamma correction into display in order to implement
	// asserts in gammaCorrection() methods, it's ugly.
	display_->supportsGammaCorrection( caps_->supportsGammaCorrection() );

 	// We assume that some form of texture transparency is supported.  All
 	// reasonable games apps. require transparency.
 	ASSERT(caps_->supportsColourKey() || caps_->supportsTextureAlpha(), runtime_error("No transparency supported by D3D."));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	if (SDL_GL_CreateContext(display_->window()) == NULL)
	{
		std::string msg("Fatal in SDL_GL_CreateContext: ");
		msg += SDL_GetError();
		SysWindowsAPI::messageBox(msg.c_str(), "Error");
		return EXIT_FAILURE;
	}

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		std::string msg("Fatal in glewInit: ");
		msg += (char*)glewGetErrorString(glew_status);
		SysWindowsAPI::messageBox(msg.c_str(), "Error");
		return EXIT_FAILURE;
	}
	if (!GLEW_VERSION_2_1)
	{
		SysWindowsAPI::messageBox("Your graphic card or driver does not support OpenGL 2.1!\nToo bad, will terminate now.", "Error");
		return EXIT_FAILURE;
	}

    //glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	createViewport();
	RenDisplay::Mode mode = display->currentMode();
	setViewport(0,0, mode.width(), mode.height());
	pImpl_->useDevice(this, RenI::FORCE_UPDATE);
	surfacesMayBeLost_ = 10;

	// Clear the both front and back surfaces just in case they are uninitialised.
	clearAllSurfaces(RenColour::black());

	return true;
}

void RenDevice::createViewport()
{
	backgroundColour(RenColour::black());
}

void RenDevice::setViewport(int left, int top, int width, int height)
{
//	CB_DEPIMPL(COMPTR(IDirect3DViewport2), viewport_);
	CB_DEPIMPL(RenIViewportMapping*, vpMapping_);

	PRE(left + width <= windowWidth());
	PRE(top + height <= windowHeight());

	const double ratio = (double) width / height;

    //glViewport( left, top, width, height );
	// NB: if the viewport is changed, these values *must* also
	// be updated.
	_DELETE(vpMapping_);
	vpMapping_ = _NEW(RenIViewportMapping(width, height, left, top));

	RENDER_STREAM("Set viewport to (" << left << "," << top << ") " << width << "x" << height << "\n");
}

////////////////////////////////// Frame delineation //////////////////////////
bool RenDevice::startFrame()
{
	CB_DEPIMPL(RenDisplay*, display_);
	CB_DEPIMPL(bool, rendering_);
	CB_DEPIMPL(int, surfacesMayBeLost_);
	CB_DEPIMPL(bool, shouldBeginScene_);
	CB_DEPIMPL(RenIViewportMapping*, vpMapping_);

//	PRE(device_);
	PRE(!rendering());

	// When the Window is activated this flag is set indicating that surface
	// memory could have been lost to another process.
	if (surfacesMayBeLost_)
	{
		// This code while still useful makes the debug version of Machines crash
		// because the timer is not callibrated early enough.
		// RENDER_STREAM("\n** Possibly restoring surfaces:\n");
		// UtlDebugTimer timer(UtlDebugTimer::RUNNING);
		bool result = true;

		// Is the back buffer unavailable for rendering?
		if (!display_->checkForLostSurfaces())
		{
			RENDER_STREAM("Primary or z-buffer surface lost.  Can't render." << std::endl);
			return false;
		}

		// These checks used to be done every frame.
		// Check to see if any texture surfaces have been lost.
		if (!RenTexManager::instance().restoreAll())
		{
			RENDER_STREAM("Texture surfaces lost.  Can't render." << std::endl);
			//return false;
		}
		else
		{
			// They're fixed, so minus one off this flag. After an activate message is recieved
			// we try 10 times to restore lost surfaces. This is because there doesn't appear
			// to be a windows message that we can use that accurately indicates when we can
			// successfully recover any lost surfaces ( WM_ACTIVATE is the most reliable ).
			--surfacesMayBeLost_;
		}

		//RENDER_STREAM("** surface restore took " << 1000 * timer.time().asDouble() << " (ms)\n\n");
	}

	if (!shouldBeginScene_)
	{
		RENDER_STREAM("RenDevice::startFrame() == false" << std::endl);
		return false;
	}

	// Only do this after we have checked that rendering can proceed.
	// It needs to occur before RenDisplay::startFrame because the display
	// does blitting which asserts that the frame has started.
	rendering_ = true;

	display_->startFrame();

	// When this method is first called, if the client hasn't set a viewport,
	// then default to full-screen.
	if (!vpMapping_)
		setViewport(0,0, windowWidth(), windowHeight());

	pImpl_->useDevice(this, RenI::LAZY_UPDATE);

	RENDER_STREAM(std::endl << "RenDevice::startFrame() == true" << std::endl << '{' << std::endl);
	RENDER_INDENT(3);

	DEBUG_RESET_FRAME_TIME;

	POST(idleRendering());
	return true;
}

void RenDevice::start2D()
{
	CB_DEPIMPL(bool, rendering3D_);
	CB_DEPIMPL(bool, rendering2D_);
	CB_DEPIMPL(RenIIlluminator*, illuminator_);
	CB_DEPIMPL(bool, shouldBeginScene_);
	CB_DEPIMPL(RenIDepthPostSorter*, alphaSorter_);

//	PRE(device_);
	PRE(idleRendering());

	rendering2D_= true;

	// Set default rendering state which is appropriate for 2D gui stuff.
	// Assume that rendering ops will use 2D transformed, lit co-ords.  Thus
	// don't initialise lighting or any projection/view matrices.
	disableFog();

    glFrontFace(GL_CW);

	alphaSorter_ = NULL;
	illuminator_->filter(RenColour::white());

	if ( shouldBeginScene_ )
	{
//		if (!TRYRENDX(device_->BeginScene()))
//			return;
	}

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("RenDevice::start2D() at " << now << "(ms)\n{\n");
	RENDER_INDENT(3);

	POST(rendering2D());
}

void RenDevice::end2D()
{
	CB_DEPIMPL(bool, shouldBeginScene_);
	CB_DEPIMPL(bool, rendering2D_);

	PRE(rendering2D());

	rendering2D_ = false;

	RENDER_STREAM("shouldBeginScene_ == " << shouldBeginScene_ << std::endl);
	RENDER_INDENT(-3);

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("RenDevice::end2D() at " << now << "(ms)\n");
	RENDER_STREAM('}' << std::endl);
}

void RenDevice::start3D(bool clearBack)
{
	CB_REN_DEVICE_DEPIMPL;

	// A camera must have been specified.
	PRE(currentCamera());
//	PRE(device_);
	PRE(idleRendering());

    //Clear out any left-over unused material bodies, from eg a persistent load
    RenIMatManager::instance().clearUnusedMatBodyList();

	if ( shouldBeginScene_ )
	{
//		if (!TRYRENDX(device_->BeginScene()))
//			return;
	}

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("RenDevice::start3D() at " << now << "(ms)\n");
	RENDER_INDENT(3);

	rendering3D_ = true;

	if (stats_)
		stats_->startFrame();

	RenSurfaceManager::instance().startFrame();

	// Viewpoint and/or camera parameters may have changed.
	updateMatrices();

	// The background colour needs to have the camera's filter applied to it.
	RenColour bgCol = background_;
	bgCol.r(bgCol.r() * currentCamera_->colourFilter().r());
	bgCol.g(bgCol.g() * currentCamera_->colourFilter().g());
	bgCol.b(bgCol.b() * currentCamera_->colourFilter().b());

	ASSERT(vpMapping_, "No viewport set; startFrame should set a default.");
    glClearColor(bgCol.r(), bgCol.g(), bgCol.b(), bgCol.a());
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// Inform the texture manager that a frame is starting.
	// RenTexManager::instance().startFrame();

	illuminator_->filter(currentCamera_->colourFilter());
	illuminator_->startFrame();
	RenMesh::startFrame();

	setFog(fogStart_, fogEnd_, fogDensity_, fogColour());

	if (pImpl_->caps_ && pImpl_->caps_->internal()->supportsZBias())
    {
        glDisable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( (GLfloat)pImpl_->caps_->internal()->minZBias(), 1.0 );
    }

    glFrontFace(GL_CW);
    glDepthMask(GL_TRUE);

	// All the alpha polygons are drawn as a post-pass in endFrame, so we can
	// turn blending off for the main pass.
	pImpl_->disableAlphaBlending();

	// Enable alpha sorting.
	alphaSorter_ = normalAlphaSorter_;

	POST(rendering3D());
}

void RenDevice::startBackground(double yon)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenIDepthPostSorter*, alphaSorter_);
	CB_DEPIMPL(bool, doingBackground_);

	PRE(rendering());

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("RenDevice::startBackground() at " << now << "(ms)\n");

	// Disable alpha sorting.  Background alpha is drawn in order.
	alphaSorter_ = NULL;
	doingBackground_ = true;
	overrideClipping(currentCamera_->hitherClipDistance(), yon);
	disableLighting();
	disableFog();
    glDepthMask(GL_FALSE);
}

inline bool isWhiteChar(char c)
{
	return c == ' ' || c == '\n' || c == '\t';
}

static bool isWhiteString(const string& str)
{
	for (int i=0; i!=str.length(); ++i)
		if (!isWhiteChar(str[i]))
			return false;

	return true;
}

void RenDevice::flush3DAlpha()
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(RenIDepthPostSorter*, alphaSorter_);
	CB_DEPIMPL(RenIPriorityPostSorter*, coplanarSorter_);
	CB_DEPIMPL(bool, doingBackground_);
	CB_DEPIMPL(RenIDepthPostSorter*, normalAlphaSorter_);

	PRE(rendering());
	PRE(rendering3D());

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("RenDevice::flush3DAlpha() at " << now << "(ms)\n");

	// Make sure all the Direct3D parameters are set up correctly. This function
	// may be called after 2D update, therefore we can not rely on these parameters
	// being set correcly.
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

	if (doingBackground_)
	{
		// The clipping will have been changed for background objects.  It
		// needs to be reset before the other post-sorters are invoked.
		const double hither = currentCamera_->hitherClipDistance();
		const double yon    = currentCamera_->yonClipDistance();
		const double h      = currentCamera_->projectionPlaneHalfHeight();
		updateProjMatrix(hither, yon, h);

		// Likewise for the lighting, fog and alpha on the coplanar polys.
		enableLighting();
        glDepthMask(GL_TRUE);
	}

	// Inter-mesh coplanar polygons are drawn with normal settings.  They are
	// drawn here because they need to come at the very end, after anything
	// they might be on top of.  (But before things with alpha.)
    restoreFog();

	coplanarSorter_->render();

	// The coplanar sorter may leave the zBias in an arbitrary state.
	if (pImpl_->caps_ && pImpl_->caps_->internal()->supportsZBias())
	{
        glDisable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( (GLfloat)pImpl_->caps_->internal()->minZBias(), 1.0 );
    }

	if (caps_->supportsFlatAlpha() || caps_->supportsTextureAlpha())
	{
		// The whole raison-d'etre of the alpha sorter is drawing the polygons
		// without the z-buffer.
        glDepthMask(GL_FALSE);
		pImpl_->enableAlphaBlending();

		normalAlphaSorter_->render();
		pImpl_->disableAlphaBlending();
		glDepthMask(GL_TRUE);
	}
	glDisable(GL_ALPHA_TEST);
}

void RenDevice::end3D()
{
	CB_DEPIMPL(bool, rendering3D_);
	CB_DEPIMPL(bool, shouldBeginScene_);
	CB_DEPIMPL(bool, staticOn_);
	CB_DEPIMPL(double, interference_);

	PRE(rendering3D());

	const double now1 = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::end3D() starts at " << now1 << "(ms)\n");

	flush3DAlpha();

	const double now2 = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::end3D() adding 2D effects at " << now2 << "(ms)\n");

	if (interference_ > 0.001)
		addInterference();

	if (staticOn_)
		addStatic();

	// If we fail to end a scene then we should NOT call DirectX
	// BeginScene again until we successfully end a scene.

	RENDER_STREAM("shouldBeginScene_ == " << shouldBeginScene_ << std::endl);
	RENDER_INDENT(-3);

	const double now3 = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::end3D() ends at " << now3 << "(ms)\n");
	RENDER_STREAM('}' << std::endl);

	rendering3D_ = false;

	POST(idleRendering());
}

void RenDevice::commonEndFrame()
{
	CB_DEPIMPL(RenDisplay*, display_);
	CB_DEPIMPL(RenStats*, stats_);
	CB_DEPIMPL(bool, shouldBeginScene_);
	CB_DEPIMPL(RenIViewportMapping*, vpMapping_);
	CB_DEPIMPL(BaseLogBuffer, extOut_);

	const double now1 = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::endFrame() starts at " << now1 << "(ms)\n");

	if (stats_)
		stats_->endFrame();

	// Print the statistics (which may be empty) followed by the debug stream.
	// Use the top-most coord of the 3D viewport as the top-most coord for text.
	// (Other areas of the display is not the responsibility of this class.)
	const string  empty;
	const string& statsStr = (stats_)? stats_->statsText(): empty;
	const string  debugStr = extOut_.dataAsString();

	ASSERT(vpMapping_, "No viewport set; startFrame should set a default.");
	const string concat = statsStr + debugStr;

	if (concat.length() > 0 and
		not isWhiteString(concat) and
		shouldBeginScene_ )
	{
		RenSurface surf = backSurface();
		surf.drawText(pImpl_->debugX_, pImpl_->debugY_, concat, RenColour::yellow());
	}

	extOut_.clear();

	const double now2 = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::endFrame() text done at " << now2 << "(ms)\n");
}

void RenDevice::endFrame()
{
	PRE(rendering());
	CB_DEPIMPL(bool, rendering_);

	commonEndFrame();
	pImpl_->display_->flipBuffers(backSurface());

	rendering_   = false;

	const double now = DEBUG_FRAME_TIME;
	RENDER_STREAM("  RenDevice::endFrame() buffers flipped at " << now << "(ms)\n");

	RENDER_INDENT(-3);
	RENDER_STREAM("}" << std::endl << std::endl);

	POST(!rendering());
	POST(!rendering3D());
}

///////////////////////// Transformation matrix support (MVP) /////////////////////////
void RenDevice::updateMatrices()
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(glm::mat4*, projViewMatrix_);

	// Recalculate the view matrix based on the camera's current position.
	// TBD: use transform IDs to avoid the calculation when the eyepoint
	// hasn't moved since the last frame.
	updateViewMatrix(view_);

	// If the camera parameters change from frame to frame, we need to update
	// the projection matrix.
	const double hither = currentCamera_->hitherClipDistance();
	const double yon    = currentCamera_->yonClipDistance();
	const double h      = currentCamera_->projectionPlaneHalfHeight();
	updateProjMatrix(hither, yon, h);

	// Cache the product of the above matrices.
    *projViewMatrix_ = projection_ * view_;
}

// From the Direct3D tunnel sample:
// Creates a matrix which is equivalent to having the camera at a
// specified position. This matrix can be used to convert vertices to
// camera coordinates.
static void computeViewMatrix
(
	glm::vec3& rP, 	// position of the camera
	glm::vec3& rD,	// direction of view
	glm::vec3& rN, 	// up vector
    glm::mat4* lpM	// result
)
{
    // Set the rotation part of the matrix and invert it. Vertices must be
    // inverse rotated to achieve the same result of a corresponding
    // camera rotation.
    glm::mat4 tmp;
    tmp[0][3] = tmp[1][3] = tmp[2][3] = tmp[3][0] = tmp[3][1] = tmp[3][2] = (float)0.0;
    tmp[3][3] = (float)1.0;

    GLMatSetRotation(&tmp, &rD, &rN);
    GLMatTranspose(lpM, &tmp);

    // Multiply the rotation matrix by a translation transform.  The
    // translation matrix must be applied first (left of rotation).
    (*lpM)[3][0] = -((*lpM)[0][0] * rP.x + (*lpM)[1][0] * rP.y + (*lpM)[2][0] * rP.z);
    (*lpM)[3][1] = -((*lpM)[0][1] * rP.x + (*lpM)[1][1] * rP.y + (*lpM)[2][1] * rP.z);
    (*lpM)[3][2] = -((*lpM)[0][2] * rP.x + (*lpM)[1][2] * rP.y + (*lpM)[2][2] * rP.z);

	// Negating these elements provides the change from DirectX's left-
	// handed co-ordinate system to a right-handed one.
	if (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)
	{
		(*lpM)[0][2] = -(*lpM)[0][2];
		(*lpM)[1][2] = -(*lpM)[1][2];
		(*lpM)[2][2] = -(*lpM)[2][2];
		(*lpM)[3][2] = -(*lpM)[3][2];
	}
}

// Use the camera's current position to set the view matrix part.
void RenDevice::updateViewMatrix(glm::mat4& view)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

//	PRE(device_);
	PRE(currentCamera_);

	const MexTransform3d& xform = currentCamera_->transform();
	MexVec3 xBasis = xform.xBasis();
	MexVec3 zBasis = xform.zBasis();
	const MexVec3 pos    = xform.position();

	xBasis.makeUnitVector();
	zBasis.makeUnitVector();

	// This is how a RenCamera is defined -- looking along the x-axis
	// with the z-axis as its up vector.
    glm::vec3 dir(xBasis.x(), xBasis.y(), xBasis.z());
    glm::vec3 up(zBasis.x(), zBasis.y(), zBasis.z());
    glm::vec3 p(pos.x(), pos.y(), pos.z());

	// The camera's direction needs to be negated for the change from an
	// LH to an RH co-ordinate system.
	if (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)
	{
		dir.x = -dir.x;
		dir.y = -dir.y;
		dir.z = -dir.z;
	}

	computeViewMatrix(p, dir, up, &view);

    //glm::mat4 view = glm::lookAt(p, dir, up);
    //view = glm::lookAt(p, dir, up);
	glUniformMatrix4fv(glViewMatrixID_, 1, GL_FALSE, &view[0][0]);
}

// Taken straight from the M$ samples.
static void computePerspectiveProjection
(
	glm::mat4*  lpd3dMatrix,
    double      dHalfHeight,
    double      dFrontClipping,
    double      dBackClipping
)
{
	PRE(lpd3dMatrix);
	PRE(dHalfHeight    > 0);
	PRE(dFrontClipping > 0);
	PRE(dFrontClipping < dBackClipping);

    double dTmp1;
    double dTmp2;

    dTmp1 = dHalfHeight / dFrontClipping;
    dTmp2 = dBackClipping / (dBackClipping - dFrontClipping);

    (*lpd3dMatrix)[0][0] =  (0.8);//(1.0);
    (*lpd3dMatrix)[0][1] =  (0.0);

    (*lpd3dMatrix)[0][2] =  (0.0);
    (*lpd3dMatrix)[0][3] =  (0.0);
    (*lpd3dMatrix)[1][0] =  (0.0);
    (*lpd3dMatrix)[1][1] =  (1.0);
    (*lpd3dMatrix)[1][2] =  (0.0);
    (*lpd3dMatrix)[1][3] =  (0.0);
    (*lpd3dMatrix)[2][0] =  (0.0);
    (*lpd3dMatrix)[2][1] =  (0.0);
    (*lpd3dMatrix)[2][2] =  (dTmp1 * dTmp2);
    (*lpd3dMatrix)[2][3] =  (dTmp1);
    (*lpd3dMatrix)[3][0] =  (0.0);
    (*lpd3dMatrix)[3][1] =  (0.0);

    (*lpd3dMatrix)[3][2] =  (-dHalfHeight * dTmp2);
    (*lpd3dMatrix)[3][3] =  (0.0);
}

void RenDevice::updateProjMatrix(double hither, double yon, double h)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenDisplay*, display_);

//	PRE(device_);
	PRE(currentCamera_);
	PRE(h > 0);
	PRE(hither < yon);

	const RenDisplay::Mode mode = display_->currentMode();

	projection_ = glm::perspectiveFovLH<float>(currentCamera_->verticalFOVAngle(), mode.width(), mode.height(), hither, yon);
}

void RenDevice::overrideClipping(double hither, double yon)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

//	PRE(device_);
	PRE(currentCamera_);
	PRE(hither < yon);

	const double h = currentCamera_->projectionPlaneHalfHeight();
	updateProjMatrix(hither, yon, h); // This causes display problems with shadows and so one
}

///////////////////////////////////// Fog support /////////////////////////////
// There are two types of fog in Direct3D, each has a different set of states
// which must be enabled separately.  Table fog is per-pixel depth-based fog
// and is a function of the raster engine.  Alternatively, the lighting module
// can calculate fog on a per-vertex basis, alter the vertex colours and have
// them interpolated across the polygon.  Table is better.
void RenDevice::setFog(float start, float end, float dense, const RenColour& colour)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(bool, fogOn_);

	ASSERT(implies(fogOn_, caps_->supportsFog()), logic_error("Unsupported fog has been switched on."));

	if (caps_->internal()->supportsTableFog())
	{
		if (fogOn_)
		{
			fogParams_ = glm::vec3 ( start, end, dense );
		}
		else
            fogParams_ = glm::vec3 ( 0, 0, 0 );
	}
	else if (caps_->internal()->supportsVertexFog())
	{
		if (fogOn_)
		{
			fogParams_ = glm::vec3 ( start, end, dense );
		}
		else
            fogParams_ = glm::vec3 ( 0, 0, 0 );
	}

	if (fogOn_ && caps_->supportsFog())
	{
		// The fog colour needs to have the camera's filter applied to it.
		RenColour filteredCol = colour;

		if (currentCamera_)
		{
			filteredCol.r(filteredCol.r() * currentCamera_->colourFilter().r());
			filteredCol.g(filteredCol.g() * currentCamera_->colourFilter().g());
			filteredCol.b(filteredCol.b() * currentCamera_->colourFilter().b());
		}

		// How do you set the colour for the light state??
		// It looks like you have to set these parts of the render state
		// for both vertex fog and table fog.

        fogColour_ = glm::vec3 ( filteredCol.r(), filteredCol.g(), filteredCol.b() );
	}
}

void RenDevice::fogOn(float start, float end, float density)
{
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(bool, fogOn_);
	CB_DEPIMPL(float, fogStart_);
	CB_DEPIMPL(float, fogEnd_);
	CB_DEPIMPL(float, fogDensity_);

	PRE(start > 0);
	PRE(start < end);
	PRE(density >= 0 && density <= 1);

	fogStart_   = start;
	fogEnd_     = end;
	fogDensity_ = density;

	if (caps_->supportsFog())
	{
		fogOn_ = true;
		setFog(fogStart_, fogEnd_, fogDensity_, fogColour());
	}
}

void RenDevice::fogOff()
{
	CB_DEPIMPL(bool, fogOn_);

	fogOn_ = false;
	fogParams_ = glm::vec3 ( 0, 0, 0 );
}

void RenDevice::fogOn()
{
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(float, fogStart_);
	CB_DEPIMPL(float, fogEnd_);
	CB_DEPIMPL(float, fogDensity_);
	CB_DEPIMPL(bool, fogOn_);

	if (caps_->supportsFog())
	{
		fogOn_ = true;
		fogParams_ = glm::vec3 ( fogStart_, fogEnd_, fogDensity_ );
	}
}

void RenDevice::disableFog()
{
	CB_DEPIMPL(float, fogStart_);
	CB_DEPIMPL(float, fogEnd_);
	CB_DEPIMPL(float, fogDensity_);
	fogStart_ = fogParams_[0];
	if(fogParams_[1] > 0) // TODO: without this condition fog may show up unintended
        fogEnd_ = fogParams_[1];
	fogDensity_ = fogParams_[2];

	fogOff();
}

void RenDevice::overrideFog(float start, float end, float density)
{
	CB_DEPIMPL(RenCapabilities*, caps_);

	if (caps_->supportsFog())
		setFog(start, end, density, fogColour());
}

void RenDevice::overrideFog(float start, float end, float density, const RenColour& col)
{
	CB_DEPIMPL(RenCapabilities*, caps_);

	if (caps_->supportsFog())
		setFog(start, end, density, col);
}

void RenDevice::restoreFog()
{
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(float, fogStart_);
	CB_DEPIMPL(float, fogEnd_);
	CB_DEPIMPL(float, fogDensity_);
	CB_DEPIMPL(float, materialFogMultiplier_);
	CB_DEPIMPL(bool, fogOn_);

	materialFogMultiplier_ = 1.0;
	fogOn_ = true;
	if (caps_->supportsFog())
		setFog(fogStart_, fogEnd_, fogDensity_, fogColour());
}

bool RenDevice::isFogOn() const
{
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(bool, fogOn_);

	ASSERT(implies(fogOn_, caps_->supportsFog()), logic_error("Unsupported fog has been switched on."));
	return fogOn_;
}

// TBD: these don't account for the overrides.  They probably ought to return
// the values currently in use.  Rewrite?
float RenDevice::fogStart() const
{
	CB_DEPIMPL(float, fogStart_);

	return fogStart_;
}

float RenDevice::fogEnd() const
{
	CB_DEPIMPL(float, fogEnd_);

	return fogEnd_;
}

float RenDevice::fogDensity() const
{
	CB_DEPIMPL(float, fogDensity_);

	return fogDensity_;
}

// fast modifier of the fog range
void RenDevice::fogEnd( float end )
{
	CB_DEPIMPL(bool, fogOn_);
	CB_DEPIMPL(RenCapabilities*, caps_);
	CB_DEPIMPL(float, fogEnd_);

	PRE(fogOn_);

    fogEnd_ = end;
}

////////////////////////////////// Lighting support ///////////////////////////
// Everything is delegated to RenIIlluminator.
const RenColour& RenDevice::ambient() const
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	return illuminator_->ambient();
}

void RenDevice::ambient(const RenColour& a)
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	illuminator_->ambient(a);
}

void RenDevice::disableLighting()
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);
	illuminator_->disableLighting();
}

void RenDevice::enableLighting()
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);
	illuminator_->enableLighting();
}

bool RenDevice::lightingEnabled() const
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);
	return illuminator_->lightingEnabled();
}

void RenDevice::addLight(RenLight* l)
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	illuminator_->addLight(l);
}

void RenDevice::removeLight(RenLight* l)
{
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	illuminator_->removeLight(l);
}

void RenDevice::useCamera(RenCamera* cam)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	PRE(cam);
	PRE(!rendering3D());
	PRE(cam->hitherClipDistance() < cam->yonClipDistance());

	currentCamera_ = cam;
	illuminator_->filter(currentCamera_->colourFilter());
}

void RenDevice::interferenceOn(double n)
{
	CB_DEPIMPL(double, interference_);

	PRE(n >= 0 && n <= 1);
	interference_ = n;
}

void RenDevice::interferenceOff()
{
	CB_DEPIMPL(double, interference_);

	interference_ = 0;
}

double RenDevice::interferenceAmount() const
{
	CB_DEPIMPL(double, interference_);

	return interference_;
}

bool RenDevice::isInterferenceOn() const
{
	CB_DEPIMPL(double, interference_);

	return interference_ > 0.0001;
}

inline float random0to1000()
{
	static MexBasicRandom random = MexBasicRandom::constructSeededFromTime();

	return mexRandomScalar(&random, 0.0, 1000.0);
}

static void graduatedNoisePolygon(RenDevice* dev, const Ren::Rect& area, double minAlpha, double maxAlpha)
{
	static RenTexture noiseTex = RenTexManager::instance().createTexture("static.bmp");
	static RenMaterial noiseMat;
	static bool first = true;
	static RenIVertex pts[6];

	if (first)
	{
		first = false;

		noiseMat.texture(noiseTex);
		noiseMat.makeNonSharable();

		for (int i=0; i!=6; ++i)
		{
			pts[i].z		  = 0.0;
			pts[i].w          = 1.0;
			pts[i].specular   = packColourUnChecked(0,0,0);
		}
	}

    pts[0].x = pts[2].x = pts[4].x = area.originX;
	pts[1].x = pts[3].x = pts[5].x = area.originX + area.width;

	pts[0].y = pts[1].y = area.originY;
	pts[2].y = pts[3].y = area.originY + area.height / 2;
	pts[4].y = pts[5].y = area.originY + area.height;

	for (int i=0; i!=6; ++i)
	{
		pts[i].tu = float(random0to1000());
		pts[i].tv = float(random0to1000());
	}

	pts[0].color = pts[1].color = packColour(1, 1, 1, minAlpha);
	pts[2].color = pts[3].color = packColour(1, 1, 1, maxAlpha);
	pts[4].color = pts[5].color = packColour(1, 1, 1, minAlpha);
	dev->impl().setMaterialHandles(noiseMat);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	dev->renderScreenspace(pts, 6, noiseMat, GL_TRIANGLE_STRIP, area.width, area.height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

static void uniformNoisePolygon(RenDevice* dev, const Ren::Rect& area, double maxAlpha)
{
	static RenTexture noiseTex = RenTexManager::instance().createTexture("static.bmp");
	static RenMaterial noiseMat;
	static bool first = true;
    static RenIVertex pts[4];

	if (first)
	{
		first = false;

		noiseMat.texture(noiseTex);
		noiseMat.makeNonSharable();

		for (int i=0; i!=4; ++i)
		{
			pts[i].z		  = (0.0);
			pts[i].w		  = (1.0);
			pts[i].specular   = packColourUnChecked(0, 0, 0);
		}
	}
	pts[0].x = pts[2].x = area.originX;
	pts[1].x = pts[3].x = area.originX + area.width;

	pts[0].y = pts[1].y = area.originY;
	pts[2].y = pts[3].y = area.originY + area.height;

	for (int i=0; i!=4; ++i)
	{
		pts[0].color = pts[1].color = packColour(1, 1, 1, maxAlpha);
		pts[2].color = pts[3].color = packColour(1, 1, 1, maxAlpha);
		pts[i].tu = float(random0to1000());
		pts[i].tv = float(random0to1000());
	}

	dev->impl().setMaterialHandles(noiseMat);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	dev->renderScreenspace(pts, 4, noiseMat, GL_TRIANGLE_STRIP, area.width, area.height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void RenDevice::addInterference()
{
	CB_DEPIMPL(RenIViewportMapping*, vpMapping_);
	CB_DEPIMPL(double, interference_);

	static MexBasicRandom random = MexBasicRandom::constructSeededFromTime();

	// If the noise amount is small, don't do anything on one quarter of
	// occaisions (randomly).
	if (interference_ < 0.2 && mexRandomInt(&random, 0, 3) == 0)
		return;

// For OpenGL rendering viewport is always starting from 0,0
//	const int viewportXOffset = vpMapping_->screenLeft();
//	const int viewportYOffset = vpMapping_->screenTop();
	const int viewportXOffset = 0;
	const int viewportYOffset = 0;

	const int viewportWidth  = vpMapping_->screenWidth();
	const int viewportHeight = vpMapping_->screenHeight();

	const int viewportRight  = viewportWidth  + viewportXOffset;
	const int viewportBottom = viewportHeight + viewportYOffset;

	if (interference_ < 0.8)
	{
		// Use graduated noise.
		const double fractionToCover = std::min(1.0, interference_ + 0.4);
		const double fractionToClear = 1 - fractionToCover;
		const double minAlpha = std::max(0.0, interference_ - 0.6);

		// Draw noise with some 100% clear gaps.
		if (fractionToClear > 0.0)
		{
			double fractionCovered = 0, fractionCleared = 0;
			int lastHeight = viewportYOffset - 0.15*viewportHeight;

			for (int i=0; i!=3 && lastHeight < viewportBottom; ++i)
			{
				const double toCoverRemaining = std::max(0.0, fractionToCover - fractionCovered);
				const double toClearRemaining = std::max(0.0, fractionToClear - fractionCleared);
				const double coverFraction = mexRandomScalar(&random, 0.0, toCoverRemaining);
				const double clearFraction = mexRandomScalar(&random, 0.0, toClearRemaining);

				if (toCoverRemaining <= 0)
					return;

				fractionCovered += coverFraction;
				fractionCleared += clearFraction;
				const int top = lastHeight + (int)(clearFraction * viewportHeight);
				const int bottom = std::min(viewportBottom, top + (int)(coverFraction * viewportHeight));
				lastHeight = bottom;

				Ren::Rect rect1(viewportXOffset, top, viewportWidth, bottom);
				graduatedNoisePolygon(this, rect1, minAlpha, interference_);
			}
		}
		else	// Enirely cover the screen, but vary the alpha.
		{
			const int coveredMinHeight = (int) (viewportHeight * 0.2);
			const int coveredMaxHeight = (int) (viewportHeight * 0.4);

			// The lower alpha value should increase smoothly with noise until
			// at 0.8 it matches the upper value.  Hence we transition
			// seamlessly into the uniform case.
			const double alpha = mexInterpolate(interference_, 0.6, 0.8, minAlpha, interference_);

			int lastHeight = viewportYOffset;
			for (int i=0; i!=2 && lastHeight < viewportBottom; ++i)
			{
				const int coveredHeight = mexRandomInt(&random, coveredMinHeight, coveredMaxHeight);
				const int top = lastHeight;
				const int bottom = std::min(viewportBottom, top + coveredHeight);
				lastHeight = bottom;

				Ren::Rect rect1(viewportXOffset, top, viewportWidth, bottom);
				graduatedNoisePolygon(this, rect1, alpha, interference_);
			}

			// Ensure that we go right to the bottom of the screen.
			if (lastHeight < viewportBottom)
			{
				Ren::Rect rect1(viewportXOffset, lastHeight, viewportWidth, viewportBottom);
				graduatedNoisePolygon(this, rect1, alpha, interference_);
			}
		}
	}
	else	// Use uniform noise rather than graduated.
	{
		Ren::Rect rect1(viewportXOffset, viewportYOffset, viewportWidth, viewportHeight);
		uniformNoisePolygon(this, rect1, interference_);
	}
}

void RenDevice::staticOn()
{
	CB_DEPIMPL(bool, staticOn_);

	staticOn_ = true;
}

void RenDevice::staticOff()
{
	CB_DEPIMPL(bool, staticOn_);

	staticOn_ = false;
}

bool RenDevice::isStaticOn() const
{
	CB_DEPIMPL(bool, staticOn_);

	return staticOn_;
}

void RenDevice::addStatic()
{
	CB_DEPIMPL(RenIViewportMapping*, vpMapping_);
	CB_DEPIMPL(RenCapabilities*, caps_);

	// These aren't static because the viewport can change size in-game.
	// Due to line drawing bugs on some cards (see RenILinesDiagnostic), we
	// avoid a 1 pixel boundary at the edges of the viewport.
	const int viewportXOffset = vpMapping_->screenLeft() + 1;
	const int viewportYOffset = vpMapping_->screenTop()  + 1;

	const int viewportWidth  = vpMapping_->screenWidth()  - 2;
	const int viewportHeight = vpMapping_->screenHeight() - 2;

	const int viewportRight  = viewportWidth  + viewportXOffset;
	const int viewportBottom = viewportHeight + viewportYOffset;

    static const uint numVertices = 300;
	static RenIVertex pts[numVertices];
	static bool first = true;
	static MexBasicRandom random = MexBasicRandom::constructSeededFromTime();

	// The colours, UV coords, depth info and even the alpha values can
	// be set once.  Only the xy coords should vary randomly between frames.
	if (first)
	{
		first = false;

		for (int i = 0; i != numVertices; i += 2)
		{
			const int x1 = (viewportWidth  * rand() / RAND_MAX ) + viewportXOffset;
			pts[i].x = (x1);

			//get random number from 2 to 13 for line length
			const int lineLength = mexRandomInt(&random, 2, 9);

			const int x2 = x1 + lineLength;

			if (x2 >= viewportRight)
				pts[i+1].x = viewportRight-1;
			else
				pts[i+1].x = x2;

			const double green = mexRandomScalar(&random, 0.6, 1.0);
			const double alpha = mexRandomScalar(&random, 0.3, 0.9);
			pts[i].z		  = pts[i+1].z		  = (0);
			pts[i].w		  = pts[i+1].w  	  = (1);
			pts[i].color      = pts[i+1].color    = packColour(0, green, 0, alpha);
			pts[i].specular   = pts[i+1].specular = packColourUnChecked(0, 0, 0);
			pts[i].tu		  = pts[i+1].tu	  = (0);
			pts[i].tv		  = pts[i+1].tv	  = (0);
		}
	}

	for (int i = 0; i != numVertices; i += 2)
	{
		// Randomize the y coordinates.
		pts[i].y = float( (viewportHeight * rand() / RAND_MAX ) + viewportYOffset );

		// All lines are horizontal.
		pts[i+1].y = pts[i].y;
	}

	disableFog();

    renderScreenspace(pts, numVertices, GL_LINES, viewportWidth, viewportHeight);
	restoreFog();
}

const RenColour& RenDevice::backgroundColour() const
{
	CB_DEPIMPL(RenColour, background_);

	return background_;
}

void RenDevice::backgroundColour(const RenColour& b)
{
	CB_DEPIMPL(RenColour, background_);

	background_ = b;
}

int RenDevice::windowWidth() const
{
	//return backSurface().width();
	return display()->currentMode().width();
}

int RenDevice::windowHeight() const
{
	//return backSurface().height();
	return display()->currentMode().height();
}

// static
void RenDevice::useDevice(RenDevice* d)
{
	PRE(d);
	RenIDeviceImpl::useDevice(d, RenI::LAZY_UPDATE);
}

void RenDevice::displayImage(const SysPathName& pathName)
{
	CB_DEPIMPL(RenDisplay*, display_);

	RenSurface backBuf = backSurface();
	RenISurfBody surf;
	surf.read(pathName.pathname());

	Ren::Rect srcArea;
	srcArea.originX = srcArea.originY = 0;
	srcArea.width = surf.width();
	srcArea.height = surf.height();
	renderSurface(&surf, srcArea, srcArea);
	display_->flipBuffers();
	// This image is no longer needed
	surf.releaseDC();

}

bool RenDevice::canSee(const MexPoint3d& pt ) const
{
    // Originally there was a DX call like viewport_->TransformVertices(1, &txData, D3DTRANSFORM_CLIPPED, &offScreen);
    return true;
}

bool RenDevice::canSee(const MexQuad3d& quad) const
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

	PRE(currentCamera());

    // TODO: same value all the time may cause some problems?
	bool retval = true;

	return retval;
}

ostream& operator <<( ostream& o, const RenDevice& t )
{

    o << "RenDevice " << (void*)&t << " start" << std::endl;
    o << "RenDevice " << (void*)&t << " end" << std::endl;

    return o;
}

void RenDevice::CLASS_INVARIANT
{
	CB_REN_DEVICE_DEPIMPL;

    INVARIANT(this != NULL);
    INVARIANT(coplanarSorter_);
    INVARIANT(normalAlphaSorter_);

	// There is always a current device.
    INVARIANT(current());

	// These are sub-parts: a device must always own one of each.
    INVARIANT(caps_);
    INVARIANT(illuminator_);
//    INVARIANT(projViewMatrix_);
}

bool RenDevice::activate()
{
	CB_DEPIMPL(int, surfacesMayBeLost_);

	// Setting this flag makes startFrame try to restore the surfaces.
	surfacesMayBeLost_ = 10;
	return true;
}

void RenDevice::updateDisplay()
{
}


bool RenDevice::setHighestAllowedDisplayMode()
{
	CB_REN_DEVICE_DEPIMPL;

	PRE(display_ );
//	PRE( caps_ );

	return display_->setHighestAllowedMode( caps_->maxAvailableDisplayMemoryAfterTextures() );
}

MexPoint3d RenDevice::screenToCamera( const MexPoint2d& screenPosition ) const
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

    PRE( currentCamera_ != NULL );

    //Get needed data from the camera
    double projectionDistance = currentCamera_->hitherClipDistance();
    double tanHalfVerticalFOVAngle = currentCamera_->tanHalfVerticalFOVAngle();

    //Get the viewport dimensions
    GLint viewPort[4];
    glGetIntegerv( GL_VIEWPORT, viewPort );
    double viewportPixelLeft( viewPort[0] );
    double viewportPixelTop( viewPort[1] );
    double viewportPixelWidth( viewPort[2] );
    double viewportPixelHeight( viewPort[3] );

    //Compute the viewport dimensions in real world coordinates
    double realViewportHeight = 2.0 * projectionDistance * tanHalfVerticalFOVAngle;
    double realViewportWidth = realViewportHeight * (viewportPixelWidth / viewportPixelHeight);

    //Compute the coordinates in world space in the projection plane of the screen position
    MATHEX_SCALAR y = realViewportWidth *
                      ((screenPosition.x() - viewportPixelLeft) / viewportPixelWidth - 0.5);
    MATHEX_SCALAR z = realViewportHeight *
                      (0.5 - (screenPosition.y() - viewportPixelTop) / viewportPixelHeight);

    return MexPoint3d( projectionDistance, y, z );
}

RenSurface RenDevice::backSurface()
{
 	CB_DEPIMPL(RenDisplay*, display_);
 	CB_DEPIMPL(RenSurface*, surfBackBuf_);

 	// Only get the surface once from the manager.  (This method is likely to
 	// be called many times.)
 	if (!surfBackBuf_)
 	{
 		surfBackBuf_ = _NEW(RenSurface);
		RenISurfaceManagerImpl& surfMan = RenSurfaceManager::instance().impl();
		*surfBackBuf_ = surfMan.createDisplaySurface(this, RenI::BACK);
 	}

 	return *surfBackBuf_;
}

RenSurface RenDevice::frontSurface()
{
	CB_DEPIMPL(RenDisplay*, display_);
	CB_DEPIMPL(RenSurface*, surfFrontBuf_);

	// Only get the surface once from the manager.  (This method is likely to
	// be called many times.)
	if (!surfFrontBuf_)
	{
		surfFrontBuf_ = _NEW(RenSurface);
		RenISurfaceManagerImpl& surfMan = RenSurfaceManager::instance().impl();
		*surfFrontBuf_ = surfMan.createDisplaySurface(this, RenI::FRONT);
	}

	return *surfFrontBuf_;
}

const RenSurface RenDevice::backSurface() const
{
	RenDevice* ncThis = _CONST_CAST(RenDevice*, this);
	return ncThis->backSurface();
}

const RenSurface RenDevice::frontSurface() const
{
	RenDevice* ncThis = _CONST_CAST(RenDevice*, this);
	return ncThis->frontSurface();
}

MexPoint2d RenDevice::cameraToScreen( const MexPoint3d& worldPosition ) const
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

    PRE( currentCamera_ != NULL );

    //Get needed data from the camera
    double projectionDistance = currentCamera_->hitherClipDistance();
    double tanHalfVerticalFOVAngle = currentCamera_->tanHalfVerticalFOVAngle();

    //Get the viewport dimensions
    GLint viewPort[4];
    glGetIntegerv( GL_VIEWPORT, viewPort );
    double viewportPixelLeft( viewPort[0] );
    double viewportPixelTop( viewPort[1] );
    double viewportPixelWidth( viewPort[2] );
    double viewportPixelHeight( viewPort[3] );

    //Compute the viewport dimensions in real world coordinates
    double realViewportHeight = 2.0 * projectionDistance * tanHalfVerticalFOVAngle;
    double realViewportWidth = realViewportHeight * (viewportPixelWidth / viewportPixelHeight);

    //Do the perspective projection of the point into the projection plane.
    //If the point lies in the camera's x=0 plane, frig it a bit.
    MATHEX_SCALAR x = worldPosition.x();
    if( fabs( x ) < 0.001 )
        x = ( x >= 0.0 ? 0.001 : - 0.001 );
    MATHEX_SCALAR f = projectionDistance / x;
    MATHEX_SCALAR y = worldPosition.y() * f;
    MATHEX_SCALAR z = worldPosition.z() * f;

    //Now convert to screen pixel coordinates
    y = viewportPixelWidth * ( y / realViewportWidth + 0.5 ) + viewportPixelLeft;
    z = viewportPixelHeight * ( 0.5 - z / realViewportHeight ) + viewportPixelTop;

    return MexPoint2d( y, z );
}

RenDevice::Filter::Filter(RenDevice* pDevice, const RenColour& col)
:pDevice_( pDevice )
{
	pDevice_->setFilter( col );
}

RenDevice::Filter::~Filter()
{
	pDevice_->resetFilter();
}

void RenDevice::setFilter(const RenColour& col)
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	const RenColour& camFilter = currentCamera_->colourFilter();

	RenColour newColour(camFilter.r()*col.r(), camFilter.g()*col.g(), camFilter.b()*col.b() );
	illuminator_->filter( newColour );
}

void RenDevice::resetFilter()
{
	CB_DEPIMPL(RenCamera*, currentCamera_);
	CB_DEPIMPL(RenIIlluminator*, illuminator_);

	illuminator_->filter( currentCamera_->colourFilter() );
}

RenCamera* RenDevice::currentCamera() const
{
	CB_DEPIMPL(RenCamera*, currentCamera_);

	return currentCamera_;
}

// static
RenDevice* RenDevice::current()
{
	return RenIDeviceImpl::current();
}

bool RenDevice::rendering() const
{
	CB_DEPIMPL(bool, rendering_);

	return rendering_;
}

bool RenDevice::rendering2D( void ) const
{
	CB_DEPIMPL(bool, rendering2D_);

	return rendering2D_;
}

bool RenDevice::rendering3D() const
{
	CB_DEPIMPL(bool, rendering3D_);

	return rendering3D_;
}

bool RenDevice::idleRendering( void ) const
{
	return rendering() and not rendering3D() and not rendering2D();
}

const RenCapabilities& RenDevice::capabilities() const
{
	CB_DEPIMPL(RenCapabilities*, caps_);

	PRE(caps_);			// This is checked by the class invariant.
	return *caps_;
}

RenStats* RenDevice::statistics()
{
	CB_DEPIMPL(RenStats*, stats_);

	return stats_;
}

const RenStats* RenDevice::statistics() const
{
	CB_DEPIMPL(RenStats*, stats_);

	return stats_;
}

RenDisplay* RenDevice::display()
{
	return pImpl_->display_;
}

const RenDisplay* RenDevice::display() const
{
	return pImpl_->display_;
}

const RenColour& RenDevice::fogColour( void ) const
{
	CB_DEPIMPL(RenColour, fogColour_);

	return fogColour_;
}

void RenDevice::fogColour(const RenColour& newFogColour)
{
	CB_DEPIMPL(RenColour, fogColour_);

	fogColour_ = newFogColour;
}

ostream& RenDevice::out()
{
	PRE(pImpl_);
	return pImpl_->extOut_;
}

RenIDeviceImpl& RenDevice::impl()
{
	PRE(pImpl_);
	return *pImpl_;
}

const RenIDeviceImpl& RenDevice::impl( void ) const
{
	PRE(pImpl_);
	return *pImpl_;
}

void RenDevice::debugTextCoords(int x, int y)
{
	pImpl_->debugX_ = x;
	pImpl_->debugY_ = y;
}

void RenDevice::debugTextCoords(int* pX, int* pY) const
{
	PRE(pX && pY);
	*pX = pImpl_->debugX_;
	*pY = pImpl_->debugY_;
}

void RenDevice::antiAliasingOn(bool o)
{
	pImpl_->antiAliasingOn_ = o;
}

bool RenDevice::antiAliasingOn() const
{
	return pImpl_->antiAliasingOn_;
}

void RenDevice::horizontalBannerText(const char* /*text*/, int /*nChars*/, double /*charsPerSecond*/)
{
}

void RenDevice::setMaterialHandles(const RenMaterial& mat)
{
    pImpl_->setMaterialHandles(mat);
}

void RenDevice::renderToTextureMode(const GLuint targetTexture, uint viewPortW, uint viewPortH)
{
    static GLint lastFrameBuff = 0;
    // Bind FBO to texture
    if(targetTexture)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFrameBuff);
        glBindFramebuffer(GL_FRAMEBUFFER, glOffscreenFrameBuffID_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0);
        glViewport(0, 0, viewPortW, viewPortH);
    }
    // Bind FBO to screen
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, lastFrameBuff);
        const RenDisplay::Mode& mode = pImpl_->display()->currentMode();
        glViewport(0, 0, mode.width(), mode.height());
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error when binding FBO" << std::endl;
}
void RenDevice::renderScreenspace
(
    const RenIVertex* vertices,
    const size_t nVertices,
    GLenum mode,
    const int targetW,
    const int targetH,
    const GLuint texture
)
{
    glUseProgram(glProgramID_GIU2D_);

    // Bind texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(gl2DUniformID_, 0);

	glUniform2f(glScreenspaceID_, (float)targetW, (float)targetH);

    glBindBuffer(GL_ARRAY_BUFFER, gl2DVertexBufferID_);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(RenIVertex), &vertices[0], GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(glVertexPosition_screenspaceID_);
	glBindBuffer(GL_ARRAY_BUFFER, gl2DVertexBufferID_);
	glVertexAttribPointer(glVertexPosition_screenspaceID_, 2, GL_FLOAT, GL_FALSE, sizeof(RenIVertex), (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(glVertexUVID_);
	glVertexAttribPointer(glVertexUVID_, 2, GL_FLOAT, GL_FALSE, sizeof(RenIVertex),           // stride
        (void*)(sizeof(RenIVertex)-2*sizeof(float)) );

    // 3rd attribute vertex colours
    glEnableVertexAttribArray(glVertexColour_screenspaceID_ );
    glVertexAttribPointer(
        glVertexColour_screenspaceID_ ,    // The attribute we want to configure
        4,                            // size
        GL_UNSIGNED_BYTE,             // type
        GL_TRUE,                     // normalized?
        sizeof(RenIVertex),            // stride
        (void*)(3*sizeof(float)+sizeof(uint)) // array buffer offset
    );

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(mode, 0, nVertices );

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

	glDisableVertexAttribArray(glVertexPosition_screenspaceID_);
	glDisableVertexAttribArray(glVertexUVID_);
	glDisableVertexAttribArray(glVertexColour_screenspaceID_ );
}

void RenDevice::renderSurface
(
    const RenISurfBody* surf,
    const Ren::Rect& srcArea,
    const Ren::Rect& dstArea,
    const uint targetW,
    const uint targetH,
    const uint colour
)
{
    RenIVertex vertices[6];

    glm::vec2 vertex_up_left    = glm::vec2(dstArea.originX,                dstArea.originY+dstArea.height); //( x     , y+srcArea.height );
    glm::vec2 vertex_up_right   = glm::vec2(dstArea.originX+dstArea.width,  dstArea.originY+dstArea.height); //( x+srcArea.width, y+srcArea.height );
    glm::vec2 vertex_down_right = glm::vec2(dstArea.originX+dstArea.width,  dstArea.originY); //( x+srcArea.width, y      );
    glm::vec2 vertex_down_left  = glm::vec2(dstArea.originX,                dstArea.originY); //( x, y      );

    // TODO handle empty surf
    float uvX = srcArea.originX / (float)surf->width();
    float uvY = srcArea.originY / (float)surf->height();
    float uvW = srcArea.width   / (float)surf->width();
    float uvH = srcArea.height  / (float)surf->height();
    glm::vec2 uv_up_left    = glm::vec2(uvX,        uvY+uvH); //( 0.0f, 1.0f );
    glm::vec2 uv_up_right   = glm::vec2(uvX+uvW,    uvY+uvH); //( 1.0f, 1.0f );
    glm::vec2 uv_down_right = glm::vec2(uvX+uvW,    uvY); //( 1.0f, 0.0f );
    glm::vec2 uv_down_left  = glm::vec2(uvX,        uvY); //( 0.0f, 0.0f );

    // Bind shader
    glUseProgram(glProgramID_GIU2D_);

    vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = vertices[4].color = vertices[5].color = colour;
    if(targetW)
    {
        glUniform2f(glScreenspaceID_, (float)targetW, -(float)targetH);

        vertices[2].x = vertex_up_left[0]; vertices[2].y = vertex_up_left[1];
        vertices[1].x = vertex_down_left[0]; vertices[1].y = vertex_down_left[1];
        vertices[0].x = vertex_up_right[0]; vertices[0].y = vertex_up_right[1];

        vertices[5].x = vertex_down_right[0]; vertices[5].y = vertex_down_right[1];
        vertices[4].x = vertex_up_right[0]; vertices[4].y = vertex_up_right[1];
        vertices[3].x = vertex_down_left[0]; vertices[3].y = vertex_down_left[1];

        vertices[2].tu = uv_up_left[0]; vertices[2].tv = uv_up_left[1];
        vertices[1].tu = uv_down_left[0]; vertices[1].tv = uv_down_left[1];
        vertices[0].tu = uv_up_right[0]; vertices[0].tv = uv_up_right[1];

        vertices[5].tu = uv_down_right[0]; vertices[5].tv = uv_down_right[1];
        vertices[4].tu = uv_up_right[0]; vertices[4].tv = uv_up_right[1];
        vertices[3].tu = uv_down_left[0]; vertices[3].tv = uv_down_left[1];
    }
    else
    {
        const RenDisplay::Mode& mode = pImpl_->display()->currentMode();
        glUniform2f(glScreenspaceID_, (float)mode.width(), (float)mode.height());

        vertices[0].x = vertex_up_left[0]; vertices[0].y = vertex_up_left[1];
        vertices[1].x = vertex_down_left[0]; vertices[1].y = vertex_down_left[1];
        vertices[2].x = vertex_up_right[0]; vertices[2].y = vertex_up_right[1];

        vertices[3].x = vertex_down_right[0]; vertices[3].y = vertex_down_right[1];
        vertices[4].x = vertex_up_right[0]; vertices[4].y = vertex_up_right[1];
        vertices[5].x = vertex_down_left[0]; vertices[5].y = vertex_down_left[1];

        vertices[0].tu = uv_up_left[0]; vertices[0].tv = uv_up_left[1];
        vertices[1].tu = uv_down_left[0]; vertices[1].tv = uv_down_left[1];
        vertices[2].tu = uv_up_right[0]; vertices[2].tv = uv_up_right[1];

        vertices[3].tu = uv_down_right[0]; vertices[3].tv = uv_down_right[1];
        vertices[4].tu = uv_up_right[0]; vertices[4].tv = uv_up_right[1];
        vertices[5].tu = uv_down_left[0]; vertices[5].tv = uv_down_left[1];
    }

	glBindBuffer(GL_ARRAY_BUFFER, gl2DVertexBufferID_);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(RenIVertex), &vertices[0], GL_STATIC_DRAW);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	if (surf->isEmpty())
        glBindTexture(GL_TEXTURE_2D, glTextureEmptyID_);
    else
        glBindTexture(GL_TEXTURE_2D, surf->handle());
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(gl2DUniformID_, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(glVertexPosition_screenspaceID_);
	glBindBuffer(GL_ARRAY_BUFFER, gl2DVertexBufferID_);
	glVertexAttribPointer(glVertexPosition_screenspaceID_, 2, GL_FLOAT, GL_FALSE, sizeof(RenIVertex), (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(glVertexUVID_);
	glVertexAttribPointer(glVertexUVID_, 2, GL_FLOAT, GL_FALSE, sizeof(RenIVertex),           // stride
        (void*)(sizeof(RenIVertex)-2*sizeof(float)) );

    // 3rd attribute vertex colours
    glEnableVertexAttribArray(glVertexColour_screenspaceID_ );
    glVertexAttribPointer(
        glVertexColour_screenspaceID_ ,    // The attribute we want to configure
        4,                            // size
        GL_UNSIGNED_BYTE,             // type
        GL_TRUE,                     // normalized?
        sizeof(RenIVertex),            // stride
        (void*)(3*sizeof(float)+sizeof(uint)) // array buffer offset
    );

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, 6 );

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

	glDisableVertexAttribArray(glVertexPosition_screenspaceID_);
	glDisableVertexAttribArray(glVertexUVID_);
	glDisableVertexAttribArray(glVertexColour_screenspaceID_ );
}

void RenDevice::renderPrimitive
(
    const RenIVertex* vertices,
    const size_t nVertices,
    const RenMaterial& mat,
    const GLenum mode
)
{
    PRE(vertices);
    PRE(nVertices < 5000);

    // Use our shader
    glUseProgram(glProgramID_Standard_);

    CB_DEPIMPL(glm::mat4*, projViewMatrix_);
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
//    glm::mat4 mvp = (*projViewMatrix_) * model_;
    glUniformMatrix4fv(glModelMatrixID_, 1, GL_FALSE, &model_[0][0]);
    glUniformMatrix4fv(glViewMatrixID_, 1, GL_FALSE, &view_[0][0]);
    glUniformMatrix4fv(glProjectionMatrixID_, 1, GL_FALSE, &projection_[0][0]);

    glUniform3fv(glFogColourID_, 1, &fogColour_[0]);
    glUniform3fv(glFogParamsID_, 1, &fogParams_[0]);
    //std::cout<<glm::to_string(MVP)<<std::endl;
    //glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    //glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    //glm::vec3 lightPos = glm::vec3(4,4,4);
    //glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    if (mat.texture().empty())
        glBindTexture(GL_TEXTURE_2D, glTextureEmptyID_);
    else
        glBindTexture(GL_TEXTURE_2D, mat.texture().handle());

    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(glTextureSamplerID_, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(glVertexPosition_modelspaceID_);
    glBindBuffer(GL_ARRAY_BUFFER, glVertexDataBufferID_);
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(RenIVertex), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        glVertexPosition_modelspaceID_,  // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)0                      // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(glVertex_modelspaceUVID_);
    glVertexAttribPointer(
        glVertex_modelspaceUVID_,                   // The attribute we want to configure
        2,                            // size : U+V => 2
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)(sizeof(RenIVertex)-2*sizeof(float))          // array buffer offset
    );

    // vertex colours
    glEnableVertexAttribArray(glVertexColour_modelspaceID_);
    glVertexAttribPointer(
        glVertexColour_modelspaceID_,    // The attribute we want to configure
        4,                            // size
        GL_UNSIGNED_BYTE,             // type
        GL_TRUE,                     // normalized?
        sizeof(RenIVertex),            // stride
        (void*)(3*sizeof(float)+sizeof(uint)) // array buffer offset
    );

    /*// 3rd attribute buffer : normals
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        vertexNormal_modelspaceID,    // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );
*/
    // Draw the triangles !
    glDrawArrays(mode, 0, nVertices );

    glDisableVertexAttribArray(glVertexPosition_modelspaceID_);
    glDisableVertexAttribArray(glVertex_modelspaceUVID_);
    glDisableVertexAttribArray(glVertexColour_modelspaceID_);
    //glDisableVertexAttribArray(vertexNormal_modelspaceID);

}

void RenDevice::renderIndexed
(
    const RenIVertex* vertices,
    const size_t nVertices,
    const Ren::VertexIdx* indices,
    const size_t nIndices,
    const RenMaterial& mat,
    GLenum mode
)
{
    PRE(vertices);
    PRE(indices);
    PRE(nVertices < 5000);
    PRE(nIndices < 5000);

    CB_DEPIMPL(glm::mat4*, projViewMatrix_);

    glUseProgram(glProgramID_Standard_);
    // Compute the MVP matrix from keyboard and mouse input

//    glm::mat4 MVP = (*projViewMatrix_) * model_;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(glModelMatrixID_, 1, GL_FALSE, &model_[0][0]);
    glUniformMatrix4fv(glViewMatrixID_, 1, GL_FALSE, &view_[0][0]);
    glUniformMatrix4fv(glProjectionMatrixID_, 1, GL_FALSE, &projection_[0][0]);

    glUniform3fv(glFogColourID_, 1, &fogColour_[0]);
    glUniform3fv(glFogParamsID_, 1, &fogParams_[0]);
    /*glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4,4,4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);*/

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    if (mat.texture().empty())
        glBindTexture(GL_TEXTURE_2D, glTextureEmptyID_);
    else
        glBindTexture(GL_TEXTURE_2D, mat.texture().handle());
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(glTextureSamplerID_, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(glVertexPosition_modelspaceID_);
    glBindBuffer(GL_ARRAY_BUFFER, glVertexDataBufferID_);
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(RenIVertex), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        glVertexPosition_modelspaceID_,  // The attribute we want to configure
        3,                            // size - 3 for XYZ 4 for XYZW
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)0                      // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(glVertex_modelspaceUVID_);
    glVertexAttribPointer(
        glVertex_modelspaceUVID_,                   // The attribute we want to configure
        2,                            // size : U+V => 2
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)(sizeof(RenIVertex)-2*sizeof(float))          // array buffer offset
    );

    // vertex colours
    glEnableVertexAttribArray(glVertexColour_modelspaceID_);
    glVertexAttribPointer(
        glVertexColour_modelspaceID_,    // The attribute we want to configure
        4,                            // size
        GL_UNSIGNED_BYTE,             // type
        GL_TRUE,                     // normalized?
        sizeof(RenIVertex),            // stride
        (void*)(3*sizeof(float)+sizeof(uint)) // array buffer offset
    );

    // 3rd attribute buffer : normals
    /*glEnableVertexAttribArray(vertexNormal_modelspaceID);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        vertexNormal_modelspaceID,    // The attribute we want to configure
        3,                            // size
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        0,                            // stride
        (void*)0                      // array buffer offset
    );*/

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glElementBufferID_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned short), indices , GL_STATIC_DRAW);

    // Draw the triangles !
    glDrawElements(
        mode,      // mode
        nIndices,    // count
        GL_UNSIGNED_SHORT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(glVertexPosition_modelspaceID_);
    glDisableVertexAttribArray(glVertex_modelspaceUVID_);
    glDisableVertexAttribArray(glVertexColour_modelspaceID_);
    //glDisableVertexAttribArray(vertexNormal_modelspaceID);
}

void RenDevice::renderIndexedScreenspace
(
    const RenIVertex* vertices,
    const size_t nVertices,
    const Ren::VertexIdx* indices,
    const size_t nIndices,
    const RenMaterial& mat,
    GLenum mode
)
{
    PRE(vertices);
    PRE(indices);
    PRE(nVertices < 5000);
    PRE(nIndices < 5000);

    CB_DEPIMPL(glm::mat4*, projViewMatrix_);

    glUseProgram(glProgramID_Billboard_);
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    if (mat.texture().empty())
        glBindTexture(GL_TEXTURE_2D, glTextureEmptyID_);
    else
        glBindTexture(GL_TEXTURE_2D, mat.texture().handle());

    glUniformMatrix4fv(glViewProjMatrix_BillboardID_, 1, GL_FALSE, &(*projViewMatrix_)[0][0]);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(glTextureSamplerBillboardID_, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(glVertexPosition_BillboardID_);
    glBindBuffer(GL_ARRAY_BUFFER, glVertexDataBufferBillboardID_);
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(RenIVertex), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        glVertexPosition_BillboardID_,  // The attribute we want to configure
        4,                            // size - 3 for XYZ 4 for XYZW
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)0                      // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(glVertex_BillboardUVID_);
    glVertexAttribPointer(
        glVertex_BillboardUVID_,                   // The attribute we want to configure
        2,                            // size : U+V => 2
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        sizeof(RenIVertex),           // stride
        (void*)(sizeof(RenIVertex)-2*sizeof(float))          // array buffer offset
    );

    // vertex colours
    glEnableVertexAttribArray(glVertexColour_BillboardID_);
    glVertexAttribPointer(
        glVertexColour_BillboardID_,    // The attribute we want to configure
        4,                            // size
        GL_UNSIGNED_BYTE,             // type
        GL_TRUE,                     // normalized?
        sizeof(RenIVertex),            // stride
        (void*)(3*sizeof(float)+sizeof(uint)) // array buffer offset
    );


    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glElementBufferBillboardID_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned short), indices , GL_STATIC_DRAW);

    // Draw the triangles !
    glDrawElements(
        mode,      // mode
        nIndices,    // count
        GL_UNSIGNED_SHORT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(glVertexPosition_BillboardID_);
    glDisableVertexAttribArray(glVertex_BillboardUVID_);
    glDisableVertexAttribArray(glVertexColour_BillboardID_);
}

#pragma GCC diagnostic pop
/* End DEVICE.CPP ***************************************************/
