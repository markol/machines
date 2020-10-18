/***********************************************************

  WARNING: This version of Rentest is modified to use
      stars.

***********************************************************/

#include "rentest/d3dapp.hpp"

#include <stdio.h>
#include <math.h>
//#include <env>					// for setenv
//#include <windows.h>
#include <sstream>
#include <algorithm>

#include "utility/time.hpp"

#include "system/pathname.hpp"

#include "device/time.hpp"
#include "device/mouse.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/keytrans.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/coordsys.hpp"
#include "mathex/degrees.hpp"
#include "mathex/radians.hpp"

#include "phys/mcfly.hpp"

#include "render/display.hpp"
#include "render/device.hpp"
#include "render/light.hpp"
#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/ttfpoly.hpp"
#include "render/ttftris.hpp"
#include "render/spinpoly.hpp"
#include "render/surface.hpp"
#include "render/fixdcurs.hpp"
#include "render/animcurs.hpp"
#include "render/uvtrans.hpp"
#include "render/scale.hpp"
#include "render/capable.hpp"
#include "render/vertex.hpp"
#include "render/hsv.hpp"
#include "render/envirnmt.hpp"
#include "render/stars.hpp"
#include "render/stats.hpp"
#include "render/drivsel.hpp"

#include "afx/resource.hpp"
#include "afx/castfns.hpp"

#include "utility/percent.hpp"

#include "profiler/profiler.hpp"

#include "system/registry.hpp"

static float randomOffsets[1000], randomRates[1000];
//extern int globalCoplanarZbias;
int globalCoplanarZbias;

class EnvNVGMatTransform : public RenMaterialTransform
{
public:
	EnvNVGMatTransform(float diffuseMult, float emissiveMult);
	virtual void transform(const RenMaterial& in, RenMaterial* out) const;
	virtual ~EnvNVGMatTransform()		{}
private:
	const float		difMult_, emiMult_;
};

EnvNVGMatTransform::EnvNVGMatTransform(float diffuseMult, float emissiveMult):
	difMult_(0.3333 * diffuseMult),
	emiMult_(0.3333 * emissiveMult)
{
}

// virtual
void EnvNVGMatTransform::transform(const RenMaterial& in, RenMaterial* out) const
{
	RenColour dif = in.diffuse();
	const float newDif = difMult_ * (dif.r() + dif.g() + dif.b());
	dif.g((newDif > 1)? 1: newDif);
	dif.r(0);
	dif.b(0);
	out->diffuse(dif);

	RenColour emi = in.emissive();
	const float newEmi = emiMult_ * (emi.r() + emi.g() + emi.b());
	emi.g((newEmi > 1)? 1: newEmi);
	emi.r(0);
	emi.b(0);
	out->emissive(emi);
}

D3DApp::D3DApp():
	models_(DevKey::KEY_M),
	lights_(DevKey::KEY_L),
	winWidth_(400),
	winHeight_(300),
	display_(NULL),
	device_(NULL),
	camera_(NULL),
	cursor_(NULL),
	eyeControl_(NULL),
	clearBack_(true),
	doFog_(false),
	nvgOn_(false),
	fogStart_(1),
	fogEnd_(100),
	fogDensity_(0.1),
	initialised_(false),
	testTimer_(0),
	matVec_(NULL),
	ttf_(NULL),
	pStars_(0)
{
}

D3DApp::~D3DApp()
{
}

int mouseSleepTime = 0, fontSize = 5;
//extern ostream* renDumpMeshStream;

// Switch to a different mode, then back to the original without doing any
// rendering or such in the meantime.
static void modeSwitchTest(RenDisplay* display)
{
	const RenDisplay::Mode original = display->currentMode();
	RENDER_STREAM("Initial " << original << "\n");

	const ctl_list<RenDisplay::Mode>& modes = display->modeList();
	ctl_list<RenDisplay::Mode>::const_iterator it = find(modes.begin(), modes.end(), original);

	if (it != modes.end())
	{
		if (it != modes.begin())
			--it;

		while (it != modes.begin() && (*it).bitDepth() != 16)
			--it;
	}

	RENDER_STREAM("Found test mode " << (*it) << "\n");

	if (it != modes.end() && !(*it == original) && (*it).bitDepth() == 16)
	{
		display->useMode(*it);

		const RenDisplay::Mode temp = display->currentMode();
		RENDER_STREAM("Swtiched to " << temp << "\n");
	}

	display->useMode(original);

	const RenDisplay::Mode temp = display->currentMode();
	RENDER_STREAM("Reverted to " << temp << "\n");
}

static void switchMode(RenDisplay* display, RenDevice* device, int direction)
{
	PRE(direction == 1 || direction == -1);

	RENDER_STREAM("\nSwitching modes:\n");
	UtlDebugTimer timer(UtlDebugTimer::RUNNING);

	const RenDisplay::Mode current = display->currentMode();
	const RenDisplay::Mode lowest = display->lowestAllowedMode();
	const RenDisplay::Mode highest = display->highestAllowedMode();
	const ctl_list<RenDisplay::Mode>& modes = display->modeList();

	ctl_list<RenDisplay::Mode>::const_iterator it = find(modes.begin(), modes.end(), current);
	ctl_list<RenDisplay::Mode>::const_iterator itLowest = find(modes.begin(), modes.end(), lowest);
	ctl_list<RenDisplay::Mode>::const_iterator itHighest = find(modes.begin(), modes.end(), highest);

	if (it != itHighest)
	{
		if (direction == 1)
		{
			++it;

			while (it != itHighest && (*it).bitDepth() != 16)
				++it;
		}
		else
		{
			if (it != itLowest)
				--it;

			while (it != itLowest && (*it).bitDepth() != 16)
				--it;
		}
	}

	if (it != itHighest && !(*it == current) && (*it).bitDepth() == 16)
	{
		if( display->useMode(*it) )
		{
			RENDER_STREAM("** switched to " << *it << ":\n");
			device->setViewport(0, 0, (*it).width(), (*it).height());
		}
		else
		{
			RENDER_STREAM("** Failed to switch to " << *it << ":\n");
		}
	}


	RENDER_STREAM("** mode change took " << 1000 * timer.time().asDouble() << " (ms)\n\n");

	POST(display->currentMode().bitDepth() >= 16);
}

void D3DApp::processInput()
{
	// If this stream is set, close it after one frame.
	// TODO check no longer used?
	/*if (renDumpMeshStream)
	{
		_DELETE(renDumpMeshStream);
		renDumpMeshStream = NULL;
	}*/

	DevEventQueue& queue = DevEventQueue::instance();

	while (!queue.isEmpty())
	{
		IAIN_STREAM(queue << std::endl);
		const DevButtonEvent event = queue.oldestEvent();
		IAIN_STREAM("App processing " << event << std::endl);

		PhysMotionControl::processButtonEvent( event );

		// Change the sensitivity of the movement controls with the - and +
		// keys.  (+ is actually the = key with shift).
/*		FlyControl* control = NULL;
		if (eyeControl_->inputEnabled())
			control = eyeControl_;
		else
			control = models_[controledModel_]->control_;
*/

		const bool shift = event.wasShiftPressed();
		switch (event.scanCode())
		{
			default:
				models_.processEvent(event);
				lights_.processEvent(event);
				break;

			case DevKey::ESCAPE:
			case DevKey::KEY_Q:		finish(); break;
			case DevKey::KEY_E:		eyeControl_  ->enableInput(); break;
			case DevKey::KEY_C:		clearBack_  = !clearBack_;    break;
			case DevKey::KEY_F:		doFog_      = !doFog_;        break;
			case DevKey::KEY_P:
				if (ProProfiler::instance().isProfilingEnabled())
					ProProfiler::instance().disableProfiling();
				else
					ProProfiler::instance().enableProfiling();
				break;

			case DevKey::KEY_Y:
				RENDER_STREAM("Toggling anti-aliasing.\n");
				device_->antiAliasingOn(!device_->antiAliasingOn());
				break;

			case DevKey::F1:		camera_->colourFilter(RenColour::red());   break;
			case DevKey::F2:		camera_->colourFilter(RenColour::green()); break;
			case DevKey::F3:		camera_->colourFilter(RenColour::blue());  break;
			case DevKey::F4:		camera_->colourFilter(RenColour(0.2));     break;
			case DevKey::F5:		camera_->colourFilter(RenColour(0.5));     break;
			case DevKey::F6:		camera_->colourFilter(RenColour::white()); break;

			case DevKey::F12:
			{
				if (!device_->capabilities().supportsEdgeAntiAliasing())
				{
					saveScreenShot();
				}
				else
				{
					// Turn on anti-aliasing, render a frame then save the screen.
					renderingForShot_ = true;
					device_->antiAliasingOn(true);
				}
				break;
			}

			case DevKey::KEY_T:
				if (shift)
					++mouseSleepTime;
				else if (mouseSleepTime > 0)
					--mouseSleepTime;
				break;

			case DevKey::KEY_X:
				if (matXform_)
				{
					_DELETE(matXform_);
					matXform_ = NULL;
				}
				else
				{
					matXform_ = _NEW(EnvNVGMatTransform(1, 1.3));
				}

				RenMaterial::globalTransform(matXform_);
				break;

			case DevKey::KEY_I:
			{
				RenStats* stats = device_->statistics();

				if (stats)
				{
					if (stats->shown())
						stats->hide();
					else
						stats->show();
				}
				break;
			}

/*			case DevKey::KEY_S:
				if (control && shift)
					control->metersPerSecond(control->metersPerSecond() * 2);
				else
					control->metersPerSecond(control->metersPerSecond() / 2);
				break;
*/

			case DevKey::KEY_D:
				if (shift)
					switchMode(display_, device_, 1);
				else
					switchMode(display_, device_, -1);
				break;

			case DevKey::KEY_O:
				// Turn a model on or off if 'O' is pressed (for timing purposes).
				if (models_.currentEnabled())
				{
					models_.current()->draw_ = !(models_.current()->draw_);
				}
				else if (lights_.currentEnabled())
				{
					RenLight* light = lights_.current()->light_;
					if (light->isOn())
						light->turnOff();
					else
						light->turnOn();
				}
				break;

			case DevKey::KEY_Z:
			{
				if (globalCoplanarZbias == 2)
					globalCoplanarZbias = 0;
				else
					globalCoplanarZbias = 2;
				break;
			}

			case DevKey::BREAK:
				if (animTimer_.paused())
					animTimer_.resume();
				else
					animTimer_.pause();
				break;
		}
	}

	// This call must come after the input tests above, just in case
	// a keypress has unfrozen a motion control.
	PhysMotionControl::updateAll();
}

void D3DApp::queueEvents()
{
	DevEventQueue::instance().queueEvents(DevKey::ESCAPE, DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_Q,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_L,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_E,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_C,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_F,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_S,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_M,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_T,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_X,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_D,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_O,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_B,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_I,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_R,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_Z,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_P,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_Y,  DevButtonEvent::PRESS);

	DevEventQueue::instance().queueEvents(DevKey::F1,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F2,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F3,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F4,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F5,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F6,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F12, DevButtonEvent::PRESS);

	DevEventQueue::instance().queueEvents(DevKey::BREAK,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::LEFT_MOUSE, DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::RIGHT_MOUSE);
}

