#pragma once

#include "render/device.hpp"
#include "render/surface.hpp"

class AniSmacker {
public:
    virtual ~AniSmacker() = default;

    virtual bool isFinished() const = 0;

    virtual unsigned int height() const = 0;
    virtual unsigned int width() const = 0;

    virtual void playNextFrame(RenDevice* pDevice) = 0;
    virtual void displaySummaryInfo() const = 0;

    virtual void rewind() = 0;

    // Ability to switch between front and back buffer blitting.
	// If using the front buffer the code assumes that you are not flipping the buffers.
	virtual void useFrontBuffer(bool ufb) = 0;
	virtual bool useFrontBuffer() const = 0;

protected:
    virtual RenSurface createSmackerSurface(RenDevice *pDevice) = 0;
    virtual void copyCurrentVideoFrameToBuffer(RenSurface& renderSurface) = 0;
    virtual uint* fillBufferForCurrentFrame() = 0;
};
