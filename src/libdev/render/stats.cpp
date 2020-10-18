/*
 * S T A T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/stats.hpp"
#include <stdio.h>
#include "ctl/list.hpp"

#ifndef _INLINE
    #include "render/stats.ipp"
#endif

RenStats::RenStats():
	show_(true),
	frameCount_(0),
	updateInterval_(0.333),
	frameTimes_(_NEW(ctl_list<double>)),
	averageTime_(0),
	polygonsDrawn_(0),
	linesDrawn_(0),
	pointsDrawn_(0),
	maxQueueLength_(10)
{
    TEST_INVARIANT;
}

// A nasty cruft: these should be member vars.
static double totalTime = 0;
static ulong  totalFrames = 0;

RenStats::~RenStats()
{
	_DELETE(frameTimes_);
    TEST_INVARIANT;

	FILE* file = fopen("timing.log", "a+");

	if (file)
	{
		fprintf(file, "Total time=%f\n", totalTime);
		fprintf(file, "Total frames=%lu\n", totalFrames);

		const double av = totalTime / totalFrames;
		fprintf(file, "Average frame rate=%f\n", av);
		fclose(file);
	}
}

double RenStats::frameRate() const
{
	return frameRate_;
}

double RenStats::frameTime(size_t n) const
{
	PRE(n < numberOfStoredFrames());
    TEST_INVARIANT;

	ctl_list<double>::const_iterator it = frameTimes_->end();
	--it;

	while (it != frameTimes_->begin() && n > 0)
	{
		--it;
		--n;
	}

	return *it;
}

double RenStats::averageFrameTime() const
{
    TEST_INVARIANT;
	return averageTime_;
}

void RenStats::numberOfStoredFrames(size_t n)
{
	PRE(n > 0);
    TEST_INVARIANT;
	maxQueueLength_ = n;
}

size_t RenStats::numberOfStoredFrames() const
{
    TEST_INVARIANT;
	return maxQueueLength_;
}

void RenStats::displayInterval(double i)
{
	PRE(i > 0);
    TEST_INVARIANT;
	updateInterval_ = i;
}

double RenStats::displayInterval() const
{
    TEST_INVARIANT;
	return updateInterval_;
}

void RenStats::show()
{
    TEST_INVARIANT;
	show_ = true;
}

void RenStats::hide()
{
    TEST_INVARIANT;
	show_ = false;
	stream_.clear();
}

bool RenStats::shown()
{
    TEST_INVARIANT;
	return show_;
}

const std::string& RenStats::statsText() const
{
    TEST_INVARIANT;
	RenStats* crufty = _CONST_CAST(RenStats*, this);
	crufty->text_ = stream_.dataAsString();
	return text_;
}

void RenStats::computeAverage()
{
	if (frameTimes_->size() >= maxQueueLength_)
		frameTimes_->pop_front();

	frameTimes_->push_back(frameTimer_.time());
    TEST_INVARIANT;

	averageTime_ = 0;
	ctl_list<double>::const_iterator it = frameTimes_->begin();
	while (it != frameTimes_->end())
	{
		averageTime_ += *it;
		++it;
	}

	ASSERT(frameTimes_->size() > 0, "Zero length list of frame times.");
	averageTime_ /= frameTimes_->size();
}

void RenStats::startFrame()
{
	frameTimer_.time(0);
	frameTimer_.resume();
}

void RenStats::endFrame()
{
    TEST_INVARIANT;
	frameTimer_.pause();
	++framesSinceUpdate_;
	++frameCount_;

	computeAverage();

	// Are we ready to update the stats display?
	if ( updateTimer_.time() > updateInterval_)
	{
		// Time the interval between successive frames.
		const double elapsed = updateTimer_.time();
		totalTime   += elapsed;
		totalFrames += framesSinceUpdate_;

		const double  avFrameTime = elapsed / framesSinceUpdate_;
		updateTimer_.time(0);
		frameRate_ = 1.0 / avFrameTime;

		// Compute how many polys went out since the last update.
		ulong avPolys = polygonsDrawn_ / framesSinceUpdate_;
		polygonsDrawn_ = 0;
		ulong avLines = linesDrawn_ / framesSinceUpdate_;
		linesDrawn_ = 0;
		ulong avTTFs = TTFsDrawn_ / framesSinceUpdate_;
		TTFsDrawn_ = 0;
		ulong avSTFs = STFsDrawn_ / framesSinceUpdate_;
		STFsDrawn_ = 0;
		ulong avPoints = pointsDrawn_ / framesSinceUpdate_;
		pointsDrawn_ = 0;

		if( show_)
		{
			stream_.clear();
			char buf[1000];
			sprintf(buf, "Frame time=%.1f; frame rate=%.1f;\n", 1000 * avFrameTime, frameRate_);
			stream_ << buf;

			stream_ << "Frame time average over " << maxQueueLength_ << " frames = "
				<< 1000 * averageFrameTime() << "\n";
			stream_ << avPolys << " polys, " << avLines << " lines, "
		        << avTTFs  << " TTFs, "  << avSTFs  << " STFs, "
				<< avPoints << " Points\n";
		}

		// Reset the frame counter.
		framesSinceUpdate_ = 0;
	}
}

void RenStats::CLASS_INVARIANT
{
	INVARIANT(updateInterval_ > 0);
	INVARIANT(maxQueueLength_ > 0);
	INVARIANT(frameTimes_);
	INVARIANT(frameTimes_->size() <= maxQueueLength_);
}

ostream& operator <<( ostream& o, const RenStats& t )
{
	o << t.text_;
    return o;
}

/* End STATS.CPP ****************************************************/
