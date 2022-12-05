#pragma once

#include <map>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>

/**
 * @class CNetworkSystem
 * This system provides the low-level communication between multiple game clients.
 * Each one of them has one of the statuses: server or client. Clients send commands
 * to the server, while the server replicates the current actors' sytem state on the clients.
 * The commands exchange takes place over the TCP protocol to guarantee the order and
 * the reliability of the important messages, while the serialization is sent over the UDP.
 * I understand that the TCP usage is not the most effective way to communicate between the
 * game clients, but implementation of the good reliability protocol over the UDP would take
 * some time, and I suppose that this approach is acceptable for the small games like this.
 */
class CNetworkSystem
{
public:

	CNetworkSystem() = default;
	CNetworkSystem(const CNetworkSystem&) = delete;

	/**
	 * @function StartServer
	 * Bind the game client to the required TCP and UDP ports.
	 * 
	 * @return True if the server started successfully, false otherwise.
	 */
	bool StartServer();

	// Free the binded ports and clear the connections list.
	void StopServer();

	/**
	 * @function Connect
	 * Bind the game client to the required TCP and UDP ports and
	 * connect to the TCP server.
	 * 
	 * @return True if the connection succeed, false otherwise.
	 */
	bool Connect(const std::string& server);
	
	// Free the binded ports
	void Disconnect();

	bool IsServerStarted() const;
	bool IsConnected() const;

	// Listen to the new connections
	void AcceptConnections();

	// Receive the new messages from the connected clients
	void ProcessClientMessages();

	// Receive the new messages from the server
	void ProcessServerMessages();

	/**
	 * @function SendServerMessage
	 * Send the message packet to the specified client.
	 * 
	 * @param clientId - identifier of the client.
	 * @param packet - packet with the data to send.
	 */
	void SendServerMessage(int clientId, sf::Packet& packet);

	/**
	 * @function BroadcastServerMessage
	 * Send the message packet to all the connected clients.
	 *
	 * @param packet - packet with the data to send.
	 */
	void BroadcastServerMessage(sf::Packet& packet);

	/**
	 * @function SendClientMessage
	 * Send the message packet to the server.
	 * 
	 * @param packet - packet with the data to send.
	 */
	void SendClientMessage(sf::Packet& packet);

	/**
	 * @function SendSerializationMessage
	 * Send the message packet to all the connected clients.
	 * 
	 * @param packet - packet with the data to send.
	 * @note Serialization is sent over the UDP, so it is not
	 * guaranteed that all the messages will be received in the
	 * proper order either received at all. But it is not so bad
	 * since all the actors are serialized at least once per second (see CActor).
	 */
	void SendSerializationMessage(sf::Packet& packet);

private:

	struct SRemoteClient
	{
		sf::TcpSocket tcpSocket;
		unsigned short udpPort = 0;
	};

	sf::IpAddress m_serverAddress;
	sf::UdpSocket m_udpClient;
	sf::TcpSocket m_tcpClient;
	
	sf::UdpSocket m_udpServer;
	sf::TcpListener m_tcpServer;
	std::map<int, SRemoteClient> m_remoteClients;
};