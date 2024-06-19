#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "MathLib.h"
#include "PlayerProjectileSpawnerComponent.h"
#include "magic_enum/magic_enum_all.hpp"

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
			
	const Uint8* keystates = SDL_GetKeyboardState(NULL);//todo use this for continuous movement

	if (keystates[SDL_SCANCODE_UP]) {
		//SDL_Log("Player input: UP is PRESSED");
	}

	for (const SDL_Event& Event : Events)
	{
		switch (Event.type)
		{
			//register key down, movement key down will only orient the player, key being pressed will move the player
			//must bind scan code to delegate, use map with scan code
			case (SDL_KEYDOWN):
			{
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
	}

	if (m_PressedKey.has_value()) {

	}

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

void PlayerInputComponent::Shoot() const
{
	if(auto ProjectileSpawner = m_ProjectileSpawnerComponent.lock())
	{
		ProjectileSpawner->DoSpawn();
	}
}
