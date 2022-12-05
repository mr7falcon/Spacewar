#pragma once

#include <vector>

#include <SFML/Graphics/Transform.hpp>

namespace PhysicalPrimitive
{
	enum EPrimitiveType
	{
		EPrimitiveType_Circle = 0,
		EPrimitiveType_Capsule,
		EPrimitiveType_Polygon,
		EPrimitiveType_Num,
	};

	/**
	 * @interface IPrimitive
	 * This struct provides the functions to distinguish the primitives
	 * between themselves and to change their properties according to the new entity transform.
	 */
	struct IPrimitive
	{
		virtual EPrimitiveType GetType() const = 0;
		virtual void Transform(const sf::Transform& transform) = 0;
	};

	// Circle is defined by the origin and radius.
	struct Circle : public IPrimitive
	{
		Circle(const sf::Vector2f& vOrg, float fRad) : m_vOrg(vOrg), m_fRad(fRad) {}
		Circle(float fRad) : m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Circle; }
		virtual void Transform(const sf::Transform& transform) override;

		sf::Vector2f m_vOrg;
		float m_fRad = 0.f;
	};

	// Capsule is defined by the two axes points and radius around the axis
	struct Capsule : public IPrimitive
	{
		Capsule(const sf::Vector2f& vA, const sf::Vector2f& vB, float fRad)
			: m_vA(vA), m_vB(vB), m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Capsule; }
		virtual void Transform(const sf::Transform& transform) override;

		sf::Vector2f m_vA;
		sf::Vector2f m_vB;
		float m_fRad = 0.f;
	};

	// Polygon is defined by the vertices positions
	struct Polygon : public IPrimitive
	{
		Polygon(const std::vector<sf::Vector2f>& vertices) : m_vertices(vertices) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Polygon; }
		virtual void Transform(const sf::Transform& transform) override;

		std::vector<sf::Vector2f> m_vertices;
	};
}

typedef bool(*Intersection)(const PhysicalPrimitive::IPrimitive*, const PhysicalPrimitive::IPrimitive*);

extern Intersection g_intersectionsTable[PhysicalPrimitive::EPrimitiveType_Num][PhysicalPrimitive::EPrimitiveType_Num];