void randomiseMaterial(RenMaterial* mat)
{
	// Test only -- set the background to a random (!) colour.
	const double r1 = (rand() & 0xffff) / 65535.0;
	const double g1 = (rand() & 0xffff) / 65535.0;
	const double b1 = (rand() & 0xffff) / 65535.0;

	mat->diffuse(RenColour(r1, g1, b1));
}

RenMaterialVec* createRandomSet(size_t nMats)
{
	RenMaterialVec* newSet = _NEW(RenMaterialVec(nMats));

	for (size_t i=0; i!=nMats; ++i)
	{
		RenMaterial mat;
		mat.makeNonSharable();
		randomiseMaterial(&mat);
		newSet->push_back(mat);
	}

	return newSet;
}

static void createTestLines(const Ren::MeshPtr& mesh, const RenMaterial& mat)
{
    MATHEX_SCALAR xMin = 0;
    MATHEX_SCALAR yMin = 0;
    MATHEX_SCALAR zMin = 0;
    MATHEX_SCALAR xMax = 3;
    MATHEX_SCALAR yMax = 3;
    MATHEX_SCALAR zMax = 2;

    //Construct a wire frame cuboid surrounding the boundary
	mesh->addLine( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMax, yMin, zMin ), mat );
	mesh->addLine( MexPoint3d( xMax, yMin, zMin ), MexPoint3d( xMax, yMax, zMin ), mat );
	mesh->addLine( MexPoint3d( xMax, yMax, zMin ), MexPoint3d( xMin, yMax, zMin ), mat );
	mesh->addLine( MexPoint3d( xMin, yMax, zMin ), MexPoint3d( xMin, yMin, zMin ), mat );

	mesh->addLine( MexPoint3d( xMin, yMin, zMax ), MexPoint3d( xMax, yMin, zMax ), mat );
	mesh->addLine( MexPoint3d( xMax, yMin, zMax ), MexPoint3d( xMax, yMax, zMax ), mat );
	mesh->addLine( MexPoint3d( xMax, yMax, zMax ), MexPoint3d( xMin, yMax, zMax ), mat );
	mesh->addLine( MexPoint3d( xMin, yMax, zMax ), MexPoint3d( xMin, yMin, zMax ), mat );

	mesh->addLine( MexPoint3d( xMin, yMin, zMin ), MexPoint3d( xMin, yMin, zMax ), mat );
	mesh->addLine( MexPoint3d( xMax, yMin, zMin ), MexPoint3d( xMax, yMin, zMax ), mat );
	mesh->addLine( MexPoint3d( xMax, yMax, zMin ), MexPoint3d( xMax, yMax, zMax ), mat );
	mesh->addLine( MexPoint3d( xMin, yMax, zMin ), MexPoint3d( xMin, yMax, zMax ), mat );
}

// Create a spin-to-face polygon which represents a gun barrel on the
// level-1 Aggressor mesh.
void D3DApp::createSpinGunBarrel(const Ren::MeshPtr& mesh)
{
	// One long thin spin to face.
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;
	verts.push_back(MexPoint2d( 0.3, 0.09375));
	verts.push_back(MexPoint2d( 0.0, 0.09375));
	verts.push_back(MexPoint2d( 0.0, -0.09375));
	verts.push_back(MexPoint2d( 0.3, -0.09375));
	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(1, 1));
	uvs.push_back(MexPoint2d(1, 0));
	uvs.push_back(MexPoint2d(0, 0));
	uvs.push_back(MexPoint2d(0, 1));
	stf.uv(uvs);

	RenMaterial mat(RenColour::white());
//	mat.emissive(RenColour::white());
	RenTexture newTex = RenTexManager::instance().createTexture("barel_b.bmp");
	mat.texture(newTex);

	MexPoint3d base(-0.22, 0.86, 1.35);
	stf.material(mat);
	stf.spinAxis(base);
	mesh->addSpinTFPolygon(stf);
}

// Create a spin-to-face polygon with the lightning texture on it.
void D3DApp::createLightning(const Ren::MeshPtr& mesh)
{
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;
	verts.push_back(MexPoint2d( 3,  0.45 ));
	verts.push_back(MexPoint2d(  0,  0.45  ));
	verts.push_back(MexPoint2d( 0, -0.45 ));
	verts.push_back(MexPoint2d( 3,  -0.45 ));
	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(1, 0));
	uvs.push_back(MexPoint2d(0, 0));
	uvs.push_back(MexPoint2d(0, 1));
	uvs.push_back(MexPoint2d(1, 1));
	stf.uv(uvs);

	RenMaterial mat(RenColour::black());
	mat.emissive(RenColour(0.2));
	RenTexture newTex = RenTexManager::instance().createTexture("elec_bt.bmp");
	mat.texture(newTex);

	//MexPoint3d base(0,0,0);
	MexPoint3d base(0.08, 0.86, 1.35);
	stf.material(mat);
	stf.spinAxis(base);
	mesh->addSpinTFPolygon(stf);
}

// Create a spin-to-face and a turn-to-face which represent a plasma bolt.
void D3DApp::createPlasmaBolt(const Ren::MeshPtr& mesh)
{
	// One long thin spin to face.
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;
	verts.push_back(MexPoint2d( 0.2,  0.1));
	verts.push_back(MexPoint2d(-0.2,  0.1));
	verts.push_back(MexPoint2d(-0.2, -0.1));
	verts.push_back(MexPoint2d( 0.2, -0.1));
	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(1, 0));
	uvs.push_back(MexPoint2d(1, 1));
	uvs.push_back(MexPoint2d(0, 1));
	uvs.push_back(MexPoint2d(0, 0));
	stf.uv(uvs);

	RenMaterial mat(RenColour::black());
	mat.emissive(RenColour::white());
	RenTexture newTex = RenTexManager::instance().createTexture("pside_bt.bmp");
	mat.texture(newTex);

	stf.material(mat);
	stf.spinAxis(MexPoint3d(0, 0, 0));
	mesh->addSpinTFPolygon(stf);

	// Add one TTF in the middle of the STF.
	RenTTFRectangle ttf;
	ttf.centre(MexPoint3d(0, 0, 0));
	ttf.width(0.14);
	ttf.height(0.14);
	ttf.depthOffset(-0.21);

	RenTexture newTex2 = RenTexManager::instance().createTexture("ptop_bt.bmp");
	mat.texture(newTex2);
	ttf.material(mat);

	mesh->addTTFPolygon(ttf);
}

static void setMarkerDimensions
(
	RenTTFTriangles& ttf,
	MATHEX_SCALAR totalWidth,
	MATHEX_SCALAR totalHeight,
	MATHEX_SCALAR cornerSize,
	MATHEX_SCALAR lineThickness
)
{
	// Corner size is a percentage of totalWidth and totalHeight.
	// Corners must always be less than half the total size.
	PRE(cornerSize > 0);
	PRE(cornerSize <= 0.5);

	// First set the polygon dimensions.
	ttf.width(totalWidth);
	ttf.height(totalHeight);

	const MATHEX_SCALAR cw = cornerSize * totalWidth;
	const MATHEX_SCALAR ch = cornerSize * totalHeight;
	ttf.cornerWidth(cw);
	ttf.cornerHeight(ch);

	// Next stretch the texture UV coords so that the lines have a xyz-space
	// thickness of lineThickness.  We need to know how thick the line is in
	// uv space.  The texture is 32x32 and the lines are one texel wide.
	const MATHEX_SCALAR uvThickness = 1.0/32.0;
	const MATHEX_SCALAR u = uvThickness * cw / lineThickness;
	const MATHEX_SCALAR v = uvThickness * ch / lineThickness;

	const MexVec2 uChanged(u, 1);
	const MexVec2 vChanged(0, 1 - v);

	ttf.uv(uChanged, 1);
	ttf.uv(uChanged, 5);
	ttf.uv(uChanged, 7);
	ttf.uv(uChanged, 11);

	ttf.uv(vChanged, 2);
	ttf.uv(vChanged, 4);
	ttf.uv(vChanged, 8);
	ttf.uv(vChanged, 10);
}

void D3DApp::createTTFs(const Ren::MeshPtr& mesh)
{
	// Add some TTFs to the first mesh.
	RenTTFRectangle ttf;
	ttf.centre(MexPoint3d(15, 55, 2));
	ttf.width(2);
	ttf.height(2);

	RenMaterial mat(RenColour(1, 1, 1, 0.6));
//	RenTexture newTex = RenTexManager::instance().createTexture("smile_t.bmp");
//	RenMaterial mat(RenColour(1, 1, 1));
	RenTexture newTex = RenTexManager::instance().createTexture("co256.bmp");
	mat.texture(newTex);
	ttf.material(mat);

	mesh->addTTFPolygon(ttf);

	// Second TTF.
	ttf.centre(MexPoint3d(10, 60, 2));
	ttf.width(2);
	ttf.height(4);

	RenMaterial mat2(RenColour(0,1,0, 0.5));
	ttf.material(mat2);

	mesh->addTTFPolygon(ttf);

	RenTTFTriangles ttf2;
	RenMaterial mat3(RenColour(0,0,0, 0.8));
//	RenMaterial mat3(RenColour(0,0,0));
	mat3.emissive(RenColour(1,1,1));
//	RenTexture newTex2 = RenTexManager::instance().createTexture("explode/bm24_bt.bmp");
//	RenTexture newTex2 = RenTexManager::instance().createTexture("smile_t.bmp");
	RenTexture newTex2 = RenTexManager::instance().createTexture("mark3_t.bmp");
	mat3.texture(newTex2);
	ttf2.material(mat3);
	ttf2.width(2);
	ttf2.height(2);
	ttf2.cornerWidth(1.0);
	ttf2.cornerHeight(0.7);
	ttf2.centre(MexPoint3d(40, 60, 2));

	const MexVec2 uvTest(0, 0.75);
	ttf2.uv(uvTest, 2);
	ttf2.uv(uvTest, 4);
	ttf2.uv(uvTest, 8);
	ttf2.uv(uvTest, 10);

	mesh->addTTFPolygon(ttf2);
	RenTTFTriangles& tmpRef = _STATIC_CAST(RenTTFTriangles&, mesh->TTFPolygon(2));
	ttf_ = &tmpRef;
}

// Create a spin-to-face which represents a tree.
void D3DApp::createSpinTree(const Ren::MeshPtr& mesh)
{
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;

	verts.push_back(MexPoint2d(6, 3));
	verts.push_back(MexPoint2d(0, 3));
	verts.push_back(MexPoint2d(0, -3));
	verts.push_back(MexPoint2d(6, -3));

	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(1,0));
	uvs.push_back(MexPoint2d(1,1));
	uvs.push_back(MexPoint2d(0,1));
	uvs.push_back(MexPoint2d(0,0));

	stf.uv(uvs);

	RenMaterial mat(RenColour(1, 1, 1));
	RenTexture newTex = RenTexManager::instance().createTexture("tree01_bt.bmp");
	mat.texture(newTex);

	stf.material(mat);
	stf.spinAxis( RenSpinTFPolygon::SpinAxis( MexPoint3d(0, 0, 0), MexVec3(1, 0, 0) ) );
	mesh->addSpinTFPolygon(stf);

	//create a second tree which spin around the y-axis
	stf.spinAxis( RenSpinTFPolygon::SpinAxis( MexPoint3d(0, 0, 0), MexVec3(0, 1, 0) ) );
	mesh->addSpinTFPolygon(stf);

	//create a third tree which spin around the z-axis
	stf.spinAxis( RenSpinTFPolygon::SpinAxis( MexPoint3d(0, 0, 0), MexVec3(0, 0, 1) ) );
	mesh->addSpinTFPolygon(stf);

}

