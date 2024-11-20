#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <vector>
#include <string>
#include "CollisionUtils.h"

typedef unsigned char uint8;

class CollisionWorld;
class Scene;
class ResourceManager;

class Engine
{

public:
	static Engine* Get();

	void Initialize();
	void MainLoop();
	void Draw();
	void ShutDown();

	SDL_Renderer* GetRenderer() { return m_Renderer; }
	SDL_Window* GetWindow() { return m_Window; }
	ResourceManager* GetResourceManager() { return m_ResourceManager; }
	const std::vector<SDL_Event>& GetEvents() { return m_Events; }
	void SetActiveScene(Scene* Scene) { m_ActiveScene = Scene; }
	Scene* GetActiveScene() const { return m_ActiveScene; }
	CollisionWorld* GetCollisionWorld() { return m_CollisionWorld; }
	void CreateActiveSceneFromTemplate(std::string Name);

private:
	Engine();

	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;
	Scene* m_ActiveScene;
	ResourceManager* m_ResourceManager;
	CollisionWorld* m_CollisionWorld;
	
	unsigned int FramesPerSecond;
	unsigned int TimePerFrameInMs;
	float TimePerFramInSceonds;

	std::vector<SDL_Event> m_Events;
};