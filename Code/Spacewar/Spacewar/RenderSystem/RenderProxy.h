#pragma once

#include "MemoryStream.h"
#include "RenderSystem.h"
#include "Game.h"

#include <SFML/Graphics/Transform.hpp>

namespace RenderCommand
{
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

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetTransform; }
		virtual void Execute() const override;

		sf::Transform transform;
	};

	struct SetTextureCommand : public RenderCommand
	{
		SetTextureCommand(SmartId _sid, int _textureId)
			: RenderCommand(_sid), textureId(_textureId) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetTexture; }
		virtual void Execute() const override;

		int textureId;
	};

	struct SetSizeCommand : public RenderCommand
	{
		SetSizeCommand(SmartId _sid, sf::Vector2f&& _size)
			: RenderCommand(_sid), size(_size) {}
		SetSizeCommand(SmartId _sid, const sf::Vector2f& _size)
			: RenderCommand(_sid), size(_size) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetSize; }
		virtual void Execute() const override;

		sf::Vector2f size;
	};

	struct SetColorCommand : public RenderCommand
	{
		SetColorCommand(SmartId _sid, sf::Color _color)
			: RenderCommand(_sid), color(_color) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetColor; }
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

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetText; }
		virtual void Execute() const override;

		char text[MaxTextLength];
	};

	struct SetStyleCommand : public RenderCommand
	{
		SetStyleCommand(SmartId _sid, uint32_t _style)
			: RenderCommand(_sid), style(_style) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetStyle; }
		virtual void Execute() const override;

		uint32_t style;
	};

	struct SetCharacterSizeCommand : public RenderCommand
	{
		SetCharacterSizeCommand(SmartId _sid, unsigned int _size)
			: RenderCommand(_sid), size(_size) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetCharacterSize; }
		virtual void Execute() const override;

		unsigned int size;
	};

	struct SetFontCommand : public RenderCommand
	{
		SetFontCommand(SmartId _sid, int _fontId)
			: RenderCommand(_sid), fontId(_fontId) {}

		static constexpr ERenderCommand GetType() { return ERenderCommand_SetFont; }
		virtual void Execute() const override;

		int fontId;
	};
}

class CRenderProxy
{
public:

	CRenderProxy() = default;
	CRenderProxy(const CRenderProxy&) = delete;

	template <typename T, typename... V>
	void OnCommand(SmartId sid, V&&... args)
	{
		m_memoryStreams[m_dWriteStream] << T::GetType();
		m_memoryStreams[m_dWriteStream].Emplace<T>(sid, std::forward<V>(args)...);
	}

	void SwitchStreams();
	void ExecuteCommands();

private:

	static constexpr const size_t MemoryBufferInitialSize = 1024;

	CMemoryStream m_memoryStreams[2] = { CMemoryStream(MemoryBufferInitialSize), CMemoryStream(MemoryBufferInitialSize) };
	int m_dWriteStream = 0;
	int m_dReadStream = 1;
};