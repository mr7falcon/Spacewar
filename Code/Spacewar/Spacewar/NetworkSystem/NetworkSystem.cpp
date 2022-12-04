#include "StdAfx.h"
#include "NetworkSystem.h"
#include "Game.h"
#include "NetworkProxy.h"

static constexpr unsigned short TcpServerPort = 7777;
static constexpr unsigned short UdpServerPort = 7778;

bool CNetworkSystem::StartServer()
{
	if (IsServerStarted())
	{
		return true;
	}

	if (IsConnected())
	{
		return false;
	}

	auto status = m_tcpServer.listen(TcpServerPort);
	if (status != sf::Socket::Done)
	{
		return false;
	}

	status = m_udpServer.bind(UdpServerPort);
	if (status != sf::Socket::Done)
	{
		return false;
	}

	m_tcpServer.setBlocking(false);
	m_udpServer.setBlocking(false);

	return true;
}

bool CNetworkSystem::Connect(const std::string& host)
{
	if (IsConnected())
	{
		return true;
	}

	if (IsServerStarted())
	{
		return false;
	}

	auto status = m_tcpClient.connect(host, TcpServerPort);
	if (status != sf::Socket::Done)
	{
		CGame::Get().GetNetworkProxy()->OnConnectionFailed();
		return false;
	}

	status = m_udpClient.bind(sf::Socket::AnyPort);
	if (status != sf::Socket::Done)
	{
		CGame::Get().GetNetworkProxy()->OnConnectionFailed();
		return false;
	}

	sf::Packet packet;
	packet << m_udpClient.getLocalPort();
	status = m_tcpClient.send(packet);
	if (status != sf::Socket::Done)
	{
		CGame::Get().GetNetworkProxy()->OnConnectionFailed();
		return false;
	}

	m_tcpClient.setBlocking(false);
	m_udpClient.setBlocking(false);

	m_serverAddress = host;
	CGame::Get().GetNetworkProxy()->OnConnect();

	return true;
}

void CNetworkSystem::Disconnect()
{
	if (IsConnected())
	{
		m_tcpClient.setBlocking(true);
		m_udpClient.setBlocking(true);
		m_tcpClient.disconnect();
		m_udpClient.unbind();
		CGame::Get().GetNetworkProxy()->OnDisconnect();
	}
}

void CNetworkSystem::StopServer()
{
	if (IsServerStarted())
	{
		m_tcpServer.setBlocking(true);
		m_udpServer.setBlocking(true);
		m_tcpServer.close();
		m_udpServer.unbind();

		for (auto& [id, client] : m_remoteClients)
		{
			client.tcpSocket.disconnect();
		}
		m_remoteClients.clear();
	}
}

bool CNetworkSystem::IsServerStarted() const
{
	return m_tcpServer.getLocalPort() != 0 && m_udpServer.getLocalPort() != 0;
}

bool CNetworkSystem::IsConnected() const
{
	return m_tcpClient.getLocalPort() != 0 && m_udpClient.getLocalPort() != 0;
}

void CNetworkSystem::AcceptConnections()
{
	int id = (int)m_remoteClients.size();
	if (m_tcpServer.accept(m_remoteClients[id].tcpSocket) != sf::Socket::Done)
	{
		m_remoteClients.erase(id);
	}
	else
	{
		m_remoteClients[id].tcpSocket.setBlocking(false);
		CGame::Get().GetNetworkProxy()->OnClientConnect(id);
	}
}

void CNetworkSystem::ProcessClientMessages()
{
	for (auto iter = m_remoteClients.begin(); iter != m_remoteClients.end();)
	{
		sf::Packet packet;
		auto status = iter->second.tcpSocket.receive(packet);
		if (status == sf::Socket::Done)
		{
			if (iter->second.udpPort == 0)
			{
				packet >> iter->second.udpPort;
			}
			else
			{
				CGame::Get().GetNetworkProxy()->OnClientMessageReceived(iter->first, packet);
			}
		}
		else if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
		{
			CGame::Get().GetNetworkProxy()->OnClientDisconnect(iter->first);
			iter = m_remoteClients.erase(iter);
			continue;
		}

		++iter;
	}
}

void CNetworkSystem::ProcessServerMessages()
{
	sf::Packet packet;
	auto status = m_tcpClient.receive(packet);
	if (status == sf::Socket::Done)
	{
		CGame::Get().GetNetworkProxy()->OnServerMessageReceived(packet);
	}
	else if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
	{
		Disconnect();
	}

	packet.clear();
	sf::IpAddress addr;
	unsigned short port;
	status = m_udpClient.receive(packet, addr, port);
	if (status == sf::Socket::Done)
	{
		CGame::Get().GetNetworkProxy()->OnSerializationReceived(packet);
	}
}

void CNetworkSystem::SendServerMessage(int clientId, sf::Packet& packet)
{
	auto fnd = m_remoteClients.find(clientId);
	if (fnd == m_remoteClients.end())
	{
		return;
	}

	sf::Socket::Status status = fnd->second.tcpSocket.send(packet);
	while (status == sf::Socket::Partial || status == sf::Socket::NotReady)
	{
		status = fnd->second.tcpSocket.send(packet);
	}

	if (status != sf::Socket::Done)
	{
		m_remoteClients.erase(fnd);
		CGame::Get().GetNetworkProxy()->OnClientDisconnect(clientId);
	}
}

void CNetworkSystem::BroadcastServerMessage(sf::Packet& packet)
{
	for (auto iter = m_remoteClients.begin(); iter != m_remoteClients.end();)
	{
		sf::Socket::Status status = iter->second.tcpSocket.send(packet);
		while (status == sf::Socket::Partial || status == sf::Socket::NotReady)
		{
			status = iter->second.tcpSocket.send(packet);
		}

		if (status != sf::Socket::Done)
		{
			CGame::Get().GetNetworkProxy()->OnClientDisconnect(iter->first);
			iter = m_remoteClients.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CNetworkSystem::SendClientMessage(sf::Packet& packet)
{
	sf::Socket::Status status = m_tcpClient.send(packet);
	while (status == sf::Socket::Partial || status == sf::Socket::NotReady)
	{
		status = m_tcpClient.send(packet);
	}

	if (status != sf::Socket::Done)
	{
		Disconnect();
	}
}

void CNetworkSystem::SendSerializationMessage(sf::Packet& packet)
{
	for (const auto& [id, client] : m_remoteClients)
	{
		while (m_udpServer.send(packet, client.tcpSocket.getRemoteAddress(), client.udpPort) == sf::Socket::Partial) {}
	}
}