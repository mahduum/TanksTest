#pragma once

#include "Engine.h"
#include "TDelegate.h"
#include <optional>
#include <functional>

enum class TriggerEventType : Uint8
{
	None,
	Started,
	Triggered,
	Completed,
	Any
};

enum class InputActionValueType : Uint8
{
	Boolean,
	Axis1D,
	Axis2D
};


template<typename T>
struct ValueTypeTraits;

template<>
struct ValueTypeTraits<bool>
{
	static constexpr InputActionValueType value = InputActionValueType::Boolean;
};

template<>
struct ValueTypeTraits<float>
{
	static constexpr InputActionValueType value = InputActionValueType::Axis1D;
};

template<>
struct ValueTypeTraits<Vector2>
{
	static constexpr InputActionValueType value = InputActionValueType::Axis2D;
};

template<typename T>
concept IsInputActionValueType = std::is_same_v<T, bool> ||
std::is_same_v<T, float> ||
std::is_same_v<T, Vector2>;

template<IsInputActionValueType T>
struct InputActionValue//Input action instance will have value and all runtime specific properties, time elapsed, applied modiriers, value etc. and pointer to source InputAction
{
public:

	InputActionValue(T inValue) : m_Value(inValue) {}
	// Support all relevant default constructors (FInputActionValue isn't movable)
	InputActionValue() = default;
	InputActionValue(const InputActionValue&) = default;
	InputActionValue& operator= (const InputActionValue&) = default;

	constexpr InputActionValueType GetType() const
	{
		return ValueTypeTraits<T>::value;
	}

	const T Get() const { return m_Value; };

protected:
	T m_Value;
};

//TODO json config populates the vector with input actions, but they need to be used to create action instances
//InputComponent will create and cache input action instances at runtime
//TODO if a key is pressed then we find matching mapping and we get the action and we find delegate for that action
class InputAction//create in json, todo bind input action to key in keymappings
{
public:
	InputActionValueType InputActionValueType = InputActionValueType::Boolean;
	/*Should this action consume input or allow it to pass through and affect other actions with lower priority?*/
	bool bConsumeInput = true;
};

class InputActionInstance//is stored in ActionInstanceData
{
	friend class PlayerInputComponent;
public:
	InputActionInstance(const std::shared_ptr<const InputAction>& inputAction) : m_SourceAction(inputAction)
	{
	}

	const std::shared_ptr<const InputAction>& GetSourceAction() { return m_SourceAction; }
	TriggerEventType GetTriggerEvent() { return TriggerEvent; }
	InputActionValue<bool> GetValue() const { return Value; }//todo correct that will have value only if triggerEvent is Triggered, else override with 0;

protected:
	TriggerEventType TriggerEvent = TriggerEventType::None;//make available for friends like input component but provide public accs
	InputActionValue<bool> Value;//remove generic
	// Total trigger processing/evaluation time (How long this action has been in event Started, Ongoing, or Triggered
	float ElapsedProcessedTime = 0.f;
	// Triggered time (How long this action has been in event Triggered only)
	float ElapsedTriggeredTime = 0.f;

private:
	//injected from action mappings
	const std::shared_ptr<const InputAction> m_SourceAction;//it is assumed to always exist, so it is shared pointer
};

