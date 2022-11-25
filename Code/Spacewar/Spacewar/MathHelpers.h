#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\Transform.hpp>

namespace MathHelpers
{
	static inline float DotProd(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	static inline float GetLength(const sf::Vector2f& v)
	{
		return sqrtf(DotProd(v, v));
	}

	static inline sf::Vector2f Normalize(const sf::Vector2f& v)
	{
		float len = GetLength(v);
		return len > 0.f ? v / len : sf::Vector2f();
	}

	static inline float GetScaleAny(const sf::Transform& t)
	{
		const float* matrix = t.getMatrix();
		sf::Vector2f col(matrix[0], matrix[4]);
		return GetLength(col);
	}
}