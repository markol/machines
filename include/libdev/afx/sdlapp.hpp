#ifndef _FRAMEWORK_AfxSdlApp_INCLUDED
#define _FRAMEWORK_AfxSdlApp_INCLUDED

#include "afx/app.hpp"

//class SDL_Window;
//union SDL_Event;
#include <SDL2/SDL.h>

class AfxSdlApp : public AfxApp
{
public:
    static AfxSdlApp& sdlInstance();
    AfxSdlApp();
    ~AfxSdlApp();
    SDL_Window* window()	{ return pWindow_; }
        virtual void finish();
        virtual bool isFinished() const;

        virtual void testPrint(const char*) const;

protected:
        virtual bool OSStartup ();
        virtual void OSShutdown();

private:
        // These are called only by this class from the run method.
        // The default implementations call the client and OS specific
        // start-up and shutdown methods.
        virtual void coreLoop();

        void dispatchEvent(SDL_Event*);
        void dispatchMouseEvent(SDL_Event*, bool);
        void dispatchKeybrdEvent(SDL_Event*, bool);
        void dispatchCharEvent(SDL_Event*);
        void dispatchTouchEvent(SDL_Event*, bool);
        void initialise(int argc, char* argv[]);

        friend int main(int , char* []);

        bool finishing_;
        bool finished_;
        SDL_Window* pWindow_;
};

#endif //_FRAMEWORK_AfxSdlApp_INCLUDED

