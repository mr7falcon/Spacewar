#pragma once

#include "StdAfx.h"

/**
 * Providen by SFML classes have poor public functionality, but reimplementation of them
 * is an unnecessary work I suppose, so there are some additional functions which are
 * used in the game code.
 */
namespace MathHelpers
{
	inline float DotProd(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	inline float GetLength(const sf::Vector2f& v)
	{
		return sqrtf(DotProd(v, v));
	}

	inline sf::Vector2f Normalize(const sf::Vector2f& v)
	{
		float len = GetLength(v);
		return len > 0.f ? v / len : sf::Vector2f();
	}

	inline float GetScaleAny(const sf::Transform& t)
	{
		const float* matrix = t.getMatrix();
		sf::Vector2f col(matrix[0], matrix[4]);
		return GetLength(col);
	}

	inline sf::Vector2f RotateVector(const sf::Vector2f& v, float a)
	{
		float sina = sinf(a);
		float cosa = cosf(a);
		return sf::Vector2f(cosa * v.x - sina * v.y, sina * v.x + cosa * v.y);
	}
}