#pragma once
#include <memory>
#include "InputActions.h"

struct InputActionEventBinding
{
public:
	InputActionEventBinding() = default;
	InputActionEventBinding(std::shared_ptr<const InputAction> InAction, TriggerEventType TriggerEvent)
		: Action(InAction), TriggerEvent(TriggerEvent)
	{

	}
	//todo add constructor for input action and trigger event
	virtual std::unique_ptr<InputActionEventBinding> Clone() const = 0;//clone with new
	virtual void Execute(const InputActionInstance& ActionData) const = 0;

private:
	std::weak_ptr<const InputAction> Action;//weak because we are cloning it?
	TriggerEventType TriggerEvent = TriggerEventType::None;
};

//different delegate signatures:
using ExampleDelegate = TInputActionDelegate<void(const InputActionInstance&)>;
//todo bindAction method will construct this delegate and 

template<typename TSignature>//a signature for which we will create specializations with concrete types 
struct InputActionEventDelegateBinding : public InputActionEventBinding
{
	//InputActionEventDelegateBinding(const InputActionEventDelegateBinding<TSignature>& CloneSource)//todo change to use handles
	//{

	//}
	InputActionEventDelegateBinding(std::shared_ptr<const InputAction> InAction, TriggerEventType TriggerEvent)
		: InputActionEventBinding(InAction, TriggerEvent)
	{}

	//todo add cloning
	virtual std::unique_ptr<InputActionEventBinding> Clone() const override
	{
		return std::make_unique<InputActionEventDelegateBinding<TSignature>>(*this);
	}

	virtual void Execute(const InputActionInstance& ActionData) const override;

	TInputActionDelegate<TSignature> m_Delegate;//set this delegate, it will be called in execute implementation function, TODO: I must only ensure that whatever delegate I am binding, that it will be compatible with TSignature
};

template<>
inline void InputActionEventDelegateBinding<ExampleDelegate>::Execute(const InputActionInstance& ActionData) const
{
}

