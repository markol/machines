/*
 * S M A C K E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#ifndef ANISMACKER_HPP
#define ANISMACKER_HPP

#include "base/base.hpp"

//////////////////////////////////////////////////////////////////////

class RenDevice;
class SysPathName;
class AniSmackerImpl;

class AniSmacker
// cannonical form revoked
{
public:

	AniSmacker( const SysPathName&, size_t xCoordTo = 0, size_t yCoordTo = 0 );
	// PRE( path.existsAsFile() );

	// additional fast parameter will allow smacker animation to run as fast as processor
	// will let it.
	AniSmacker( const SysPathName&, size_t xCoordTo, size_t yCoordTo, bool fast );
	// PRE( path.existsAsFile() );

	~AniSmacker();

	bool isFinished() const;

	unsigned height() const;
	unsigned width() const;

	void playNextFrame( RenDevice * );
	// PRE( not isFinished() );

	void displaySummaryInfo() const;

	void rewind();

	// If synchronisation is enabled, AniSmacker will sit in a loop until
	// it is ready to display another frame.
	void SynchronisationOn( void );
	void SynchronisationOff( void );

	// Ability to switch between front and back buffer blitting.
	// If using the front buffer the code assumes that you are not flipping the buffers.
	void useFrontBuffer( bool );
	bool useFrontBuffer() const;

private:
	AniSmacker( const AniSmacker& );
	AniSmacker& operator =( const AniSmacker& );
	bool operator ==( const AniSmacker& ) const;

	///////////////////////////////

	// Data
	AniSmackerImpl * pImpl_;
};

//////////////////////////////////////////////////////////////////////

#endif	// ANISMACKER_HPP

/* End SMACKER.HPP **************************************************/
