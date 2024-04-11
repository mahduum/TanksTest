#pragma once

#include "EntityComponent.h"
#include "Engine.h"

class TextureCompoent : public EntityComponent
{

public:
	TextureCompoent(Entity* Owner);
	TextureCompoent();

	virtual EntityComponent* Clone() const override { return new TextureCompoent(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void Draw() override;

	void SetTextureFromAssetName(std::string Name);
	void SetPosition(int x, int y);
	void SetScale(int w, int h);
	SDL_Rect& GetRectangle() { return m_Rectangle; }

private:
	std::string TexturePath;
	SDL_Rect m_Rectangle;
};