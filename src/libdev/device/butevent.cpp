/*
 * B U T E V E N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "device/butevent.hpp"
#include <ctype.h>
#include <iomanip>
#include "device/time.hpp"

DevButtonEvent::DevButtonEvent
(
	DevButtonEvent::ScanCode code,
	DevButtonEvent::Action   action,
	bool prev, bool shift, bool ctrl, bool alt,
	double time,
	int x, int y,
	ushort repeat,
	char print
):
	code_(code),
	press_(action == PRESS),
	previous_( prev ? 1 : 0 ),
	shift_(shift),
	alt_(alt),
	ctrl_(ctrl),
	coords_(MexPoint2d(x,y)),
	repeatCount_(repeat),
	printable_(print),
	time_(time)
{
    //  Removed because of the recording
//	PRE(time <= DevTime::instance().time());
}

// This ctor is only supplied because the template instantiation of list
// requires it.  If one were actually to construct a default object, it
// would fail lots of assertions because the MAX_CODE value is not allowed.
DevButtonEvent::DevButtonEvent():
	code_(DevKey::MAX_CODE),
	press_(PRESS),
	previous_(0),
	shift_(0),
	alt_(0),
	ctrl_(0),
	coords_(MexPoint2d()),
	repeatCount_(0),
	printable_(0),
	time_(0)
{
}

DevButtonEvent::DevButtonEvent(const DevButtonEvent& b):
	code_(b.code_),
	press_(b.press_),
	previous_(b.previous_),
	shift_(b.shift_),
	alt_(b.alt_),
	ctrl_(b.ctrl_),
	coords_(b.coords_),
	repeatCount_(b.repeatCount_),
	printable_(b.printable_),
	time_(b.time_)
{}

DevButtonEvent& DevButtonEvent::operator=(const DevButtonEvent& b)
{
	press_			= b.press_;
	previous_		= b.previous_;
	shift_			= b.shift_;
	alt_			= b.alt_;
	ctrl_			= b.ctrl_;
	printable_		= b.printable_;
	code_			= b.code_;
	coords_			= b.coords_;
	repeatCount_	= b.repeatCount_;
	time_			= b.time_;

	return *this;
}

DevButtonEvent::ScanCode DevButtonEvent::scanCode() const
{
	return code_;
}

DevButtonEvent::Action DevButtonEvent::action() const
{
	return (press_)? PRESS: RELEASE;
}

bool DevButtonEvent::previous() const
{
	return previous_;
}

size_t DevButtonEvent::repeatCount() const
{
	return repeatCount_;
}

bool DevButtonEvent::wasShiftPressed() const
{
	return shift_;
}
bool DevButtonEvent::wasCtrlPressed() const
{
	return ctrl_;
}

bool DevButtonEvent::wasAltPressed() const
{
	return alt_;
}

double DevButtonEvent::time() const
{
// This doesn't seem to work.  TBD: investigate the Windows time functions
// and find out why it's broken.
//	POST(time_ <= DevTime::instance().time());
	return time_;
}

double DevButtonEvent::age() const
{
	const double result = DevTime::instance().time() - time_;
// This doesn't seem to work.  TBD: investigate the Windows time functions
// and find out why it's broken.
//	POST(result >= 0);
	return result;
}

const DevButtonEvent::Coord& DevButtonEvent::cursorCoords() const
{
	return coords_;
}

char DevButtonEvent::printableChar() const
{
	PRE( isPrintable() );
	return printable_;
}

bool DevButtonEvent::isPrintable() const
{
	return isprint(printable_);
}

char DevButtonEvent::getChar() const
{
	return printable_;
}

bool DevButtonEvent::compressRepeats(const DevButtonEvent& ev)
{
	const bool result = *this == ev;

	if (result)
		repeatCount_ += ev.repeatCount_;

	return result;
}

DevButtonEvent DevButtonEvent::decompressRepeats()
{
	PRE(repeatCount() > 1);

	DevButtonEvent result = *this;
	--repeatCount_;
	result.repeatCount_ = 1;

	POST(result.repeatCount() == 1);
	POST(repeatCount() >= 1);
	return result;
}

bool DevButtonEvent::operator==(const DevButtonEvent& ev) const
{
	return	code_      == ev.code_      &&
			coords_    == ev.coords_    &&
			printable_ == ev.printable_ &&
			press_     == ev.press_     &&
			shift_     == ev.shift_     &&
			ctrl_      == ev.ctrl_      &&
			alt_       == ev.alt_;
}

bool DevButtonEvent::operator<(const DevButtonEvent& ev) const
{
	return code_ < ev.code_;
}

ostream& operator <<( ostream& o, const DevButtonEvent& t )
{
	o << "Button event: "
	  << std::setprecision(4) << " age=" << t.age()
	  << " s=" << t.wasShiftPressed()
	  << " c=" << t.wasCtrlPressed()
	  << " a=" << t.wasAltPressed()
	  << " rpt=" << t.repeatCount() << " ";
	DevKey::printScanCode(o, t.scanCode());
	o << ((t.action() == DevButtonEvent::PRESS)? " down": " up  ");
	o << " coords=" << t.cursorCoords();
    return o;
}

bool DevButtonEvent::isCharEvent() const
{
	return printable_ != 0;
}

bool DevButtonEvent::isKeyEvent() const
{
	// Key event if the printable is zero ( although there is a char 0 it is
	// sufficiently useless as a char event	that we can use it to distinguish
	// between key and char events ).
	return printable_ == 0;
}

/* End BUTEVENT.CPP *************************************************/
