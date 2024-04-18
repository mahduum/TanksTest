#pragma once

#include <typeindex>
#include <nlohmann/json.hpp>

class Entity;

class EntityComponent
{

public:
	virtual ~EntityComponent() = default;
	EntityComponent() = delete;
	EntityComponent(Entity* Owner);
	
	virtual std::shared_ptr<EntityComponent> Clone() const = 0;

	virtual void LoadFromConfig(nlohmann::json Config);
	virtual void OnLoaded();
	virtual void Initialize();
	virtual void Update(float DeltaTime);
	virtual void Draw();
	virtual void UnInitialize();
	virtual void OnUpdateSceneTransform();

	void SetOwner(Entity* Owner) { m_Owner = Owner; }
	Entity* GetOwner() { return m_Owner; }//todo make it shared
	virtual std::type_index GetTypeIndex() const { return typeid(*this); }

protected:
	std::vector<EntityComponent*> m_RequiredComponents;

private:
	Entity* m_Owner;
};