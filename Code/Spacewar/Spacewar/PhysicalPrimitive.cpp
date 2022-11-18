#include "PhysicalPrimitive.h"

static inline sf::Vector2f GetPos(const sf::Transform& t)
{
	return sf::Vector2f(t.getMatrix()[2], t.getMatrix()[5]);
}

static inline float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

bool Intersect(const Circle* p1, const sf::Transform& t1, const Circle* p2, const sf::Transform& t2)
{
	float dist2 = Dot(GetPos(t1), GetPos(t2));
	return dist2 <= (p1->m_fRad + p2->m_fRad) * (p1->m_fRad + p2->m_fRad);
}

bool Intersect(const Capsule* p1, const sf::Transform& t1, const Capsule* p2, const sf::Transform& t2)
{
	return false;
}

bool Intersect(const Circle* p1, const sf::Transform& t1, const Capsule* p2, const sf::Transform& t2)
{
	return false;
}

bool Intersect(const Capsule* p1, const sf::Transform& t1, const Circle* p2, const sf::Transform& t2)
{
	return Intersect(p2, t2, p1, t1);
}