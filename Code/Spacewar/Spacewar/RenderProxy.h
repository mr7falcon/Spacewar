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
		ERenderCommand_SetColor,
		ERenderCommand_SetTexture,
		ERenderCommand_SetSize,
		ERenderCommand_SetText,
		ERenderCommand_SetStyle,
		ERenderCommand_SetCharacterSize,
		ERenderCommand_SetFont
	};

	CRenderProxy() = default;
	CRenderProxy(const CRenderProxy&) = delete;

	template <ERenderCommand cmd, typename... V>
	void OnCommand(SmartId sid, V&&... args)
	{
		m_memoryStreams[m_dWriteStream] << cmd;
		if constexpr (cmd == ERenderCommand_SetTransform)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetTransformCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetTexture)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetTextureCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetSize)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetSizeCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetColor)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetColorCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetText)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetTextCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetStyle)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetStyleCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetCharacterSize)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetCharacterSizeCommand>(sid, std::forward<V>(args)...);
		}
		else if constexpr (cmd == ERenderCommand_SetFont)
		{
			m_memoryStreams[m_dWriteStream].Emplace<SetFontCommand>(sid, std::forward<V>(args)...);
		}
	}

	void SwitchStreams();
	void ExecuteCommands();

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

	struct SetTextureCommand : public RenderCommand
	{
		SetTextureCommand(SmartId _sid, int _textureId)
			: RenderCommand(_sid), textureId(_textureId) {}

		virtual void Execute() const override;

		int textureId;
	};

	struct SetSizeCommand : public RenderCommand
	{
		SetSizeCommand(SmartId _sid, sf::Vector2f&& _size)
			: RenderCommand(_sid), size(_size) {}
		SetSizeCommand(SmartId _sid, const sf::Vector2f& _size)
			: RenderCommand(_sid), size(_size) {}

		virtual void Execute() const override;

		sf::Vector2f size;
	};

	struct SetColorCommand : public RenderCommand
	{
		SetColorCommand(SmartId _sid, sf::Color _color)
			: RenderCommand(_sid), color(_color) {}

		virtual void Execute() const override;

		sf::Color color;
	};

	struct SetTextCommand : public RenderCommand
	{
		static constexpr int MaxTextLength = 256;

		SetTextCommand(SmartId _sid, const std::string& _text)
			: RenderCommand(_sid)
		{
			strcpy_s(text, _text.c_str());
		}

		virtual void Execute() const override;

		char text[MaxTextLength];
	};

	struct SetStyleCommand : public RenderCommand
	{
		SetStyleCommand(SmartId _sid, uint32_t _style)
			: RenderCommand(_sid), style(_style) {}
		
		virtual void Execute() const override;

		uint32_t style;
	};

	struct SetCharacterSizeCommand : public RenderCommand
	{
		SetCharacterSizeCommand(SmartId _sid, unsigned int _size)
			: RenderCommand(_sid), size(_size) {}

		virtual void Execute() const override;

		unsigned int size;
	};

	struct SetFontCommand : public RenderCommand
	{
		SetFontCommand(SmartId _sid, int _fontId)
			: RenderCommand(_sid), fontId(_fontId) {}

		virtual void Execute() const override;

		int fontId;
	};

	static constexpr const size_t memory_buffer_initial_size = 1024;

	CMemoryStream m_memoryStreams[2] = { CMemoryStream(memory_buffer_initial_size), CMemoryStream(memory_buffer_initial_size) };
	int m_dWriteStream = 0;
	int m_dReadStream = 1;
};