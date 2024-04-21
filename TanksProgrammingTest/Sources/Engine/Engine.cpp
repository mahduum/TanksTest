#include "Engine.h"
#include <SDL.h>
#include <stdio.h>
#include "Scene.h"
#include "ResourceManager.h"
#include "CollisionWorld.h"

Engine::Engine()
	: m_Window(nullptr)
	, m_Renderer(nullptr)
	, m_ActiveScene(nullptr)
	, m_ResourceManager(nullptr)
	, m_CollisionWorld(nullptr)
	, FramesPerSecond(60)
	, TimePerFrameInMs(1000 / FramesPerSecond)
	, TimePerFramInSceonds(TimePerFrameInMs / 1000.f)
{
}

Engine* Engine::Get()
{
	static Engine s_Instance;
	return &s_Instance;
}

void Engine::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
		printf("Error initializing SDL: %s\n", SDL_GetError());
	}

	m_Window = SDL_CreateWindow("ProgrammingTest", 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		860, 640, 0);

	Uint32 renderFlags = SDL_RENDERER_ACCELERATED;

	m_Renderer = SDL_CreateRenderer(m_Window, -1, renderFlags);

	m_CollisionWorld = new CollisionWorld();

	m_ResourceManager = new ResourceManager("Resources");
	m_ResourceManager->LoadResources();

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Initialize();
	}
}

void Engine::MainLoop()
{
	bool Close = false;
	unsigned int CurrentTime = SDL_GetTicks();
	unsigned int LastTime = CurrentTime;

	while (!Close)
	{
		CurrentTime = SDL_GetTicks();
		while (!Close && CurrentTime > LastTime + TimePerFrameInMs)
		{
			m_Events.clear();
			SDL_Event Event;
			while (SDL_PollEvent(&Event))
			{
				if (Event.type == SDL_QUIT || Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					Close = true;
					break;
				}
				else
				{
					m_Events.push_back(Event);
				}
			}

			if (m_ActiveScene != nullptr)
			{
				m_ActiveScene->Update(TimePerFramInSceonds);
				m_CollisionWorld->TestSweepAndPrune(m_CollisionWorld->CollisionHandler);
				m_ActiveScene->RemoveDeadEntities();
			}

			LastTime += TimePerFrameInMs;
		}

		Draw();
	}
}

void Engine::Draw()
{
	SDL_RenderClear(m_Renderer);

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Draw();
	}

	SDL_RenderPresent(m_Renderer);
}

void Engine::ShutDown()
{
	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->UnInitialize();
	}

	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void Engine::CreateActiveSceneFromTemplate(std::string Name)
{
	if (m_ResourceManager != nullptr)
	{
		nlohmann::json SceneConfig = m_ResourceManager->GetJsonConfig(Name, ResourceType::Scene);
		Scene* SceneFromTemplate = new Scene();
		SceneFromTemplate->LoadFromConfig(SceneConfig);
		SceneFromTemplate->Initialize();
		m_ActiveScene = SceneFromTemplate;
	}
}
