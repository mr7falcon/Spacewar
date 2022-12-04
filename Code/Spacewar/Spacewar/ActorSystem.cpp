#include "ActorSystem.h"

void CActorSystem::RemoveActor(SmartId sid, bool immediate)
{
	if (immediate)
	{
		m_actors.erase(sid);
	}
	else
	{
		m_removeDeferred.push_back(sid);
	}
	CGame::Get().GetNetworkProxy()->BroadcastServerMessage<ServerMessage::SRemoveActorMessage>(sid);
}

CActor* CActorSystem::GetActor(SmartId sid)
{
	auto fnd = m_actors.find(sid);
	return fnd != m_actors.end() ? fnd->second.get() : nullptr;
}

void CActorSystem::ForEachPlayer(std::function<bool(CPlayer*)> f)
{
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->GetType() == EActorType_Player)
		{
			if (!f(static_cast<CPlayer*>(pActor.get())))
			{
				break;
			}
		}
	}
}

void CActorSystem::RemoveProjectiles()
{
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->GetType() == EActorType_Projectile)
		{
			m_removeDeferred.push_back(pActor->GetEntityId());
		}
	}
}

int CActorSystem::GetNumPlayers() const
{
	int count = 0;
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->GetType() == EActorType_Player)
		{
			++count;
		}
	}
	return count;
}

SmartId CActorSystem::GetFirstPlayerId() const
{
	for (auto iter = m_actors.begin(); iter != m_actors.end(); ++iter)
	{
		if (iter->second->GetType() == EActorType_Player)
		{
			return iter->first;
		}
	}
	return InvalidLink;
}

SmartId CActorSystem::GetLastPlayerId() const
{
	for (auto iter = m_actors.rbegin(); iter != m_actors.rend(); ++iter)
	{
		if (iter->second->GetType() == EActorType_Player)
		{
			return iter->first;
		}
	}
	return InvalidLink;
}

void CActorSystem::Update(sf::Time dt)
{
	for (const auto& [sid, pActor] : m_actors)
	{
		pActor->Update(dt);
	}
}

void CActorSystem::CollectGarbage()
{
	for (SmartId sid : m_removeDeferred)
	{
		m_actors.erase(sid);
	}
	m_removeDeferred.clear();
}

void CActorSystem::Release()
{
	m_removeDeferred.clear();
	m_actors.clear();
}


#include <iostream>


void CActorSystem::Serialize(sf::Packet& packet, bool bReading)
{
	if (!bReading)
	{
		for (auto& [sid, pActor] : m_actors)
		{
			if (pActor->NeedSerialize())
			{
				packet << sid;
				
				uint16_t size = 0;
				pActor->Serialize(packet, ESerializationMode_Count, size);
				packet << size;

				size = 0;
				pActor->Serialize(packet, ESerializationMode_Write, size);
			}
		}
	}
	else
	{
		while (!packet.endOfPacket())
		{
			SmartId serverId;
			uint16_t size;
			packet >> serverId;
			packet >> size;

			if (CActor* pActor = GetActor(CGame::Get().GetNetworkProxy()->GetLocalEntityId(serverId)))
			{
				size = 0;
				pActor->Serialize(packet, ESerializationMode_Read, size);
			}
			else
			{
				std::cout << "Serialize invalid actor " << serverId << std::endl;
				for (uint16_t i = 0; i < size; ++i)
				{
					uint8_t byte;
					packet >> byte;
				}
			}
		}
	}
}

void CActorSystem::SetPlayersShooting(bool bShooting)
{
	ForEachPlayer([bShooting](CPlayer* pPlayer) 
		{ 
			pPlayer->SetShooting(bShooting);
			return true;
		});
	m_bPlayersShooting = bShooting;
}