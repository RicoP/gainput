
#include <gainput/gainput.h>
#include <gainput/gestures/GainputHoldGesture.h>

#ifdef GAINPUT_ENABLE_HOLD_GESTURE

namespace gainput
{

HoldGesture::HoldGesture(InputManager& manager, DeviceId device) :
	InputGesture(device),
	manager_(manager),
	oneShotReset_(true),
	firstDownTime_(0)
{
	actionButton_.buttonId = InvalidDeviceButtonId;
	xAxis_.buttonId = InvalidDeviceButtonId;
	yAxis_.buttonId = InvalidDeviceButtonId;

	state_ = manager_.GetAllocator().New<InputState>(manager.GetAllocator(), 1);
	GAINPUT_ASSERT(state_);
	previousState_ = manager_.GetAllocator().New<InputState>(manager.GetAllocator(), 1);
	GAINPUT_ASSERT(previousState_);
}

HoldGesture::~HoldGesture()
{
	manager_.GetAllocator().Delete(state_);
	manager_.GetAllocator().Delete(previousState_);
}

void
HoldGesture::Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, bool oneShot, uint64_t timeSpan)
{
	actionButton_.deviceId = actionButtonDevice;
	actionButton_.buttonId = actionButton;
	xAxis_.buttonId = InvalidDeviceButtonId;
	yAxis_.buttonId = InvalidDeviceButtonId;
	oneShot_ = oneShot;
	timeSpan_ = timeSpan;
}

void
HoldGesture::Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, 
		DeviceId xAxisDevice, DeviceButtonId xAxis, float xTolerance, 
		DeviceId yAxisDevice, DeviceButtonId yAxis, float yTolerance, 
		bool oneShot, 
		uint64_t timeSpan)
{
	actionButton_.deviceId = actionButtonDevice;
	actionButton_.buttonId = actionButton;
	xAxis_.deviceId = xAxisDevice;
	xAxis_.buttonId = xAxis;
	xTolerance_ = xTolerance;
	yAxis_.deviceId = yAxisDevice;
	yAxis_.buttonId = yAxis;
	yTolerance_ = yTolerance;
	oneShot_ = oneShot;
	timeSpan_ = timeSpan;
}

void
HoldGesture::Update(InputDeltaState* delta)
{
	if (actionButton_.buttonId == InvalidDeviceButtonId)
	{
		return;
	}

	*previousState_ = *state_;

	const InputDevice* actionDevice = manager_.GetDevice(actionButton_.deviceId);
	GAINPUT_ASSERT(actionDevice);

	bool positionValid = false;
	float posX, posY;
	if (xAxis_.buttonId != InvalidDeviceButtonId && yAxis_.buttonId != InvalidDeviceButtonId)
	{
		const InputDevice* xAxisDevice = manager_.GetDevice(xAxis_.deviceId);
		GAINPUT_ASSERT(xAxisDevice);
		posX = xAxisDevice->GetFloat(xAxis_.buttonId);
		const InputDevice* yAxisDevice = manager_.GetDevice(yAxis_.deviceId);
		GAINPUT_ASSERT(yAxisDevice);
		posY = yAxisDevice->GetFloat(yAxis_.buttonId);
		positionValid = true;
	}

	if (actionDevice->GetBool(actionButton_.buttonId))
	{
		if (firstDownTime_ == 0)
		{
			firstDownTime_ = manager_.GetTime();
			firstDownX_ = posX;
			firstDownY_ = posY;
		}
	}
	else
	{
		oneShotReset_ = true;
		firstDownTime_ = 0;
		state_->Set(HoldTriggered, false);
		return;
	}

	if (positionValid 
		&& (Abs(posX - firstDownX_) > xTolerance_ || Abs(posY - firstDownY_) > yTolerance_))
	{
		firstDownTime_ = 0;
	}

	bool downLongEnough = firstDownTime_ + timeSpan_ <= manager_.GetTime();

	if (oneShot_)
	{
		if (downLongEnough && oneShotReset_)
		{
			state_->Set(HoldTriggered, true);
			oneShotReset_ = false;
		}
		else
		{
			state_->Set(HoldTriggered, false);
		}
	}
	else
	{
		state_->Set(HoldTriggered, downLongEnough);
	}
}

}

#endif

