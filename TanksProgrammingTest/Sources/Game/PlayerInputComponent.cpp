#include "PlayerInputComponent.h"

#include "BoxColliderComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"


PlayerInputComponent::PlayerInputComponent(Entity* Owner)
	: EntityComponent(Owner)
	, m_TextureComponent(nullptr)
{
}

PlayerInputComponent::PlayerInputComponent()
	: PlayerInputComponent(nullptr)
{
}

void PlayerInputComponent::Initialize()
{
	m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();
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

	FixCollisions();
	
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
