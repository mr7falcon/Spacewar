#pragma once

#include <filesystem>
#include <map>

#include <SFML/System/Vector2.hpp>
#include <pugixml.hpp>

#include "PhysicalPrimitive.h"
#include "ResourceSystem.h"

class CEntityConfiguration
{
public:

	struct IPrimitiveConfig {};
	
	struct CircleConfig : public IPrimitiveConfig
	{
		CircleConfig(float _fRad) : fRad(_fRad) {}

		float fRad = 0.f;
	};

	struct CapsuleConfig : public IPrimitiveConfig
	{
		CapsuleConfig(float _fRad, float _fHalfHeight, sf::Vector2f&& _vAxis)
			: fRad(_fRad), fHalfHeight(_fHalfHeight), vAxis(_vAxis) {}

		float fRad = 0.f;
		float fHalfHeight = 0.f;
		sf::Vector2f vAxis;
	};

	struct SEntityClass
	{
		int texture = InvalidResourceId;
		sf::Vector2f vScale;
		PhysicalPrimitive::EPrimitiveType physicsType = PhysicalPrimitive::EPrimitiveType_Num;
		std::unique_ptr<IPrimitiveConfig> pPhysics;
	};

	CEntityConfiguration(const std::filesystem::path& path);
	CEntityConfiguration(const CEntityConfiguration&) = delete;

	const SEntityClass* GetEntityClass(const std::string& name) const;

private:

	void ParsePhysics(const pugi::xml_node& node, const std::string& name, SEntityClass& entityClass);

private:

	std::map<std::string, SEntityClass> m_entityClasses;

};