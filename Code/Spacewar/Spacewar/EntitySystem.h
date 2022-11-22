#pragma once

#include <vector>
#include <algorithm>
#include <functional>

typedef uint32_t SmartId;
static constexpr const uint32_t InvalidLink = (uint32_t)-1;

template <typename T, bool SafeRemove>
class CEntitySystem
{
public:

	CEntitySystem(uint32_t initialSize)
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
			[this](uint32_t l) { return l >= m_entities.size(); });
		if (fnd != m_smartLinks.end())
		{
			*fnd = m_entities.size();
			sid = (SmartId)(fnd - m_smartLinks.begin());
		}
		else
		{
			m_smartLinks.push_back(m_entities.size());
			sid = (SmartId)m_smartLinks.size() - 1;
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
		if (sid < m_smartLinks.size() && m_smartLinks[sid] < m_entities.size())
		{
			return &m_entities[m_smartLinks[sid]].first;
		}
		return nullptr;
	}

	inline uint32_t GetNumEntities() const
	{
		return (uint32_t)m_entities.size();
	}

	inline void ForEachEntity(std::function<void(T&)> f, uint32_t dRange = InvalidLink)
	{
		if (dRange > m_entities.size())
		{
			dRange = (uint32_t)m_entities.size();
		}

		for (uint32_t i = 0; i < dRange; ++i)
		{
			if (m_entities[i].second < m_smartLinks.size())
			{
				f(m_entities[i].first);
			}
		}
	}

	void CollectGarbage()
	{
		for (uint32_t i = 0; i < m_entities.size(); ++i)
		{
			if (m_entities[i].second >= m_smartLinks.size())
			{
				DeleteEntity(i);
			}
		}
	}

private:

	inline void UnlinkEntity(SmartId sid)
	{
		m_entities[m_smartLinks[sid]].second = InvalidLink;
		m_smartLinks[sid] = InvalidLink;
	}

	inline void DeleteEntity(uint32_t idx)
	{
		std::iter_swap(m_entities.begin() + idx, m_entities.end() - 1);
		if (m_entities[idx].second < m_smartLinks.size())
		{
			m_smartLinks[m_entities[idx].second] = idx;
		}
		m_entities.pop_back();
	}

protected:

	std::vector<std::pair<T, SmartId>> m_entities;
	std::vector<uint32_t> m_smartLinks;

};