#include "StdAfx.h"
#include "PhysicalPrimitive.h"
#include "MathHelpers.h"

void PhysicalPrimitive::Circle::Transform(const sf::Transform& transform)
{
	m_vOrg = transform.transformPoint(m_vOrg);
	m_fRad *= MathHelpers::GetScaleAny(transform);
}

void PhysicalPrimitive::Capsule::Transform(const sf::Transform& transform)
{
	m_vA = transform.transformPoint(m_vA);
	m_vB = transform.transformPoint(m_vB);
	m_fRad *= MathHelpers::GetScaleAny(transform);
}

inline static void CheckMinMax(float& min, float& max)
{
	if (max < min)
	{
		std::swap(min, max);
	}
}

void PhysicalPrimitive::Polygon::Transform(const sf::Transform& transform)
{
	for (auto& vertex : m_vertices)
	{
		vertex = transform.transformPoint(vertex);
	}
}

#define INTERSECTION(T1, T2) Intersection_##T1##_##T2
#define IMPLEMENT_INTERSECTION(T1, T2) static bool INTERSECTION(T1, T2)(const PhysicalPrimitive::IPrimitive* p1, const PhysicalPrimitive::IPrimitive* p2)
#define CAST_ARGS(T1, N1, T2, N2)\
	const PhysicalPrimitive::T1* N1 = static_cast<const PhysicalPrimitive::T1*>(p1);\
	const PhysicalPrimitive::T2* N2 = static_cast<const PhysicalPrimitive::T2*>(p2)

IMPLEMENT_INTERSECTION(Default, Default)
{
	return false;
}

IMPLEMENT_INTERSECTION(Circle, Circle)
{
	CAST_ARGS(Circle, c1, Circle, c2);
	
	sf::Vector2f diff = c1->m_vOrg - c2->m_vOrg;
	float dist2 = MathHelpers::DotProd(diff, diff);
	float width = c1->m_fRad + c2->m_fRad;
	return dist2 <= width * width;
}

static inline sf::Vector2f FindClosestPointOnCapsuleAxis(const PhysicalPrimitive::Capsule* pCap, const sf::Vector2f& vPt)
{
	sf::Vector2f vDir = pCap->m_vA - pCap->m_vB;
	float len = MathHelpers::GetLength(vDir);
	if (len > 0.f)
	{
		vDir /= len;
		float projLen = MathHelpers::DotProd(vDir, vPt - pCap->m_vB);
		return pCap->m_vB + vDir * std::clamp(projLen, 0.f, len);
	}
	return pCap->m_vB;
}

IMPLEMENT_INTERSECTION(Capsule, Capsule)
{
	CAST_ARGS(Capsule, c1, Capsule, c2);

	float d2a1a2 = MathHelpers::DotProd(c1->m_vA, c2->m_vA);
	float d2a1b2 = MathHelpers::DotProd(c1->m_vB, c2->m_vB);
	float d2b1a2 = MathHelpers::DotProd(c1->m_vB, c2->m_vA);
	float d2b1b2 = MathHelpers::DotProd(c1->m_vB, c2->m_vB);

	sf::Vector2f best1 = (d2b1a2 < d2a1a2 || d2b1a2 < d2a1b2 || d2b1b2 < d2a1a2 || d2b1b2 < d2a1b2) ? c1->m_vB : c1->m_vA;
	sf::Vector2f best2 = FindClosestPointOnCapsuleAxis(c2, best1);
	best1 = FindClosestPointOnCapsuleAxis(c1, best2);

	PhysicalPrimitive::Circle c1c(best1, c1->m_fRad);
	PhysicalPrimitive::Circle c2c(best2, c2->m_fRad);

	return INTERSECTION(Circle, Circle)(&c1c, &c2c);
}

IMPLEMENT_INTERSECTION(Circle, Capsule)
{
	CAST_ARGS(Circle, c1, Capsule, c2);
	sf::Vector2f proj = FindClosestPointOnCapsuleAxis(c2, c1->m_vOrg);

	PhysicalPrimitive::Circle c2c(proj, c2->m_fRad);

	return INTERSECTION(Circle, Circle)(c1, &c2c);
}

IMPLEMENT_INTERSECTION(Capsule, Circle)
{
	return INTERSECTION(Circle, Capsule)(p2, p1);
}

