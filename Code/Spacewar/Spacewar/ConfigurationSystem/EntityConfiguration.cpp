#include "StdAfx.h"
#include "EntityConfiguration.h"
#include "Game.h"
#include "ResourceSystem.h"

inline static PhysicalPrimitive::EPrimitiveType ParsePrimitiveType(const std::string& type)
{
	if (type == "Circle")
	{
		return PhysicalPrimitive::EPrimitiveType_Circle;
	}
	else if (type == "Capsule")
	{
		return PhysicalPrimitive::EPrimitiveType_Capsule;
	}
	else if (type == "Polygon")
	{
		return PhysicalPrimitive::EPrimitiveType_Polygon;
	}
	return PhysicalPrimitive::EPrimitiveType_Num;
}

void CEntityConfiguration::ParsePhysics(const pugi::xml_node& node, const std::string& name, CEntityConfiguration::SEntityClass& entityClass)
{
	entityClass.physicsType = ParsePrimitiveType(node.attribute("type").value());
	if (entityClass.physicsType != PhysicalPrimitive::EPrimitiveType_Num)
	{
		switch (entityClass.physicsType)
		{
		case PhysicalPrimitive::EPrimitiveType_Circle:
		{
			float fRad = node.attribute("radius").as_float();
			entityClass.pPhysics = std::make_unique<CEntityConfiguration::CircleConfig>(fRad);
		}
		break;
		case PhysicalPrimitive::EPrimitiveType_Capsule:
		{
			float fRad = node.attribute("radius").as_float();
			float fHalfHeight = node.attribute("halfheight").as_float();
			sf::Vector2f vAxis = node.attribute("axis").as_vector();
			entityClass.pPhysics = std::make_unique<CEntityConfiguration::CapsuleConfig>(fRad, fHalfHeight, std::move(vAxis));
		}
		break;
		case PhysicalPrimitive::EPrimitiveType_Polygon:
		{
			std::vector<sf::Vector2f> vertices;
			for (auto iter = node.begin(); iter != node.end(); ++iter)
			{
				vertices.push_back(iter->attribute("coords").as_vector());
			}
			entityClass.pPhysics = std::make_unique<CEntityConfiguration::PolygonConfig>(std::move(vertices));
		}
		break;
		}
	}
	else
	{
		Log("Invalid physics type ", node.attribute("type").value(), " specified for entity ", name);
	}
}

CLogicalEntity::SRenderSlot CEntityConfiguration::ParseRender(const pugi::xml_node& node, const std::string& name)
{
	CLogicalEntity::SRenderSlot slot;
	
	slot.textureId = CGame::Get().GetResourceSystem()->GetTextureId(node.attribute("texture").value());
	if (slot.textureId < 0)
	{
		Log("Invalid texture ", node.attribute("texture").value(), " specified for entity ", name);
	}

	slot.vSize = node.attribute("size").as_vector();
	return slot;
}

CEntityConfiguration::CEntityConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		Log("Entities configuration loading failed: ", res.description());
		return;
	}

	auto root = doc.child("Entities");
	if (!root)
	{
		Log("Invalid root element in entities configuration");
		return;
	}

	for (auto iter = root.begin(); iter != root.end(); ++iter)
	{
		std::string name = iter->attribute("name").value();
		if (!name.empty())
		{
			auto fnd = m_entityClasses.find(name);
			if (fnd != m_entityClasses.end())
			{
				Log("Entity with name ", name, " already exists");
				continue;
			}

			SEntityClass entityClass;

			auto physics = iter->child("Physics");
			if (physics)
			{
				ParsePhysics(physics, name, entityClass);
			}

			auto render = iter->children("Render");
			for (auto node = render.begin(); node != render.end(); ++node)
			{
				entityClass.renderSlots.push_back(ParseRender(*node, name));
			}

			m_entityClasses.emplace(std::move(name), std::move(entityClass));
		}
	}
}

const CEntityConfiguration::SEntityClass* CEntityConfiguration::GetEntityClass(const std::string& name) const
{
	auto fnd = m_entityClasses.find(name);
	return fnd != m_entityClasses.end() ? &fnd->second : nullptr;
}