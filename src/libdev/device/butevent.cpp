/*
 * B U T E V E N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "device/butevent.hpp"
#include <ctype.h>
#include "device/time.hpp"

template<typename DevTimeDep>
DevButtonEventT<DevTimeDep>::DevButtonEventT
(
    ScanCode code,
    DevButtonEventT::Action   action,
	bool prev, bool shift, bool ctrl, bool alt,
	double time,
	int x, int y,
	ushort repeat,
	char print
):
	code_(code),
    action_(action),
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
template<typename DevTimeDep>
DevButtonEventT<DevTimeDep>::DevButtonEventT():
	code_(DevKey::MAX_CODE),
    action_(PRESS),
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

template<typename DevTimeDep>
DevButtonEventT<DevTimeDep>::DevButtonEventT(const DevButtonEventT& b):
	code_(b.code_),
    action_(b.action_),
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

template<typename DevTimeDep>
DevButtonEventT<DevTimeDep>& DevButtonEventT<DevTimeDep>::operator=(const DevButtonEventT& b)
{
	press_			= b.press_;
	previous_		= b.previous_;
	shift_			= b.shift_;
	alt_			= b.alt_;
	ctrl_			= b.ctrl_;
	printable_		= b.printable_;
	code_			= b.code_;
    action_         = b.action_;
	coords_			= b.coords_;
	repeatCount_	= b.repeatCount_;
	time_			= b.time_;

	return *this;
}

template<typename DevTimeDep>
typename DevButtonEventT<DevTimeDep>::ScanCode DevButtonEventT<DevTimeDep>::scanCode() const
{
	return code_;
}

template<typename DevTimeDep>
typename DevButtonEventT<DevTimeDep>::Action DevButtonEventT<DevTimeDep>::action() const
{
    return action_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::previous() const
{
	return previous_;
}

template<typename DevTimeDep>
size_t DevButtonEventT<DevTimeDep>::repeatCount() const
{
	return repeatCount_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::wasShiftPressed() const
{
	return shift_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::wasCtrlPressed() const
{
	return ctrl_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::wasAltPressed() const
{
	return alt_;
}

template<typename DevTimeDep>
double DevButtonEventT<DevTimeDep>::time() const
{
// This doesn't seem to work.  TBD: investigate the Windows time functions
// and find out why it's broken.
//	POST(time_ <= DevTime::instance().time());
	return time_;
}

template<typename DevTimeDep>
double DevButtonEventT<DevTimeDep>::age() const
{
    const double result = timeDependency_.get().time() - time_;
// This doesn't seem to work.  TBD: investigate the Windows time functions
// and find out why it's broken.
//	POST(result >= 0);
	return result;
}

template<typename DevTimeDep>
const typename DevButtonEventT<DevTimeDep>::Coord& DevButtonEventT<DevTimeDep>::cursorCoords() const
{
	return coords_;
}

template<typename DevTimeDep>
char DevButtonEventT<DevTimeDep>::printableChar() const
{
	PRE( isPrintable() );
	return printable_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::isPrintable() const
{
	return isprint(printable_);
}

template<typename DevTimeDep>
char DevButtonEventT<DevTimeDep>::getChar() const
{
	return printable_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::compressRepeats(const DevButtonEventT& ev)
{
	const bool result = *this == ev;

	if (result)
		repeatCount_ += ev.repeatCount_;

	return result;
}

template<typename DevTimeDep>
DevButtonEventT<DevTimeDep> DevButtonEventT<DevTimeDep>::decompressRepeats()
{
	PRE(repeatCount() > 1);

	DevButtonEvent result = *this;
	--repeatCount_;
	result.repeatCount_ = 1;

	POST(result.repeatCount() == 1);
	POST(repeatCount() >= 1);
	return result;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::operator==(const DevButtonEventT& ev) const
{
	return	code_      == ev.code_      &&
            action_    == ev.action_    &&
			coords_    == ev.coords_    &&
			printable_ == ev.printable_ &&
			press_     == ev.press_     &&
			shift_     == ev.shift_     &&
			ctrl_      == ev.ctrl_      &&
			alt_       == ev.alt_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::operator<(const DevButtonEventT& ev) const
{
	return code_ < ev.code_;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::isCharEvent() const
{
	return printable_ != 0;
}

template<typename DevTimeDep>
bool DevButtonEventT<DevTimeDep>::isKeyEvent() const
{
	// Key event if the printable is zero ( although there is a char 0 it is
	// sufficiently useless as a char event	that we can use it to distinguish
	// between key and char events ).
	return printable_ == 0;
}

//Instantiate the template identified by DevButtonEvent alias
template class DevButtonEventT<DevTime>;
/* End BUTEVENT.CPP *************************************************/
