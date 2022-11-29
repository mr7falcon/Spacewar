#include <SFML/Window/Event.hpp>

#include "UISystem.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "LevelSystem.h"
#include "ActorSystem.h"
#include "Layout.h"
#include "Player.h"
#include "ConfigurationSystem.h"
#include "PlayerConfiguration.h"

static void StartLevel(ILayout* pCaller, const std::string& name)
{
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel(name);
}

static void ChangeLayout(ILayout* pCaller, const std::string& path)
{
	CGame::Get().GetUISystem()->LoadGlobalLayout(path);
}

static void SetActiveItem(ILayout* pCaller, const std::string& id)
{
	pCaller->ActivateItem(id);
}

static std::string GetPlayerConfigName(SmartId sid)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		return pPlayer->GetConfigName();
	}
	return "";
}

static std::string GetNextPlayerConfig(SmartId sid)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		return CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetNextConfiguration(pPlayer->GetConfigName());
	}
	return "";
}

static std::string GetPreviousPlayerConfig(SmartId sid)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		return CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetPreviousConfiguration(pPlayer->GetConfigName());
	}
	return "";
}

static void ChangePlayerConfig(SmartId sid, const std::string& config)
{
	std::shared_ptr<IController> pController;

	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	if (CPlayer* pPlayer = static_cast<CPlayer*>(pActorSystem->GetActor(sid)))
	{
		pController = pPlayer->GetController();
	}
	pActorSystem->RemoveActor(sid, true);

	CLevelSystem* pLevelSystem = CGame::Get().GetLogicalSystem()->GetLevelSystem();
	sid = pLevelSystem->SpawnPlayer(config, pController);
}

static void NextPlayerConfig(SmartId sid)
{
	const std::string& nextConfig = GetNextPlayerConfig(sid);
	ChangePlayerConfig(sid, nextConfig);
}

static void PreviousPlayerConfig(SmartId sid)
{
	const std::string& nextConfig = GetNextPlayerConfig(sid);
	ChangePlayerConfig(sid, nextConfig);
}

static void SpawnPlayer(ILayout* pCaller, const std::string& controller)
{
	const std::string& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetDefaultConfiguration();
	auto pController = CGame::Get().GetConfigurationSystem()->GetControllerConfiguration()->CreateController(controller);
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(config, pController);
}

static void RemovePlayer()
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId sid = pActorSystem->GetLastPlayerId();
	if (sid != InvalidLink)
	{
		pActorSystem->RemoveActor(sid);
	}
}

static void StartShootAll()
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([](CPlayer* pPlayer)
		{
			pPlayer->SetShooting(true);
		});
}

static void StartShoot(SmartId playerId)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(playerId)))
	{
		pPlayer->SetShooting(true);
	}
}

static void StopShootAll()
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([](CPlayer* pPlayer)
		{
			pPlayer->SetShooting(false);
		});
}

static void PauseGame()
{
	CGame::Get().Pause(true);
}

static void ResumeGame()
{
	CGame::Get().Pause(false);
}

static std::string GetPlayerScore(SmartId sid)
{
	return "0";
}

static std::string GetPlayerAmmo(SmartId sid)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		return std::to_string(pPlayer->GetAmmoCount());
	}
	return "0";
}

static std::string GetPlayerFuel(SmartId sid)
{
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		return std::to_string((int)pPlayer->GetFuel());
	}
	return "0";
}

static void ReloadLayout()
{
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadGlobalLayout();
}

#define REGISTER_FUNCTION(F) m_functions[#F] = F

CUISystem::CUISystem(const std::filesystem::path& path)
	: m_root(path) 
{
	REGISTER_FUNCTION(StartLevel);
	REGISTER_FUNCTION(ChangeLayout);
	REGISTER_FUNCTION(SetActiveItem);
	REGISTER_FUNCTION(GetPlayerConfigName);
	REGISTER_FUNCTION(GetNextPlayerConfig);
	REGISTER_FUNCTION(GetPreviousPlayerConfig);
	REGISTER_FUNCTION(NextPlayerConfig);
	REGISTER_FUNCTION(PreviousPlayerConfig);
	REGISTER_FUNCTION(SpawnPlayer);
	REGISTER_FUNCTION(RemovePlayer);
	REGISTER_FUNCTION(StartShootAll);
	REGISTER_FUNCTION(StartShoot);
	REGISTER_FUNCTION(StopShootAll);
	REGISTER_FUNCTION(PauseGame);
	REGISTER_FUNCTION(ResumeGame);
	REGISTER_FUNCTION(GetPlayerScore);
	REGISTER_FUNCTION(GetPlayerAmmo);
	REGISTER_FUNCTION(GetPlayerFuel);
	REGISTER_FUNCTION(ReloadLayout);

	m_pController = CGame::Get().GetConfigurationSystem()->GetControllerConfiguration()->CreateDefaultController();
}

CUISystem::~CUISystem() = default;

void CUISystem::Update()
{
	if (!m_newLayout.empty())
	{
		LoadGlobalLayoutInternal();
	}

	if (m_pLayout)
	{
		m_pLayout->Update();
	}
}

void CUISystem::Release()
{
	m_pLayout.reset();
	m_pController.reset();
}

typedef CLayout<> CGlobalLayout;

void CUISystem::LoadGlobalLayout(const std::string& path)
{
	m_newLayout = path;
	if (!m_pLayout)
	{
		LoadGlobalLayoutInternal();
	}
}

void CUISystem::LoadGlobalLayoutInternal()
{
	m_pLayout = std::make_unique<CLayout<>>(m_newLayout, sf::Vector2f());
	static_cast<CGlobalLayout*>(m_pLayout.get())->Load(m_root);
	m_pLayout->SetController(m_pController);
	m_newLayout.clear();
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadPlayersLayouts();
}

void CUISystem::LoadPlayerLayout(const std::string& id, SmartId playerId)
{
	if (m_pLayout)
	{
		if (ILayout* pPlayerLayout = static_cast<CGlobalLayout*>(m_pLayout.get())->ActivateSubLayout<SmartId>(id, m_root, playerId))
		{
			if (playerId != InvalidLink)
			{
				if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(playerId)))
				{
					pPlayerLayout->SetController(pPlayer->GetController());
				}
			}

		}
	}
}

void CUISystem::ResetLayout()
{
	m_pLayout.reset();
}