#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>

#include "RenderSystem.h"
#include "PhysicalSystem.h"
#include "LogicalSystem.h"

class CGame
{
public:

	CGame(const CGame&) = delete;

	static CGame& Get()
	{
		static CGame game;
		return game;
	}

	void Start();

public:

	CLogicalSystem& GetLogicalSystem() { return m_logicalSystem; }
	CPhysicalSystem& GetPhysicalSystem() { return m_physicalSystem; }
	CRenderSystem& GetRenderSystem() { return m_renderSystem; }

private:

	CGame();

	static void StartRender();

private:

	CLogicalSystem m_logicalSystem;
	CPhysicalSystem m_physicalSystem;
	CRenderSystem m_renderSystem;

	sf::RenderWindow m_window;
};