void D3DApp::createTTFDepthTest(const Ren::MeshPtr& mesh)
{
	// Add some TTFs to the first mesh.
	RenTTFRectangle ttf;
	ttf.centre(MexPoint3d(0, 0, 2));
	ttf.width(4);
	ttf.height(4);

	RenMaterial mat(RenColour(1, 1, 1, 0.6));
	RenTexture newTex = RenTexManager::instance().createTexture("smile_t.bmp");
	mat.texture(newTex);
	ttf.material(mat);

	mesh->addTTFPolygon(ttf);
}

void D3DApp::createSTFDepthTest(const Ren::MeshPtr& mesh)
{
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;

	verts.push_back(MexPoint2d(2, 2));
	verts.push_back(MexPoint2d(-2, 2));
	verts.push_back(MexPoint2d(-2, 0));
	verts.push_back(MexPoint2d(2, 0));

	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(0,0));
	uvs.push_back(MexPoint2d(1,0));
	uvs.push_back(MexPoint2d(1,1));
	uvs.push_back(MexPoint2d(0,1));

	stf.uv(uvs);

	RenMaterial mat(RenColour(1, 1, 1));
	RenTexture newTex = RenTexManager::instance().createTexture("tree01_bt.bmp");
	mat.texture(newTex);

	stf.material(mat);
	stf.spinAxis(MexPoint3d(0, 0, 0));
	mesh->addSpinTFPolygon(stf);
}

static void perVertexTest(const Ren::MeshPtr& mesh)
{
	RenVertex vtx0 = mesh->vertex(0);
	RenVertex vtx1 = mesh->vertex(1);

	vtx0.material(RenColour::red());
	vtx1.material(RenColour::green());
}

//  Commented out because I don't have time to get this working right now - BOB
// bool D3DApp::persistentReadModels( const SysPathName& pathname)
// {
//     ifstream fstr( pathname.c_str(), ios::binary );
//     PerIstream  istr( fstr );
//     istr >> models_;
//
// //     for( size_t i = 0; i < models_.size(); ++i )
// //     {
// // 		ModelData* pModel = models_[ i ];
// //
// // 		pModel->control_ = _NEW(FlyControl(pModel->xform_));
// // 		pModel->control_->degreesPerSecond(8);
// // 		pModel->control_->metersPerSecond(0.6);
// // 		pModel->control_->setDefaultPosition(pModel->xform_);
// // 		pModel->control_->resetPosition();
// //     }
//
//
// 	POST(models_.size() >= 1);
//
// 	return true;
// }

bool D3DApp::readModels(const SysPathName& pathname)
{
	std::ifstream ifs(pathname.pathname().c_str());
	std::istream& is = ifs;

	if (!is)
	{
		std::cout << "Couldn't read file " << pathname << std::endl;
		return false;
	}

	// The first six numbers should be the camera's postion and orientation.
	MATHEX_SCALAR x,y,z, a,e,r;
	is >> x >> y >> z >> a >> e >> r;
	eyeXform_.position(MexPoint3d(x,y,z));
	eyeXform_.rotation(MexEulerAngles(a,e,r));

	while (is)
	{
		string meshFile, modelName;
		MATHEX_SCALAR x,y,z, a,e,r, scale;
		is >> meshFile >> modelName >> x >> y >> z >> a >> e >> r >> scale;

		if (meshFile.length() != 0 && modelName.length() != 0)
		{
			Ren::MeshPtr newMesh;

			if (modelName == "ttfs")
			{
				newMesh = RenMesh::createEmpty();
				createTTFs(newMesh);
				newMesh->meshName(modelName);
			}
			else if (modelName == "plasma")
			{
				newMesh = RenMesh::createEmpty();
				createPlasmaBolt(newMesh);
				newMesh->meshName(modelName);
			}
			else if (modelName == "elecbolt")
			{
				newMesh = RenMesh::createEmpty();
				createLightning(newMesh);
				newMesh->meshName(modelName);
			}
			else if (modelName == "stf_depth_test")
			{
				newMesh = RenMesh::createEmpty();
				createSTFDepthTest(newMesh);
				newMesh->meshName(modelName);
			}
			else if (modelName == "line_test")
			{
				const RenColour col(1, 0, 0);
				newMesh = RenMesh::createEmpty();
				createTestLines(newMesh, col);
				newMesh->meshName(modelName);
			}
			else if (modelName == "ttf_depth_test")
			{
				newMesh = RenMesh::createEmpty();
				createTTFDepthTest(newMesh);
				newMesh->meshName(modelName);
			}
			else
			{
				newMesh = RenMesh::createUnique(meshFile, modelName, scale);
			}

			if (!newMesh.isDefined())
				return false;

			// If this is the aggressor, add an experimental gun sausage.
			if (modelName == "ag1")
			{
				createSpinGunBarrel(newMesh);
				createLightning(newMesh);
			}

			if (modelName == "spin_tree")
				createSpinTree(newMesh);

			if (modelName == "tri" || modelName ==  "poly_test")
			{
				newMesh->backFaceAll(false);
				perVertexTest(newMesh);
			}

			MexTransform3d tx(MexEulerAngles(MexDegrees(a), MexDegrees(e), MexDegrees(r)), MexPoint3d(x,y,z));
			ControlledModel* model = _NEW(ControlledModel(newMesh, tx));
			models_.push_back(model);
		}
	}

//  Commented out because I don't have time to get this working right now - BOB

     //std::ofstream fstr( "models.bin", std::ios::binary );
     //PerOstream  ostr( fstr );
     //ostr << models_;

	POST(models_.size() >= 1);
	return true;
}

// Read the environment parameters from a file.  These are: sky colour; light
// intensity; light direction; and ambient light level.
void D3DApp::readEnvironment(const SysPathName& planetName)
{
	PRE(device_);

	ctl_pvector<RenDirectionalLight>	dirLights;
	ctl_pvector<RenPointLight>			ptLights;
	ctl_pvector<RenUniformLight>		uniformLights;
	SysPathName withExt = planetName;
	withExt.extension("env");

	renReadEnvironment(withExt, device_, &dirLights, &ptLights, &uniformLights);

	for (int i=0; i!=dirLights.size(); ++i)
	{
		RenDirectionalLight* light = dirLights[i];
		ControlledLight* data = _NEW(ControlledDirLight(light));
		lights_.push_back(data);
	}

	for (int i=0; i!=ptLights.size(); ++i)
	{
		RenPointLight* light = ptLights[i];
		ControlledLight* data = _NEW(ControlledPointLight(light));
		lights_.push_back(data);
	}

	for (int i=0; i!=uniformLights.size(); ++i)
	{
		RenUniformLight* light = uniformLights[i];
		ControlledLight* data = _NEW(ControlledUniformLight(light));
		lights_.push_back(data);
	}
}

static void usage()
{
	std::cout << "Command line parameters are:" << std::endl;
	std::cout << "     -w|-f <file name> <mesh name> <screen width> <screen height> [<bitmap name>]" << std::endl;
	std::cout << "Where:" << std::endl;
	std::cout << "     -w means run in window mode (the default)" << std::endl;
	std::cout << "     -f means run in full-screen mode" << std::endl;
	std::cout << std::endl;
}

void D3DApp::testCursor()
{
	PRE(device_);
	PRE(display_);

	DevMouse::instance().hide();

	RenSurface backBuf = device_->backSurface();
	RenSurface surf1 = RenSurface::createSharedSurface("cursor/attack1.bmp", backBuf);
	RenSurface surf2 = RenSurface::createSharedSurface("cursor/attack2.bmp", backBuf);
	RenSurface surf3 = RenSurface::createSharedSurface("cursor/attack3.bmp", backBuf);
	RenSurface surf4 = RenSurface::createSharedSurface("cursor/attack4.bmp", backBuf);
	surf1.enableColourKeying();
	surf2.enableColourKeying();
	surf3.enableColourKeying();
	surf4.enableColourKeying();

	RenAnimCursor2d* animCursor = _NEW(RenAnimCursor2d);
	animCursor->addFrame(surf1);
	animCursor->addFrame(surf2);
	animCursor->addFrame(surf3);
	animCursor->addFrame(surf4);

	animCursor->origin(15,15);
	animCursor->targetFrequency(5);

	cursor_ = animCursor;
	display_->useCursor(cursor_);
}

/* Load up the material manager with lots of unique materials.
   This makes test conditions much more like those of a real app.
   Not used right now.
static void createLotsOfMats(int nMats)
{
	RenMaterial* silly = _NEW_ARRAY(RenMaterial, nMats);
	RenMaterial* ptr = silly;

	for (int i=0; i!=nMats; ++i, ++ptr)
		randomiseMaterial(ptr);
}
*/

// Functions for reporting available texture or video memory don't appear to be
// reliable.  Repeatedly load large textures until memory runs out, thus giving
// a true indication of the amount available.
void D3DApp::exhaustTexMem()
{
	const int total = 50;
	RenTexture textures[total];

	bool failed = false;
	for (int i=0; i!=total && !failed; ++i)
	{
		RENDER_STREAM("Prior to allocating 256x256 no. " << i << std::endl);
		RENDER_STREAM(device_->capabilities());

		std::ostringstream ostr;
		ostr << "big_" << i << ".bmp" << std::ends;
		textures[i] = RenTexManager::instance().createTexture(ostr.str());

		if (textures[i].isNull())
		{
			RENDER_STREAM("Allocated texture no. " << i << std::endl);
			failed = true;
		}
		else
		{
			RENDER_STREAM("Failed to allocated texture no. " << i << std::endl);
		}
	}
}

