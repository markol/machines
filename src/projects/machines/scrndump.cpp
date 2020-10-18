/*
 * S C R N D U M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

// Definitions of non-inline non-template methods and global functions.

#include "machines/scrndump.hpp"

//#include <string.hpp>
#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "device/keyboard.hpp"

#include "recorder/recorder.hpp"

#include "render/render.hpp"
#include "render/device.hpp"
#include "render/surface.hpp"

#include "utility/tostring.hpp"

// Static.
MachScreenDumper& MachScreenDumper::instance()
{
    static MachScreenDumper instance_;
    return instance_;
}

void MachScreenDumper::update()
{
	if(RecRecorder::instance().state() == RecRecorder::PLAYING)
	{
		if(dumpAction_ == IDLE)
		{
			if(DevKeyboard::instance().keyCodeNoRecord(DevKey::HOME))
				dumpAction_ = DUMPING;
		}
		else
			if(DevKeyboard::instance().keyCodeNoRecord(DevKey::END))
			{
				dumpAction_ = IDLE;
				if(majorVersion_ < 'Z')
					++majorVersion_;
				minorVersion_ = 1;
			}
	}
	else
		dumpAction_ = IDLE;
}

void MachScreenDumper::dump()
{
	if(dumpAction_ == DUMPING)
	{
		RenSurface frontBuf = RenDevice::current()->frontSurface();
		frontBuf.saveAsBmp(Ren::Rect(0, 0, frontBuf.width(), frontBuf.height()), filename());

		++minorVersion_;
	}
}

string MachScreenDumper::filename() const
{
	// Mac is for Machines **not** Ronald MacDonald.
	string result("Dmp");

	result += majorVersion_;

	string minor = utlToString(minorVersion_);

	ASSERT(minor.length() <= 4, "The filename for this screen dump will invalidate the 8.3 filename regime.");

	// Get the number 4 digits long.
	while(minor.length() < 4)
		minor.insert(0, "0");

	result += minor + ".bmp";

	return result;
}

// Private.
MachScreenDumper::MachScreenDumper()

	:dumpAction_(IDLE),
	majorVersion_('A'),
	minorVersion_(1)
{
    TEST_INVARIANT;
}

void MachScreenDumper::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End SCRNDUMP.CPP *************************************************/
