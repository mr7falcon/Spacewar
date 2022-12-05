#pragma once

#include "NetworkSystem.h"
#include "Game.h"
#include "EntitySystem.h"
#include "Controllers/Controller.h"
#include "LogicalSystem/Actor.h"
#include "ConfigurationSystem/PlayerConfiguration.h"

#include <string>

#include <SFML/Network/Packet.hpp>
#include <SFML/System/Time.hpp>

/**
 * @class CNetworkController
 * This class retranslates the linked remote client's controller
 * events to the locally created player.
 */
class CNetworkController : public CController
{
public:

	~CNetworkController();

	virtual EControllerType GetType() const override { return Network; }

	void SetClientId(int clientId) { m_clientId = clientId; }
	int GetClientId() const { return m_clientId; }

	void ProcessEvent(EControllerEvent evt);

private:

	int m_clientId = -1;
};

inline sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2f& vec)
{
	return packet << vec.x << vec.y;
}

enum EConnectionResult : uint8_t
{
	EConnectionResult_Failed,
	EConnectionResult_OutOfSockets,
	EConnectionResult_Success,
};

namespace ClientMessage
{
	enum EClientMessage : uint8_t
	{
		EClientMessage_ChangePlayerPreset,
		EClientMessage_ControllerInput,
		EClientMessage_SetPause,
	};

	struct SClientMessage
	{
		virtual void OnReceive(int dClientId) const = 0;
	};

	struct SChangePlayerPresetMessage : public SClientMessage
	{
		static constexpr EClientMessage GetType() { return EClientMessage_ChangePlayerPreset; }
		virtual void OnReceive(int dClientId) const override;

		SChangePlayerPresetMessage() = default;
		SChangePlayerPresetMessage(const std::string& _preset) : preset(_preset) {}
		SChangePlayerPresetMessage(std::string&& _preset) : preset(_preset) {}

		std::string preset;
	};

	struct SControllerInputMessage : public SClientMessage
	{
		static constexpr EClientMessage GetType() { return EClientMessage_ControllerInput; }
		virtual void OnReceive(int dClientId) const override;

		SControllerInputMessage() = default;
		SControllerInputMessage(EControllerEvent _event) : event(_event) {}

		uint8_t event = EControllerEvent_Invalid;
	};

	struct SSetPauseMessage : public SClientMessage
	{
		static constexpr EClientMessage GetType() { return EClientMessage_SetPause; }
		virtual void OnReceive(int dClientId) const override;

		SSetPauseMessage() = default;
		SSetPauseMessage(bool _bPause) : bPause(_bPause) {}

		bool bPause = false;
	};
}

namespace ServerMessage
{
	enum EServerMessage : uint8_t
	{
		EServerMessage_Connect,
		EServerMessage_CreateActor,
		EServerMessage_RemoveActor,
		EServerMessage_LocalPlayer,
		EServerMessage_StartLevel,
		EServerMessage_SetPause,
	};

	struct SServerMessage
	{
		virtual void OnReceive() const = 0;
	};

	struct SConnectMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_Connect; }
		virtual void OnReceive() const override;

		SConnectMessage() = default;
		SConnectMessage(uint8_t _result) : result(_result) {}

		uint8_t result = EConnectionResult_Failed;
	};

	struct SCreateActorMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_CreateActor; }
		virtual void OnReceive() const override;

		SCreateActorMessage() = default;
		SCreateActorMessage(SmartId _sid, EActorType _type, const std::string& _config)
			: sid(_sid), type(_type), config(_config) {}
		SCreateActorMessage(SmartId _sid, EActorType _type, std::string&& _config)
			: sid(_sid), type(_type), config(_config) {}

		int32_t sid = InvalidLink;
		uint8_t type = EActorType_Player;
		std::string config;
	};

	struct SRemoveActorMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_RemoveActor; }
		virtual void OnReceive() const override;

		SRemoveActorMessage() = default;
		SRemoveActorMessage(SmartId _sid) : sid(_sid) {}

		int32_t sid = InvalidLink;
	};

	struct SLocalPlayerMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_LocalPlayer; }
		virtual void OnReceive() const override;

		SLocalPlayerMessage() = default;
		SLocalPlayerMessage(SmartId _sid) : sid(_sid) {}

		int32_t sid = InvalidLink;
	};

	struct SStartLevelMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_StartLevel; }
		virtual void OnReceive() const override;

		SStartLevelMessage() = default;
		SStartLevelMessage(const std::string& _level) : level(_level) {}
		SStartLevelMessage(std::string&& _level) : level(_level) {}

		std::string level;
	};

	struct SSetPauseMessage : public SServerMessage
	{
		static constexpr EServerMessage GetType() { return EServerMessage_SetPause; }
		virtual void OnReceive() const override;

		SSetPauseMessage() = default;
		SSetPauseMessage(bool _bPause) : bPause(_bPause) {}

		bool bPause = false;
	};
}

