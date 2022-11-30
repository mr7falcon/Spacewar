#include "NetworkSystem.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "Game.h"
#include "Player.h"

void CNetworkController::ProcessEvent(EControllerEvent evt)			// make this function member of IController
{
	for (auto iter = m_listeners.begin(); iter != m_listeners.end();)
	{
		if (*iter == nullptr)
		{
			iter = m_listeners.erase(iter);
		}
		else
		{
			(*iter)->OnControllerEvent(evt);
			++iter;
		}
	}
}

CNetworkSystem::CNetworkSystem()
{
	m_client.setBlocking(false);
	m_server.setBlocking(false);
}

std::shared_ptr<CNetworkController> CNetworkSystem::CreateNetworkController()
{
	auto pController = std::make_shared<CNetworkController>();
	m_controllers.push_back(pController);
	return pController;
}

void CNetworkSystem::SetVirtualController(const std::shared_ptr<IController>& pController)
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

SmartId CNetworkSystem::OuterToInner(SmartId outerId)
{
	auto fnd = m_actorBindings.find(outerId);
	return fnd != m_actorBindings.end() ? fnd->second : InvalidLink;
}

SmartId CNetworkSystem::InnerToOuter(SmartId innerId)
{
	auto fnd = std::find_if(m_actorBindings.begin(), m_actorBindings.end(),
		[innerId](const std::pair<SmartId, SmartId>& bind) { return bind.second == innerId; });
	return fnd != m_actorBindings.end() ? fnd->first : InvalidLink;
}

const CNetworkSystem::SSocket* CNetworkSystem::FindClient(int dClientId)
{
	auto fnd = m_remoteClients.find(dClientId);
	return fnd != m_remoteClients.end() ? &fnd->second : nullptr;
}

int CNetworkSystem::GetClientId(const CNetworkSystem::SSocket& socket)
{
	auto fnd = std::find_if(m_remoteClients.begin(), m_remoteClients.end(),
		[socket](const std::pair<int, SSocket>& s) { return s.second == socket; });
	return fnd != m_remoteClients.end() ? fnd->first : -1;
}

static constexpr unsigned short ServerPort = 7777;

bool CNetworkSystem::Connect(const std::string& host)
{
	m_serverAddress = host;
	m_client.bind(sf::Socket::AnyPort);

	sf::Packet packet;
	packet << EClientMessage_Connect;
	return SendMessageToServer(packet);
}

bool CNetworkSystem::Disconnect()
{
	sf::Packet packet;
	packet << EClientMessage_Disconnect;
	OnDisconnect();
	return SendMessageToServer(packet);
}

void CNetworkSystem::BroadcastMessage(sf::Packet& packet)
{
	for (auto iter = m_remoteClients.cbegin(); iter != m_remoteClients.cend();)
	{
		if (m_server.send(packet, iter->second.addr, iter->second.port) == sf::Socket::Done)
		{
			++iter;
		}
		else
		{
			iter = m_remoteClients.erase(iter);
		}
	}
}

bool CNetworkSystem::SendMessageToServer(sf::Packet& packet)
{
	return m_client.send(packet, m_serverAddress, ServerPort) == sf::Socket::Done;
}

bool CNetworkSystem::SendMessageToClient(sf::Packet& packet, int dClientId)
{
	if (const SSocket* pClient = FindClient(dClientId))
	{
		return m_server.send(packet, pClient->addr, pClient->port) == sf::Socket::Done;
	}
	return false;
}

bool CNetworkSystem::StartServer()
{
	return m_server.bind(ServerPort) == sf::Socket::Done;
}

void CNetworkSystem::ProcessServerMessages()
{
	sf::Packet packet;
	SSocket socket;
	if (m_server.receive(packet, socket.addr, socket.port) == sf::Socket::Done)
	{
		sf::Uint8 message;
		packet >> message;

		bool res = false;
		int dClientId = GetClientId(socket);

		switch (message)
		{
		case EClientMessage_Connect:
			res = ProcessConnection(socket);
			break;
		case EClientMessage_Disconnect:
			res = false;
			break;
		case EClientMessage_ChangePlayerPreset:
			res = ProcessChangePlayerPreset(dClientId, packet);
			break;
		case EClientMessage_ControllerInput:
			res = ProcessControllerInput(dClientId, packet);
			break;
		case EClientMessage_SetPause:
			res = ProcessPause(packet);
			break;
		}

		if (!res)
		{
			ProcessDisconnection(dClientId);
		}
	}
}

