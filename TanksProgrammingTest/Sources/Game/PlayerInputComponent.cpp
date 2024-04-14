#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "MathLib.h"
#include "ProjectileSpawnerComponent.h"
#include "ResourceManager.h"
#include "Scene.h"


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
	SDL_Rect& SrcRectangle = m_TextureComponent->GetRectangle();

	int DeltaX = 0;
	int DeltaY = 0;

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
						DeltaY -= Speed / 30;
						break;
					case SDL_SCANCODE_A :
					case SDL_SCANCODE_LEFT :
						DeltaX -= Speed / 30;
						break;
					case SDL_SCANCODE_S :
					case SDL_SCANCODE_DOWN :
						DeltaY += Speed / 30;
						break;
					case SDL_SCANCODE_D :
					case SDL_SCANCODE_RIGHT :
						DeltaX += Speed / 30;
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

	//iterate all entities: todo return only nearby objects
	if(DeltaX == 0 && DeltaY == 0)
	{
		return;
	}

	SrcRectangle.x += DeltaX;
	SrcRectangle.y += DeltaY;

	//FixCollisions();
	Vector2 CollisionDelta(0, 0);
	FixCollisionsAABB(CollisionDelta);

	if (DeltaX != 0 && std::abs(CollisionDelta.x) < std::abs(DeltaX))
	{
		//moved horizontally set rotation in x
		GetOwner()->SetRotation(DeltaX > 0 ? FacingDirection::RIGHT : FacingDirection::LEFT);
	}
	else if (std::abs(CollisionDelta.y) < std::abs(DeltaY))
	{
		//moved vertically set rotation in y
		GetOwner()->SetRotation(DeltaY > 0 ? FacingDirection::DOWN : FacingDirection::UP);
	}
	
	//window limits:
	int MaxWidth = 0, MaxHeight = 0;
	SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

	if (SrcRectangle.x + SrcRectangle.w > MaxWidth)
	{
		SrcRectangle.x = MaxWidth - SrcRectangle.w;
	}

	if (SrcRectangle.x < 0)
	{
		SrcRectangle.x = 0;
	}

	if (SrcRectangle.y + SrcRectangle.h > MaxHeight)
	{
		SrcRectangle.y = MaxHeight - SrcRectangle.h;
	}

	if (SrcRectangle.y < 0)
	{
		SrcRectangle.y = 0;
	}

	//todo: change rotation on first click, on second move
}

void PlayerInputComponent::Shoot() const
{
	SDL_Log("Spawning projectile...");
	m_ProjectileSpawnerComponent->SpawnProjectile();
}

void PlayerInputComponent::FixCollisions()
{
	SDL_Rect& SrcRectangle = m_TextureComponent->GetRectangle();

	const std::vector<BoxColliderComponent*> Colliders = Engine::Get()->GetCollisionWorld()->GetBoxes();
	BoxColliderComponent* PlayerCollider = GetOwner()->GetComponent<BoxColliderComponent>();
	SDL_Rect PlayerRect{ *PlayerCollider->GetRectangle() };

	SDL_Rect Intersection{ 0, 0, 0, 0 };

	for (BoxColliderComponent* Collider : Colliders)
	{
		if (Collider != PlayerCollider && SDL_HasIntersection(Collider->GetRectangle(), PlayerCollider->GetRectangle()))
		{
			SDL_Rect* ColliderRect = Collider->GetRectangle();

			SDL_Rect IntersectionTemp{ 0, 0, 0, 0 };
			SDL_IntersectRect(ColliderRect, PlayerCollider->GetRectangle(), &IntersectionTemp);

			if (Intersection.w < IntersectionTemp.w) 
			{
				Intersection.w = IntersectionTemp.w;
				Intersection.x = IntersectionTemp.x;
			}
			if (Intersection.h < IntersectionTemp.h)
			{
				Intersection.h = IntersectionTemp.h;
				Intersection.y = IntersectionTemp.y;
			}
		}
	}

	if (Intersection.w < Intersection.h)
	{
		if (Intersection.x == PlayerRect.x)
		{
			SrcRectangle.x += Intersection.w;
		}

		if (Intersection.x > PlayerRect.x)
		{
			SrcRectangle.x -= Intersection.w;
		}
	}
	else
	{
		if (Intersection.y == PlayerRect.y)
		{
			SrcRectangle.y += Intersection.h;
		}

		if (Intersection.y > PlayerRect.y)
		{
			SrcRectangle.y -= Intersection.h;
		}
	}


}

void PlayerInputComponent::FixCollisionsAABB(Vector2& CollisionDelta)
{
	SDL_Rect& SrcRectangle = m_TextureComponent->GetRectangle();

	const std::vector<BoxColliderComponent*> Colliders = Engine::Get()->GetCollisionWorld()->GetBoxes();
	BoxColliderComponent* PlayerCollider = GetOwner()->GetComponent<BoxColliderComponent>();
	PlayerCollider->OnUpdateWorldTransform();

	for (BoxColliderComponent* Collider : Colliders)
	{
		Vector2 TempCollisionDelta(0, 0);

		if (Collider != PlayerCollider && PlayerCollider->TryGetCollisionDelta(*Collider, TempCollisionDelta))
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

	if(MathLib::NearZero(CollisionDelta.x) && MathLib::NearZero(CollisionDelta.y))
	{
		return;
	}

	SrcRectangle.x -= static_cast<int>(CollisionDelta.x);
	SrcRectangle.y -= static_cast<int>(CollisionDelta.y);

	PlayerCollider->OnUpdateWorldTransform();//todo and other comps
}
