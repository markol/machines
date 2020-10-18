#include "device/cdlist.hpp"

DevCDPlayList::DevCDPlayList( DevCDTrackIndex nTracks, SelectionOrder o ):
selectionOrder_(o),
currentTrack_(-1),
nTracks_(nTracks)
{
}

bool DevCDPlayList::isFinished() const
{
	return (currentTrack_ == (nSelections()-1));
}

DevCDPlayList::SelectionOrder DevCDPlayList::selectionOrder() const
{
	return selectionOrder_;
}

DevCDTrackIndex DevCDPlayList::nTracks() const
{
	return nTracks_;
}
		
bool DevCDPlayList::hasTrack( DevCDTrackIndex index ) const
{
	PRE( index < nTracks() );
	PlayList::const_iterator i = playList_.begin();
	PlayList::const_iterator j = playList_.end();

	bool found = false;
	for(;i!=j and not found; ++i)
	{
		if((*i) == index)
			found = true;
	}

	return found;
}

void DevCDPlayList::selectionOrder( SelectionOrder newOrder)
{
	selectionOrder_ = newOrder;
}
					
void DevCDPlayList::addTrack( DevCDTrackIndex i )
{
	PRE( i < nTracks() );
	PRE(not hasTrack( i ) );

	playList_.push_back(i);

	POST( hasTrack( i ) );
}

void DevCDPlayList::removeTrack( DevCDTrackIndex index )
{
	PRE( index < nTracks() );
	PRE( hasTrack( index ) );

	PlayList::iterator i = playList_.begin();
	PlayList::iterator j = playList_.end();

	bool found = false;
	for(;i!=j and not found; ++i)
	{
		if((*i) == index)
		{
			playList_.erase(i);
			found = true;
		}
	}

	POST( not hasTrack( index ) );
}

size_t DevCDPlayList::nSelections() const
{
	return playList_.size();
}
	
DevCDTrackIndex DevCDPlayList::nextTrack()
{
	PRE(nSelections() > 0)

	currentTrack_++;

	return playList_[currentTrack_];
}

DevCDTrackIndex DevCDPlayList::peekNextTrack() const
{
	PRE(nSelections() > 0)

	DevCDTrackIndex nextTrack = playList_[currentTrack_+1];
	
	return nextTrack;
}
		
DevCDTrackIndex DevCDPlayList::firstTrack() const
{
	PRE(nSelections() > 0)

	return playList_[0];
}
		
DevCDTrackIndex DevCDPlayList::lastTrack() const
{
	PRE(nSelections() > 0)

	return playList_[nTracks()-1];
}

DevCDPlayList& DevCDPlayList::operator =( const DevCDPlayList& copyMe)
{
	playList_ = copyMe.playList_;
	selectionOrder_ = copyMe.selectionOrder_;
	currentTrack_ = copyMe.currentTrack_;
	nTracks_ = copyMe.nTracks_;
	nSelections_ = copyMe.nSelections_;

	return *this;
}

void DevCDPlayList::reset()
{
	PRE(nSelections() > 0);
	currentTrack_ = 0;
}

//int DevCDPlayList::getSelectionNumber( DevCDTrackIndex index )
//{
//	PRE( index < nTracks() );
//
//	PlayList::const_iterator i = playList_.begin();
//	PlayList::const_iterator j = playList_.end();
//
//	bool found = false;
//	int indexRef = 0;
//	for(;i!=j and not found; ++i)
//	{
//		if(i == index)
//			found = true;
//		else
//			indexRef++;
//	}
//
//	return indexRef;
//}	
