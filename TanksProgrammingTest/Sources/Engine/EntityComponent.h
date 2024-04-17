#pragma once

#include <nlohmann/json.hpp>

class Entity;

class EntityComponent
{

public:
	virtual ~EntityComponent() = default;
	EntityComponent() = delete;
	EntityComponent(Entity* Owner);
	
	virtual EntityComponent* Clone() const = 0;

	virtual void LoadFromConfig(nlohmann::json Config);
	virtual void OnLoaded();
	virtual void Initialize();
	virtual void Update(float DeltaTime);
	virtual void Draw();
	virtual void UnInitialize();
	virtual void OnUpdateSceneTransform();

	void SetOwner(Entity* Owner) { m_Owner = Owner; }
	Entity* GetOwner() { return m_Owner; }

protected:
	std::vector<EntityComponent*> m_RequiredComponents;

private:
	Entity* m_Owner;
};