inline sf::Packet& operator<<(sf::Packet& packet, ClientMessage::SChangePlayerPresetMessage& msg)
{
	return packet << msg.preset;
}

inline sf::Packet& operator>>(sf::Packet& packet, ClientMessage::SChangePlayerPresetMessage& msg)
{
	return packet >> msg.preset;
}

inline sf::Packet& operator<<(sf::Packet& packet, ClientMessage::SControllerInputMessage& msg)
{
	return packet << msg.event;
}

inline sf::Packet& operator>>(sf::Packet& packet, ClientMessage::SControllerInputMessage& msg)
{
	return packet >> msg.event;
}

inline sf::Packet& operator<<(sf::Packet& packet, ClientMessage::SSetPauseMessage& msg)
{
	return packet << msg.bPause;
}

inline sf::Packet& operator>>(sf::Packet& packet, ClientMessage::SSetPauseMessage& msg)
{
	return packet >> msg.bPause;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SConnectMessage& msg)
{
	return packet << msg.result;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SConnectMessage& msg)
{
	return packet >> msg.result;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SCreateActorMessage& msg)
{
	return packet << msg.sid << msg.type << msg.config;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SCreateActorMessage& msg)
{
	return packet >> msg.sid >> msg.type >> msg.config;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SRemoveActorMessage& msg)
{
	return packet << msg.sid;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SRemoveActorMessage& msg)
{
	return packet >> msg.sid;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SLocalPlayerMessage& msg)
{
	return packet << msg.sid;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SLocalPlayerMessage& msg)
{
	return packet >> msg.sid;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SStartLevelMessage& msg)
{
	return packet << msg.level;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SStartLevelMessage& msg)
{
	return packet >> msg.level;
}

inline sf::Packet& operator<<(sf::Packet& packet, ServerMessage::SSetPauseMessage& msg)
{
	return packet << msg.bPause;
}

inline sf::Packet& operator>>(sf::Packet& packet, ServerMessage::SSetPauseMessage& msg)
{
	return packet >> msg.bPause;
}

inline sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f& vec)
{
	return packet >> vec.x >> vec.y;
}

enum ESerializationMode : uint8_t
{
	ESerializationMode_Read,
	ESerializationMode_Write,
	ESerializationMode_Count
};

inline void SerializeParameters(sf::Packet& packet, uint8_t mode, uint16_t& size) {}

template<typename T, typename... V>
inline void SerializeParameters(sf::Packet& packet, uint8_t mode, uint16_t& size, T& first, V&... rest)
{
	if (mode == ESerializationMode_Read)
	{
		packet >> first;
	}
	else if (mode == ESerializationMode_Write)
	{
		packet << first;
	}
	size += sizeof(first);
	SerializeParameters(packet, mode, size, rest...);
}

/**
 * @class CNetworkProxy
 * This class is an intermediate layer between the game logic and the network.
 * It packs the game messages and sends them to the network as well as receives
 * the remote messages from the network and processes them. CNetworkProxy also
 * owns and udpates all the network controllers in the game.
 */
class CNetworkProxy : public IControllerEventListener
{
public:

	enum EConnectionState : uint8_t
	{
		Disconnected,
		InProcess,
		Connected,
		Server,
		Failed,
		Rejected,
	};

	CNetworkProxy() = default;
	CNetworkProxy(const CNetworkProxy&) = delete;