void CNetworkSystem::ProcessClientMessages()
{
	sf::Packet packet;
	SSocket socket;
	if (m_client.receive(packet, socket.addr, socket.port) == sf::Socket::Done)
	{
		sf::Uint8 message;
		packet >> message;

		bool res = false;

		switch (message)
		{
		case EServerMessage_Connect:
			res = ProcessConnectionResult(packet);
			break;
		case EServerMessage_Disconnect:
			res = false;
			break;
		case EServerMessage_CreateActor:
			res = ProcessCreateActor(packet);
			break;
		case EServerMessage_RemoveActor:
			res = ProcessRemoveActor(packet);
			break;
		case EServerMessage_LocalPlayer:
			res = ProcessLocalPlayer(packet);
			break;
		case EServerMessage_Serialize:
			res = ProcessSerialize(packet);
			break;
		case EServerMessage_StartLevel:
			res = ProcessStartLevel(packet);
			break;
		case EServerMessage_SetPause:
			res = ProcessPause(packet);
			break;
		}

		if (!res)
		{
			OnDisconnect();
		}
	}
}

void CNetworkSystem::ProcessMessages()
{
	ProcessServerMessages();
	ProcessClientMessages();
}

bool CNetworkSystem::ProcessConnection(const SSocket& socket)
{
	int dClientId = GetClientId(socket);
	if (dClientId == -1)
	{
		int id = (int)m_remoteClients.size();
		m_remoteClients[id] = socket;
		dClientId = id;
	}

	EConnectionResult res = BindToPlayer(dClientId) ? EConnectionResult_Success : EConnectionResult_OutOfSockets;
	if (SendConnectionResult(dClientId, res))
	{
		return SendPlayers(dClientId);
	}

	return false;
}

bool CNetworkSystem::ProcessConnectionResult(sf::Packet& packet)
{
	sf::Uint8 result;
	packet >> result;
	
	if (result == EConnectionResult_Success)
	{
		CGame::Get().SetServer(false);
		CGame::Get().GetLogicalSystem()->GetActorSystem()->Release();
	}

	return true;
}

void CNetworkSystem::ProcessDisconnection(int dClientId)
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	pActorSystem->ForEachPlayer([&](CPlayer* pPlayer)
		{
			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == dClientId)
				{
					pActorSystem->RemoveActor(pPlayer->GetEntityId());
					static_cast<CNetworkController*>(pController.get())->SetClientId(-1);
					return false;
				}
			}
			return true;
		});

	m_remoteClients.erase(dClientId);
}

bool CNetworkSystem::ProcessControllerInput(int dClientId, sf::Packet& packet)
{
	auto fnd = std::find_if(m_controllers.begin(), m_controllers.end(),
		[dClientId](const std::shared_ptr<CNetworkController>& pController) { return pController->GetClientId() == dClientId; });
	if (fnd != m_controllers.end())
	{
		sf::Uint8 event;
		packet >> event;

		(*fnd)->ProcessEvent((EControllerEvent)event);
		
		return true;
	}
	return false;
}

bool CNetworkSystem::ProcessChangePlayerPreset(int dClientId, sf::Packet& packet)
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();

	CPlayer* pBindedPlayer = nullptr;
	
	pActorSystem->ForEachPlayer([&](CPlayer* pPlayer)
		{
			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == dClientId)
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

		std::string preset;
		packet >> preset;

		SmartId sid = CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(preset, pController);
		return SendLocalPlayer(dClientId, sid);
	}

	return false;
}

bool CNetworkSystem::ProcessCreateActor(sf::Packet& packet)
{
	sf::Uint32 outerId;
	std::string config;
	uint8_t type;
	packet >> outerId >> type >> config;

	auto fnd = m_actorBindings.find(outerId);
	if (fnd != m_actorBindings.end())
	{
		return true;
	}

	SmartId innerId = InvalidLink;
	if (type != EActorType_Player)
	{
		innerId = CGame::Get().GetLogicalSystem()->GetActorSystem()->CreateActor((EActorType)type, config);
	}
	else
	{
		innerId = CGame::Get().GetLogicalSystem()->GetLevelSystem()->SpawnPlayer(config, m_pVirtualController);
	}

	if (innerId != InvalidLink)
	{
		m_actorBindings[outerId] = innerId;
		return true;
	}

	return false;
}

bool CNetworkSystem::ProcessRemoveActor(sf::Packet& packet)
{
	sf::Uint32 outerId;
	packet >> outerId;

	auto fnd = m_actorBindings.find(outerId);
	if (fnd != m_actorBindings.end())
	{
		CGame::Get().GetLogicalSystem()->GetActorSystem()->RemoveActor(fnd->second);
		m_actorBindings.erase(fnd);
	}

	return true;
}

