#pragma once

#include "smacker.h"
#include "ani/AniSmacker.hpp"
#include "system/pathname.hpp"
#include "render/surface.hpp"
#include "sound/internal/alsound.hpp"

class AniSmackerRegular : public AniSmacker {
public:
    AniSmackerRegular( const SysPathName& path, size_t xCoordTo, size_t yCoordTo );
    AniSmackerRegular( const SysPathName& path, size_t xCoordTo, size_t yCoordTo, bool fast );
    virtual ~AniSmackerRegular();

    AniSmackerRegular(const AniSmackerRegular& other) = delete;
    AniSmackerRegular& operator=(const AniSmackerRegular) = delete;
    bool operator==(const AniSmackerRegular& ) const = delete;

    virtual bool isFinished() const override;

    virtual unsigned int height() const override;
    virtual unsigned int width() const override;

    virtual void playNextFrame(RenDevice* pDevice) override;
    virtual void displaySummaryInfo() const override;

    virtual void rewind() override;

    // Ability to switch between front and back buffer blitting.
	// If using the front buffer the code assumes that you are not flipping the buffers.
	virtual void useFrontBuffer(bool ufb) override;
	virtual bool useFrontBuffer() const override;

protected:
    virtual RenSurface createSmackerSurface(RenDevice *pDevice) override;
    virtual void copyCurrentVideoFrameToBuffer(RenSurface& renderSurface) override;
    virtual uint* fillBufferForCurrentFrame() override;

private:
    void copyCurrentFrameToBuffer( RenSurface& dst );
    void unpackBufferToSurface(  const RenSurface& dst, const RenSurface& src );

    bool isFinishedNoRecord() const;

    // Smacker needs to avoid doing certain calls on the last frame
    bool isPenultimateFrame() const;

    void getNextFrame();

    // Member Data
	smk		    pSmack_;
    uint *	pBuffer_;
    RenSurface  surface_;
	unsigned	frame_;
	size_t 		xCoordTo_;
	size_t 		yCoordTo_;
	size_t      width_, height_;
	double      frameTime_;
	double      lastFrameTime_;
	bool 		advanceToNextFrame_;
	bool 		fast_;
	bool 		synchronisation_;
	bool		useFrontBuffer_;
	SysPathName fileName_;
	bool		finished_;

	// Sound
	ALsizei    alFrequency_;
	ALenum     alFormat_;
	ALuint     alBuffers_[8],  alSource_;
	std::vector<ALuint> freedBuffers_;
};
