#pragma once

#include "EntityComponent.h"
#include "Engine.h"

class TextureComponent : public EntityComponent
{

public:
	TextureComponent(Entity* Owner);
	TextureComponent();

	virtual std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<TextureComponent>(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void Draw() override;
	virtual void OnUpdateSceneTransform() override;


	void SetTextureFromAssetName(std::string Name);
	void SetPosition(int x, int y);
	void SetScale(int w, int h);
	SDL_Rect& GetRectangle() { return m_Rectangle; }


private:
	std::string TexturePath;
	SDL_Rect m_Rectangle;

	int m_OffsetX;
	int m_OffsetY;

	SDL_Surface* Surface;
	SDL_Texture* Texture;
};