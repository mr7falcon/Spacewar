#pragma once

#include <map>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>

class CNetworkSystem
{
public:

	CNetworkSystem() = default;
	CNetworkSystem(const CNetworkSystem&) = delete;

	bool StartServer();
	void StopServer();
	bool Connect(const std::string& server);
	void Disconnect();

	bool IsServerStarted() const;
	bool IsConnected() const;

	void AcceptConnections();
	void ProcessClientMessages();
	void ProcessServerMessages();

	void SendServerMessage(int clientId, sf::Packet& packet);
	void BroadcastServerMessage(sf::Packet& packet);
	void SendClientMessage(sf::Packet& packet);
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