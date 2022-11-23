#include "LogicalEntity.h"
#include "Game.h"
#include "PhysicalSystem.h"
#include "RenderProxy.h"

void CLogicalEntity::UpdateTransform()
{
	sf::Transform transform;
	transform.translate(m_vPos);
	transform.rotate(m_fRot);
	transform.scale(m_vScale);

	if (CPhysicalEntity* pPhysicalEntity = CGame::Get().GetPhysicalSystem()->GetEntity(m_physicalEntityId))
	{
		pPhysicalEntity->OnTransformChanged(transform);
	}
	CGame::Get().GetRenderProxy()->OnCommand<CRenderProxy::ERenderCommand_SetTransform>(m_renderEntityId, transform);
}

void CLogicalEntity::SetPosition(const sf::Vector2f& vPos)
{
	if (vPos != m_vPos)
	{
		m_vPos = vPos;
		UpdateTransform();
	}
}

void CLogicalEntity::SetRotation(float fRot)
{
	if (fRot != m_fRot)
	{
		m_fRot = fRot;
		UpdateTransform();
	}
}

void CLogicalEntity::SetScale(const sf::Vector2f& vScale)
{
	if (vScale != m_vScale)
	{
		m_vScale = vScale;
		UpdateTransform();
	}
}

void CLogicalEntity::Update(sf::Time dt)
{
	m_vVel += m_vAccel * dt.asSeconds();
	m_fAngSpeed += m_fAngAccel * dt.asSeconds();
	SetPosition(GetPosition() + m_vVel * dt.asSeconds());
	SetRotation(GetRotation() + m_fAngSpeed * dt.asSeconds());
}