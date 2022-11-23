#pragma once

#include <vector>
#include <algorithm>
#include <functional>

typedef int SmartId;
static constexpr const int InvalidLink = -1;

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

	template <typename... V>
	SmartId CreateEntity(V&&... args)
	{
		SmartId sid = InvalidLink;

		auto fnd = std::find_if(m_smartLinks.begin(), m_smartLinks.end(),
			[this](int l) { return l == InvalidLink; });
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

		m_entities.emplace_back(std::piecewise_construct, std::make_tuple(std::forward<V>(args)...), std::make_tuple(sid));

		return sid;
	}

	void RemoveEntity(SmartId sid, bool immediate = false)
	{
		if (sid < m_smartLinks.size())
		{
			if constexpr (SafeRemove || !immediate)
			{
				UnlinkEntity(sid);
			}
			else
			{
				DeleteEntity(m_smartLinks[sid]);
			}
		}
	}

	inline T* GetEntity(SmartId sid)
	{
		if (sid != InvalidLink && m_smartLinks[sid] != InvalidLink)
		{
			return &m_entities[m_smartLinks[sid]].first;
		}
		return nullptr;
	}

	inline int GetNumEntities() const
	{
		return (int)m_entities.size();
	}

	inline void ForEachEntity(std::function<void(T&)> f, int dRange = InvalidLink)
	{
		if (dRange == InvalidLink)
		{
			dRange = (int)m_entities.size();
		}

		for (int i = 0; i < dRange; ++i)
		{
			if (m_entities[i].second != InvalidLink)
			{
				f(m_entities[i].first);
			}
		}
	}

	void CollectGarbage()
	{
		for (int i = 0; i < m_entities.size(); ++i)
		{
			if (m_entities[i].second == InvalidLink)
			{
				DeleteEntity(i);
			}
		}
	}

private:

	inline void UnlinkEntity(int sid)
	{
		m_entities[m_smartLinks[sid]].second = InvalidLink;
		m_smartLinks[sid] = InvalidLink;
	}

	inline void DeleteEntity(int idx)
	{
		std::iter_swap(m_entities.begin() + idx, m_entities.end() - 1);
		if (m_entities[idx].second != InvalidLink)
		{
			m_smartLinks[m_entities[idx].second] = idx;
		}
		m_entities.pop_back();
	}

protected:

	std::vector<std::pair<T, int>> m_entities;
	std::vector<int> m_smartLinks;

};