
#ifndef GAINPUTINPUTDEVICETOUCHANDROID_H_
#define GAINPUTINPUTDEVICETOUCHANDROID_H_

namespace gainput
{
	
class InputDeviceTouchImpl
{
public:
	InputDeviceTouchImpl(InputManager& manager, DeviceId device);
	~InputDeviceTouchImpl();

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	int32_t HandleInput(AInputEvent* event);

private:
	InputManager& manager_;
	DeviceId device_;
	InputState* state_;
	InputState* previousState_;
	InputDeltaState* delta_;
};

}

#endif
