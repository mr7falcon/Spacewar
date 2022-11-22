#pragma once

#include <vector>

class CMemoryStream
{
public:

	CMemoryStream(size_t initialSize) : m_buffer(initialSize) {}

	template <typename T>
	void operator<<(const T& val)
	{
		if (m_dWriteOffset + sizeof(val) > m_buffer.size())
		{
			m_buffer.resize(2 * (m_buffer.size() + sizeof(val)));
		}
		memcpy(&m_buffer[m_dWriteOffset], &val, sizeof(val));
		m_dWriteOffset += sizeof(val);
	}

	template <typename T, typename... V>
	void Emplace(V&&... args)
	{
		if (m_dWriteOffset + sizeof(T) > m_buffer.size())
		{
			m_buffer.resize(2 * (m_buffer.size() + sizeof(T)));
		}
		new (&m_buffer[m_dWriteOffset]) T(std::forward<V>(args)...);
		m_dWriteOffset += sizeof(T);
	}

	template <typename T>
	void operator>>(T& val)
	{
		memcpy(&val, &m_buffer[m_dReadOffset], sizeof(val));
		m_dReadOffset += sizeof(val);
	}

	template <typename T>
	const T& Extract()
	{
		const T& val = (*(T*)(&m_buffer[m_dReadOffset]));
		m_dReadOffset += sizeof(T);
		return val;
	}

	void Clear() { m_dReadOffset = m_dWriteOffset = 0; }
	bool Empty() const { return m_dReadOffset >= m_dWriteOffset; }

private:

	std::vector<char> m_buffer;
	size_t m_dReadOffset = 0;
	size_t m_dWriteOffset = 0;
};