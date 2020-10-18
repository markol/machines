/***********************************************************

  Afx SDL2.0 input framework test app.

***********************************************************/

#include "sdlapp.hpp"

#include <stdio.h>
#include <math.h>
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



#include "afx/resource.hpp"
#include "afx/castfns.hpp"

#include "utility/percent.hpp"

#include "profiler/profiler.hpp"


static float randomOffsets[1000], randomRates[1000];
//extern int globalCoplanarZbias;
int globalCoplanarZbias;

SDLApp::SDLApp():
	winWidth_(400),
	winHeight_(300),
	clearBack_(true),
	doFog_(false),
	nvgOn_(false),
	fogStart_(1),
	fogEnd_(100),
	fogDensity_(0.1),
	initialised_(false),
	testTimer_(0)
{
}

SDLApp::~SDLApp()
{
}

int mouseSleepTime = 0, fontSize = 5;
//extern ostream* renDumpMeshStream;




void SDLApp::processInput()
{

	DevEventQueue& queue = DevEventQueue::instance();

	while (!queue.isEmpty())
	{
		IAIN_STREAM(queue << std::endl);
		const DevButtonEvent event = queue.oldestEvent();
		IAIN_STREAM("App processing " << event << std::endl);
		const bool shift = event.wasShiftPressed();

		DevKey::printScanCode(std::cout, event.scanCode()); std::cout << " IN APP" << std::endl;
		std::cout << "  SHIFT: " << event.wasShiftPressed() << " CTRL:" << event.wasCtrlPressed()
            <<" ALT:"<< event.wasAltPressed() << std::endl;
        std::cout << "   COORDS: " << event.cursorCoords() << std::endl;

		switch (event.scanCode())
		{
			default:
				break;

			case DevKey::ESCAPE:
			case DevKey::KEY_Q:		finish(); break;
		}
	}

}

void SDLApp::queueEvents()
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



bool SDLApp::clientStartup()
{

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
        std::cout << "USAGE: app <-w|-f> <models.txt> <width> <height>" << std::endl;
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


	queueEvents();

	initialised_ = true;

	return true;
}

void SDLApp::clientShutdown()
{

}

// Update the display.
void SDLApp::loopCycle()
{
	processInput();


}

/*/ virtual
bool SDLApp::activate(WORD )
{
	return true;
}*/

// virtual
void SDLApp::updateDisplay()
{
}

// virtual
void SDLApp::getWindowSize(int&, int&, int& w, int& h)
{
	w = winWidth_;
	h = winHeight_;
}
