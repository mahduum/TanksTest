#pragma once

#include "EntityComponent.h"
#include "Engine.h"
#include "TDelegate.h"
#include <optional>
#include <functional>

class BoxColliderComponent;
class PlayerProjectileSpawnerComponent;
class ProjectileSpawnerComponent;
class Vector2Int;
class TextureComponent;

enum class TriggerEvent : Uint8
{
	None,
	Started,
	Triggered,
	Completed,
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

	InputActionValue(T inValue) : m_Value(inValue){}

	constexpr InputActionValueType GetType() const
	{
		return ValueTypeTraits<T>::value;
	}

protected:
	T m_Value;
};

class InputAction
{
public:
	InputActionValueType InputActionValueType = InputActionValueType::Boolean;
	/*Should this action consume input or allow it to pass through and affect other actions with lower priority?*/
	bool bConsumeInput = true;
};

class InputActionInstance//is stored in ActionInstanceData
{
public:
	InputActionInstance(const std::shared_ptr<InputAction>& inputAction) : m_SourceAction(inputAction)
	{
	}

	const std::shared_ptr<InputAction>& GetSourceAction() { return m_SourceAction; }

private:
	//injected from action mappings
	const std::shared_ptr<InputAction> m_SourceAction;//it is assumed to always exist, so it is shared pointer
};

struct InputActionEventBinding
{
public:
	//todo add constructor for input action and trigger event
	virtual void Execute(const InputActionInstance& ActionData) const = 0;

private:
	std::weak_ptr<InputAction> Action;//weak because we are cloning it?
	TriggerEvent TriggerEvent = TriggerEvent::None;
};

//different delegate signatures:
using ExampleDelegate = TDelegate<void(const InputActionInstance&)>;

template<typename TSignature>//a signature for which we will create specializations with concrete types 
struct InputActionEventDelegateBinding : public InputActionEventBinding
{
public:
	//todo add cloning
	virtual void Execute(const InputActionInstance& ActionData) const override;

	TDelegate<TSignature> m_Delegate;//set this delegate
};

template<>
inline void InputActionEventDelegateBinding<ExampleDelegate>::Execute(const InputActionInstance& ActionData) const
{
}

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<PlayerInputComponent>(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void Shoot() const;

private:
	std::weak_ptr<PlayerProjectileSpawnerComponent> m_ProjectileSpawnerComponent;
	std::weak_ptr<BoxColliderComponent> m_BoxColliderComponent;

	std::optional<uint16_t> m_PressedKey = std::nullopt;
};