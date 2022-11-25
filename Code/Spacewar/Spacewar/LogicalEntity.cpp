#include "LogicalEntity.h"
#include "Game.h"
#include "PhysicalSystem.h"
#include "RenderProxy.h"
#include "MathHelpers.h"

void CLogicalEntity::OnTransformUpdated()
{
	if (CPhysicalEntity* pPhysicalEntity = CGame::Get().GetPhysicalSystem()->GetEntity(m_physicalEntityId))
	{
		pPhysicalEntity->OnTransformChanged(getTransform());
	}
	CGame::Get().GetRenderProxy()->OnCommand<CRenderProxy::ERenderCommand_SetTransform>(m_renderEntityId, getTransform());
}

void CLogicalEntity::SetPosition(const sf::Vector2f& vPos)
{
	if (vPos != getPosition())
	{
		setPosition(vPos);
		OnTransformUpdated();
	}
}

void CLogicalEntity::SetRotation(float fRot)
{
	if (fRot != getRotation())
	{
		setRotation(fRot);
		OnTransformUpdated();
	}
}

void CLogicalEntity::SetScale(float fScale)
{
	if (fScale != getScale().x)
	{
		setScale(sf::Vector2f(fScale, fScale));
		OnTransformUpdated();
	}
}

void CLogicalEntity::Update(sf::Time dt)
{
	SetPosition(GetPosition() + m_vVel * dt.asSeconds());
	SetRotation(GetRotation() + m_fAngSpeed * dt.asSeconds());
}

sf::Vector2f CLogicalEntity::GetForwardDirection() const
{
	static sf::Vector2f ForwardVector(0.f, 1.f);
	return MathHelpers::Normalize(getTransform().transformPoint(ForwardVector) - getPosition());
}