static void loadLotsOfTexture1()
{
	// Load everything in the current Machines texture directory!
	RenTexManager& texMan = RenTexManager::instance();
	static RenTexture tex_Beeds = texMan.createTexture("Beeds.bmp");
	static RenTexture tex_Beeds_b = texMan.createTexture("Beeds_b.bmp");
	static RenTexture tex_Bhit0_bc = texMan.createTexture("Bhit0_bc.bmp");
	static RenTexture tex_Bhit1_bc = texMan.createTexture("Bhit1_bc.bmp");
	static RenTexture tex_Bhit2_bc = texMan.createTexture("Bhit2_bc.bmp");
	static RenTexture tex_Blam1_bc = texMan.createTexture("Blam1_bc.bmp");
	static RenTexture tex_Blam2_bc = texMan.createTexture("Blam2_bc.bmp");
	static RenTexture tex_Blam3_bc = texMan.createTexture("Blam3_bc.bmp");
	static RenTexture tex_Blam_bc = texMan.createTexture("Blam_bc.bmp");
	static RenTexture tex_Camo1_b = texMan.createTexture("Camo1_b.bmp");
	static RenTexture tex_Camo2_b = texMan.createTexture("Camo2_b.bmp");
	static RenTexture tex_Camo3_b = texMan.createTexture("Camo3_b.bmp");
	static RenTexture tex_Camo4_b = texMan.createTexture("Camo4_b.bmp");
	static RenTexture tex_Chim1 = texMan.createTexture("Chim1.bmp");
	static RenTexture tex_Chim2 = texMan.createTexture("Chim2.bmp");
	static RenTexture tex_Chim3 = texMan.createTexture("Chim3.bmp");
	static RenTexture tex_Chim4 = texMan.createTexture("Chim4.bmp");
	static RenTexture tex_CivFA1_b = texMan.createTexture("CivFA1_b.bmp");
	static RenTexture tex_CivFA2_b = texMan.createTexture("CivFA2_b.bmp");
	static RenTexture tex_CivFA3_b = texMan.createTexture("CivFA3_b.bmp");
	static RenTexture tex_CivFA4_b = texMan.createTexture("CivFA4_b.bmp");
	static RenTexture tex_CivFB1_b = texMan.createTexture("CivFB1_b.bmp");
	static RenTexture tex_CivFB2_b = texMan.createTexture("CivFB2_b.bmp");
	static RenTexture tex_CivFB3_b = texMan.createTexture("CivFB3_b.bmp");
	static RenTexture tex_CivFB4_b = texMan.createTexture("CivFB4_b.bmp");
	static RenTexture tex_CivFC1_b = texMan.createTexture("CivFC1_b.bmp");
	static RenTexture tex_CivFC2_b = texMan.createTexture("CivFC2_b.bmp");
	static RenTexture tex_CivFC3_b = texMan.createTexture("CivFC3_b.bmp");
	static RenTexture tex_CivFC4_b = texMan.createTexture("CivFC4_b.bmp");
	static RenTexture tex_GApan1_b = texMan.createTexture("GApan1_b.bmp");
	static RenTexture tex_GApan2_b = texMan.createTexture("GApan2_b.bmp");
	static RenTexture tex_GApan3_b = texMan.createTexture("GApan3_b.bmp");
	static RenTexture tex_GApan4_b = texMan.createTexture("GApan4_b.bmp");
	static RenTexture tex_GAsid1 = texMan.createTexture("GAsid1.bmp");
	static RenTexture tex_GAsid2 = texMan.createTexture("GAsid2.bmp");
	static RenTexture tex_GAsid3 = texMan.createTexture("GAsid3.bmp");
	static RenTexture tex_GAsid4 = texMan.createTexture("GAsid4.bmp");
	static RenTexture tex_GAtop1_b = texMan.createTexture("GAtop1_b.bmp");
	static RenTexture tex_GAtop2_b = texMan.createTexture("GAtop2_b.bmp");
	static RenTexture tex_GAtop3_b = texMan.createTexture("GAtop3_b.bmp");
	static RenTexture tex_GAtop4_b = texMan.createTexture("GAtop4_b.bmp");
	static RenTexture tex_Grill_b = texMan.createTexture("Grill_b.bmp");
	static RenTexture tex_Lpan1_b = texMan.createTexture("Lpan1_b.bmp");
	static RenTexture tex_Lpan2_b = texMan.createTexture("Lpan2_b.bmp");
	static RenTexture tex_Lpan3_b = texMan.createTexture("Lpan3_b.bmp");
	static RenTexture tex_Lpan4_b = texMan.createTexture("Lpan4_b.bmp");
	static RenTexture tex_Mlend1_b = texMan.createTexture("Mlend1_b.bmp");
	static RenTexture tex_Mlend2_b = texMan.createTexture("Mlend2_b.bmp");
	static RenTexture tex_Mlend3_b = texMan.createTexture("Mlend3_b.bmp");
	static RenTexture tex_Mlend4_b = texMan.createTexture("Mlend4_b.bmp");
	static RenTexture tex_Nuke_b = texMan.createTexture("Nuke_b.bmp");
	static RenTexture tex_Nuke_bc = texMan.createTexture("Nuke_bc.bmp");
	static RenTexture tex_SPanA1_b = texMan.createTexture("SPanA1_b.bmp");
	static RenTexture tex_SPanA2_b = texMan.createTexture("SPanA2_b.bmp");
	static RenTexture tex_SPanA3_b = texMan.createTexture("SPanA3_b.bmp");
	static RenTexture tex_SPanA4_b = texMan.createTexture("SPanA4_b.bmp");
	static RenTexture tex_SPanB1_b = texMan.createTexture("SPanB1_b.bmp");
	static RenTexture tex_SPanB2_b = texMan.createTexture("SPanB2_b.bmp");
	static RenTexture tex_SPanB3_b = texMan.createTexture("SPanB3_b.bmp");
}

static void loadLotsOfTexture2()
{
	// Load everything in the current Machines texture directory!
	RenTexManager& texMan = RenTexManager::instance();
	static RenTexture tex_SPanB4_b = texMan.createTexture("SPanB4_b.bmp");
	static RenTexture tex_SPanC1_b = texMan.createTexture("SPanC1_b.bmp");
	static RenTexture tex_SPanC2_b = texMan.createTexture("SPanC2_b.bmp");
	static RenTexture tex_SPanC3_b = texMan.createTexture("SPanC3_b.bmp");
	static RenTexture tex_SPanC4_b = texMan.createTexture("SPanC4_b.bmp");
	static RenTexture tex_SPanD1_b = texMan.createTexture("SPanD1_b.bmp");
	static RenTexture tex_SPanD2_b = texMan.createTexture("SPanD2_b.bmp");
	static RenTexture tex_SPanD3_b = texMan.createTexture("SPanD3_b.bmp");
	static RenTexture tex_SPanD4_b = texMan.createTexture("SPanD4_b.bmp");
	static RenTexture tex_SPanE1_b = texMan.createTexture("SPanE1_b.bmp");
	static RenTexture tex_SPanE2_b = texMan.createTexture("SPanE2_b.bmp");
	static RenTexture tex_SPanE3_b = texMan.createTexture("SPanE3_b.bmp");
	static RenTexture tex_SPanE4_b = texMan.createTexture("SPanE4_b.bmp");
	static RenTexture tex_SPanF1_b = texMan.createTexture("SPanF1_b.bmp");
	static RenTexture tex_SPanF2_b = texMan.createTexture("SPanF2_b.bmp");
	static RenTexture tex_SPanF3_b = texMan.createTexture("SPanF3_b.bmp");
	static RenTexture tex_SPanF4_b = texMan.createTexture("SPanF4_b.bmp");
	static RenTexture tex_Shing = texMan.createTexture("Shing.bmp");
	static RenTexture tex_Shkwv_bc = texMan.createTexture("Shkwv_bc.bmp");
	static RenTexture tex_TUBE = texMan.createTexture("TUBE.bmp");
	static RenTexture tex_Trax = texMan.createTexture("Trax.bmp");
	static RenTexture tex_WALL0_BC = texMan.createTexture("WALL0_BC.bmp");
	static RenTexture tex_WALL1_BC = texMan.createTexture("WALL1_BC.bmp");
	static RenTexture tex_WALL2_BC = texMan.createTexture("WALL2_BC.bmp");
	static RenTexture tex_WALL3_BC = texMan.createTexture("WALL3_BC.bmp");
	static RenTexture tex_WALL4_BC = texMan.createTexture("WALL4_BC.bmp");
	static RenTexture tex_WALL5_BC = texMan.createTexture("WALL5_BC.bmp");
	static RenTexture tex_WL_RM = texMan.createTexture("WL_RM.bmp");
	static RenTexture tex_agnam1_b = texMan.createTexture("agnam1_b.bmp");
	static RenTexture tex_agnam2_b = texMan.createTexture("agnam2_b.bmp");
	static RenTexture tex_agnam3_b = texMan.createTexture("agnam3_b.bmp");
	static RenTexture tex_agnam4_b = texMan.createTexture("agnam4_b.bmp");
	static RenTexture tex_agnb1_b = texMan.createTexture("agnb1_b.bmp");
	static RenTexture tex_agnb2_b = texMan.createTexture("agnb2_b.bmp");
	static RenTexture tex_agnb3_b = texMan.createTexture("agnb3_b.bmp");
	static RenTexture tex_agnb4_b = texMan.createTexture("agnb4_b.bmp");
	static RenTexture tex_agnbt1_b = texMan.createTexture("agnbt1_b.bmp");
	static RenTexture tex_agnbt2_b = texMan.createTexture("agnbt2_b.bmp");
	static RenTexture tex_agnbt3_b = texMan.createTexture("agnbt3_b.bmp");
	static RenTexture tex_agnbt4_b = texMan.createTexture("agnbt4_b.bmp");
	static RenTexture tex_agnf1_b = texMan.createTexture("agnf1_b.bmp");
	static RenTexture tex_agnf2_b = texMan.createTexture("agnf2_b.bmp");
	static RenTexture tex_agnf3_b = texMan.createTexture("agnf3_b.bmp");
	static RenTexture tex_agnf4_b = texMan.createTexture("agnf4_b.bmp");
	static RenTexture tex_agnfa1_b = texMan.createTexture("agnfa1_b.bmp");
	static RenTexture tex_agnfa2_b = texMan.createTexture("agnfa2_b.bmp");
	static RenTexture tex_agnfa3_b = texMan.createTexture("agnfa3_b.bmp");
	static RenTexture tex_agnfa4_b = texMan.createTexture("agnfa4_b.bmp");
	static RenTexture tex_agnfg1_b = texMan.createTexture("agnfg1_b.bmp");
	static RenTexture tex_agnfg2_b = texMan.createTexture("agnfg2_b.bmp");
	static RenTexture tex_agnfg3_b = texMan.createTexture("agnfg3_b.bmp");
	static RenTexture tex_agnfg4_b = texMan.createTexture("agnfg4_b.bmp");
	static RenTexture tex_agnpn1_b = texMan.createTexture("agnpn1_b.bmp");
	static RenTexture tex_agnpn2_b = texMan.createTexture("agnpn2_b.bmp");
	static RenTexture tex_agnpn3_b = texMan.createTexture("agnpn3_b.bmp");
	static RenTexture tex_agnpn4_b = texMan.createTexture("agnpn4_b.bmp");
	static RenTexture tex_agnth1_b = texMan.createTexture("agnth1_b.bmp");
	static RenTexture tex_agnth2_b = texMan.createTexture("agnth2_b.bmp");
	static RenTexture tex_agnth3_b = texMan.createTexture("agnth3_b.bmp");
	static RenTexture tex_agnth4_b = texMan.createTexture("agnth4_b.bmp");
	static RenTexture tex_agnto1_b = texMan.createTexture("agnto1_b.bmp");
	static RenTexture tex_agnto2_b = texMan.createTexture("agnto2_b.bmp");
	static RenTexture tex_agnto3_b = texMan.createTexture("agnto3_b.bmp");
	static RenTexture tex_agnto4_b = texMan.createTexture("agnto4_b.bmp");
	static RenTexture tex_arrw_bc = texMan.createTexture("arrw_bc.bmp");
	static RenTexture tex_as2hd1_b = texMan.createTexture("as2hd1_b.bmp");
	static RenTexture tex_as2hd2_b = texMan.createTexture("as2hd2_b.bmp");
	static RenTexture tex_as2hd3_b = texMan.createTexture("as2hd3_b.bmp");
	static RenTexture tex_as2hd4_b = texMan.createTexture("as2hd4_b.bmp");
	static RenTexture tex_asleg1_b = texMan.createTexture("asleg1_b.bmp");
	static RenTexture tex_asleg2_b = texMan.createTexture("asleg2_b.bmp");
	static RenTexture tex_asleg3_b = texMan.createTexture("asleg3_b.bmp");
	static RenTexture tex_asleg4_b = texMan.createTexture("asleg4_b.bmp");
	static RenTexture tex_aura_bc = texMan.createTexture("aura_bc.bmp");
	static RenTexture tex_awast1_b = texMan.createTexture("awast1_b.bmp");
	static RenTexture tex_awast2_b = texMan.createTexture("awast2_b.bmp");
	static RenTexture tex_awast3_b = texMan.createTexture("awast3_b.bmp");
	static RenTexture tex_awast4_b = texMan.createTexture("awast4_b.bmp");
	static RenTexture tex_back_b = texMan.createTexture("back_b.bmp");
	static RenTexture tex_bal4L1_B = texMan.createTexture("bal4L1_B.bmp");
	static RenTexture tex_bal4L2_B = texMan.createTexture("bal4L2_B.bmp");
	static RenTexture tex_bal4L3_B = texMan.createTexture("bal4L3_B.bmp");
	static RenTexture tex_bal4L4_B = texMan.createTexture("bal4L4_B.bmp");
	static RenTexture tex_bal4S1_B = texMan.createTexture("bal4S1_B.bmp");
	static RenTexture tex_bal4S2_B = texMan.createTexture("bal4S2_B.bmp");
	static RenTexture tex_bal4S3_B = texMan.createTexture("bal4S3_B.bmp");
	static RenTexture tex_bal4S4_B = texMan.createTexture("bal4S4_B.bmp");
	static RenTexture tex_bc3e1_b = texMan.createTexture("bc3e1_b.bmp");
	static RenTexture tex_bc3e2_b = texMan.createTexture("bc3e2_b.bmp");
	static RenTexture tex_bc3e3_b = texMan.createTexture("bc3e3_b.bmp");
	static RenTexture tex_bc3e4_b = texMan.createTexture("bc3e4_b.bmp");
	static RenTexture tex_beac1_b = texMan.createTexture("beac1_b.bmp");
	static RenTexture tex_beac2_b = texMan.createTexture("beac2_b.bmp");
	static RenTexture tex_beac3_b = texMan.createTexture("beac3_b.bmp");
	static RenTexture tex_beac4_b = texMan.createTexture("beac4_b.bmp");
	static RenTexture tex_beam_bc = texMan.createTexture("beam_bc.bmp");
	static RenTexture tex_bng00_bc = texMan.createTexture("bng00_bc.bmp");
	static RenTexture tex_bng01_bc = texMan.createTexture("bng01_bc.bmp");
	static RenTexture tex_bng02_bc = texMan.createTexture("bng02_bc.bmp");
	static RenTexture tex_bng03_bc = texMan.createTexture("bng03_bc.bmp");
	static RenTexture tex_bng04_bc = texMan.createTexture("bng04_bc.bmp");
	static RenTexture tex_bng05_bc = texMan.createTexture("bng05_bc.bmp");
	static RenTexture tex_bng06_bc = texMan.createTexture("bng06_bc.bmp");
	static RenTexture tex_bng07_bc = texMan.createTexture("bng07_bc.bmp");
	static RenTexture tex_bng08_bc = texMan.createTexture("bng08_bc.bmp");
	static RenTexture tex_bng09_bc = texMan.createTexture("bng09_bc.bmp");
	static RenTexture tex_brrla_t = texMan.createTexture("brrla_t.bmp");
	static RenTexture tex_brrlb_t = texMan.createTexture("brrlb_t.bmp");
	static RenTexture tex_brrle_t = texMan.createTexture("brrle_t.bmp");
	static RenTexture tex_cheq1_b = texMan.createTexture("cheq1_b.bmp");
	static RenTexture tex_cheq2_b = texMan.createTexture("cheq2_b.bmp");
	static RenTexture tex_cheq3_b = texMan.createTexture("cheq3_b.bmp");
	static RenTexture tex_cheq4_b = texMan.createTexture("cheq4_b.bmp");
	static RenTexture tex_civZX1_b = texMan.createTexture("civZX1_b.bmp");
	static RenTexture tex_civZX2_b = texMan.createTexture("civZX2_b.bmp");
	static RenTexture tex_civZX3_b = texMan.createTexture("civZX3_b.bmp");
}

