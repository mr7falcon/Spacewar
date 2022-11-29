#pragma once

#include <string>
#include <memory>

#include "LogicalEntity.h"

class CActorSystem;
class CLevelSystem;

class CLogicalSystem : public CEntitySystem<CLogicalEntity, false>
{
public:

	CLogicalSystem();
	~CLogicalSystem();

	SmartId CreateEntityFromClass(const std::string& name);

	virtual void RemoveEntity(SmartId sid, bool immediate = false) override;
	virtual void CollectGarbage() override;
	virtual void Clear() override;

	void Update(sf::Time dt);

	CActorSystem* GetActorSystem() { return m_pActorSystem.get(); }
	CLevelSystem* GetLevelSystem() { return m_pLevelSystem.get(); }

private:

	std::unique_ptr<CActorSystem> m_pActorSystem;
	std::unique_ptr<CLevelSystem> m_pLevelSystem;
};