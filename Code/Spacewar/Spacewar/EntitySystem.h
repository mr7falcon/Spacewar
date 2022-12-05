#pragma once

#include <vector>
#include <algorithm>
#include <functional>

typedef int SmartId;
static constexpr const int InvalidLink = -1;

/**
 * @class CEntity
 * This base class allows its heirs to be part of CEntitySystem (see below).
 * The main entitiy characteristic is it's ID, which is unique and should be used
 * to identify entity within the entity system and operate with it.
 */
class CEntity
{
public:

	SmartId GetId() const { return m_id; }
	void SetId(SmartId sid) { m_id = sid; }

private:

	SmartId m_id = InvalidLink;
};

/**
 * @class CEntitySystem
 * The base class providing effective storing and processing the entities.
 * All the entities are sored in one array to guarantee fast iterating over them.
 * A entity can be accessed via special it's unique identifier - SmartId.
 * SmartIds are simply the indices of an array storing indices of the entity array.
 * SmartIds allows to rearrange the entities while operating them in their array
 * without invalidating identifiers outside of the system.
 * CEntitySystem can be thread safe since the new entities are being added only in the
 * end of the array, and the number of the currect active entities can be fixed each frame.
 * At the same time one can enforce system to use safe removing method, which just
 * unlinks entity from it's SmartId without actual deletion until the end of the frame.
 * 
 * @template param T - type of the entities, contained in the system. Must be inheritor of CEntity.
 * @template param SafeRemive - by enabling, enforce the system to use safe removing.
 */
template <typename T, bool SafeRemove>
class CEntitySystem
{
public:

	CEntitySystem(int initialSize)
	{
		m_entities.reserve(initialSize);
		m_smartLinks.reserve(initialSize);
	}

	CEntitySystem(const CEntitySystem&) = delete;

	virtual ~CEntitySystem() = default;

	/**
	* @function CreateEntity
	* Template function to create entity in the array in-place.
	* The new entity is linked to an empty SmartId slot or this slot is created.
	* 
	* @template params V - types of arguments which are forwarder into the entity constructor.
	* @params args - arguments of the entity constructor.
	* @return SmartId linked to the new entity.
	*/
	template <typename... V>
	SmartId CreateEntity(V&&... args)
	{
		SmartId sid = InvalidLink;

		auto fnd = std::find(m_smartLinks.begin(), m_smartLinks.end(), InvalidLink);
		if (fnd != m_smartLinks.end())
		{
			*fnd = (int)m_entities.size();
			sid = (SmartId)(fnd - m_smartLinks.begin());
		}
		else
		{
			m_smartLinks.push_back((int)m_entities.size());
			sid = (SmartId)(m_smartLinks.size() - 1);
		}

		m_entities.emplace_back(std::forward<V>(args)...).SetId(sid);

		return sid;
	}
	
	/**
	* @function RemoveEntity
	* Unlink the entity so it cannot be accessed by this SmartId anymore
	* if the SafeRemove param is set in true or the immediate param is false.
	* Delete the entity from the container otherwise.
	* 
	* @param sid - SmartId linked to the entity, which should be removed.
	* @param immediate - "physical" delete the entity from the container immediately.
	* @note If the SafeRemove template parameter is set, the immediate parameter is ignored.
	*/
	virtual void RemoveEntity(SmartId sid, bool immediate = false)
	{
		if (sid < m_smartLinks.size() && m_smartLinks[sid] != InvalidLink)
		{
			if (SafeRemove || !immediate)
			{
				UnlinkEntity(sid);
			}
			else
			{
				DeleteEntity(m_smartLinks[sid]);
			}
		}
	}

	/**
	* @function Clear
	* Unlink all the entities so they cannot be accessed by SmartIds anymore
	* if SafeRemove is set. Clear the entity container otherwise.
	*/
	virtual void Clear()
	{
		for (SmartId i = 0; i < m_smartLinks.size(); ++i)
		{
			if (m_smartLinks[i] != InvalidLink)
			{
				UnlinkEntity(i);
			}
		}

		if constexpr (!SafeRemove)
		{
			m_entities.clear();
		}
	}

	/**
	* @function GetEntity
	* Returns a pointer to the entity linked to the SmartId if it's valid.
	* 
	* @param sid - SmartId of the entity.
	* @return A pointer to the entity linked to the SmartId or nullptr.
	*/
	inline T* GetEntity(SmartId sid)
	{
		if (sid >= 0 && sid < m_smartLinks.size() && m_smartLinks[sid] != InvalidLink)
		{
			return &m_entities[m_smartLinks[sid]];
		}
		return nullptr;
	}

	/**
	* @function GetNumEntities
	* Returns the current number of the entities in the container.
	* 
	* @return The number of the entities in the container.
	*/
	inline int GetNumEntities() const
	{
		return (int)m_entities.size();
	}

	/**
	* @function ForEachEntity
	* Function to iterate over the valid entities in the container.
	* 
	* @param f - function, which is applied to every entity.
	*/
	inline void ForEachEntity(std::function<void(T&)> f)
	{
		for (int i = 0; i < m_entities.size(); ++i)
		{
			if (m_entities[i].GetId() != InvalidLink)
			{
				f(m_entities[i]);
			}
		}
	}

	/**
	* @function CollectGarbage
	* Delete all the unlinked entities from the container.
	* 
	* @note This function is used after the system update and
	* the safety is guaranted by the caller.
	*/
	virtual void CollectGarbage()
	{
		for (int i = 0; i < m_entities.size(); ++i)
		{
			if (m_entities[i].GetId() == InvalidLink)
			{
				DeleteEntity(i);
			}
		}
	}

private:

	inline void UnlinkEntity(SmartId sid)
	{
		m_entities[m_smartLinks[sid]].SetId(InvalidLink);
		m_smartLinks[sid] = InvalidLink;
	}

	inline void DeleteEntity(int idx)
	{
		std::iter_swap(m_entities.begin() + idx, m_entities.end() - 1);
		if (m_entities[idx].GetId() != InvalidLink)
		{
			m_smartLinks[m_entities[idx].GetId()] = idx;
		}
		m_entities.pop_back();
	}

protected:

	std::vector<T> m_entities;
	std::vector<int> m_smartLinks;
};