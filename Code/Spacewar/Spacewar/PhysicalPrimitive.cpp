#include "PhysicalPrimitive.h"
#include "MathHelpers.h"

void PhysicalPrimitive::Circle::Transform(const sf::Transform& transform)
{
	m_vOrg = transform.transformPoint(m_vOrg);
	m_fRad *= MathHelpers::GetScaleAny(transform);
}

void PhysicalPrimitive::Capsule::Transform(const sf::Transform& transform)
{
	m_vOrg = transform.transformPoint(m_vOrg);
	float scale = MathHelpers::GetScaleAny(transform);
	m_fRad *= scale;
	m_fHalfHeight *= scale;
	m_vDir = MathHelpers::Normalize(transform.transformPoint(m_vDir) - m_vOrg);
}

#define INTERSECTION(T1, T2) Intersection_##T1##_##T2
#define IMPLEMENT_INTERSECTION(T1, T2) static bool INTERSECTION(T1, T2)(const PhysicalPrimitive::Primitive* p1, const PhysicalPrimitive::Primitive* p2)
#define CAST_ARGS(T1, N1, T2, N2)\
	const PhysicalPrimitive::T1* N1 = static_cast<const PhysicalPrimitive::T1*>(p1);\
	const PhysicalPrimitive::T2* N2 = static_cast<const PhysicalPrimitive::T2*>(p2)

IMPLEMENT_INTERSECTION(Default, Default)
{
	return false;
}

static inline bool SpheresIntersection(const sf::Vector2f& vOrg1, float fRad1, const sf::Vector2f& vOrg2, float fRad2)
{
	sf::Vector2f diff = vOrg1 - vOrg2;
	float dist2 = MathHelpers::DotProd(diff, diff);
	float width = fRad1 + fRad2;
	return dist2 <= width * width;
}

IMPLEMENT_INTERSECTION(Circle, Circle)
{
	CAST_ARGS(Circle, c1, Circle, c2);
	return SpheresIntersection(c1->m_vOrg, c1->m_fRad, c2->m_vOrg, c2->m_fRad);
}

static inline sf::Vector2f FindClosestPointOnCapsuleAxis(const PhysicalPrimitive::Capsule* pCap, const sf::Vector2f& vPt)
{
	float projLen = MathHelpers::DotProd(pCap->m_vDir, vPt - pCap->m_vOrg);
	return pCap->m_vOrg + pCap->m_vDir * std::clamp(projLen, -pCap->m_fHalfHeight, pCap->m_fHalfHeight);
}

IMPLEMENT_INTERSECTION(Capsule, Capsule)
{
	CAST_ARGS(Capsule, c1, Capsule, c2);

	sf::Vector2f ax1 = c1->m_vDir * c1->m_fHalfHeight;
	sf::Vector2f a1 = c1->m_vOrg + ax1;
	sf::Vector2f b1 = c1->m_vOrg - ax1;

	sf::Vector2f ax2 = c2->m_vDir * c2->m_fHalfHeight;
	sf::Vector2f a2 = c2->m_vOrg + ax2;
	sf::Vector2f b2 = c2->m_vOrg - ax2;

	float d2a1a2 = MathHelpers::DotProd(a1, a2);
	float d2a1b2 = MathHelpers::DotProd(a1, b2);
	float d2b1a2 = MathHelpers::DotProd(b1, a2);
	float d2b1b2 = MathHelpers::DotProd(b1, b2);

	sf::Vector2f best1 = (d2b1a2 < d2a1a2 || d2b1a2 < d2a1b2 || d2b1b2 < d2a1a2 || d2b1b2 < d2a1b2) ? b1 : a1;
	sf::Vector2f best2 = FindClosestPointOnCapsuleAxis(c2, best1);
	best1 = FindClosestPointOnCapsuleAxis(c1, best2);

	return SpheresIntersection(best1, c1->m_fRad, best2, c2->m_fRad);
}

IMPLEMENT_INTERSECTION(Circle, Capsule)
{
	CAST_ARGS(Circle, c1, Capsule, c2);
	sf::Vector2f proj = FindClosestPointOnCapsuleAxis(c2, c1->m_vOrg);
	return SpheresIntersection(c1->m_vOrg, c1->m_fRad, proj, c2->m_fRad);
}

IMPLEMENT_INTERSECTION(Capsule, Circle)
{
	return INTERSECTION(Circle, Capsule)(p2, p1);
}

Intersection g_intersectionsTable[PhysicalPrimitive::EPrimitiveType_Num][PhysicalPrimitive::EPrimitiveType_Num] =
{
	Intersection_Circle_Circle, Intersection_Circle_Capsule,
	Intersection_Capsule_Circle, Intersection_Capsule_Capsule
};