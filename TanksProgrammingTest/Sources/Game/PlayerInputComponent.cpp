#include "PlayerInputComponent.h"
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
	m_TextureComponent = GetOwner()->GetComponent<TextureCompoent>();
}

void PlayerInputComponent::Update(float DeltaTime)
{
	const int Speed = 300;
	SDL_Rect& Rectangle = m_TextureComponent->GetRectangle();
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
						Rectangle.y -= Speed / 30;
						break;
					case SDL_SCANCODE_A :
					case SDL_SCANCODE_LEFT :
						Rectangle.x -= Speed / 30;
						break;
					case SDL_SCANCODE_S :
					case SDL_SCANCODE_DOWN :
						Rectangle.y += Speed / 30;
						break;
					case SDL_SCANCODE_D :
					case SDL_SCANCODE_RIGHT :
						Rectangle.x += Speed / 30;
						break;
					default:
						break;
				}
			}
		}
	}

	int MaxWidth = 0, MaxHeight = 0;
	SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

	if (Rectangle.x + Rectangle.w > MaxWidth)
	{
		Rectangle.x = MaxWidth - Rectangle.w;
	}

	if (Rectangle.x < 0)
	{
		Rectangle.x = 0;
	}

	if (Rectangle.y + Rectangle.h > MaxHeight)
	{
		Rectangle.y = MaxHeight - Rectangle.h;
	}

	if (Rectangle.y < 0)
	{
		Rectangle.y = 0;
	}
}
