#pragma once

#include <SFML/Graphics/Transform.hpp>

enum PrimitiveType
{
	PrimitiveType_Circle = 0,
	PrimitiveType_Capsule,
	PrimitiveType_Num,
};

struct IPhysicalPrimitive 
{
	virtual PrimitiveType GetType() const = 0;
};

struct Circle : public IPhysicalPrimitive
{
	Circle(float fRad) : m_fRad(fRad) {}

	virtual PrimitiveType GetType() const override { return PrimitiveType_Circle; }

	float m_fRad = 0.f;
};

struct Capsule : public IPhysicalPrimitive
{
	Capsule(const sf::Vector2f& vDir, float fHalfHeight, float fRad)
		: m_vDir(vDir), m_fHalfHeight(fHalfHeight), m_fRad(fRad) {}

	virtual PrimitiveType GetType() const override { return PrimitiveType_Capsule; }

	sf::Vector2f m_vDir;
	float m_fHalfHeight = 0.f;
	float m_fRad = 0.f;
};

bool Intersect1(const Circle* p1, const sf::Transform& t1, const Circle* p2, const sf::Transform& t2);
bool Intersect(const Capsule* p1, const sf::Transform& t1, const Capsule* p2, const sf::Transform& t2);
bool Intersect(const Circle* p1, const sf::Transform& t1, const Capsule* p2, const sf::Transform& t2);
bool Intersect(const Capsule* p1, const sf::Transform& t1, const Circle* p2, const sf::Transform& t2);

bool (*g_intersectionTable[PrimitiveType_Num][PrimitiveType_Num])(const IPhysicalPrimitive*, const sf::Transform&, const IPhysicalPrimitive*, const sf::Transform&) =
{
	Intersect1
};