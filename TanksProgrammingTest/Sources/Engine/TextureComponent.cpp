#include "TextureComponent.h"
#include "Engine.h"

TextureCompoent::TextureCompoent(Entity* Owner)
	: EntityComponent(Owner)
	, m_Rectangle{0,0,0,0}
{
}

TextureCompoent::TextureCompoent()
	: TextureCompoent(nullptr)
{
}

void TextureCompoent::LoadFromConfig(nlohmann::json Config)
{
	std::string TextureName = Config.value("Texture", "");
	if (!TextureName.empty())
	{
		SetTextureFromAssetName(TextureName);
	}

	m_Rectangle.w = Config.value("Width", 10);
	m_Rectangle.h = Config.value("Height", 10);
	m_Rectangle.x = Config.value("PositionX", 0);
	m_Rectangle.y = Config.value("PositionY", 0);
}

void TextureCompoent::Initialize()
{
}

void TextureCompoent::UnInitialize()
{
}

void TextureCompoent::Draw()
{
	SDL_Surface* Surface = IMG_Load(TexturePath.c_str());
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), Surface);
	SDL_FreeSurface(Surface);

	SDL_RenderCopy(Engine::Get()->GetRenderer(), Texture, nullptr, &m_Rectangle);
}

void TextureCompoent::SetTextureFromAssetName(std::string Name)
{
	TexturePath = "Resources/Images/" + Name;
}

void TextureCompoent::SetPosition(int x, int y)
{
	m_Rectangle.x = x;
	m_Rectangle.y = y;
}

void TextureCompoent::SetScale(int w, int h)
{
	m_Rectangle.w = w;
	m_Rectangle.h = h;
}
