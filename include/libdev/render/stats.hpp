/*
 * S T A T S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_STATS_HPP
#define _RENDER_STATS_HPP

#include <string>
#include "base/logbuff.hpp"
#include "device/timer.hpp"

template <class T> class ctl_list;

// A class which collects and displays statistics about rendering performance.
class RenStats
{
public:
    RenStats();
    virtual ~RenStats();

	// The number of frames rendered since this object was created.
	ulong frameCount() const;

	// The times taken to render historic frames.  Call with an argument of 0
	// to get the most recent time; use 1 to get the next oldest time, etc.
	// There is data for numberOfStoredFrames (this is always guaranteed to be
	// one or more).
	// PRE(n < numberOfStoredFrames());
	double frameTime(size_t n=0) const;

	// The average frame time computed over numberOfStoredFrames();
	double averageFrameTime() const;

	double frameRate() const;

	// For averaging purposes, a record of the frame time is kept for this many
	// past frames.  The default is 10 frames.
	void numberOfStoredFrames(size_t n);	// PRE(n > 0);
	size_t numberOfStoredFrames() const;	// POST(result > 0);

	// The statistics which are written to the screen are updated whenever this
	// much time has ellapsed.  Defaults to 0.333 seconds.
	void displayInterval(double i);		// PRE(i > 0);
	double displayInterval() const;

	// Should the statistics be displayed on-screen?
	void show();
	void hide();
	bool shown();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenStats& t );

	// Internal methods for use by other parts of the render library.
	void startFrame();
	void endFrame();
	const std::string& statsText() const;
	void incrPolyCount(ulong number);
	void incrLineCount(ulong number);
	void incrSTFCount(ulong number);
	void incrTTFCount(ulong number);
	void incrPointsCount(ulong number);

private:
	void computeAverage();

	bool				show_;
	ulong				frameCount_;

	DevTimer			updateTimer_;
	ulong  				framesSinceUpdate_;
	double				updateInterval_;

	DevTimer			frameTimer_;
	ctl_list<double>*	frameTimes_;
	double				averageTime_;
	// Warning: different from 1/averageFrameTime_
	double				frameRate_;
	size_t				maxQueueLength_;

	ulong				polygonsDrawn_, linesDrawn_, TTFsDrawn_, STFsDrawn_, pointsDrawn_;

	BaseLogBuffer		stream_;
	std::string				text_;

    // Operations deliberately revoked
    RenStats( const RenStats& );
    RenStats& operator =( const RenStats& );
    bool operator ==( const RenStats& );
};

#ifdef _INLINE
    #include "render/stats.ipp"
#endif

#endif

/* End STATS.HPP ****************************************************/
