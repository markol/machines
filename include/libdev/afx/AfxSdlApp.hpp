#pragma once

#include "afx/app.hpp"
#include "AfxConfiguration.hpp"

#include <SDL2/SDL.h>

class AfxSdlApp : public AfxApp
{
public:

    static AfxSdlApp& sdlInstance();

    AfxSdlApp() = default;

    virtual ~AfxSdlApp() = default;

    SDL_Window* window()
    {
        return pWindow_;
    }

    virtual void finish() override;
    virtual bool isFinished() const override;

    virtual void testPrint(const char*) const override;

protected:

    virtual bool OSStartup() override;
    virtual void OSShutdown() override;

private:
    // These are called only by this class from the run method.
    // The default implementations call the client and OS specific
    // start-up and shutdown methods.
    virtual void coreLoop() override;

    void dispatchEvent(SDL_Event*);
    void dispatchMouseEvent(SDL_Event*, bool);
    void dispatchMouseScrollEvent(SDL_Event*);
    void dispatchKeybrdEvent(SDL_Event*, bool);
    void dispatchCharEvent(SDL_Event*);
    void dispatchTouchEvent(SDL_Event*, bool);
    void initialise(int argc, char* argv[]);

    friend int main(int, char* []);

    bool finishing_{ false};
    bool finished_{ false};
    SDL_Window* pWindow_;

    // Config values needed at startup
    AfxConfiguration configuration_;
} ;
