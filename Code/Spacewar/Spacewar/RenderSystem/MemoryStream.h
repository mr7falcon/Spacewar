#pragma once

#include "StdAfx.h"

/**
 * @class CMemoryStream
 * This class provides effective methods for the storing
 * and accessing structures from the coherent memory buffer.
 */
class CMemoryStream
{
public:

	CMemoryStream(size_t initialSize) : m_buffer(initialSize) {}
	
	// Check if the buffer is needed to be expanded
	template <typename T>
	inline void CheckSize()
	{
		if (m_dWriteOffset + sizeof(T) > m_buffer.size())
		{
			m_buffer.resize(2 * (m_buffer.size() + sizeof(T)));
		}
	}

	// Copy the value from the variable into the buffer
	template <typename T>
	void operator<<(const T& val)
	{
		CheckSize<T>();
		memcpy(&m_buffer[m_dWriteOffset], &val, sizeof(val));
		m_dWriteOffset += sizeof(val);
	}

	// Copy the value from the buffer into the variable
	template <typename T>
	void operator>>(T& val)
	{
		memcpy(&val, &m_buffer[m_dReadOffset], sizeof(val));
		m_dReadOffset += sizeof(val);
	}

	// Construct the value in the buffer in-place
	template <typename T, typename... V>
	void Emplace(V&&... args)
	{
		CheckSize<T>();
		new (&m_buffer[m_dWriteOffset]) T(std::forward<V>(args)...);
		m_dWriteOffset += sizeof(T);
	}

	// Get the reference on the value from the buffer
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