static void loadLotsOfTexture3()
{
	// Load everything in the current Machines texture directory!
	RenTexManager& texMan = RenTexManager::instance();
	static RenTexture tex_civZX4_b = texMan.createTexture("civZX4_b.bmp");
	static RenTexture tex_civlB1_b = texMan.createTexture("civlB1_b.bmp");
	static RenTexture tex_civlB2_b = texMan.createTexture("civlB2_b.bmp");
	static RenTexture tex_civlB3_b = texMan.createTexture("civlB3_b.bmp");
	static RenTexture tex_civlB4_b = texMan.createTexture("civlB4_b.bmp");
	static RenTexture tex_clouds_bc = texMan.createTexture("clouds_bc.bmp");
	static RenTexture tex_co256 = texMan.createTexture("co256.bmp");
	static RenTexture tex_cob2l1_b = texMan.createTexture("cob2l1_b.bmp");
	static RenTexture tex_cob2l2_b = texMan.createTexture("cob2l2_b.bmp");
	static RenTexture tex_cob2l3_b = texMan.createTexture("cob2l3_b.bmp");
	static RenTexture tex_cob2l4_b = texMan.createTexture("cob2l4_b.bmp");
	static RenTexture tex_cob2s1_b = texMan.createTexture("cob2s1_b.bmp");
	static RenTexture tex_cob2s2_b = texMan.createTexture("cob2s2_b.bmp");
	static RenTexture tex_cob2s3_b = texMan.createTexture("cob2s3_b.bmp");
	static RenTexture tex_cob2s4_b = texMan.createTexture("cob2s4_b.bmp");
	static RenTexture tex_cool1_b = texMan.createTexture("cool1_b.bmp");
	static RenTexture tex_cool2_b = texMan.createTexture("cool2_b.bmp");
	static RenTexture tex_cryst_b = texMan.createTexture("cryst_b.bmp");
	static RenTexture tex_dome_b = texMan.createTexture("dome_b.bmp");
	static RenTexture tex_doors1_b = texMan.createTexture("doors1_b.bmp");
	static RenTexture tex_doors2_b = texMan.createTexture("doors2_b.bmp");
	static RenTexture tex_doors3_b = texMan.createTexture("doors3_b.bmp");
	static RenTexture tex_doors4_b = texMan.createTexture("doors4_b.bmp");
	static RenTexture tex_drtrok_b = texMan.createTexture("drtrok_b.bmp");
	static RenTexture tex_ear = texMan.createTexture("ear.bmp");
	static RenTexture tex_edges = texMan.createTexture("edges.bmp");
	static RenTexture tex_elec1_bc = texMan.createTexture("elec1_bc.bmp");
	static RenTexture tex_elec2_bc = texMan.createTexture("elec2_bc.bmp");
	static RenTexture tex_elec3_bc = texMan.createTexture("elec3_bc.bmp");
	static RenTexture tex_elec4_bc = texMan.createTexture("elec4_bc.bmp");
	static RenTexture tex_ex00_bc = texMan.createTexture("ex00_bc.bmp");
	static RenTexture tex_ex01_bc = texMan.createTexture("ex01_bc.bmp");
	static RenTexture tex_ex02_bc = texMan.createTexture("ex02_bc.bmp");
	static RenTexture tex_ex03_bc = texMan.createTexture("ex03_bc.bmp");
	static RenTexture tex_ex04_bc = texMan.createTexture("ex04_bc.bmp");
	static RenTexture tex_ex05_bc = texMan.createTexture("ex05_bc.bmp");
	static RenTexture tex_ex06_bc = texMan.createTexture("ex06_bc.bmp");
	static RenTexture tex_ex07_bc = texMan.createTexture("ex07_bc.bmp");
	static RenTexture tex_ex08_bc = texMan.createTexture("ex08_bc.bmp");
	static RenTexture tex_ex09_bc = texMan.createTexture("ex09_bc.bmp");
	static RenTexture tex_ex10_bc = texMan.createTexture("ex10_bc.bmp");
	static RenTexture tex_ex11_bc = texMan.createTexture("ex11_bc.bmp");
	static RenTexture tex_ex12_bc = texMan.createTexture("ex12_bc.bmp");
	static RenTexture tex_ex13_bc = texMan.createTexture("ex13_bc.bmp");
	static RenTexture tex_ex14_bc = texMan.createTexture("ex14_bc.bmp");
	static RenTexture tex_ex15_bc = texMan.createTexture("ex15_bc.bmp");
	static RenTexture tex_ex16_bc = texMan.createTexture("ex16_bc.bmp");
	static RenTexture tex_eye1a = texMan.createTexture("eye1a.bmp");
	static RenTexture tex_eye2a = texMan.createTexture("eye2a.bmp");
	static RenTexture tex_eye3a = texMan.createTexture("eye3a.bmp");
	static RenTexture tex_fan_b = texMan.createTexture("fan_b.bmp");
	static RenTexture tex_fangrl = texMan.createTexture("fangrl.bmp");
	static RenTexture tex_fangrl_t = texMan.createTexture("fangrl_t.bmp");
	static RenTexture tex_fcchm1_b = texMan.createTexture("fcchm1_b.bmp");
	static RenTexture tex_fcsda1_b = texMan.createTexture("fcsda1_b.bmp");
	static RenTexture tex_fctpa1_b = texMan.createTexture("fctpa1_b.bmp");
	static RenTexture tex_flag_b = texMan.createTexture("flag_b.bmp");
	static RenTexture tex_flap_b = texMan.createTexture("flap_b.bmp");
	static RenTexture tex_flar2_bc = texMan.createTexture("flar2_bc.bmp");
	static RenTexture tex_flare_bc = texMan.createTexture("flare_bc.bmp");
	static RenTexture tex_foggy_bc = texMan.createTexture("foggy_bc.bmp");
	static RenTexture tex_gnt1s1_b = texMan.createTexture("gnt1s1_b.bmp");
	static RenTexture tex_gnt1s2_b = texMan.createTexture("gnt1s2_b.bmp");
	static RenTexture tex_gnt1s3_b = texMan.createTexture("gnt1s3_b.bmp");
	static RenTexture tex_gnt1s4_b = texMan.createTexture("gnt1s4_b.bmp");
	static RenTexture tex_gnt2L1_B = texMan.createTexture("gnt2L1_B.bmp");
	static RenTexture tex_gnt2L2_B = texMan.createTexture("gnt2L2_B.bmp");
	static RenTexture tex_gnt2L3_B = texMan.createTexture("gnt2L3_B.bmp");
	static RenTexture tex_gnt2L4_B = texMan.createTexture("gnt2L4_B.bmp");
	static RenTexture tex_gnt2s1_b = texMan.createTexture("gnt2s1_b.bmp");
	static RenTexture tex_gnt2s2_b = texMan.createTexture("gnt2s2_b.bmp");
	static RenTexture tex_gnt2s3_b = texMan.createTexture("gnt2s3_b.bmp");
	static RenTexture tex_gnt2s4_b = texMan.createTexture("gnt2s4_b.bmp");
	static RenTexture tex_gr3bk1_b = texMan.createTexture("gr3bk1_b.bmp");
	static RenTexture tex_gr3bk2_b = texMan.createTexture("gr3bk2_b.bmp");
	static RenTexture tex_gr3bk3_b = texMan.createTexture("gr3bk3_b.bmp");
	static RenTexture tex_gr3bk4_b = texMan.createTexture("gr3bk4_b.bmp");
	static RenTexture tex_gr3bk_b = texMan.createTexture("gr3bk_b.bmp");
	static RenTexture tex_gr3fr1_b = texMan.createTexture("gr3fr1_b.bmp");
	static RenTexture tex_gr3fr2_b = texMan.createTexture("gr3fr2_b.bmp");
	static RenTexture tex_gr3fr3_b = texMan.createTexture("gr3fr3_b.bmp");
	static RenTexture tex_gr3fr4_b = texMan.createTexture("gr3fr4_b.bmp");
	static RenTexture tex_gr3frn_b = texMan.createTexture("gr3frn_b.bmp");
	static RenTexture tex_grgn1_b = texMan.createTexture("grgn1_b.bmp");
	static RenTexture tex_grgn2_b = texMan.createTexture("grgn2_b.bmp");
	static RenTexture tex_grgn3_b = texMan.createTexture("grgn3_b.bmp");
	static RenTexture tex_grgn4_b = texMan.createTexture("grgn4_b.bmp");
	static RenTexture tex_grgnb1_b = texMan.createTexture("grgnb1_b.bmp");
	static RenTexture tex_grgnb2_b = texMan.createTexture("grgnb2_b.bmp");
	static RenTexture tex_grgnb3_b = texMan.createTexture("grgnb3_b.bmp");
	static RenTexture tex_grgnb4_b = texMan.createTexture("grgnb4_b.bmp");
	static RenTexture tex_halo_bc = texMan.createTexture("halo_bc.bmp");
	static RenTexture tex_heal_bc = texMan.createTexture("heal_bc.bmp");
	static RenTexture tex_heat_bc = texMan.createTexture("heat_bc.bmp");
	static RenTexture tex_in256 = texMan.createTexture("in256.bmp");
	static RenTexture tex_in_b = texMan.createTexture("in_b.bmp");
	static RenTexture tex_ionA1 = texMan.createTexture("ionA1.bmp");
	static RenTexture tex_ionA2 = texMan.createTexture("ionA2.bmp");
	static RenTexture tex_ionA3 = texMan.createTexture("ionA3.bmp");
	static RenTexture tex_ionA4 = texMan.createTexture("ionA4.bmp");
	static RenTexture tex_ionB1 = texMan.createTexture("ionB1.bmp");
	static RenTexture tex_ionB2 = texMan.createTexture("ionB2.bmp");
	static RenTexture tex_ionB3 = texMan.createTexture("ionB3.bmp");
	static RenTexture tex_ionB4 = texMan.createTexture("ionB4.bmp");
	static RenTexture tex_ionC1 = texMan.createTexture("ionC1.bmp");
	static RenTexture tex_ionC2 = texMan.createTexture("ionC2.bmp");
	static RenTexture tex_ionC3 = texMan.createTexture("ionC3.bmp");
	static RenTexture tex_ionC4 = texMan.createTexture("ionC4.bmp");
	static RenTexture tex_k3arm1_b = texMan.createTexture("k3arm1_b.bmp");
	static RenTexture tex_k3arm2_b = texMan.createTexture("k3arm2_b.bmp");
	static RenTexture tex_k3arm3_b = texMan.createTexture("k3arm3_b.bmp");
	static RenTexture tex_k3arm4_b = texMan.createTexture("k3arm4_b.bmp");
	static RenTexture tex_k3hed1_b = texMan.createTexture("k3hed1_b.bmp");
	static RenTexture tex_k3hed2_b = texMan.createTexture("k3hed2_b.bmp");
	static RenTexture tex_k3hed3_b = texMan.createTexture("k3hed3_b.bmp");
	static RenTexture tex_k3hed4_b = texMan.createTexture("k3hed4_b.bmp");
}

