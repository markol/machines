#ifndef _DEV_CD_DEVCDPLAYLIST
#define _DEV_CD_DEVCDPLAYLIST

#include "ctl/vector.hpp"

#include "device/cddefs.hpp"

class DevCDPlayList
// memberwise cannonical
{
public:

	enum SelectionOrder 
	{ 
		SEQUENTIAL, 
		RANDOM,
	};

	DevCDPlayList( DevCDTrackIndex nTracks, SelectionOrder o = SEQUENTIAL );

	///////////////////////////////

	bool isFinished() const;

	SelectionOrder selectionOrder() const;

	DevCDTrackIndex nTracks() const;
		
	bool hasTrack( DevCDTrackIndex ) const;
	// PRE( i < nTracks() );
	
	///////////////////////////////
	
	void selectionOrder( SelectionOrder );
					
	void addTrack( DevCDTrackIndex i );
	// PRE( i < nTracks() );
	// POST( not hasTrack( i ) );

	void removeTrack( DevCDTrackIndex i );
	// PRE( i < nTracks() );
	// POST( not hasTrack( i ) );
	
	DevCDTrackIndex nextTrack();
	// PRE( not isFinished() );

	DevCDTrackIndex peekNextTrack() const;
	// PRE( not isFinished() );

	DevCDTrackIndex firstTrack() const;
		
	DevCDTrackIndex lastTrack() const;

	size_t nSelections() const;

	void reset();

    DevCDPlayList& operator =( const DevCDPlayList& );

	///////////////////////////////

private:
	typedef ctl_vector<DevCDTrackIndex> PlayList;

	PlayList			playList_;
	SelectionOrder		selectionOrder_;
	DevCDTrackIndex		currentTrack_;
	DevCDTrackIndex		nTracks_;
	size_t				nSelections_;
};

#endif