#pragma once

#include "LogicalEntity.h"

#include <string>
#include <memory>

class CActorSystem;
class CLevelSystem;
class CFeedbackSystem;

/**
 * @class CLogicalSystem
 * System which contains all the logical entities and some of the game subsystems.
 * Provides functions to create, remove and process the logical entities.
 */
class CLogicalSystem : public CEntitySystem<CLogicalEntity, false>
{
public:

	CLogicalSystem();
	~CLogicalSystem();

	/**
	 * @function CreateEntityFromClass
	 * Create entity from the specified configuration (see CEntityConfiguration).
	 * 
	 * @param name - entity configuration's name to create
	 */
	SmartId CreateEntityFromClass(const std::string& name);

	/**
	 * @function RemoveEntity
	 * Remove the entity with the specified SmartId. Overrides the CEntitySystem
	 * method to remove the owner physical and render entities as well.
	 * 
	 * @param sid - SmartId of the entity to remove.
	 * @immediate - "physical" delete the entity from the container immediately.
	 */
	virtual void RemoveEntity(SmartId sid, bool immediate = false) override;

	/**
	 * @function CollectGarbage
	 * Override of the CEntitySystem method. Also initiates the CActorSystem's
	 * garbage collection.
	 */
	virtual void CollectGarbage() override;

	/**
	 * @function Clear
	 * Override of the CEntitySystem method. Also initiates the CActorSystem's clearing.
	 */
	virtual void Clear() override;

	/**
	 * @function Update
	 * Function to update all the logical entities' states. Also updates all the logical
	 * subsystems. Should be called each frame.
	 *
	 * @param dt - delta time since the last function call.
	 */
	void Update(sf::Time dt);

	// Release the logical subsystems in the proper order
	void Release();

	CActorSystem* GetActorSystem() { return m_pActorSystem.get(); }
	CLevelSystem* GetLevelSystem() { return m_pLevelSystem.get(); }
	CFeedbackSystem* GetFeedbackSystem() { return m_pFeedbackSystem.get(); }

private:

	std::unique_ptr<CActorSystem> m_pActorSystem;
	std::unique_ptr<CLevelSystem> m_pLevelSystem;
	std::unique_ptr<CFeedbackSystem> m_pFeedbackSystem;
};