bool CNetworkSystem::ProcessLocalPlayer(sf::Packet& packet)
{
	SmartId sid;
	packet >> sid;

	if (CPlayer* pPlayer = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid)))
	{
		pPlayer->SetController(m_pVirtualController);
		CGame::Get().GetLogicalSystem()->GetLevelSystem()->ReloadPlayersLayouts();
		return true;
	}
	
	return false;
}

bool CNetworkSystem::ProcessStartLevel(sf::Packet& packet)
{
	std::string level;
	packet >> level;

	m_actorBindings.clear();

	CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel(level);
	return true;
}

bool CNetworkSystem::ProcessSerialize(sf::Packet& packet)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->Serialize(packet, true);
	return true;
}

bool CNetworkSystem::ProcessPause(sf::Packet& packet)
{
	bool pause;
	packet >> pause;
	CGame::Get().Pause(pause);
	return true;
}

bool CNetworkSystem::BindToPlayer(int dClientId)
{
	auto fnd = std::find_if(m_controllers.begin(), m_controllers.end(),
		[](const std::shared_ptr<CNetworkController>& pController) { return pController->GetClientId() == -1; });
	if (fnd != m_controllers.end())
	{
		(*fnd)->SetClientId(dClientId);
		return true;
	}
	return false;
}

void CNetworkSystem::OnDisconnect()
{
	m_actorBindings.clear();
	CGame::Get().SetServer(true);
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->CreateLevel("Menu");
}

void CNetworkSystem::OnControllerEvent(EControllerEvent evt)
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		SendControllerInput(evt);
	}
}

bool CNetworkSystem::SendConnectionResult(int dClientId, EConnectionResult result)
{
	sf::Packet packet;
	packet << EServerMessage_Connect << result;
	return SendMessageToClient(packet, dClientId);
}

bool CNetworkSystem::SendControllerInput(EControllerEvent event)
{
	sf::Packet packet;
	packet << EClientMessage_ControllerInput << event;
	return SendMessageToServer(packet);
}

bool CNetworkSystem::SendChangePlayerPreset(const std::string& preset)
{
	sf::Packet packet;
	packet << EClientMessage_ChangePlayerPreset << preset;
	return SendMessageToServer(packet);
}

bool CNetworkSystem::SendCreateActor(SmartId sid, uint8_t type, const std::string& config, const CPlayerConfiguration::SPlayerConfiguration* pConfig)
{
	if (!CGame::Get().IsServer())
	{
		return false;
	}

	if (type != EActorType_Player && !CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return true;
	}

	sf::Packet packet;
	packet << EServerMessage_CreateActor << sid << type << config;
	
	BroadcastMessage(packet);
	return true;
}

bool CNetworkSystem::SendRemoveActor(SmartId sid)
{
	if (!CGame::Get().IsServer())
	{
		return false;
	}

	sf::Packet packet;
	packet << EServerMessage_RemoveActor << sid;

	BroadcastMessage(packet);
	return true;
}

bool CNetworkSystem::SerializeActors()
{
	if (CGame::Get().IsServer())
	{
		sf::Packet packet;
		packet << EServerMessage_Serialize;

		CGame::Get().GetLogicalSystem()->GetActorSystem()->Serialize(packet, false);
		
		if (packet.getDataSize() > sizeof(EServerMessage))
		{
			BroadcastMessage(packet);
		}
	}
	return true;
}

bool CNetworkSystem::SendPlayers(int dClientId)
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([&](CPlayer* pPlayer)
		{
			SendCreateActor(pPlayer->GetEntityId(), EActorType_Player, pPlayer->GetConfigName(), nullptr);

			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() == IController::Network && static_cast<CNetworkController*>(pController.get())->GetClientId() == dClientId)
				{
					SendLocalPlayer(dClientId, pPlayer->GetEntityId());
				}
			}

			return true;
		});

	return true;
}

bool CNetworkSystem::SendLocalPlayer(int dClientId, SmartId sid)
{
	sf::Packet packet;
	packet << EServerMessage_LocalPlayer << sid;
	return SendMessageToClient(packet, dClientId);
}

bool CNetworkSystem::SendStartLevel(const std::string& level)
{
	if (!CGame::Get().IsServer())
	{
		return false;
	}

	sf::Packet packet;
	packet << EServerMessage_StartLevel << level;

	BroadcastMessage(packet);
	return true;
}

bool CNetworkSystem::SendPause(bool bPause)
{
	sf::Packet packet;
	packet << (sf::Uint8)(CGame::Get().IsServer() ? EServerMessage_SetPause : EClientMessage_SetPause) << bPause;
	if (CGame::Get().IsServer())
	{
		BroadcastMessage(packet);
	}
	else
	{
		SendMessageToServer(packet);
	}
	return true;
}