#include "StdAfx.h"
#include "UISystem.h"
#include "Game.h"
#include "LogicalSystem/LogicalSystem.h"
#include "LogicalSystem/LevelSystem.h"
#include "LogicalSystem/ActorSystem.h"
#include "LogicalSystem/Player.h"
#include "NetworkSystem/NetworkProxy.h"
#include "ConfigurationSystem/ConfigurationSystem.h"
#include "ConfigurationSystem/PlayerConfiguration.h"
#include "ConfigurationSystem/ControllerConfiguration.h"
#include "Layout.h"

static void StartLevel(ILayout* pCaller, const std::string& name)
{
	if (CGame::Get().IsServer())
	{
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel(name);
	}
}

static void ChangeLayout(ILayout* pCaller, const std::string& path)
{
	CGame::Get().GetUISystem()->LoadGlobalLayout(path);
}

static void SetActiveItem(ILayout* pCaller, const std::string& id)
{
	pCaller->ActivateItem(id);
}

inline static CPlayer* GetPlayer(SmartId sid)
{
	return static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid));
}

static std::string GetPlayerConfigName(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return pPlayer->GetConfigName();
	}
	return "";
}

static std::string GetNextPlayerConfig(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetNextConfiguration(pPlayer->GetConfigName());
	}
	return "";
}

static std::string GetPreviousPlayerConfig(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetPreviousConfiguration(pPlayer->GetConfigName());
	}
	return "";
}

static void ChangePlayerConfig(SmartId sid, const std::string& config)
{
	if (!CGame::Get().IsServer())
	{
		CGame::Get().GetNetworkProxy()->SendClientMessage<ClientMessage::SChangePlayerPresetMessage>(config);
	}
	else
	{
		std::shared_ptr<CController> pController;
		if (CPlayer* pPlayer = GetPlayer(sid))
		{
			pController = pPlayer->GetController();
		}
		CGame::Get().GetLogicalSystem()->GetActorSystem()->RemoveActor(sid, true);
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(config, pController);
	}
}

static void NextPlayerConfig(SmartId sid)
{
	ChangePlayerConfig(sid, GetNextPlayerConfig(sid));
}

static void PreviousPlayerConfig(SmartId sid)
{
	ChangePlayerConfig(sid, GetPreviousPlayerConfig(sid));
}

static void SpawnPlayer(ILayout* pCaller, const std::string& controller)
{
	auto pController = CGame::Get().GetConfigurationSystem()->GetControllerConfiguration()->CreateController(controller);
	if (!CGame::Get().IsServer())
	{
		CGame::Get().GetNetworkProxy()->SetVirtualController(pController);
		CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([pController](CPlayer* pPlayer)
			{
				if (pPlayer->GetController())
				{
					pPlayer->SetController(pController);
				}
				return true;
			});
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadPlayersLayouts();
	}
	else
	{
		const std::string& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetDefaultConfiguration();
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(config, pController);
	}
}

static void RemovePlayer(ILayout* pCaller)
{
	if (CGame::Get().IsServer())
	{
		CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
		SmartId sid = pActorSystem->GetLastPlayerId();
		if (sid != InvalidLink)
		{
			pActorSystem->RemoveActor(sid);
		}
	}
}

static void StartShootAll(ILayout* pCaller)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->SetPlayersShooting(true);
}

static void StopShootAll(ILayout* pCaller)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->SetPlayersShooting(false);
}

static void PauseGame(ILayout* pCaller)
{
	CGame::Get().Pause(true);
}

static void ResumeGame(ILayout* pCaller)
{
	CGame::Get().Pause(false);
}

static std::string GetPlayerScore(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return std::to_string(pPlayer->GetScore());
	}
	return "0";
}

static std::string GetPlayerAmmo(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return std::to_string(pPlayer->GetAmmoCount());
	}
	return "0";
}

static std::string GetPlayerFuel(SmartId sid)
{
	if (CPlayer* pPlayer = GetPlayer(sid))
	{
		return std::to_string((int)pPlayer->GetFuel());
	}
	return "0";
}

static void ReloadLayout(ILayout* pCaller)
{
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadGlobalLayout();
}

static void Connect(ILayout* pCaller)
{
	CGame::Get().GetNetworkSystem()->Connect(pCaller->GetText());
	CGame::Get().GetNetworkProxy()->SetVirtualController(CGame::Get().GetConfigurationSystem()->GetControllerConfiguration()->CreateDefaultController());
}

static void Disconnect(ILayout* pCaller)
{
	CGame::Get().GetNetworkSystem()->Disconnect();
}

static std::string GetConnectionStatus(ILayout* pCaller)
{
	auto status = CGame::Get().GetNetworkProxy()->GetConnectionState();
	switch (status)
	{
	case CNetworkProxy::Connected:
		return "Connected";
	case CNetworkProxy::Disconnected:
		return "Disconnected";
	case CNetworkProxy::Rejected:
		return "Host rejected connection";
	case CNetworkProxy::Failed:
		return "Failed to connect to host";
	case CNetworkProxy::InProcess:
		return "In progress";
	case CNetworkProxy::Server:
		return "Server";
	}
	return "";
}

static void EnableText(ILayout* pCaller)
{
	pCaller->SetWriteText(true);
}

static void DisableText(ILayout* pCaller)
{
	pCaller->SetWriteText(false);
}

static std::string GetText(ILayout* pCaller)
{
	return pCaller->GetText();
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
	REGISTER_FUNCTION(StopShootAll);
	REGISTER_FUNCTION(PauseGame);
	REGISTER_FUNCTION(ResumeGame);
	REGISTER_FUNCTION(GetPlayerScore);
	REGISTER_FUNCTION(GetPlayerAmmo);
	REGISTER_FUNCTION(GetPlayerFuel);
	REGISTER_FUNCTION(ReloadLayout);
	REGISTER_FUNCTION(Connect);
	REGISTER_FUNCTION(Disconnect);
	REGISTER_FUNCTION(GetConnectionStatus);
	REGISTER_FUNCTION(EnableText);
	REGISTER_FUNCTION(DisableText);
	REGISTER_FUNCTION(GetText);

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

typedef CLayout<> CGlobalLayout;

void CUISystem::LoadGlobalLayout(const std::string& path)
{
	m_newLayout = path;
	if (!m_pLayout)
	{
		LoadGlobalLayoutInternal();
	}
	else
	{
		m_pLayout->Unload();
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

void CUISystem::ReloadPlayerLayout(const std::string& id, SmartId playerId)
{
	if (!m_pLayout)
	{
		return;
	}

	if (playerId == InvalidLink && !static_cast<CGlobalLayout*>(m_pLayout.get())->IsSubLayoutLoaded(id))
	{
		return;
	}

	ILayout* pPlayerLayout = static_cast<CGlobalLayout*>(m_pLayout.get())->ActivateSubLayout<SmartId>(id, m_root, playerId);
	if (!pPlayerLayout)
	{
		return;
	}

	if (playerId != InvalidLink)
	{
		if (CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem())
		{
			if (CPlayer* pPlayer = static_cast<CPlayer*>(pActorSystem->GetActor(playerId)))
			{
				pPlayerLayout->SetController(pPlayer->GetController());
			}
		}
	}
}