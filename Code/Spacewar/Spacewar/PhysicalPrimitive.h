#pragma once

#include <SFML/Graphics/Transform.hpp>

namespace PhysicalPrimitive
{
	enum EPrimitiveType
	{
		EPrimitiveType_Circle = 0,
		EPrimitiveType_Capsule,
		EPrimitiveType_Rectangle,
		EPrimitiveType_Num,
	};

	struct Primitive
	{
		Primitive(const sf::Vector2f& vOrg = sf::Vector2f()) : m_vOrg(vOrg) {}

		virtual EPrimitiveType GetType() const = 0;
		virtual void Transform(const sf::Transform& transform) = 0;

		sf::Vector2f m_vOrg;
	};

	struct Circle : public Primitive
	{
		Circle(float fRad) : m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Circle; }
		virtual void Transform(const sf::Transform& transform) override;

		float m_fRad = 0.f;
	};

	struct Capsule : public Primitive
	{
		Capsule(const sf::Vector2f& vDir, float fHalfHeight, float fRad)
			: m_vDir(vDir), m_fHalfHeight(fHalfHeight), m_fRad(fRad) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Capsule; }
		virtual void Transform(const sf::Transform& transform) override;

		sf::Vector2f m_vDir;
		float m_fHalfHeight = 0.f;
		float m_fRad = 0.f;
	};

	struct Rectangle : public Primitive
	{
		Rectangle(float fHalfWidth, float fHalfHeight)
			: m_fHalfWidth(fHalfWidth), m_fHalfHeight(fHalfHeight) {}

		virtual EPrimitiveType GetType() const override { return EPrimitiveType_Rectangle; }
		virtual void Transform(const sf::Transform& transform) override;

		float m_fHalfWidth = 0.f;
		float m_fHalfHeight = 0.f;
	};
}

typedef bool(*Intersection)(const PhysicalPrimitive::Primitive*, const PhysicalPrimitive::Primitive*);

extern Intersection g_intersectionsTable[PhysicalPrimitive::EPrimitiveType_Num][PhysicalPrimitive::EPrimitiveType_Num];