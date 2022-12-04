#include <iostream>

#include "NetworkProxy.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "LevelSystem.h"
#include "ActorSystem.h"

CNetworkController::~CNetworkController()
{
	CGame::Get().GetNetworkProxy()->OnNetworkControllerRemoved();
}

void CNetworkController::ProcessEvent(EControllerEvent evt)			// make this function member of IController
{
	SendEvents({ evt });
}

std::shared_ptr<CNetworkController> CNetworkProxy::CreateNetworkController()
{
	auto pController = std::make_shared<CNetworkController>();
	m_controllers.push_back(pController);
	if (m_state == Disconnected && CGame::Get().GetNetworkSystem()->StartServer())
	{
		m_state = Server;
	}
	return pController;
}

void CNetworkProxy::OnNetworkControllerRemoved()
{
	for (auto iter = m_controllers.cbegin(); iter != m_controllers.cend();)
	{
		if (iter->expired())
		{
			iter = m_controllers.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (m_controllers.empty())
	{
		CGame::Get().GetNetworkSystem()->StopServer();
		m_state = Disconnected;
	}
}

void CNetworkProxy::SetVirtualController(const std::shared_ptr<IController>& pController)
{
	if (m_pVirtualController)
	{
		m_pVirtualController->UnregisterEventListener(this);
	}
	m_pVirtualController = pController;
	if (m_pVirtualController)
	{
		m_pVirtualController->RegisterEventListener(this);
	}
}

SmartId CNetworkProxy::GetLocalEntityId(SmartId serverId) const
{
	auto fnd = m_actorBindings.find(serverId);
	return fnd != m_actorBindings.end() ? fnd->second : InvalidLink;
}

void CNetworkProxy::OnConnectionFailed()
{
	m_state = Failed;
}

void CNetworkProxy::OnConnect()
{
	m_state = InProcess;
}

void CNetworkProxy::OnDisconnect()
{
	m_actorBindings.clear();
	m_state = Disconnected;
	CGame::Get().SetServer(true);
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel("Menu");
}

void CNetworkProxy::OnClientConnect(int clientId)
{
	EConnectionResult res = BindToPlayer(clientId) ? EConnectionResult_Success : EConnectionResult_OutOfSockets;
	SendServerMessage<ServerMessage::SConnectMessage>(clientId, res);
	if (res == EConnectionResult_Success)
	{
		SendPlayers(clientId);
	}
}

void CNetworkProxy::OnClientDisconnect(int clientId)
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	pActorSystem->ForEachPlayer([&](CPlayer* pPlayer)
		{
			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == clientId)
				{
					pActorSystem->RemoveActor(pPlayer->GetEntityId());
					static_cast<CNetworkController*>(pController.get())->SetClientId(-1);
					return false;
				}
			}
			return true;
		});
}

void CNetworkProxy::ProcessControllerEvent(int clientId, EControllerEvent event)
{
	for (int i = 0; i < m_controllers.size(); ++i)
	{
		if (auto pController = m_controllers[i].lock())
		{
			if (pController->GetClientId() == clientId)
			{
				pController->ProcessEvent((EControllerEvent)event);
			}
		}
	}
}

void CNetworkProxy::CreateActor(SmartId serverId, EActorType type, const std::string& config)
{
	auto fnd = m_actorBindings.find(serverId);
	if (fnd != m_actorBindings.end())
	{
		return;
	}

	SmartId localId = InvalidLink;
	if (type != EActorType_Player)
	{
		localId = CGame::Get().GetLogicalSystem()->GetActorSystem()->CreateActor((EActorType)type, config);
	}
	else
	{
		localId = CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(config, nullptr);
	}

	if (localId != InvalidLink)
	{
		m_actorBindings[serverId] = localId;
	}
}

void CNetworkProxy::RemoveActor(SmartId serverId)
{
	auto fnd = m_actorBindings.find(serverId);
	if (fnd != m_actorBindings.end())
	{
		CGame::Get().GetLogicalSystem()->GetActorSystem()->RemoveActor(fnd->second);
		m_actorBindings.erase(fnd);
	}
}

void CNetworkProxy::StartLevel(const std::string& level)
{
	m_actorBindings.clear();
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel(level);
}

void CNetworkProxy::SendCreateActor(SmartId sid, EActorType type, const std::string& config, const CPlayerConfiguration::SPlayerConfiguration* pConfig)
{
	if (!CGame::Get().IsServer())
	{
		return;
	}

	if (type != EActorType_Player && !CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return;
	}

	BroadcastServerMessage<ServerMessage::SCreateActorMessage>(sid, type, config);
}

void ServerMessage::SConnectMessage::OnReceive() const
{
	if (result == EConnectionResult_Success)
	{
		CGame::Get().GetNetworkProxy()->SetConnectionState(CNetworkProxy::Connected);
		CGame::Get().SetServer(false);
		CGame::Get().GetLogicalSystem()->GetActorSystem()->Release();
	}
	else
	{
		CGame::Get().GetNetworkSystem()->Disconnect();
		CGame::Get().GetNetworkProxy()->SetConnectionState(CNetworkProxy::Rejected);
	}
}

