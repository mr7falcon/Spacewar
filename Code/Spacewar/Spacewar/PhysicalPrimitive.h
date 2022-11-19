#pragma once

#include <SFML/Graphics/Transform.hpp>

namespace PhysicalPrimitive
{
	enum EPrimitiveType
	{
		EPrimitiveType_Circle = 0,
		EPrimitiveType_Capsule,
		EPrimitiveType_Num,
	};

	struct IPhysicalPrimitive
	{
		virtual EPrimitiveType GetType() const = 0;
		virtual void Transform(const sf::Transform& transform) = 0;
	};

	struct Circle : public IPhysicalPrimitive
	{
		Circle(const sf::Vector2f&& vOrg, float fRad) : m_vOrg(vOrg), m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Circle; }
		virtual void Transform(const sf::Transform& transform) override;

		sf::Vector2f m_vOrg;
		float m_fRad = 0.f;
	};

	struct Capsule : public IPhysicalPrimitive
	{
		Capsule(const sf::Vector2f&& vOrg, const sf::Vector2f&& vDir, float fHalfHeight, float fRad)
			: m_vOrg(vOrg), m_vDir(vDir), m_fHalfHeight(fHalfHeight), m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Capsule; }
		virtual void Transform(const sf::Transform& transform) override;

		sf::Vector2f m_vOrg;
		sf::Vector2f m_vDir;
		float m_fHalfHeight = 0.f;
		float m_fRad = 0.f;
	};
}

typedef bool(*Intersection)(const PhysicalPrimitive::IPhysicalPrimitive*, const PhysicalPrimitive::IPhysicalPrimitive*);

extern Intersection g_intersectionsTable[PhysicalPrimitive::EPrimitiveType_Num][PhysicalPrimitive::EPrimitiveType_Num];