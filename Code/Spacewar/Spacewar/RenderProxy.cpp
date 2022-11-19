#include "RenderProxy.h"
#include "Game.h"

void CRenderProxy::OnTransformChanged(SmartId sid, const sf::Transform& transform)
{
	m_memoryStreams[m_dWriteStream] << ERenderCommand_SetTransform;
	SSetTransformCommand cmd;
	cmd.sid = sid;
	cmd.transform = transform;
	m_memoryStreams[m_dWriteStream] << cmd;
}

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
			ExecuteSetTransformCommand();
			break;
		}
	}
}

void CRenderProxy::ExecuteSetTransformCommand()
{
	SSetTransformCommand cmd;
	m_memoryStreams[m_dReadStream] >> cmd;

	if (CRenderEntity* pRenderEntity = CGame::Get().GetRenderSystem()->GetEntity(cmd.sid))
	{
		pRenderEntity->SetTransform(cmd.transform);
	}
}