void ClientMessage::SControllerInputMessage::OnReceive(int clientId) const
{
	CGame::Get().GetNetworkProxy()->ProcessControllerEvent(clientId, (EControllerEvent)event);
}

void ClientMessage::SChangePlayerPresetMessage::OnReceive(int clientId) const
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();

	CPlayer* pBindedPlayer = nullptr;

	pActorSystem->ForEachPlayer([&](CPlayer* pPlayer)
		{
			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == clientId)
				{
					pBindedPlayer = pPlayer;
					return false;
				}
			}
			return true;
		});

	if (pBindedPlayer)
	{
		auto pController = pBindedPlayer->GetController();
		pActorSystem->RemoveActor(pBindedPlayer->GetEntityId(), true);

		SmartId sid = CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(preset, pController);
		CGame::Get().GetNetworkProxy()->SendServerMessage<ServerMessage::SLocalPlayerMessage>(clientId, sid);
	}
}

void ServerMessage::SCreateActorMessage::OnReceive() const
{
	std::cout << "Create actor " << sid << std::endl;
	CGame::Get().GetNetworkProxy()->CreateActor(sid, (EActorType)type, config);
}

void ServerMessage::SRemoveActorMessage::OnReceive() const
{
	std::cout << "Remove actor " << sid << std::endl;
	CGame::Get().GetNetworkProxy()->RemoveActor(sid);
}

void ServerMessage::SLocalPlayerMessage::OnReceive() const
{
	SmartId localId = CGame::Get().GetNetworkProxy()->GetLocalEntityId(sid);
	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(localId)))
	{
		pPlayer->SetController(CGame::Get().GetNetworkProxy()->GetVirtualController());
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadPlayersLayouts();
	}
}

void ServerMessage::SStartLevelMessage::OnReceive() const
{
	CGame::Get().GetNetworkProxy()->StartLevel(level);
}

void ClientMessage::SSetPauseMessage::OnReceive(int clientId) const
{
	CGame::Get().Pause(bPause);
}

void ServerMessage::SSetPauseMessage::OnReceive() const
{
	CGame::Get().Pause(bPause);
}

void CNetworkProxy::OnSerializationReceived(sf::Packet& packet)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->Serialize(packet, true);
}

bool CNetworkProxy::BindToPlayer(int clientId)
{
	for (int i = 0; i < m_controllers.size(); ++i)
	{
		if (auto pController = m_controllers[i].lock())
		{
			if (pController->GetClientId() == -1)
			{
				pController->SetClientId(clientId);
				return true;
			}
		}
	}
	return false;
}

void CNetworkProxy::OnControllerEvent(EControllerEvent evt)
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		SendClientMessage<ClientMessage::SControllerInputMessage>(evt);
	}
}

void CNetworkProxy::Serialize()
{
	if (m_state == Server)
	{
		sf::Packet packet;
		CGame::Get().GetLogicalSystem()->GetActorSystem()->Serialize(packet, false);
		if (packet.getDataSize() > 0)
		{
			CGame::Get().GetNetworkSystem()->SendSerializationMessage(packet);
		}
	}
}

void CNetworkProxy::SendPlayers(int clientId)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([&](CPlayer* pPlayer)
		{
			SendCreateActor(pPlayer->GetEntityId(), EActorType_Player, pPlayer->GetConfigName(), nullptr);

			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == clientId)
				{
					SendServerMessage<ServerMessage::SLocalPlayerMessage>(clientId, pPlayer->GetEntityId());
				}
			}

			return true;
		});
}

void CNetworkProxy::SetConnectionState(EConnectionState state)
{
	m_state = state;
}

void CNetworkProxy::OnClientMessageReceived(int clientId, sf::Packet& packet)
{
	uint8_t msg;
	packet >> msg;
	switch (msg)
	{
	case ClientMessage::EClientMessage_ChangePlayerPreset:
	{
		ClientMessage::SChangePlayerPresetMessage body;
		packet >> body;
		body.OnReceive(clientId);
	}
	break;
	case ClientMessage::EClientMessage_ControllerInput:
	{
		ClientMessage::SControllerInputMessage body;
		packet >> body;
		body.OnReceive(clientId);
	}
	break;
	case ClientMessage::EClientMessage_SetPause:
	{
		ClientMessage::SSetPauseMessage body;
		packet >> body;
		body.OnReceive(clientId);
	}
	break;
	}
}

void CNetworkProxy::OnServerMessageReceived(sf::Packet& packet)
{
	uint8_t msg;
	packet >> msg;
	switch (msg)
	{
	case ServerMessage::EServerMessage_Connect:
	{
		ServerMessage::SConnectMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	case ServerMessage::EServerMessage_CreateActor:
	{
		ServerMessage::SCreateActorMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	case ServerMessage::EServerMessage_RemoveActor:
	{
		ServerMessage::SRemoveActorMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	case ServerMessage::EServerMessage_LocalPlayer:
	{
		ServerMessage::SLocalPlayerMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	case ServerMessage::EServerMessage_SetPause:
	{
		ServerMessage::SSetPauseMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	case ServerMessage::EServerMessage_StartLevel:
	{
		ServerMessage::SStartLevelMessage body;
		packet >> body;
		body.OnReceive();
	}
	break;
	}
}