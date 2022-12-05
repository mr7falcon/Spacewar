#pragma once

#include <vector>

#include <SFML/Audio/Sound.hpp>

/**
 * @class CSoundSystem
 * Simplae class to play and process sounds.
 */
class CSoundSystem
{
public:

	CSoundSystem() = default;
	CSoundSystem(const CSoundSystem&) = delete;

	// Get the sound with the specified identifier
	int PlaySound(int resourceId, bool bCicled = false);
	
	// Stop the sound with the specified identifier
	void StopSound(int soundId);

	// Get the sfml sound istance by the identifier
	sf::Sound* GetSound(int id);

	void Update();
	void Clear();

private:

	std::vector<sf::Sound> m_activeSounds;
};