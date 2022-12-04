#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <functional>
#include <filesystem>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include "Windows.h"
#endif

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>

#include <pugixml.hpp>

inline void Log()
{
	std::cout << std::endl;
}

template <typename T, typename... V>
inline void Log(const T& first, const V&... rest)
{
	std::cout << first;
	Log(rest...);
}