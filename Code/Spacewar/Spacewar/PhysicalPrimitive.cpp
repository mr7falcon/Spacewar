#include "PhysicalPrimitive.h"

static inline sf::Vector2f GetPos(const sf::Transform& t)
{
	return sf::Vector2f(t.getMatrix()[2], t.getMatrix()[5]);
}

static inline float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

static inline sf::Vector2f TransformVector(const sf::Vector2f& v, const sf::Transform& t)
{
	const float* matrix = t.getMatrix();
	return sf::Vector2f(matrix[0] * v.x + matrix[1] * v.y, matrix[3] * v.x + matrix[4] * v.y);		// rotate without translation and scaling
}

void PhysicalPrimitive::Circle::Transform(const sf::Transform& transform)
{
	m_vOrg = transform.transformPoint(m_vOrg);
	m_fRad *= transform.getMatrix()[6];				// scale by x
}

void PhysicalPrimitive::Capsule::Transform(const sf::Transform& transform)
{
	m_vOrg = transform.transformPoint(m_vOrg);
	m_fRad *= transform.getMatrix()[6];				// scale by x
	m_fHalfHeight *= transform.getMatrix()[7];		// scale by y
	m_vDir = TransformVector(m_vDir, transform);
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
	float dist2 = Dot(diff, diff);
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
	float projLen = Dot(pCap->m_vDir, vPt - pCap->m_vOrg);
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

	float d2a1a2 = Dot(a1, a2);
	float d2a1b2 = Dot(a1, b2);
	float d2b1a2 = Dot(b1, a2);
	float d2b1b2 = Dot(b1, b2);

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