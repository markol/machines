/*
 * T A R G E T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_TARGET_HPP
#define _RENDER_TARGET_HPP

#include "render/render.hpp"
#include "mathex/transf3d.hpp"
#include "device/keyboard.hpp"
#include "device/butevent.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "stdlib/string.hpp"

class DevButtonEvent;
class PhysFlyControl;
class RenLight;
class RenDirectionalLight;
class RenPointLight;
class RenUniformLight;
class RenUVTransform;
class DevTimer;

struct ControlTarget
{
	ControlTarget(const MexTransform3d&);
	virtual ~ControlTarget();
	virtual string description() = 0;

	MexTransform3d		xform_;
	PhysFlyControl* 	control_;
};

// All the bits needed to load and move a model.
struct ControlledModel : public ControlTarget
{
	ControlledModel(const Ren::MeshPtr&, const MexTransform3d&);
	virtual ~ControlledModel();
	virtual string description();

	const RenUVTransform* uvAnim()	{ return uvAnim_; }
	void toggleWarpAnim(const DevTimer& timer);

	Ren::MeshPtr		mesh_;
	bool				draw_;
	bool				randomiseIntensities_;
	bool				randomiseColours_;

private:
	RenUVTransform*		uvAnim_;
};

// All the bits needed to create and move a light.
struct ControlledLight : public ControlTarget
{
	virtual ~ControlledLight();
	virtual void update() = 0;

	RenLight*	light_;

protected:
	ControlledLight(RenLight*, const MexTransform3d&);
};

class ControlledDirLight : public ControlledLight
{
public:
	ControlledDirLight(RenDirectionalLight*);
	virtual void update();
	virtual string description();

private:
	RenDirectionalLight*	dirLight_;
};

class ControlledPointLight : public ControlledLight
{
public:
	ControlledPointLight(RenPointLight*);
	virtual void update();
	virtual string description();

private:
	RenPointLight*	ptLight_;
};

class ControlledUniformLight : public ControlledLight
{
public:
	ControlledUniformLight(RenUniformLight*);
	virtual void update();
	virtual string description();

private:
	RenUniformLight*	ptLight_;
};

// Assumes that T is a subclass of ControlTarget.
template <class T>
class ControlTargetVector : public ctl_pvector<T>
{
public:
	ControlTargetVector(DevKey::ScanCode key):
        myKey_(key),
        current_(0)
    {
    }

	bool processEvent(const DevButtonEvent& event)
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

    T* current() const
    {
        if (this->size() == 0)
            return NULL;
        else
            return (*this)[current_];
    }

    bool currentEnabled() const
    {
        return current() && current()->control_->inputEnabled();
    }

private:
	const DevKey::ScanCode myKey_;
	int current_;
};

#endif

/* End TARGET.HPP ***************************************************/