	/**
	 * @function SendClientMessage
	 * Create the client message, pack it and forward to the network.
	 * 
	 * @template param T - client message type.
	 * @template params V - arguments for the message creation.
	 */
	template <typename T, typename... V>
	inline void SendClientMessage(V&&... args)
	{
		sf::Packet packet;
		T msg(std::forward<V>(args)...);
		packet << T::GetType() << msg;
		CGame::Get().GetNetworkSystem()->SendClientMessage(packet);
	}

	/**
	 * @function SendServerMessage
	 * Create the server message, pack it and send it to the specified client.
	 *
	 * @template param T - server message type.
	 * @template params V - arguments for the message creation.
	 */
	template <typename T, typename...V>
	inline void SendServerMessage(int clientId, V&&... args)
	{
		sf::Packet packet;
		T msg(std::forward<V>(args)...);
		packet << T::GetType() << msg;
		CGame::Get().GetNetworkSystem()->SendServerMessage(clientId, packet);
	}

	/**
	 * @function BroadcastServerMessage
	 * Create the server message, pack it and send it to all the clients.
	 *
	 * @template param T - server message type.
	 * @template params V - arguments for the message creation.
	 */
	template <typename T, typename... V>
	inline void BroadcastServerMessage(V&&... args)
	{
		sf::Packet packet;
		T msg(std::forward<V>(args)...);
		packet << T::GetType() << msg;
		CGame::Get().GetNetworkSystem()->BroadcastServerMessage(packet);
	}

	/**
	 * @function Serialize
	 * Initiate actors' system serialization. Pack the actors' states and
	 * forward them to the network.
	 */
	void Serialize();

	/**
	 * @function OnClientMessageReceived
	 * Called by the network when the new client message received.
	 * The function determines the message type and process it.
	 * 
	 * @param clientId - client identifier from which the message was received.
	 * @param packet - data packet, containing the message.
	 */
	void OnClientMessageReceived(int clientId, sf::Packet& packet);

	/**
	 * @function OnServerMessageReceived
	 * Called by the network when the new server message received.
	 * The function determines the message type and process it.
	 *
	 * @param packet - data packet, containing the message.
	 */
	void OnServerMessageReceived(sf::Packet& packet);
	
	/**
	 * @function OnSerializationMessageReceived
	 * Called by the network when the new serialization message received.
	 * The function initiates the actors' system serialization from the 
	 * received data.
	 *
	 * @param packet - data packet, containing the message.
	 */
	void OnSerializationReceived(sf::Packet& packet);

	// CNetworkSystem connection events' handlers
	void OnConnectionFailed();
	void OnConnect();
	void OnDisconnect();
	void OnClientDisconnect(int clientId);
	void OnClientConnect(int clientId);

	// Create and register a new network controller
	std::shared_ptr<CNetworkController> CreateNetworkController();

	// Called when the network controller removed. The function unregisteres
	// it and kicks the player who is connected to the deleted controller.
	void OnNetworkControllerRemoved();

	// Set the controller which input will be sent to the server
	void SetVirtualController(const std::shared_ptr<CController>& pController);
	const std::shared_ptr<CController>& GetVirtualController() const { return m_pVirtualController; }
	void ProcessControllerEvent(int clientId, EControllerEvent event);
	virtual void OnControllerEvent(EControllerEvent event) override;

	// Transform server entity id into the local one
	SmartId GetLocalEntityId(SmartId serverId) const;

	void SetConnectionState(EConnectionState state);
	EConnectionState GetConnectionState() const { return m_state; }

	void CreateActor(SmartId serverId, EActorType type, const std::string& config);
	void RemoveActor(SmartId sid);
	void StartLevel(const std::string& level);
	void SendCreateActor(SmartId sid, EActorType type, const std::string& config, const CPlayerConfiguration::SPlayerConfiguration* pConfig = nullptr);

private:

	bool BindToPlayer(int clientId);
	void SendPlayers(int clientId);

private:

	std::shared_ptr<CController> m_pVirtualController;
	std::vector<std::weak_ptr<CNetworkController>> m_controllers;

	std::map<SmartId, SmartId> m_actorBindings;

	EConnectionState m_state = Disconnected;
};