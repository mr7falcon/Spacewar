#include <thread>

#include <SFML/Window/Event.hpp>

#include "Game.h"
#include "RenderSystem.h"
#include "PhysicalSystem.h"
#include "LogicalSystem.h"
#include "RenderProxy.h"
#include "ResourceSystem.h"
#include "ConfigurationSystem.h"

CGame::CGame() = default;
CGame::~CGame() = default;

void CGame::Initialize()
{
	m_pLogicalSystem = std::make_unique<CLogicalSystem>();
	m_pPhysicalSystem = std::make_unique<CPhysicalSystem>();
	m_pRenderSystem = std::make_unique<CRenderSystem>();
	m_pRenderProxy = std::make_unique<CRenderProxy>();
	m_pResourceSystem = std::make_unique<CResourceSystem>("Resources");
	m_pConfigurationSystem = std::make_unique<CConfigurationSystem>("Configuration");

	m_window.create(sf::VideoMode(800, 800), "Spacewar");
	m_window.setActive(false);
	m_view.reset(sf::FloatRect(0.f, 0.f, 800.f, 800.f));
}

void CGame::Start()
{
	Initialize();

	std::thread render(StartRender);

	sf::Clock frameClock;



	SmartId sid = m_pLogicalSystem->CreateEntityFromClass("Spaceship");
	m_pLogicalSystem->GetEntity(sid)->SetPosition(sf::Vector2f(400.f, 400.f));
	m_pLogicalSystem->GetEntity(sid)->SetAcceleration(sf::Vector2f(0.f, -3.f));
	m_pLogicalSystem->GetEntity(sid)->SetAngularAcceleration(3.f);

	sid = m_pLogicalSystem->CreateEntityFromClass("Projectile");
	m_pLogicalSystem->GetEntity(sid)->SetPosition(sf::Vector2f(300.f, 300.f));
	m_pLogicalSystem->GetEntity(sid)->SetAcceleration(sf::Vector2f(5.f, 0.f));
	m_pLogicalSystem->GetEntity(sid)->SetAngularAcceleration(7.f);



	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		m_pPhysicalSystem->ProcessCollisions();
		m_pLogicalSystem->Update(frameClock.getElapsedTime());

		frameClock.restart();

		m_pLogicalSystem->CollectGarbage();
		m_pPhysicalSystem->CollectGarbage();

		{
			std::unique_lock<std::mutex> lock(m_renderLock);
			m_renderSync.wait(lock, [this]() { return m_bRenderComplete; });
			
			m_bRenderComplete = false;

			m_pRenderSystem->CollectGarbage();
			m_pRenderSystem->FixNumActiveEntities();
			m_pRenderProxy->SwitchStreams();

			m_bMainComplete = true;
		}
		m_renderSync.notify_one();
	}

	render.join();
}

void CGame::StartRender()
{
	CGame& game = CGame::Get();

	game.m_window.setActive(true);

	while (game.m_window.isOpen())
	{
		{
			std::unique_lock<std::mutex> lock(game.m_renderLock);
			game.m_renderSync.wait(lock, [&]() { return game.m_bMainComplete; });

			game.m_bMainComplete = false;

			game.m_pRenderProxy->ExecuteCommands();

			game.m_window.clear(sf::Color::Black);
			game.m_pRenderSystem->Render(game.m_window);
			game.m_window.display();

			game.m_bRenderComplete = true;
		}
		game.m_renderSync.notify_one();
	}
}