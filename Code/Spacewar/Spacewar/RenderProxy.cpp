#include "RenderProxy.h"

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