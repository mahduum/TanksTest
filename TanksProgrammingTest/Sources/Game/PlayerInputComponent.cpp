#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "MathLib.h"
#include "ProjectileSpawnerComponent.h"


PlayerInputComponent::PlayerInputComponent(Entity* Owner)
	: EntityComponent(Owner)
	, m_TextureComponent(nullptr)
	, m_ProjectileSpawnerComponent(nullptr)
{
}

PlayerInputComponent::PlayerInputComponent()
	: PlayerInputComponent(nullptr)
{
}

void PlayerInputComponent::Initialize()
{
	m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();
	m_ProjectileSpawnerComponent = GetOwner()->GetComponent<ProjectileSpawnerComponent>();
}

void PlayerInputComponent::Update(float DeltaTime)
{
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

	//TODO: All below goes into transform component, if this is a collision fixer

	if(PredictedDeltaX == 0 && PredictedDeltaY == 0)
	{
		return;
	}

	GetOwner()->SetTranslation(PredictedDeltaX, PredictedDeltaY);
	
	Vector2 BacktraceCollisionDelta(0, 0);

	FixCollisionsAABB(BacktraceCollisionDelta);//if collision delta is the same as movement delta then now update

	if (PredictedDeltaX != 0 && std::abs(BacktraceCollisionDelta.x) < std::abs(PredictedDeltaX))
	{
		//moved horizontally set rotation in x
		GetOwner()->SetRotation(PredictedDeltaX > 0 ? FacingDirection::RIGHT : FacingDirection::LEFT);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
	else if (std::abs(BacktraceCollisionDelta.y) < std::abs(PredictedDeltaY))
	{
		//moved vertically set rotation in y
		GetOwner()->SetRotation(PredictedDeltaY > 0 ? FacingDirection::DOWN : FacingDirection::UP);
		GetOwner()->SetComponentsTransformDirty();//will update in entity update
	}
	else
	{
		SDL_Log("Entity cannot move.");
	}
}

void PlayerInputComponent::FixCollisionsAABB(Vector2& CollisionDelta)
{
	//SDL_Rect& SrcRectangle = m_TextureComponent->GetRectangle();

	const std::vector<BoxColliderComponent*> Colliders = Engine::Get()->GetCollisionWorld()->GetBoxes();
	BoxColliderComponent* OwnersCollider = GetOwner()->GetComponent<BoxColliderComponent>();

	//first update only collider for testing
	OwnersCollider->OnUpdateWorldTransform();//todo with line segment first cache the previous position then the next one and combine boxes prev-predicted to do sweep
												//for diagonal shots can do rotated box collision check

	for (BoxColliderComponent* Collider : Colliders)
	{
		Vector2 TempCollisionDelta(0, 0);

		if (Collider != OwnersCollider && OwnersCollider->TryGetCollisionDelta(*Collider, TempCollisionDelta))
		{
			SDL_Log("Found collision with delta x: %f, y: %f", TempCollisionDelta.x, TempCollisionDelta.y);
			if(MathLib::Abs(TempCollisionDelta.x) > MathLib::Abs(CollisionDelta.x))
			{
				CollisionDelta.x = TempCollisionDelta.x;
			}

			if (MathLib::Abs(TempCollisionDelta.y) > MathLib::Abs(CollisionDelta.y))
			{
				CollisionDelta.y = TempCollisionDelta.y;
			}
		}
	}

	auto OwnersBox = OwnersCollider->GetBox();
	int MaxWidth = 0, MaxHeight = 0;
	SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

	if (MathLib::NearZero(CollisionDelta.x))
	{
		//check screen
		//window limits:

		if (OwnersBox.m_Max.x > MaxWidth)
		{
			CollisionDelta.x = OwnersBox.m_Max.x - MaxWidth;
		}

		if (OwnersBox.m_Min.x < 0)
		{
			CollisionDelta.x = OwnersBox.m_Min.x;
		}
	}

	if (MathLib::NearZero(CollisionDelta.y))
	{
		if (OwnersBox.m_Max.y > MaxHeight)
		{
			CollisionDelta.y = OwnersBox.m_Max.y - MaxHeight;
		}

		if (OwnersBox.m_Min.y < 0)
		{
			CollisionDelta.y = OwnersBox.m_Min.y;
		}
	}

	if (MathLib::NearZero(CollisionDelta.x) && MathLib::NearZero(CollisionDelta.y))
	{
		return;
	}

	//SrcRectangle.x -= static_cast<int>(CollisionDelta.x);
	//SrcRectangle.y -= static_cast<int>(CollisionDelta.y);

	GetOwner()->SetTranslation(-static_cast<int>(CollisionDelta.x), -static_cast<int>(CollisionDelta.y));

	//todo only here update everything with position:
	//...

	OwnersCollider->OnUpdateWorldTransform();//todo and other comps
}

void PlayerInputComponent::Shoot() const
{
	SDL_Log("Spawning projectile...");
	m_ProjectileSpawnerComponent->SpawnProjectile();
}
