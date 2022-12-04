#include "StdAfx.h"
#include "PhysicalEntity.h"

void CPhysicalEntity::OnTransformChanged(const sf::Transform& transform)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Transform(transform * m_transform.getInverse());
	}
	m_transform = transform;
}

void CPhysicalEntity::OnCollision(SmartId sid)
{
	for (IPhysicalEventListener* pListener : m_eventListeners)
	{
		pListener->OnCollision(sid);
	}
}

void CPhysicalEntity::RegisterEventListener(IPhysicalEventListener* pListener)
{
	auto fnd = std::find(m_eventListeners.begin(), m_eventListeners.end(), pListener);
	if (fnd == m_eventListeners.end())
	{
		m_eventListeners.push_back(pListener);
	}
}

void CPhysicalEntity::UnregisterEventListener(IPhysicalEventListener* pListener)
{
	auto fnd = std::find(m_eventListeners.begin(), m_eventListeners.end(), pListener);
	if (fnd != m_eventListeners.end())
	{
		m_eventListeners.erase(fnd);
	}
}