#pragma once

#include <optional>
#include <functional>
#include <unordered_set>
#include "EntityComponent.h"
#include "Engine.h"
#include "TDelegate.h"
#include "InputActions.h"
#include "KeyState.h"
#include "ActionKeyMapping.h"
#include "InputActionBindings.h"

class BoxColliderComponent;
class PlayerProjectileSpawnerComponent;
class ProjectileSpawnerComponent;
class Vector2Int;
class TextureComponent;

/*
* ProcessInputStack:
1. Register in array KeysPressedThisTick (from SDL event poll) and KeysReleasedThisTick.
* EvaluateKeyMapState:
1. Reserve vector for KeyDownPrevious for KeyStateMap.num
2. For each key in KeyStateMap evaluate bWasDown based on bDownPrevious or events
2. Update KeyStateMap [key, keystate], state: value, bDown, bDownPrevious, bConsumed, VecRawValueAcc, num events received
* EvaluateInputDelegates:
1. Get key state from KeyStateMap by action mappings given key - make method for it.
2. Retrieve the key value - if it is a vector then retrieve the vector.
3. Check if it was down last tick (KeyDownPrevious map key to boolean)
4. If the key is in map `KeysPressedThisTick` or equivalent update raw key value.
* ProcessActionMappingEvent (In loop for each mapping within EvaluateInputDelegates):
1. Set bool resetActionData with ActionsWithEventsThisTick if does not contain current action for the first event
2. If some key event (Actuated or Held) or always triggers, recalculate values for instances, merge, accumulate etc.
* Post tick UpdateActionInstanceData:
1. Get through ActionsWithEventsThisTick and update times and trigger events
* EventDelegatesBindings (for each input component, each component has its own value in the stack):
1. For each event binding get ActionDataInstance based on InputAction in binding
2. Add all trigger event types delegates (but Started in front) by cloning to TriggeredDelegates array.
3. Add trigger event type Triggered to TriggeredActionsThisTick array.
4. Iterate through triggred and execute with action data (TriggeredActionsThisTick only used to set bool for DependentChordActions)
5. Clear both arrays, LastFrameTime = CurrentFrameTime, reset KeysPressedThisTick or equivalent.
* FinishProcessingInput
1. Reiterate KeyStateMap and set: KeyState.bDownPrevious = KeyState.bDown;
*/
class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<PlayerInputComponent>(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;//gathers events

	/* Called in `ProcessActionMappingEvent (TObjectPtr<const UInputAction> Action, float DeltaTime, bool bGamePaused, FInputActionValue RawKeyValue, EKeyEvent KeyEvent, const TArray<UInputModifier*>& Modifiers, const TArray<UInputTrigger*>& Triggers)`
	and is responsible for updating values inside InputActionInstances based on trigger events and modifiers and delta time.
	Is called from `EvaluateInputDelegates(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused, const TArray<TPair<FKey, FKeyState*>>& KeysWithEvents)`
	where values from mappings are retrieved (InputAction, modifiers, triggers, delta time and key event is established:
	`EKeyEvent KeyEvent = bKeyIsHeld ? EKeyEvent::Held : ((bKeyIsDown || bKeyIsReleased) ? EKeyEvent::Actuated : EKeyEvent::None);` based on `KeyState`
	if action is not in actions of this tick, it is reset and added to ActionsWithEventsThisTick they will have additional treatment (additional modifiers, state tracking)
	
	Then delegate bindings will look for const instance datas, compare trigger markers and be called with values*/

	void Shoot() const;//todo define elsewhere for example in controller
	void Move(const InputActionValue<Vector2>& value) const;//todo define elsewhere for example in controller

private:
	InputActionInstance& FindOrAddActionInstanceData(std::shared_ptr<const InputAction> InputAction);
	void ProcessActionMapping(std::shared_ptr<const InputAction> Action, float DeltaTime, KeyEventType KeyEvent, InputActionValue<bool> RawActionValue  /*todo but cannot be generic InputActionValue ActionValue and input trigger*/);

	std::weak_ptr<PlayerProjectileSpawnerComponent> m_ProjectileSpawnerComponent;
	std::weak_ptr<BoxColliderComponent> m_BoxColliderComponent;

	std::optional<uint16_t> m_PressedKey = std::nullopt;

	std::vector<ActionKeyMapping> m_ActionKeyMappings;//todo: filled from InputMappingContext

	//unreal simply processes pairs
	std::unordered_map<std::shared_ptr<const InputAction>, InputActionInstance> ActionInstanceData;

	std::unordered_set<SDL_Scancode> KeysPressedThisTick;

	std::unordered_map<SDL_Scancode, KeyState> KeyStateMap;

	std::unordered_map<SDL_Scancode, bool> KeyDownPrevious;

public:
	//call this in initialize
	template<class UserClass, typename TSignature, typename... VarTypes>
	InputActionEventBinding& BindAction(std::shared_ptr<const InputAction> Action, TriggerEventType TriggerEvent, UserClass& Object, TSignature Func, VarTypes... Vars)
	{
		//created pointer to event delegate binding
		auto ActionBinding = std::make_unique <InputActionEventDelegateBinding<TSignature>>(Action, TriggerEvent);//main delegate is defined by signature, what input data is forwarded to the delegate
		//bind to internal delegate the function that is being passed
		ActionBinding->m_Delegate.BindDelegate(Object, Func, Vars...);//Vars are args to be bound with this delegate (additional input that subsriber wants to add), the delegate combines its own ParamTypes... and Method pointer, ParamTypes..., Vars...
			//move pointer to array of unique pointers ActionEventBindings
		return ActionBinding;
	}
};