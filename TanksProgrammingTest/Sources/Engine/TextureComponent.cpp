#include "TextureComponent.h"
#include "Engine.h"
#include "Entity.h"
#include "../Game/PlayerInputComponent.h"

TextureComponent::TextureComponent(Entity* Owner)
	: EntityComponent(Owner)
	  , m_Rectangle{0, 0, 0, 0}, m_OffsetX(0), m_OffsetY(0), Surface(nullptr), Texture(nullptr)
{
}

TextureComponent::TextureComponent()
	: TextureComponent(nullptr)
{
}

void TextureComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);
	std::string TextureName = Config.value("Texture", "");
	if (!TextureName.empty())
	{
		SetTextureFromAssetName(TextureName);
	}

	m_Rectangle.w = Config.value("Width", 10);
	m_Rectangle.h = Config.value("Height", 10);
	m_Rectangle.x = Config.value("PositionX", 0);
	m_Rectangle.y = Config.value("PositionY", 0);

	m_OffsetX = Config.value("OffsetX", 0);
	m_OffsetY = Config.value("OffsetY", 0);
	//set position in owner
}

void TextureComponent::Initialize()
{
	Surface = IMG_Load(TexturePath.c_str());
	Texture = SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), Surface);
}

void TextureComponent::UnInitialize()
{
	if(Texture != nullptr)
	{
		SDL_DestroyTexture(Texture);
	}

	if(Surface != nullptr)
	{
		SDL_FreeSurface(Surface);
	}
}

void TextureComponent::Draw()
{
	//SDL_RenderCopy(Engine::Get()->GetRenderer(), Texture, nullptr, &m_Rectangle);

	if(GetOwner()->GetComponent<PlayerInputComponent>().has_value() || GetOwner()->GetName() == "EnemyTank")
	{
		const SDL_Point Center{ m_Rectangle.w/2, m_Rectangle.h/2 };
		SDL_Rect Rect{ m_Rectangle.x, m_Rectangle.y, m_Rectangle.h, m_Rectangle.w };
		SDL_RenderCopyEx(Engine::Get()->GetRenderer(), Texture, nullptr, &m_Rectangle, GetOwner()->GetRotationDegrees(), &Center, SDL_FLIP_NONE);
	}
	else
	{
		SDL_RenderCopy(Engine::Get()->GetRenderer(), Texture, nullptr, &m_Rectangle);
	}
}

void TextureComponent::OnUpdateSceneTransform()
{
	auto [x, y] = GetOwner()->GetPositionXY();
	SetPosition(x - m_OffsetX, y - m_OffsetY);
}

void TextureComponent::SetTextureFromAssetName(std::string Name)
{
	TexturePath = "Resources/Images/" + Name;
}

void TextureComponent::SetPosition(int x, int y)
{
	m_Rectangle.x = x;
	m_Rectangle.y = y;
}

void TextureComponent::SetScale(int w, int h)
{
	m_Rectangle.w = w;
	m_Rectangle.h = h;
}
