#pragma once

#include <SFML/Graphics/Transform.hpp>

#include "MemoryStream.h"
#include "RenderSystem.h"
#include "Game.h"

class CRenderProxy
{
public:

	enum ERenderCommand : unsigned char
	{
		ERenderCommand_SetTransform,
	};

	CRenderProxy() = default;
	CRenderProxy(const CRenderProxy&) = delete;

	template <typename... V>
	void OnCommand(ERenderCommand cmd, SmartId sid, V&&... args)
	{
		bool schedule = CGame::Get().GetRenderSystem()->IsActiveEntity(sid);

		switch (cmd)
		{
		case ERenderCommand_SetTransform:
			PushCommand<SetTransformCommand>(cmd, sid, schedule, std::forward<V>(args)...);
			break;
		}
	}

	void SwitchStreams();
	void ExecuteCommands();

private:

	template <typename T, typename... V>
	inline void PushCommand(ERenderCommand cmd, SmartId sid, bool schedule, V&&... args)
	{
		if (schedule)
		{
			m_memoryStreams[m_dWriteStream] << cmd;
			m_memoryStreams[m_dWriteStream].Emplace<T>(sid, std::forward<V>(args)...);
		}
		else
		{
			T(sid, std::forward<V>(args)...).Execute();
		}
	}

private:

	struct RenderCommand
	{
		RenderCommand(SmartId _sid) : sid(_sid) {}

		virtual void Execute() const = 0;

		SmartId sid;
	};

	struct SetTransformCommand : public RenderCommand
	{
		SetTransformCommand(SmartId _sid, sf::Transform&& _transform)
			: RenderCommand(_sid), transform(_transform) {}
		SetTransformCommand(SmartId _sid, const sf::Transform& _transform)
			: RenderCommand(_sid), transform(_transform) {}
		
		virtual void Execute() const override;

		sf::Transform transform;
	};

	static constexpr const size_t memory_buffer_initial_size = 1024;

	CMemoryStream m_memoryStreams[2] = { CMemoryStream(memory_buffer_initial_size), CMemoryStream(memory_buffer_initial_size) };
	int m_dWriteStream = 0;
	int m_dReadStream = 1;
};