inline static void CalculateNormals(const std::vector<sf::Vector2f>& vertices, std::vector<sf::Vector2f>& normals)
{
	if (vertices.size() < 2)
	{
		return;
	}

	auto pushIfNotExist = [&](const sf::Vector2f& norm)
	{
		for (const auto& normal : normals)
		{
			if (normal == norm || normal == -norm)
			{
				return;
			}
		}

		normals.push_back(norm);
	};

	auto findNorm = [pushIfNotExist](const sf::Vector2f& pt1, const sf::Vector2f& pt2)
	{
		sf::Vector2f dir(pt2 - pt1);
		sf::Vector2f norm = MathHelpers::Normalize(sf::Vector2f(dir.y, -dir.x));
		pushIfNotExist(norm);
	};

	for (int i = 1; i < vertices.size(); ++i)
	{
		findNorm(vertices[i - 1], vertices[1]);
	}
	findNorm(vertices[0], vertices[vertices.size() - 1]);
}

inline static void CalculateMinMaxProjects(const std::vector<sf::Vector2f>& vertices, const sf::Vector2f& normal, float& min, float& max)
{
	min = FLT_MAX;
	max = FLT_MIN;

	for (const auto& vertex : vertices)
	{
		float proj = MathHelpers::DotProd(normal, vertex);
		
		if (proj < min)
		{
			min = proj;
		}
		
		if (proj > max)
		{
			max = proj;
		}
	}
}

IMPLEMENT_INTERSECTION(Polygon, Polygon)
{
	CAST_ARGS(Polygon, pg1, Polygon, pg2);
	
	std::vector<sf::Vector2f> normals;
	CalculateNormals(pg1->m_vertices, normals);
	CalculateNormals(pg2->m_vertices, normals);

	for (const auto& normal : normals)
	{
		float min1, min2, max1, max2;
		CalculateMinMaxProjects(pg1->m_vertices, normal, min1, max1);
		CalculateMinMaxProjects(pg2->m_vertices, normal, min2, max2);
		
		if (min1 > max2 || min2 > max1)
		{
			return false;
		}
	}

	return true;
}

IMPLEMENT_INTERSECTION(Polygon, Circle)
{
	CAST_ARGS(Polygon, pg, Circle, c);

	std::vector<sf::Vector2f> normals;
	CalculateNormals(pg->m_vertices, normals);

	for (const auto& normal : normals)
	{
		float min1, max1;
		CalculateMinMaxProjects(pg->m_vertices, normal, min1, max1);
		
		float centerProj = MathHelpers::DotProd(normal, c->m_vOrg);
		float min2 = centerProj - c->m_fRad;
		float max2 = centerProj + c->m_fRad;

		if (min1 > max2 || min2 > max1)
		{
			return false;
		}
	}

	return true;
}

IMPLEMENT_INTERSECTION(Polygon, Capsule)
{
	CAST_ARGS(Polygon, pg, Capsule, c);

	sf::Vector2f capsuleAxis = MathHelpers::Normalize(c->m_vA - c->m_vB);
	sf::Vector2f capsuleNorm(capsuleAxis.y, -capsuleAxis.x);

	std::vector<sf::Vector2f> capsuleVertices;
	capsuleVertices.push_back(c->m_vB + c->m_fRad * capsuleNorm);
	capsuleVertices.push_back(c->m_vB + c->m_fRad * -capsuleNorm);
	capsuleVertices.push_back(c->m_vA + c->m_fRad * capsuleNorm);
	capsuleVertices.push_back(c->m_vA + c->m_fRad * -capsuleNorm);

	PhysicalPrimitive::Polygon cpg(capsuleVertices);
	PhysicalPrimitive::Circle cc1(c->m_vA, c->m_fRad);
	PhysicalPrimitive::Circle cc2(c->m_vB, c->m_fRad);

	return INTERSECTION(Polygon, Circle)(pg, &cc1) || INTERSECTION(Polygon, Circle)(pg, &cc2) || INTERSECTION(Polygon, Polygon)(pg, &cpg);
}

IMPLEMENT_INTERSECTION(Circle, Polygon)
{
	return INTERSECTION(Polygon, Circle)(p2, p1);
}

IMPLEMENT_INTERSECTION(Capsule, Polygon)
{
	return INTERSECTION(Polygon, Capsule)(p2, p1);
}

Intersection g_intersectionsTable[PhysicalPrimitive::EPrimitiveType_Num][PhysicalPrimitive::EPrimitiveType_Num] =
{
	Intersection_Circle_Circle, Intersection_Circle_Capsule, Intersection_Circle_Polygon,
	Intersection_Capsule_Circle, Intersection_Capsule_Capsule, Intersection_Capsule_Polygon,
	Intersection_Polygon_Circle, Intersection_Polygon_Capsule, Intersection_Polygon_Polygon
};