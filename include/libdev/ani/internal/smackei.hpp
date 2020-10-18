/*
 * S M A C K E I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#ifndef _ANI_SMACKEI
#define _ANI_SMACKEI

#include "base/base.hpp"
#include "smacker.h"
#include "system/pathname.hpp"
#include "render/surface.hpp"
#include "sound/internal/alsound.hpp"

class RenDevice;

////////////////////////////////////////////////////////////////////////

class AniSmackerImpl
{
private:

	AniSmackerImpl( const SysPathName&, size_t xCoordTo = 0, size_t yCoordTo = 0 );
	// PRE( path.existsAsFile() );

	// additional fast parameter will allow smacker animation to run as fast as processor
	// will let it.
	AniSmackerImpl( const SysPathName&, size_t xCoordTo, size_t yCoordTo, bool fast );
	// PRE( path.existsAsFile() );

	~AniSmackerImpl();

	void initialise( const SysPathName& path, size_t xCoordTo, size_t yCoordTo, bool fast );

	void copyCurrentFrameToBuffer( RenSurface& dst );
	void unpackBufferToSurface(  const RenSurface& dst, const RenSurface& src );
	// PRE( not isFinished() );

	bool isFinished() const;
    //  This is only supplied for use in pre and post consitions.
	bool isFinishedNoRecord() const;

	unsigned height() const;
	unsigned width() const;

	void playNextFrame( RenDevice * );
	// PRE( not isFinished() );

	void displaySummaryInfo() const;

	void rewind();

	void SynchronisationOn( void ) { synchronisation_ = true; }
	void SynchronisationOff( void ) { synchronisation_ = false; }

	// Ability to switch between front and back buffer blitting.
	// If using the front buffer the code assumes that you are not flipping the buffers.
	void useFrontBuffer( bool );
	bool useFrontBuffer() const;

	// Smacker needs to avoid doing certain calls on the last frame
	bool isPenultimateFrame() const;

	void getNextFrame();

	AniSmackerImpl( const AniSmackerImpl& );
	AniSmackerImpl& operator =( const AniSmackerImpl& );
	bool operator ==( const AniSmackerImpl& ) const;

	///////////////////////////////

	// Member Data
	smk		    pSmack_;
	uint *	    pBuffer_;
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

	friend class AniSmacker;
};

#endif

////////////////////////////////////////////////////////////////////////
