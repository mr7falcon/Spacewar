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
		ERenderCommand cmd;
		m_memoryStreams[m_dReadStream] >> cmd;

		switch (cmd)
		{
		case ERenderCommand_SetTransform:
			m_memoryStreams[m_dReadStream].Extract<SetTransformCommand>().Execute();
			break;
		case ERenderCommand_SetTexture:
			m_memoryStreams[m_dReadStream].Extract<SetTextureCommand>().Execute();
			break;
		case ERenderCommand_SetSize:
			m_memoryStreams[m_dReadStream].Extract<SetSizeCommand>().Execute();
			break;
		case ERenderCommand_SetColor:
			m_memoryStreams[m_dReadStream].Extract<SetColorCommand>().Execute();
			break;
		case ERenderCommand_SetText:
			m_memoryStreams[m_dReadStream].Extract<SetTextCommand>().Execute();
			break;
		case ERenderCommand_SetStyle:
			m_memoryStreams[m_dReadStream].Extract<SetStyleCommand>().Execute();
			break;
		case ERenderCommand_SetCharacterSize:
			m_memoryStreams[m_dReadStream].Extract<SetCharacterSizeCommand>().Execute();
			break;
		case ERenderCommand_SetFont:
			m_memoryStreams[m_dReadStream].Extract<SetFontCommand>().Execute();
			break;
		}
	}
}

void CRenderProxy::SetTransformCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetTransform(transform);
	}
}

void CRenderProxy::SetTextureCommand::Execute() const
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

void CRenderProxy::SetSizeCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetSize(size);
	}
}

void CRenderProxy::SetColorCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetColor(color);
	}
}

void CRenderProxy::SetTextCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetText(text);
	}
}

void CRenderProxy::SetStyleCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetStyle(style);
	}
}

void CRenderProxy::SetCharacterSizeCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		pRenderEntity->SetCharacterSize(size);
	}
}

void CRenderProxy::SetFontCommand::Execute() const
{
	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
	{
		if (const sf::Font* pFont = CGame::Get().GetResourceSystem()->GetFont(fontId))
		{
			pRenderEntity->SetFont(*pFont);
		}
	}
}