static void loadLotsOfTexture4()
{
	// Load everything in the current Machines texture directory!
	RenTexManager& texMan = RenTexManager::instance();
	static RenTexture tex_k5fac1_b = texMan.createTexture("k5fac1_b.bmp");
	static RenTexture tex_k5fac2_b = texMan.createTexture("k5fac2_b.bmp");
	static RenTexture tex_k5fac3_b = texMan.createTexture("k5fac3_b.bmp");
	static RenTexture tex_k5fac4_b = texMan.createTexture("k5fac4_b.bmp");
	static RenTexture tex_kface1_b = texMan.createTexture("kface1_b.bmp");
	static RenTexture tex_kface2_b = texMan.createTexture("kface2_b.bmp");
	static RenTexture tex_kface3_b = texMan.createTexture("kface3_b.bmp");
	static RenTexture tex_kface4_b = texMan.createTexture("kface4_b.bmp");
	static RenTexture tex_khov1_b = texMan.createTexture("khov1_b.bmp");
	static RenTexture tex_khov2_b = texMan.createTexture("khov2_b.bmp");
	static RenTexture tex_khov3_b = texMan.createTexture("khov3_b.bmp");
	static RenTexture tex_khov4_b = texMan.createTexture("khov4_b.bmp");
	static RenTexture tex_ksymb_b = texMan.createTexture("ksymb_b.bmp");
	static RenTexture tex_ktrak1_b = texMan.createTexture("ktrak1_b.bmp");
	static RenTexture tex_ktrak2_b = texMan.createTexture("ktrak2_b.bmp");
	static RenTexture tex_ktrak3_b = texMan.createTexture("ktrak3_b.bmp");
	static RenTexture tex_ktrak4_b = texMan.createTexture("ktrak4_b.bmp");
	static RenTexture tex_lcasd1_b = texMan.createTexture("lcasd1_b.bmp");
	static RenTexture tex_lcatp1_b = texMan.createTexture("lcatp1_b.bmp");
	static RenTexture tex_light_b = texMan.createTexture("light_b.bmp");
	static RenTexture tex_lobA1_b = texMan.createTexture("lobA1_b.bmp");
	static RenTexture tex_lobA2_b = texMan.createTexture("lobA2_b.bmp");
	static RenTexture tex_lobA3_b = texMan.createTexture("lobA3_b.bmp");
	static RenTexture tex_lobA4_b = texMan.createTexture("lobA4_b.bmp");
	static RenTexture tex_lobB1_b = texMan.createTexture("lobB1_b.bmp");
	static RenTexture tex_lobB2_b = texMan.createTexture("lobB2_b.bmp");
	static RenTexture tex_lobB3_b = texMan.createTexture("lobB3_b.bmp");
	static RenTexture tex_lobB4_b = texMan.createTexture("lobB4_b.bmp");
	static RenTexture tex_logo = texMan.createTexture("logo.bmp");
	static RenTexture tex_logo1 = texMan.createTexture("logo1.bmp");
	static RenTexture tex_logo1_b = texMan.createTexture("logo1_b.bmp");
	static RenTexture tex_logo2 = texMan.createTexture("logo2.bmp");
	static RenTexture tex_logo3 = texMan.createTexture("logo3.bmp");
	static RenTexture tex_logo4 = texMan.createTexture("logo4.bmp");
	static RenTexture tex_mach256 = texMan.createTexture("mach256.bmp");
	static RenTexture tex_met5_bc = texMan.createTexture("met5_bc.bmp");
	static RenTexture tex_mfact1_b = texMan.createTexture("mfact1_b.bmp");
	static RenTexture tex_midgar = texMan.createTexture("midgar.bmp");
	static RenTexture tex_misshole = texMan.createTexture("misshole.bmp");
	static RenTexture tex_misslx_b = texMan.createTexture("misslx_b.bmp");
	static RenTexture tex_mltoz1_b = texMan.createTexture("mltoz1_b.bmp");
	static RenTexture tex_mltoz2_b = texMan.createTexture("mltoz2_b.bmp");
	static RenTexture tex_mltoz3_b = texMan.createTexture("mltoz3_b.bmp");
	static RenTexture tex_mltoz4_b = texMan.createTexture("mltoz4_b.bmp");
	static RenTexture tex_noise4_b = texMan.createTexture("noise4_b.bmp");
	static RenTexture tex_nuty1_b = texMan.createTexture("nuty1_b.bmp");
	static RenTexture tex_nuty2_b = texMan.createTexture("nuty2_b.bmp");
	static RenTexture tex_nuty3_b = texMan.createTexture("nuty3_b.bmp");
	static RenTexture tex_nuty4_b = texMan.createTexture("nuty4_b.bmp");
	static RenTexture tex_nutz1_b = texMan.createTexture("nutz1_b.bmp");
	static RenTexture tex_nutz2_b = texMan.createTexture("nutz2_b.bmp");
	static RenTexture tex_nutz3_b = texMan.createTexture("nutz3_b.bmp");
	static RenTexture tex_nutz4_b = texMan.createTexture("nutz4_b.bmp");
	static RenTexture tex_pad_b = texMan.createTexture("pad_b.bmp");
	static RenTexture tex_pipB1_b = texMan.createTexture("pipB1_b.bmp");
	static RenTexture tex_pipB2_b = texMan.createTexture("pipB2_b.bmp");
	static RenTexture tex_pipB3_b = texMan.createTexture("pipB3_b.bmp");
	static RenTexture tex_pipB4_b = texMan.createTexture("pipB4_b.bmp");
	static RenTexture tex_pipC1_b = texMan.createTexture("pipC1_b.bmp");
	static RenTexture tex_pipC2_b = texMan.createTexture("pipC2_b.bmp");
	static RenTexture tex_pipC3_b = texMan.createTexture("pipC3_b.bmp");
	static RenTexture tex_pipC4_b = texMan.createTexture("pipC4_b.bmp");
	static RenTexture tex_pipD1_b = texMan.createTexture("pipD1_b.bmp");
	static RenTexture tex_pipD2_b = texMan.createTexture("pipD2_b.bmp");
	static RenTexture tex_pipD3_b = texMan.createTexture("pipD3_b.bmp");
	static RenTexture tex_pipD4_b = texMan.createTexture("pipD4_b.bmp");
	static RenTexture tex_pipe_b = texMan.createTexture("pipe_b.bmp");
	static RenTexture tex_plsmb_bc = texMan.createTexture("plsmb_bc.bmp");
	static RenTexture tex_pod1 = texMan.createTexture("pod1.bmp");
	static RenTexture tex_pod1_b = texMan.createTexture("pod1_b.bmp");
	static RenTexture tex_pod2_b = texMan.createTexture("pod2_b.bmp");
	static RenTexture tex_pod3_b = texMan.createTexture("pod3_b.bmp");
	static RenTexture tex_pod4_b = texMan.createTexture("pod4_b.bmp");
	static RenTexture tex_pside_bc = texMan.createTexture("pside_bc.bmp");
	static RenTexture tex_ptop_bc = texMan.createTexture("ptop_bc.bmp");
	static RenTexture tex_putop_b = texMan.createTexture("putop_b.bmp");
	static RenTexture tex_rad00_bc = texMan.createTexture("rad00_bc.bmp");
	static RenTexture tex_rad01_bc = texMan.createTexture("rad01_bc.bmp");
	static RenTexture tex_rad02_bc = texMan.createTexture("rad02_bc.bmp");
	static RenTexture tex_rad03_bc = texMan.createTexture("rad03_bc.bmp");
	static RenTexture tex_rad04_bc = texMan.createTexture("rad04_bc.bmp");
	static RenTexture tex_rad05_bc = texMan.createTexture("rad05_bc.bmp");
	static RenTexture tex_rad06_bc = texMan.createTexture("rad06_bc.bmp");
	static RenTexture tex_rad07_bc = texMan.createTexture("rad07_bc.bmp");
	static RenTexture tex_rad08_bc = texMan.createTexture("rad08_bc.bmp");
	static RenTexture tex_rad09_bc = texMan.createTexture("rad09_bc.bmp");
	static RenTexture tex_rad10_bc = texMan.createTexture("rad10_bc.bmp");
	static RenTexture tex_rad11_bc = texMan.createTexture("rad11_bc.bmp");
	static RenTexture tex_rad12_bc = texMan.createTexture("rad12_bc.bmp");
	static RenTexture tex_rad13_bc = texMan.createTexture("rad13_bc.bmp");
	static RenTexture tex_rad14_bc = texMan.createTexture("rad14_bc.bmp");
	static RenTexture tex_rad15_bc = texMan.createTexture("rad15_bc.bmp");
	static RenTexture tex_ring_bc = texMan.createTexture("ring_bc.bmp");
	static RenTexture tex_rlite_bc = texMan.createTexture("rlite_bc.bmp");
	static RenTexture tex_rocket_b = texMan.createTexture("rocket_b.bmp");
	static RenTexture tex_rockt1 = texMan.createTexture("rockt1.bmp");
	static RenTexture tex_rockt2 = texMan.createTexture("rockt2.bmp");
	static RenTexture tex_rockt3 = texMan.createTexture("rockt3.bmp");
	static RenTexture tex_rockt4 = texMan.createTexture("rockt4.bmp");
	static RenTexture tex_rockt5_b = texMan.createTexture("rockt5_b.bmp");
	static RenTexture tex_rockt7_b = texMan.createTexture("rockt7_b.bmp");
	static RenTexture tex_sattop_b = texMan.createTexture("sattop_b.bmp");
	static RenTexture tex_scn_b = texMan.createTexture("scn_b.bmp");
	static RenTexture tex_sent1_b = texMan.createTexture("sent1_b.bmp");
	static RenTexture tex_sent2_b = texMan.createTexture("sent2_b.bmp");
	static RenTexture tex_sent3_b = texMan.createTexture("sent3_b.bmp");
	static RenTexture tex_sent4_b = texMan.createTexture("sent4_b.bmp");
	static RenTexture tex_shblly_b = texMan.createTexture("shblly_b.bmp");
	static RenTexture tex_shnose_b = texMan.createTexture("shnose_b.bmp");
	static RenTexture tex_shock_bc = texMan.createTexture("shock_bc.bmp");
	static RenTexture tex_smlt1_b = texMan.createTexture("smlt1_b.bmp");
	static RenTexture tex_spark_bc = texMan.createTexture("spark_bc.bmp");
	static RenTexture tex_spl00_bc = texMan.createTexture("spl00_bc.bmp");
	static RenTexture tex_spl01_bc = texMan.createTexture("spl01_bc.bmp");
	static RenTexture tex_spl02_bc = texMan.createTexture("spl02_bc.bmp");
	static RenTexture tex_spl03_bc = texMan.createTexture("spl03_bc.bmp");
}

