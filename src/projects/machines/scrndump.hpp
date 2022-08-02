/*
 * S C R N D U M P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachScreenDumper

    Saves sequences of screens as .bmp files so that people can create flics from games.
*/

#ifndef _MACHINES_SCRNDUMP_HPP
#define _MACHINES_SCRNDUMP_HPP

#include "base/base.hpp"

//#include <strfwd.hpp>
#include "stdlib/string.hpp"

class MachScreenDumper
// Canonical form revoked.
{
public:
	enum DumpAction
	{
		DUMPING,
		IDLE
	};

    static MachScreenDumper& instance();

	// Decides whether to dump next time dump() is called.
	// This should be called before any code that looks at keyboard input.
	void update();

	// This should be called after update(), keyboard input and also after rendering.
	void dump();

	// This produces filenames from the major and minor version members. The names it produces
	// are of the form : MacX0000.bmp, where X is the major version letter and 0000 is the dump number.
	string filename() const;

    void CLASS_INVARIANT;

private:
	// Operation deliberatly revoked.
    MachScreenDumper( const MachScreenDumper& );
    MachScreenDumper& operator =( const MachScreenDumper& );

	DumpAction dumpAction_;
	char majorVersion_;
	int minorVersion_;

    MachScreenDumper();
};


#endif

/* End SCRNDUMP.HPP *************************************************/
