#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "MathLib.h"
#include "PlayerProjectileSpawnerComponent.h"
#include "magic_enum/magic_enum_all.hpp"
#include "ActionKeyMapping.h"
#include "InputActionBindings.h"

PlayerInputComponent::PlayerInputComponent(Entity* Owner)
	: EntityComponent(Owner)
{
}

PlayerInputComponent::PlayerInputComponent()
	: PlayerInputComponent(nullptr)
{
}

void PlayerInputComponent::Initialize()
{
	m_ProjectileSpawnerComponent = GetOwner()->GetComponent<PlayerProjectileSpawnerComponent>();
	m_BoxColliderComponent = GetOwner()->GetComponent<BoxColliderComponent>();

	//todo initialize bindings
}

void PlayerInputComponent::Update(float DeltaTime)
{
	//SDL_Log("Player input: Delta time: %f", DeltaTime);
	const int Speed = 100;

	int PredictedDeltaX = 0;
	int PredictedDeltaY = 0;

	std::vector<SDL_Event> Events = Engine::Get()->GetEvents();
	//get all events, bind delegate to action, and to when this action is triggered
	//action has different states, started, triggered, ended
	//mapper binds action to an input, when input is pressed/triggered/held - define this kind of events, process sub inputs into complex events - then
	//there is an array of FInputActionEventBinding, which has action, trigger event that rises delegate and virtual execute method that takes in action
	//from this class next in inheritance is FInputActionEventDelegateBinding that also includes the delagate that is called within execute method
	//the array includes the least derived class, it is contained within input component, has const accessor
	//the bindings are looped in EvaluateInputDelegates:
	
	//*****EvaluateInputDelegates start and setup****
		//1. Loops through action mappings, any key state is special, ignore for now
		//2. Other key mappings have their key state retrieved by key (SDL_Left etc.) from mapping
		//3. Additional helper containers like KeyDownPrevious etc. bKeyIsHeld = bKeyIsDown/*state*/ && bDownLastTick
		//4. KeyEvent: either held or actuated (released of pressed) or NONE
		//5. Establish whether key was pressed this tick if there is a pointer valid FVector* PressedThisTickValue = KeysPressedThisTick.Find(Mapping.Key);
		//6. ProcessKeyMappingEvent with acquired values: (Mapping.Action, deltaTime, bGamePaused, RawKeyValue, KeyEvent, Mapping.Modifiers, Mapping.Triggers) 
	//*****ProcessKeyMappingEvent******
		//1. Adds of finds appropriate ActionData from mapping.Action
		//2. If key event is NOT NONE or bHasAnyAlwaysTickTriggers it will proceed with updating values
		//3. tries to reset action data and add it to ActionsWithEventsThisTick if: bool bResetActionData = !ActionsWithEventsThisTick.Contains(Action);
		//4. Reads value FInputActionValue from action data, from modifiers, value type(boolean, axis1D etc.), raw key value and delta time
		//5. Sets or updates (cumulates) the value: ActionData.Value = FInputActionValue(ValueType, Merged);
	//*****EvaluateInputDelegates end****
		//1. More complex logic in EvaluateInputDelegates in iteration over updated ActionInstanceData: updating triggers, trigger states etc. 
		//2. Now when EvaluateInputDelegates processes delegates and iterates over ActionInstanceData
		//	for each action instance delegate will be called with their values
		//3. ActionsWithEventsThisTick after updating values will re-evaluate trigger state 
	//*****EvaluateInputDelegates loop****
		//1. get action data for binding->getAction 
		//2. match it against action data or against started && Actiondata->internal == startedAndTriggered
		//3. if bound trigger event is started, emplace it at 0 in TriggeredDelegates(0, Binding->Clone);
		//4. else just emplace(Binding->Clone())
		//5. if event is triggered type, od it to TriggeredActionsThisTick.Add(ActionData->GetSourceAction());
		//6. action data is input action from the binding, it is used to pull it up from instance action data
		//7. input action pointer is mapped to FInputActionInstance(InputAction) added with FindOrAdd(const InputAction*) called from ProcessActionMappingsEvent
		//8. ProcessInputStack calls EvaluateInputDelegates calls ProcessActionMappingsEvent
	//****How triggered delegates started (list begin) and other, and TriggeredActionsThisTick are treated****
		//1. They are looped over in next loop
		//2. Checked for DependantAction (optional in chord actions) for this is used TriggeredThisTick
		//3. Action data is retrieved once again for this Delegate->GetAction and calls delegate with this action pointer (can ignore or can retireve action value)
		//4. Triggered delegates and triggered delegates this tick are emptied (they were filled with matching actions).
		//5. Action value bindings are updated with current value (its used for dynamic binding and blueprints
			
	const Uint8* keystates = SDL_GetKeyboardState(NULL);//bools true for pressed, false for not pressed

	/*for (auto keyCode : KeysPressedThisTick)
	{

	}*/

	/*for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
		if (keystates[i]) {
			std::cout << "Key with scancode " << i << " is pressed." << std::endl;
		}
	}*/

	//Evaluate Key map state
	for (auto& pair : KeyStateMap)
	{
		bool currentState = keystates[pair.first];
		pair.second.bDownPrevious = pair.second.bDown;
		pair.second.bDown = currentState;
		KeyDownPrevious.emplace(pair.first, pair.second.bDownPrevious);//todo check if I need to reset it first and reserve a new
	}


	//if (keystates[SDL_SCANCODE_UP])//for count use: SDL_NUM_SCANCODES
	//{
	//	//SDL_Log("Player input: UP is PRESSED");
	//	//get to Keymaps and check for pressed keys and update bPrevious
	//}

	//if key is not in map, then it is not pressed, check this from Unreal: `enum EInputEvent : int`

	for (const SDL_Event& Event : Events)
	{
		switch (Event.type)
		{
			//register key down, movement key down will only orient the player, key being pressed will move the player
			//must bind scan code to delegate, use map with scan code
			case (SDL_KEYDOWN)://todo register any key that was pressed
			{
				//todo fill KeyMapStates:
				switch (Event.key.keysym.scancode)
				{
					case SDL_SCANCODE_W :
					case SDL_SCANCODE_UP :
						PredictedDeltaY -= Speed / 30;
						break;
					case SDL_SCANCODE_A :
					case SDL_SCANCODE_LEFT :
						PredictedDeltaX -= Speed / 30;
						break;
					case SDL_SCANCODE_S :
					case SDL_SCANCODE_DOWN :
						PredictedDeltaY += Speed / 30;
						break;
					case SDL_SCANCODE_D :
					case SDL_SCANCODE_RIGHT :
						PredictedDeltaX += Speed / 30;
						break;
					case SDL_SCANCODE_SPACE ://SHOOT AND RETURN
						Shoot();
						return;
					default:
						break;
				}
			}
		}

		if (Event.type == SDL_KEYDOWN)//when to remove
		{
			SDL_Scancode keyCode = Event.key.keysym.scancode;
			if (KeyDownPrevious.contains(keyCode) == false ||
				KeyDownPrevious[keyCode] == false)
			{
				KeyState keyState;
				keyState.bDown = true;
				KeyStateMap.emplace(keyCode, keyState);//try find first, and check if emplace on top works
			}
			//add to keymap with values, copy (pressed previous frame) , held state
		}
		else if (Event.type == SDL_KEYUP)//dont need this, if the key is not bDown, but was bPrevious than it means that is was released
		{

		}
	}

	//Evaluate delegates, iterate by
	for (const auto& mapping : m_ActionKeyMappings)
	{
		auto keyCode = mapping.Key;
		auto it = KeyStateMap.find(keyCode);
		if (it == KeyStateMap.end()) {
			continue;
		}
		auto keyState = it->second;

		bool bKeyIsReleased = !keyState.bDown && keyState.bDownPrevious;
		bool bKeyIsHeld = keyState.bDown && keyState.bDownPrevious;

		KeyEventType keyEvent = bKeyIsHeld ? KeyEventType::Held
			: (bKeyIsHeld || bKeyIsReleased) ? KeyEventType::Actuated
			: KeyEventType::None;

		ProcessActionMapping(mapping.Action, DeltaTime, keyEvent, keyState.bDown/*conversion*/);
	}

	//todo post tick processing of action instanced data
	// ...
	// 
	/////For each input component (todo in the future):
	// Evaluate action event bindings:
	static std::vector<std::unique_ptr<InputActionEventBinding/*is like interface*/>> TriggeredDelegates;//clone delegates into it
	//iterate over bindings:
	// ...
	// Evaluate action value bindings:
	// ...
	//this will be irrelevant:

	if(PredictedDeltaX == 0 && PredictedDeltaY == 0)
	{
		return;
	}

	GetOwner()->SetTranslation(PredictedDeltaX, PredictedDeltaY);
	
	Vector2Int BacktraceCollisionDelta(0, 0);

	if(auto BoxCollider = m_BoxColliderComponent.lock())
	{
		BoxCollider->BacktraceCollisionsDelta(BacktraceCollisionDelta);
	}

	if (PredictedDeltaX != 0 && std::abs(BacktraceCollisionDelta.x) < std::abs(PredictedDeltaX))
	{
		//moved horizontally set rotation in x
		GetOwner()->SetFacingDirection(PredictedDeltaX > 0 ? FacingDirection::Right : FacingDirection::Left);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
	else if (std::abs(BacktraceCollisionDelta.y) < std::abs(PredictedDeltaY))
	{
		//moved vertically set rotation in y
		GetOwner()->SetFacingDirection(PredictedDeltaY > 0 ? FacingDirection::Down : FacingDirection::Up);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
}

InputActionInstance& PlayerInputComponent::FindOrAddActionInstanceData(std::shared_ptr<const InputAction> InputAction)
{
	auto It = ActionInstanceData.find(InputAction);

	if (It != ActionInstanceData.end())
	{
		return It->second;
	}

	return (ActionInstanceData.emplace(InputAction, InputActionInstance(InputAction))).first->second;
}

void PlayerInputComponent::Shoot() const
{
	if(auto ProjectileSpawner = m_ProjectileSpawnerComponent.lock())
	{
		ProjectileSpawner->DoSpawn();
	}
}

//TODO must map this delegate, map each key to direction, also move to controller class!
void PlayerInputComponent::Move(const InputActionValue<Vector2>& value) const//substitute for specializations of InputActionValue Get(), 
{
	const int Speed = 100;
	auto delta = Speed / 30;

	auto [PredictedDeltaX, PredictedDeltaY] = Vector2 (value.Get().x * delta, value.Get().y * delta);

	if (PredictedDeltaX == 0 && PredictedDeltaY == 0)
	{
		return;
	}

	GetOwner()->SetTranslation(PredictedDeltaX, PredictedDeltaY);

	Vector2Int BacktraceCollisionDelta(0, 0);

	if (auto BoxCollider = m_BoxColliderComponent.lock())
	{
		BoxCollider->BacktraceCollisionsDelta(BacktraceCollisionDelta);
	}

	if (PredictedDeltaX != 0 && std::abs(BacktraceCollisionDelta.x) < std::abs(PredictedDeltaX))
	{
		//moved horizontally set rotation in x
		GetOwner()->SetFacingDirection(PredictedDeltaX > 0 ? FacingDirection::Right : FacingDirection::Left);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
	else if (std::abs(BacktraceCollisionDelta.y) < std::abs(PredictedDeltaY))
	{
		//moved vertically set rotation in y
		GetOwner()->SetFacingDirection(PredictedDeltaY > 0 ? FacingDirection::Down : FacingDirection::Up);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
}

void PlayerInputComponent::ProcessActionMapping(const std::shared_ptr<const InputAction> Action, float DeltaTime, KeyEventType KeyEvent, InputActionValue<bool> RawActionValue)
{
	auto InstanceActionData = FindOrAddActionInstanceData(Action);

	//todo add TriggerStateTracker for trigger states
	//todo get action value type from actiondata.value.getvaluetype() for
	if (KeyEvent != KeyEventType::None /*or should always trigger*/)
	{
		//todo apply modifiers to raw key value
		InstanceActionData.Value = RawActionValue;//todo later set to modified

		//TODO!!!!!!!!!
		//for some reason trigger events are post tick
		//use trigger state conversions from `GetTriggerStateChangeEvent`
	}

	//todo set trigger trackers
}
