#include <thread>

#include <SFML/Window/Event.hpp>

#include "Game.h"
#include "RenderSystem.h"
#include "PhysicalSystem.h"
#include "LogicalSystem.h"

CGame::CGame()
{
	m_window.create(sf::VideoMode(200, 200), "Spacewar");
	m_window.setActive(false);
}

void CGame::Start()
{
	std::thread render(StartRender);

	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}
	}

	render.join();
}

void CGame::StartRender()
{
	CGame& game = CGame::Get();

	game.m_window.setActive(true);

	while (game.m_window.isOpen())
	{
		game.m_window.clear();
		game.m_renderSystem.Render(game.m_window);
		game.m_window.display();
	}
}