static void loadLotsOfTexture5()
{
	// Load everything in the current Machines texture directory!
	RenTexManager& texMan = RenTexManager::instance();
	static RenTexture tex_splt0_bc = texMan.createTexture("splt0_bc.bmp");
	static RenTexture tex_splt1_bc = texMan.createTexture("splt1_bc.bmp");
	static RenTexture tex_splt2_bc = texMan.createTexture("splt2_bc.bmp");
	static RenTexture tex_splt3_bc = texMan.createTexture("splt3_bc.bmp");
	static RenTexture tex_star_bc = texMan.createTexture("star_bc.bmp");
	static RenTexture tex_static = texMan.createTexture("static.bmp");
	static RenTexture tex_stripes1 = texMan.createTexture("stripes1.bmp");
	static RenTexture tex_top_b = texMan.createTexture("top_b.bmp");
	static RenTexture tex_tower1_b = texMan.createTexture("tower1_b.bmp");
	static RenTexture tex_tower2_b = texMan.createTexture("tower2_b.bmp");
	static RenTexture tex_trks1_b = texMan.createTexture("trks1_b.bmp");
	static RenTexture tex_trks2_b = texMan.createTexture("trks2_b.bmp");
	static RenTexture tex_trks3_b = texMan.createTexture("trks3_b.bmp");
	static RenTexture tex_trks4_b = texMan.createTexture("trks4_b.bmp");
	static RenTexture tex_trn2l1_b = texMan.createTexture("trn2l1_b.bmp");
	static RenTexture tex_trn2l2_b = texMan.createTexture("trn2l2_b.bmp");
	static RenTexture tex_trn2l3_b = texMan.createTexture("trn2l3_b.bmp");
	static RenTexture tex_trn2l4_b = texMan.createTexture("trn2l4_b.bmp");
	static RenTexture tex_trn2s1_b = texMan.createTexture("trn2s1_b.bmp");
	static RenTexture tex_trn2s2_b = texMan.createTexture("trn2s2_b.bmp");
	static RenTexture tex_trn2s3_b = texMan.createTexture("trn2s3_b.bmp");
	static RenTexture tex_trn2s4_b = texMan.createTexture("trn2s4_b.bmp");
	static RenTexture tex_vrus1_bc = texMan.createTexture("vrus1_bc.bmp");
	static RenTexture tex_vrus2_bc = texMan.createTexture("vrus2_bc.bmp");
	static RenTexture tex_vrus3_bc = texMan.createTexture("vrus3_bc.bmp");
	static RenTexture tex_vrus4_bc = texMan.createTexture("vrus4_bc.bmp");
	static RenTexture tex_waltop_b = texMan.createTexture("waltop_b.bmp");
	static RenTexture tex_wave_bc = texMan.createTexture("wave_bc.bmp");
	static RenTexture tex_whl_t = texMan.createTexture("whl_t.bmp");
	static RenTexture tex_windo_b = texMan.createTexture("windo_b.bmp");
	static RenTexture tex_yelre1_b = texMan.createTexture("yelre1_b.bmp");
	static RenTexture tex_yelre2_b = texMan.createTexture("yelre2_b.bmp");
	static RenTexture tex_yelre3_b = texMan.createTexture("yelre3_b.bmp");
	static RenTexture tex_yelre4_b = texMan.createTexture("yelre4_b.bmp");

	RENDER_STREAM(RenTexManager::instance() << "\n");
}

inline float rand0To1()
{
	return (rand() & 255) / 255.0;
}

void overrideDefaultKeyboardMapping( PhysFlyControl* pControl )
{
    DevKeyToCommandTranslator* pKeyTranslator = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::ENTER_PAD, PhysMotionControlWithTrans::FOWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::DELETE_PAD, PhysMotionControlWithTrans::BACKWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_1, PhysMotionControlWithTrans::SLIDE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_3, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_4, PhysMotionControlWithTrans::ROTATE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_6, PhysMotionControlWithTrans::ROTATE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, PhysMotionControlWithTrans::UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, PhysMotionControlWithTrans::DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_2, PhysMotionControlWithTrans::PITCH_UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_8, PhysMotionControlWithTrans::PITCH_DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_7, PhysMotionControlWithTrans::ROLL_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_9, PhysMotionControlWithTrans::ROLL_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_0, PhysFlyControl::STOP_MOVING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_5, PhysFlyControl::STOP_ROTATING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_POS, DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_ORIENTATION, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, PhysMotionControlWithTrans::ROTATE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, PhysMotionControlWithTrans::ROTATE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, PhysMotionControlWithTrans::PITCH_UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, PhysMotionControlWithTrans::PITCH_DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAGE_UP, PhysMotionControlWithTrans::ROLL_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pControl->setKeyTranslator( pKeyTranslator );
}

// Run through each available mode and time how long a mode change takes.
static void modeChangeTest(RenDisplay* display)
{
	RENDER_STREAM("Initial " << display->currentMode() << "\n");

	const ctl_list<RenDisplay::Mode>& modes = display->modeList();

	ctl_list<RenDisplay::Mode>::const_iterator it = modes.begin();
	while (it != modes.end())
	{
		RENDER_STREAM("Changing to " << *it << ":\n");
		UtlDebugTimer timer(UtlDebugTimer::RUNNING);

		if (display->useMode(*it))
			RENDER_STREAM("  succeeded\n");
		else
			RENDER_STREAM("  failed\n");

		RENDER_STREAM("  now in " << display->currentMode() << "\n");
		RENDER_STREAM("  mode change took " << 1000 * timer.time().asDouble() << " (ms)\n");
		++it;
	}
}

