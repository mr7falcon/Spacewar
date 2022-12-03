#pragma once

#include <vector>

#include <SFML/Audio/Sound.hpp>

class CSoundSystem
{
public:

	CSoundSystem() = default;
	CSoundSystem(const CSoundSystem&) = delete;

	int PlaySound(int resourceId, bool bCicled = false);
	void StopSound(int soundId);
	sf::Sound* GetSound(int id);

	void Update();

private:

	std::vector<sf::Sound> m_activeSounds;
};