#include <iostream>

#include "SoundSystem.h"
#include "Game.h"
#include "ResourceSystem.h"

int CSoundSystem::PlaySound(int resourceId, bool bCicled)
{
	const sf::SoundBuffer* pBuffer = CGame::Get().GetResourceSystem()->GetSound(resourceId);
	if (!pBuffer)
	{
		Log("Failed to play sound");
		return -1;
	}

	sf::Sound& sound = m_activeSounds.emplace_back();
	sound.setBuffer(*pBuffer);
	sound.setLoop(bCicled);
	sound.play();

	return (int)(m_activeSounds.size() - 1);
}

void CSoundSystem::StopSound(int soundId)
{
	if (soundId >= 0 && soundId < m_activeSounds.size())
	{
		m_activeSounds[soundId].stop();
	}
}

sf::Sound* CSoundSystem::GetSound(int id)
{
	if (id >= 0 && id < m_activeSounds.size())
	{
		return &m_activeSounds[id];
	}
	return nullptr;
}

void CSoundSystem::Update()
{
	for (int i = 0; i < m_activeSounds.size(); ++i)
	{
		if (m_activeSounds[i].getStatus() == sf::Sound::Stopped)
		{
			std::iter_swap(m_activeSounds.begin() + i, m_activeSounds.rbegin());
			m_activeSounds.pop_back();
		}
	}
} 

void CSoundSystem::Clear()
{
	for (sf::Sound& sound : m_activeSounds)
	{
		sound.stop();
	}
	m_activeSounds.clear();
}