bool D3DApp::clientStartup()
{
	// The debug timer doesn't work if this environment var isn't set.
	//setenv("CHARYBDIS", "x", false);
	putenv("CHARYBDIS");
	UtlDebugTimer::startCalibration();

    //Initialise profiling
    double profileInterval = 50.0;
    char* pMs = getenv( "PROFILE_RATE" );
    if( pMs )
        profileInterval = atof( pMs );
    ProProfiler::instance().traceInterval( profileInterval / 1000.0 );

    SysPathName::rootEnvironmentVariable( "RENDER_ROOT" );

    if (invokeArgs().size() != 4)
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

	ASSERT(meshListFile.filename().length() > 0, logic_error());

	// Daniel: Set the coordinate system to left-handed.
	MexCoordSystem::instance().set(MexCoordSystem::LEFT_HANDED);

	Ren::initialise();

	// get and set the video driver
    char* ddrawDriver = getenv( "cb_ddrawdriver" );
    if( ddrawDriver )
	{
		SysRegistry::instance().currentStubKey( "SOFTWARE\\Acclaim Entertainment\\Machines" );
		RenDriverSelector* driverSelector;
		driverSelector = _NEW( RenDriverSelector() );

		bool driverFound=false;
		for( RenDriverSelector::RenDrivers::const_iterator dDrawIt = driverSelector->dDrawDrivers().begin();
			 dDrawIt != driverSelector->dDrawDrivers().end() and not driverFound; ++dDrawIt )
		{
			if( (*dDrawIt)->name() == ddrawDriver )
			{
				driverFound=true;
				driverSelector->useDDrawDriver( (*dDrawIt) );
				driverSelector->updateDriverRegistries();
			}
		}
		_DELETE( driverSelector );
	}

	display_ = _NEW(RenDisplay(window()));

	if (!windowMode)
	{
		// Initially, pick the lowest-res 16-bit mode.
		display_->useFullScreen();
		UtlDebugTimer::finishCalibration();

		std::cout << "Trying to select display mode " << modeW << "x" << modeH << "x" << 16 << std::endl;
		if (!display_->useMode(modeW, modeH, 16))
		{
			std::cout << "Failed to select that mode -- the nearest alternative will be chosen." << std::endl;
			display_->useNearestMode(modeW * modeH, 16);
		}
		// for test only shoud not be checked in !
//		display_->lowestAllowedMode( 640, 480, 16 );

		// If there are no 16-bit modes, then mode should default to the
		// first mode in the list, regardless of depth.
		const RenDisplay::Mode& mode = display_->currentMode();
		std::cout << "Initially, setting mode: " << mode << std::endl;

		// Ask the mouse to give us coordinates scale to the current Direct3D
		// resolution (which doesn't necessarilly match the Windows resolution).
		DevMouse::instance().scaleCoordinates(mode.width(), mode.height());
	}

	std::cout << *display_  << std::endl;
	device_ = _NEW(RenDevice(display_));

	// set this after the device has been created: we need the capability class
	// to find out how much memory is available for display
	bool highestModeSet = device_->setHighestAllowedDisplayMode();
	ASSERT(highestModeSet, "Could tot find a mode fitting in the amount of display memory available");
	RENDER_STREAM(device_->capabilities());
	if (!windowMode)
		testCursor();

	// Daniel: Some lovely stars for you.
	// pStars_ = _NEW(RenStars(100.0, 0.8, 1000));

    RenTexManager::PathNames searchList = RenTexManager::instance().searchList();
    searchList.push_back( SysPathName( "textures" ) );
    searchList.push_back( SysPathName( "textures/exhaust" ) );
    searchList.push_back( SysPathName( "texture4" ) );
    RenTexManager::instance().searchList( searchList );

	readEnvironment(meshListFile);

	if (!readModels(meshListFile) || models_.size() < 1)
	{
		std::cout << "Couldn't read the meshes listed in " << meshListFile << std::endl;
		return false;
	}

	//loadLotsOfTexture1();
	//loadLotsOfTexture2();
	//loadLotsOfTexture3();
	//loadLotsOfTexture4();
	//loadLotsOfTexture5();

	RENDER_STREAM(device_->capabilities());

	MexRadians a1 = MexDegrees(40);
	camera_ = _NEW(RenCamera());
    camera_->verticalFOVAngle(a1.asScalar());
	camera_->yonClipDistance(500);
	camera_->transform(eyeXform_);

	// Set-up a flight sim like flying eyepoint.
	eyeControl_ = _NEW( PhysFlyControl( _NEW( PhysMotionControlledTransform( &eyeXform_ ) ) ) );
	eyeControl_->enableInput();
	overrideDefaultKeyboardMapping( eyeControl_ );
	eyeControl_->initEventQueue();
	eyeControl_->metresPerSecond(2);
	eyeControl_->degreesPerSecond(10);
	eyeControl_->setDefaultPosition(eyeXform_);
	eyeControl_->resetPosition();

 	device_->useCamera(camera_);

	// Fill-up the material manager with lots of pointless materials in
	// order to make it's preformance more like that of the Machines app.
	// createLotsOfMats(300);

	RenMesh::emptyCache();
	RenMesh::removeAllFromFactory();
//	UtlDebugTimer::finishCalibration();
	queueEvents();

	animTimer_.time(0);
	initialised_ = true;

	for (int i=0; i!=1000; ++i)
	{
		randomOffsets[i] = 2 * rand0To1();
		randomRates  [i] = 1 + rand0To1();
	}

	device_->statistics()->hide();

	return true;
}

void D3DApp::clientShutdown()
{
	_DELETE(testSurf_);
	_DELETE(eyeControl_);
	_DELETE(camera_);
	_DELETE(uvAnim_);
	_DELETE(matVec_);
	_DELETE(device_);
	_DELETE(display_);
	_DELETE(pStars_);
}

// Update the display.
void D3DApp::loopCycle()
{
	processInput();

	// Daniel: The ratio needed for RenCamera::horizontalFOVAngle()
	double ratio;

	for (int i=0; i!=lights_.size(); ++i)
		lights_[i]->update();

	RenStats* stats = device_->statistics();

	if (stats && stats->shown())
	{
		device_->out() << "Current " << display_->currentMode() << std::endl;

		// Say what is being controlled by the keyboard.
		if (models_.currentEnabled())
		{
			device_->out() << "Controlling " << models_.current()->description() << std::endl;
		}
		else if (lights_.currentEnabled())
		{
			device_->out() << "Controlling " << lights_.current()->description() << std::endl;
		}
		else if (eyeControl_->inputEnabled())
		{
			device_->out() << "Controlling ";
		}

		// Always print the eye's position.
		const MexVec3 pos = eyeXform_.position();
		MexEulerAngles params;
	    eyeXform_.rotation(&params);
		device_->out() << "Eye at " << pos << ", " << params << std::endl;

		if (ProProfiler::instance().isProfilingEnabled())
			device_->out() << "++Profiling enabled.\n";

		if (device_->antiAliasingOn())
			device_->out() << "Edge anti-aliasing enabled.\n";

	}

	if (globalCoplanarZbias == 2)
		device_->out() << "Gioachino Rossini version.\n";
	else
		device_->out() << "Gustav Mahler version.\n";

	// The camera and light must both be explicitly positioned each frame.
	camera_->transform(eyeXform_);

	if (doFog_)
		device_->fogOn();
	else
		device_->fogOff();

	if (nvgOn_)
		device_->overrideFog(1, 30, 0.1, RenColour(0, 0.07, 0));

	RenSurface backBuf  = device_->backSurface();
	RenSurface frontBuf = device_->frontSurface();
//	const Ren::Rect vpRect(0, 0, backBuf.width()/2, backBuf.height()/2);
//	device_->setViewport(vpRect.originX, vpRect.originY, vpRect.width, vpRect.height);
	ratio = (backBuf.width() - 190) / (backBuf.height() - 90);

	// Do RenSurface testing -- draw a background frame etc.
	static int frameNo = 0;

	if (frameNo < 2)
	{
		RenSurface::Rect fillArea(0,0, backBuf.width(), backBuf.height());
		backBuf.filledRectangle(fillArea, RenColour(0.6));

/*		RenSurface::Points pts1;
		pts1.push_back(MexPoint2d(79, 29));
		pts1.push_back(MexPoint2d(backBuf.width() - 29, 29));
		pts1.push_back(MexPoint2d(backBuf.width() - 29, backBuf.height() - 29));
		pts1.push_back(MexPoint2d(79, backBuf.height() - 29));
		pts1.push_back(MexPoint2d(79, 29));
		backBuf.polyLine(pts1, RenColour::cyan(), 2);
*/
	}

	if (matVec_)
	{
		UtlDebugTimer timer(UtlDebugTimer::RUNNING);

		for (int i=0; i!=100; ++i)
		{
			RenMaterialVec::iterator it = matVec_->begin();
			while (it != matVec_->end())
			{
				RenMaterial& mat = *it;
				randomiseMaterial(&mat);
				++it;
			}
		}

		Ren::out() << "Randomising materials took " << 1000 * timer.time().asDouble() << " (ms)\n";
	}

	const double now = animTimer_.time();
	if (ttf_)
	{
		const MATHEX_SCALAR frameMod = fabs(sin(now));
		const MATHEX_SCALAR totalWidth  = frameMod + 1;
		const MATHEX_SCALAR totalHeight = 2 - frameMod;

		setMarkerDimensions(*ttf_, totalWidth, totalHeight, 0.4, 0.1);
	}

	for (int i=0; i!=models_.size(); ++i)
	{
		ControlledModel* model = models_[i];

		if (model && model->mesh_.isDefined() && model->draw_)
		{
			if (model->randomiseIntensities_)
			{
				Ren::MeshPtr mesh = model->mesh_;
				for (Ren::VertexIdx i=0; i!=mesh->nVertices(); ++i)
				{
					RenVertex v = mesh->vertex(i);
					const float vi = fabs(sin(randomRates[i] * now + randomOffsets[i]));
					v.intensity(vi);
				}
			}

			if (model->randomiseColours_)
			{
				Ren::MeshPtr mesh = model->mesh_;
				for (Ren::VertexIdx i=0; i!=mesh->nVertices(); ++i)
				{
					RenColourHSV hsv;
					hsv.hue(180 * (sin(randomRates[i] * now + randomOffsets[i]) + 1));
					hsv.saturation(1);
					hsv.value(1);

					RenVertex v = mesh->vertex(i);
					v.material(hsv.rgb());
				}
			}
		}
	}

	if (device_->startFrame())
	{
		device_->start3D(clearBack_);

		//Draw the models.
		for (int i=0; i!=models_.size(); ++i)
		{
			ControlledModel* model = models_[i];
			if (model && model->mesh_.isDefined() && model->draw_)
			{
				const bool sky = (model->mesh_->meshName() == "sky03");
				const bool ttf = (model->mesh_->meshName() == "ttf");

				if (doFog_ && (sky || ttf))
					device_->disableFog();

				if (sky)
					device_->disableLighting();

				static RenUnityScale defaultScale;

				if (model->uvAnim())
				{
					model->mesh_->render(model->xform_, *(model->uvAnim()), defaultScale);
				}
				else
				{
					model->mesh_->render(model->xform_, defaultScale);
				}

				if (doFog_ && (sky || ttf))
					device_->restoreFog();

				if (sky)
					device_->enableLighting();
			}
		}

		device_->end3D();

		// Front-to-back blit test.
//		backBuf.simpleBlit(frontBuf, vpRect, -20, backBuf.height() - vpRect.height + 50);

		device_->endFrame();

		// If we have just rendered an anti-aliased frame for a screenshot,
		// then save the shot and turn anti-aliasing off again.
		if (renderingForShot_)
		{
			renderingForShot_ = false;
			device_->antiAliasingOn(false);
			saveScreenShot();
		}

		++frameNo;
	}
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
	w = winWidth_;
	h = winHeight_;
}

void D3DApp::saveScreenShot()
{
    // TODO there was no type
	static uint8 shotNo = 0;
	std::ostringstream ostr;
	ostr << "shot" << shotNo << ".bmp" << std::ends;
	++shotNo;

	RenSurface backBuf = device_->frontSurface();
	Ren::Rect rect(0,0, backBuf.width(), backBuf.height());
	backBuf.saveAsBmp(rect, ostr.str());
}
