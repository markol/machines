#include <sstream>
#include "afx/sdlapp.hpp"
#include "device/mouse.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/time.hpp"

#include "recorder/recorder.hpp"
#include <SDL2/SDL.h>

AfxSdlApp::AfxSdlApp():
finished_(false),
finishing_(false)
{
}

AfxSdlApp::~AfxSdlApp()
{}

int main(int argc, char* argv[])
{
    Diag::instance();

    try
    {
        AfxSdlApp::sdlInstance().initialise(argc, argv);
        return AfxApp::abstractInstance().run();
    }
    catch(...)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Crash",
                                 "A fatal error has occurred, application will be terminated.",
                                 NULL);
        return -1;
    }
}

void AfxSdlApp::finish()
{
    // Make multiple finish calls benign.
	if (not isFinished())
	{
		finishing_ = true;
		finished_ = true;
		SDL_Quit();
	}
}

bool AfxSdlApp::isFinished() const
{
    return finished_;
}


void AfxSdlApp::testPrint(const char*) const
{
}

void AfxSdlApp::initialise(int argc, char* argv[])
{
    AfxInvokeArgs args;
    // start from second arg
	for (int i = 1; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}

	setArgs(args);
}

bool AfxSdlApp::OSStartup ()
{
    // Create window
    SDL_Init(SDL_INIT_VIDEO);
	pWindow_ = SDL_CreateWindow( name().c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, // initial width and height
		SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (pWindow_ == NULL)
	{
		std::cerr << "Error: can't create window: " << SDL_GetError() << std::endl;
		return false;
	}

    // Can this method fail?
	return true;
}

void AfxSdlApp::OSShutdown()
{
    SDL_Quit();
}


void AfxSdlApp::coreLoop()
{
    bool callApp = true;

    while (not isFinished())
    {
        // Check for messages in the queue.
        SDL_Event ev;
        if(SDL_PollEvent(&ev))
        {
            //If any message other than key down or mouse move, ensure we
            //call the application
            if( ev.type != SDL_KEYDOWN and ev.type != SDL_KEYUP and ev.type != SDL_MOUSEMOTION )
            {
                callApp = true;
            }
            if (ev.type == SDL_QUIT)
            {
                // When should this be set?  I assume that there could be outstanding
				// messages in the queue when PostQuitMessage is called.  Therefore,
				// the program may be viewed as still running until SDL_QUIT is
				// processed and GetMessage returns false.
                finished_ = true;
            }
            else
            {
                dispatchEvent (&ev);
            }
            //if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                //onResize(ev.window.data1, ev.window.data2);

            //If we haven't yet decided to call the app, don't call it if there
            //are any keyboard messages still in the queue
            if( not callApp )
            {
                callApp = not SDL_PollEvent(&ev);
                if(not callApp) dispatchEvent(&ev);
            }
        }
        else
            callApp = true; //No messages waiting

        // Let the client app do its thing.
		// Note: this main loop will continue until SDL_QUIT is processed,
		// however, the client's loopCycle() method will not be called
		// after finishing_ is set by AfxWin95App::finish().
        if (callApp && !finished_ && !finishing_)
        {
			loopCycle();
            callApp = false;
        }
    }


	NEIL_STREAM( "Finished app" << std::endl );
	POST(isFinished());
}

void AfxSdlApp::dispatchEvent(SDL_Event* event)
{
    RecRecorder::instance().recordingAllowed( false );
    switch ( event->type )
    {
        case SDL_WINDOWEVENT:
            if (!finished_ && !finishing_)
                updateDisplay();
            break;

        case SDL_MOUSEMOTION:
            int x, y; SDL_GetMouseState( &x, &y );
            DevMouse::instance().position(x, y);
            break;

        case SDL_MOUSEBUTTONUP:
            dispatchMouseEvent(event, false);
            break;

        case SDL_MOUSEBUTTONDOWN:
            dispatchMouseEvent(event, true);
            break;

        case SDL_KEYUP:
            dispatchKeybrdEvent(event, false);
            break;

        case SDL_KEYDOWN:
            dispatchKeybrdEvent(event, true);
            break;

        case SDL_TEXTINPUT:
            dispatchCharEvent(event);
            break;
    }
    RecRecorder::instance().recordingAllowed( true );
}

void AfxSdlApp::dispatchMouseEvent( SDL_Event* event, bool pressed )
{
    uint8 button = event->button.button;
    PRE(button >= 1 && button <= 3);

	// The argument is a bool so that we don't need the definition of
	// DevButtonEvent in this class's header file.  Decode the bool.
	const DevButtonEvent::Action act = (pressed)? DevButtonEvent::PRESS: DevButtonEvent::RELEASE;

	// Get the position of the cursor at the time of the event.
	const DevMouse::Position pos = DevMouse::instance().getMessagePos();
	const int x = pos.first;
	const int y = pos.second;

	// Get the states of the modifiers keys at the time of the event.
	const Uint8* kStates = SDL_GetKeyboardState(NULL);
	const bool shift = kStates[SDL_SCANCODE_LSHIFT] || kStates[SDL_SCANCODE_RSHIFT];
	const bool ctrl  = kStates[SDL_SCANCODE_LCTRL] || kStates[SDL_SCANCODE_RCTRL];
	const bool alt   = kStates[SDL_SCANCODE_LALT] || kStates[SDL_SCANCODE_RALT];

	// Get the message's time.
	const double time = DevTime::instance().resolution() * event->button.timestamp;

	// Decode wParam and lParam.
	const DevButtonEvent::ScanCode code = (button == 1)? DevKey::LEFT_MOUSE: DevKey::RIGHT_MOUSE;
	const bool previous = 0;
	const size_t repeats = 1;

	const DevButtonEvent ev(code, act, previous, shift, ctrl, alt, time, x,y, repeats);
	DevMouse::instance().wm_button(ev);
}

void AfxSdlApp::dispatchKeybrdEvent( SDL_Event* event, bool pressed )
{
    // The argument is a bool so that we don't need the definition of
	// DevButtonEvent in this class's header file.  Decode the bool.
	const DevButtonEvent::Action act = (pressed)? DevButtonEvent::PRESS: DevButtonEvent::RELEASE;

	// Get the position of the cursor at the time of the event.
	const DevMouse::Position pos = DevMouse::instance().getMessagePos();
	const int x = pos.first;
	const int y = pos.second;

	// Get the states of the modifiers keys at the time of the event.
	const Uint8* kStates = SDL_GetKeyboardState(NULL);
	const bool shift = kStates[SDL_SCANCODE_LSHIFT] || kStates[SDL_SCANCODE_RSHIFT];
	const bool ctrl  = kStates[SDL_SCANCODE_LCTRL] || kStates[SDL_SCANCODE_RCTRL];
	const bool alt   = kStates[SDL_SCANCODE_LALT] || kStates[SDL_SCANCODE_RALT];

	// Get the message's time.
	const double time = DevTime::instance().resolution() * event->key.timestamp;

	const DevButtonEvent::ScanCode code = _STATIC_CAST(DevButtonEvent::ScanCode,
                        DevSdlKeyboard::scanCodeToKeyNumMap[event->key.keysym.scancode]);
	const bool previous = false;
	const ushort rpt = event->key.repeat+1;

	const DevButtonEvent ev(code, act, previous, shift, ctrl, alt, time, x,y, rpt);
	DevSdlKeyboard::sdlInstance().wm_key(ev);
}

void AfxSdlApp::dispatchCharEvent( SDL_Event* event)
{
    // Just default these values. We're not interested in them when it's a char event.
	const bool shift = false;
	const bool ctrl  = false;
	const bool alt   = false;
	const bool previous = false;
	const ushort rpt = 1;
	const DevButtonEvent::ScanCode code = DevKey::KEY_A;
	const DevButtonEvent::Action act = DevButtonEvent::PRESS;

	// Get the position of the cursor at the time of the event.
	const DevMouse::Position pos = DevMouse::instance().getMessagePos();
	const int x = pos.first;
	const int y = pos.second;

	// Get the message's time.
	const double time = DevTime::instance().resolution() * event->text.timestamp;

	const DevButtonEvent ev(code, act, previous, shift, ctrl, alt, time, x,y, rpt, (char)*(event->text.text) );

	DEBUG_STREAM( DIAG_NEIL, "char event " << (char)*(event->text.text) << std::endl );

	DevSdlKeyboard::sdlInstance().wm_char(ev);
}
