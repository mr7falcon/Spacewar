#include "RenderProxy.h"
#include "ResourceSystem.h"

void CRenderProxy::SwitchStreams()
{
	std::swap(m_dReadStream, m_dWriteStream);
	m_memoryStreams[m_dWriteStream].Clear();
}

void CRenderProxy::ExecuteCommands()
{
	while (!m_memoryStreams[m_dReadStream].Empty())
	{
		RenderCommand::ERenderCommand cmd;
		m_memoryStreams[m_dReadStream] >> cmd;

		switch (cmd)
		{
		case RenderCommand::ERenderCommand_SetTransform:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetTransformCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetTexture:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetTextureCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetSize:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetSizeCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetColor:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetColorCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetText:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetTextCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetStyle:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetStyleCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetCharacterSize:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetCharacterSizeCommand>().Execute();
			break;
		case RenderCommand::ERenderCommand_SetFont:
			m_memoryStreams[m_dReadStream].Extract<RenderCommand::SetFontCommand>().Execute();
			break;
		}
	}
}

void RenderCommand::SetTransformCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetTransform(transform);
	}
}

void RenderCommand::SetTextureCommand::Execute() const
{
	CRenderSystem* pRenderSystem = CGame::Get().GetRenderSystem();
	if (CRenderEntity* pRenderEntity = pRenderSystem->GetEntity(sid))
	{
		if (const sf::Texture* pTexture = pRenderSystem->LoadTexture(textureId))
		{
			pRenderEntity->SetTexture(pTexture);
		}
	}
}

void RenderCommand::SetSizeCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetSize(size);
	}
}

void RenderCommand::SetColorCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetColor(color);
	}
}

void RenderCommand::SetTextCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetText(text);
	}
}

void RenderCommand::SetStyleCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetStyle(style);
	}
}

void RenderCommand::SetCharacterSizeCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetCharacterSize(size);
	}
}

void RenderCommand::SetFontCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		if (const sf::Font* pFont = CGame::Get().GetResourceSystem()->GetFont(fontId))
		{
			pRenderEntity->SetFont(*pFont);
		}
	}
}