#include "phys/mcfly.hpp"
#include "device/butevent.hpp"

template <class T>
ControlTargetVector<T>::ControlTargetVector(DevKey::ScanCode key):
	myKey_(key),
	current_(0)
{
}

template <class T>
bool ControlTargetVector<T>::processEvent(const DevButtonEvent& event)
{
	if ( this->size() > 0 && event.scanCode() == myKey_)
	{
		// If we are already controlling a model, advance to the next/previous one.
		if (this->size() > 1 && (*this)[current_]->control_->inputEnabled())
		{
			const int incr = (event.wasShiftPressed())? this->size() - 1: 1;
			current_ = ((current_ + incr) % this->size());
		}

		(*this)[current_]->control_->enableInput();
		return true;
	}
	else
	{
		return false;
	}
}

template <class T>
T* ControlTargetVector<T>::current() const
{
	if (this->size() == 0)
		return NULL;
	else
		return (*this)[current_];
}

template <class T>
bool ControlTargetVector<T>::currentEnabled() const
{
	return current() && current()->control_->inputEnabled();
}

