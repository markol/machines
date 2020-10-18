#include "w4dtest/sdlapp.hpp"

#include "device/timer.hpp"
#include "device/eventq.hpp"
#include "device/butevent.hpp"
//#include "ctl/countptr.hpp"

#include "render/device.hpp"

#include "world4d/composit.hpp"
#include "world4d/camera.hpp"
#include "world4d/scenemgr.hpp"

#include "phys/mcfly.hpp"
#include "phys/mcground.hpp"

static void toggleFog(RenDevice* dev)
{
	if (dev->isFogOn())
		dev->fogOff();
	else
		dev->fogOn();
}

static void togglePaused(DevTimer& timer)
{
	if (timer.paused())
		timer.resume();
	else
		timer.pause();
}

void SDLApp::processInput()
{
	DevEventQueue& queue = DevEventQueue::instance();

	while (!queue.isEmpty())
	{
		const DevButtonEvent event = queue.oldestEvent();

		PhysMotionControl::processButtonEvent( event );

		PhysFlyControl* control = NULL;
  		if (eyeControl_->inputEnabled())
   			control = eyeControl_;
	  	else
			control = models_[controledModel_]->control_;

		switch (event.scanCode())
		{
			case DevKey::ESCAPE:
			case DevKey::KEY_Q:		finish(); break;
			case DevKey::KEY_H:		displayHelp_ = !displayHelp_; break;
			case DevKey::KEY_I:		showInfo_    = !showInfo_;    break;
			case DevKey::KEY_L:		lightControl_->enableInput(); break;
			case DevKey::KEY_F:		toggleFog(device_);           break;
			case DevKey::BREAK:		togglePaused(gameTimer_);	  break;

        	case DevKey::F1:
                pGroundControl_->snapTo(manager_->currentCamera()->globalTransform());
    			manager_->useCamera( pGroundCamera_ );
                pGroundControl_->enableInput();
				break;

			case DevKey::KEY_E:
        	case DevKey::F2:
                eyeControl_->snapTo(manager_->currentCamera()->globalTransform());
//                camera_->globalTransform( manager_->currentCamera()->globalTransform() );
    			manager_->useCamera( camera_ );
                eyeControl_->enableInput();
				break;

			//Press 'a' to cycle animation names, then 'z' plays them
			case DevKey::KEY_A:
	            if (animSequence_)
	 	            animSequence_->cycleAnims( &(models_[controledModel_]->plan_) );
				break;

			// Play the currently selected animation if 'z' is pressed.
			case DevKey::KEY_Z:
			{
			    W4dCompositePlanPtr tplan = models_[controledModel_]->plan_;

				if (tplan.isDefined())
		            if (animSequence_)
					   	animSequence_->plan(*tplan, gameTimer_.time()+0.1, 0);
				break;
			}

			// Change the end fog distance when 'd' is pressed.
			case DevKey::KEY_D:
				if (event.wasShiftPressed())
					fogEnd_ *= 2;
				else
					fogEnd_ /= 2;
				break;

			// Change the speed of the current control if 's' is pressed.
			case DevKey::KEY_S:
				if (control)
				{
					if (event.wasShiftPressed())
						control->metresPerSecond(control->metresPerSecond() * 2);
					else
						control->metresPerSecond(control->metresPerSecond() / 2);
				}
				break;

			// Switch to controlling a model when 'm' is pressed.
			case DevKey::KEY_M:
				// If we are already controlling a model, advance to the next one.
				if (models_[controledModel_]->control_->inputEnabled() && models_.size() > 1)
				{
					const int incr = (event.wasShiftPressed())? models_.size() - 1: 1;
					controledModel_ = ((controledModel_ + incr) % models_.size());
				}

				models_[controledModel_]->control_->enableInput();
				animSequence_ = models_[controledModel_]->composite_;
				break;
			case DevKey::KEY_U:
				// Toggle on/off frame rate	regulator
				if (event.wasShiftPressed())
					manager_->autoAdjustFrameRate( false );
				else
					manager_->autoAdjustFrameRate( true );

		}
	}

	// This call must come after the input tests above, just in case
	// a keypress has unfrozen a motion control.
	PhysMotionControl::updateAll();
}

void SDLApp::queueEvents()
{
	DevEventQueue::instance().queueEvents(DevKey::ESCAPE, DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::BREAK,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F1,     DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::F2,     DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_Q,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_H,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_I,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_L,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_E,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_F,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_A,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_Z,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_D,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_S,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_M,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_X,  DevButtonEvent::PRESS);
	DevEventQueue::instance().queueEvents(DevKey::KEY_U,  DevButtonEvent::PRESS);
}

