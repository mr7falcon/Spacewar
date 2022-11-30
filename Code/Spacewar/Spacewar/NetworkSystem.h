#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>

#include "IController.h"
#include "EntitySystem.h"
#include "Game.h"
#include "PlayerConfiguration.h"

class CNetworkController : public IController
{
public:

	virtual EControllerType GetType() const override { return Network; }
	
	void SetClientId(int dClientId) { m_dClientId = dClientId; }
	int GetClientId() const { return m_dClientId; }

	void ProcessEvent(EControllerEvent evt);

private:

	int m_dClientId = -1;
};

class CNetworkSystem : public IControllerEventListener
{
public:

	CNetworkSystem();
	CNetworkSystem(const CNetworkSystem&) = delete;

	bool StartServer();
	bool Connect(const std::string& host);
	bool Disconnect();
	void ProcessMessages();

	bool SendChangePlayerPreset(const std::string& preset);
	bool SendCreateActor(SmartId sid, uint8_t type, const std::string& config, const CPlayerConfiguration::SPlayerConfiguration* pConfig = nullptr);
	bool SendRemoveActor(SmartId sid);
	bool SendStartLevel(const std::string& level);
	bool SendPause(bool bPause);
	bool SerializeActors();

	virtual void OnControllerEvent(EControllerEvent evt) override;

	std::shared_ptr<CNetworkController> CreateNetworkController();
	void SetVirtualController(const std::shared_ptr<IController>& pController);

	SmartId OuterToInner(SmartId outerId);
	SmartId InnerToOuter(SmartId innerId);

private:

	enum EConnectionResult : sf::Uint8
	{
		EConnectionResult_Failed,
		EConnectionResult_OutOfSockets,
		EConnectionResult_Success,
	};

	enum EClientMessages : sf::Uint8
	{
		EClientMessage_Connect,
		EClientMessage_Disconnect,
		EClientMessage_ChangePlayerPreset,
		EClientMessage_ControllerInput,
		EClientMessage_SetPause,
	};

	enum EServerMessage : sf::Uint8
	{
		EServerMessage_Connect,
		EServerMessage_Disconnect,
		EServerMessage_CreateActor,
		EServerMessage_RemoveActor,
		EServerMessage_Serialize,
		EServerMessage_StartLevel,
		EServerMessage_SetPause,
	};

	struct SSocket
	{
		sf::IpAddress addr;
		unsigned short port;

		bool operator==(const SSocket& socket) const { return addr == socket.addr && port == socket.port; }
	};

private:

	bool SendConnectionResult(int dClientId, EConnectionResult result);
	bool SendControllerInput(EControllerEvent event);
	bool SendPlayers();

	void ProcessServerMessages();
	void ProcessClientMessages();
	bool ProcessConnection(const SSocket& socket);
	void ProcessDisconnection(int dClientId);
	bool ProcessConnectionResult(sf::Packet& packet);
	bool ProcessControllerInput(int dClientId, sf::Packet& packet);
	bool ProcessChangePlayerPreset(int dClientId, sf::Packet& packet);
	bool ProcessCreateActor(sf::Packet& packet);
	bool ProcessRemoveActor(sf::Packet& packet);
	bool ProcessStartLevel(sf::Packet& packet);
	bool ProcessSerialize(sf::Packet& packet);
	bool ProcessPause(sf::Packet& packet);

	bool BindToPlayer(int dClientId);
	void OnDisconnect();

	const SSocket* FindClient(int dClientId);
	int GetClientId(const CNetworkSystem::SSocket& socket);

	void BroadcastMessage(sf::Packet& packet);
	bool SendMessageToServer(sf::Packet& packet);
	bool SendMessageToClient(sf::Packet& packet, int dClientId);

private:

	sf::IpAddress m_serverAddress;
	sf::UdpSocket m_client;
	std::shared_ptr<IController> m_pVirtualController;

	std::map<int, SSocket> m_remoteClients;
	sf::UdpSocket m_server;
	std::vector<std::shared_ptr<CNetworkController>> m_controllers;

	std::map<SmartId, SmartId> m_actorBindings;
};

inline sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2f& vec)
{
	return packet << vec.x << vec.y;
}

inline sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f& vec)
{
	return packet >> vec.x >> vec.y;
}

inline void SerializeParameters(sf::Packet& packet, bool bReading) {}

template <typename... V>
inline void SerializeParameters(sf::Packet& packet, bool bReading, SmartId& sid, V&... rest)
{
	if (bReading)
	{
		sf::Uint32 outerId;
		packet >> outerId;
		sid = (SmartId)CGame::Get().GetNetworkSystem()->OuterToInner(sid);
	}
	else
	{
		packet << (sf::Uint32)sid;
	}
}

template<typename T, typename... V>
inline void SerializeParameters(sf::Packet& packet, bool bReading, T& first, V&... rest)
{
	if (bReading)
	{
		packet >> first;
	}
	else
	{
		packet << first;
	}
	SerializeParameters(packet, bReading, rest...);
}