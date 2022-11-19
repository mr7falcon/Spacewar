#pragma once

#include <SFML/Graphics/Transform.hpp>

#include "MemoryStream.h"
#include "RenderSystem.h"

class CRenderProxy
{
public:

	CRenderProxy() = default;
	CRenderProxy(const CRenderProxy&) = delete;

	void OnTransformChanged(SmartId sid, const sf::Transform& transform);

	void SwitchStreams();
	void ExecuteCommands();

private:

	void ExecuteSetTransformCommand();

private:

	enum ERenderCommand : unsigned char
	{
		ERenderCommand_SetTransform,
	};

	struct SSetTransformCommand
	{
		SmartId sid;
		sf::Transform transform;
	};

	static constexpr const size_t memory_buffer_initial_size = 1024;

	CMemoryStream m_memoryStreams[2] = { CMemoryStream(memory_buffer_initial_size), CMemoryStream(memory_buffer_initial_size) };
	int m_dWriteStream = 0;
	int m_dReadStream = 1;
};