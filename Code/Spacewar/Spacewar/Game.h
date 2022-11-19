#pragma once

#include <memory>
#include <mutex>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

class CLogicalSystem;
class CPhysicalSystem;
class CRenderSystem;
class CRenderProxy;

class CGame
{
public:

	CGame(const CGame&) = delete;
	~CGame();

	static CGame& Get()
	{
		static CGame game;
		return game;
	}

	void Start();

public:

	CLogicalSystem* GetLogicalSystem() { return m_pLogicalSystem.get(); }
	CPhysicalSystem* GetPhysicalSystem() { return m_pPhysicalSystem.get(); }
	CRenderSystem* GetRenderSystem() { return m_pRenderSystem.get(); }
	CRenderProxy* GetRenderProxy() { return m_pRenderProxy.get(); }

private:

	CGame();

	static void StartRender();

private:

	std::unique_ptr<CLogicalSystem> m_pLogicalSystem;
	std::unique_ptr<CPhysicalSystem> m_pPhysicalSystem;
	std::unique_ptr<CRenderSystem> m_pRenderSystem;
	std::unique_ptr<CRenderProxy> m_pRenderProxy;

	std::mutex m_renderLock;

	sf::RenderWindow m_window;
	sf::View m_view;
};