#pragma once

#include <utility>
#include <SDL2/SDL.h>

struct SdlDelegate
{
    SdlDelegate() {}
    virtual ~SdlDelegate() {}

//////////////////////////////////////////////////////////////////////

    virtual int showCursor(const int toggle)
    {
        return SDL_ShowCursor(toggle);
    }

    virtual std::pair<int, int> getCursorPosition()
    {
        int x, y;
        SDL_GetMouseState( &x, &y );
        return std::make_pair(x, y);
    }

    virtual void moveCursorToPosition(SDL_Window* window, const int x, const int y)
    {
        return SDL_WarpMouseInWindow(window, x, y);
    }
};
