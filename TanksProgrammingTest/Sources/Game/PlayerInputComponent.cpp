#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "MathLib.h"
#include "PlayerProjectileSpawnerComponent.h"


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
	//SDL_Log("Delta time: %f", DeltaTime);
	const int Speed = 100;

	int PredictedDeltaX = 0;
	int PredictedDeltaY = 0;

	std::vector<SDL_Event> Events = Engine::Get()->GetEvents();
	
	for (const SDL_Event& Event : Events)
	{
		switch (Event.type) 
		{
			case SDL_KEYDOWN :
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

	if(PredictedDeltaX == 0 && PredictedDeltaY == 0)
	{
		return;
	}

	GetOwner()->SetTranslation(PredictedDeltaX, PredictedDeltaY);
	
	Vector2 BacktraceCollisionDelta(0, 0);

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
