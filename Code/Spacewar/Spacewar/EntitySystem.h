#pragma once

#include <vector>
#include <algorithm>
#include <functional>

typedef int SmartId;
static constexpr const int InvalidLink = -1;

class CEntity
{
public:

	SmartId GetId() const { return m_id; }
	void SetId(SmartId sid) { m_id = sid; }

private:

	SmartId m_id = InvalidLink;
};

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

	template <typename... V>
	SmartId CreateEntity(V&&... args)
	{
		SmartId sid = InvalidLink;

		auto fnd = std::find(m_smartLinks.begin(), m_smartLinks.end(), InvalidLink);
		if (fnd != m_smartLinks.end())
		{
			*fnd = m_entities.size();
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

	inline T* GetEntity(SmartId sid)
	{
		if (sid >= 0 && sid < m_smartLinks.size() && m_smartLinks[sid] != InvalidLink)
		{
			return &m_entities[m_smartLinks[sid]];
		}
		return nullptr;
	}

	inline int GetNumEntities() const
	{
		return (int)m_entities.size();
	}

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