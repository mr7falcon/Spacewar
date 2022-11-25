#include <iostream>

#include "EntityConfiguration.h"
#include "Game.h"

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
		}
	}
	else
	{
		std::cout << "Invalid physics type " << node.attribute("type").value() << " specified for entity " << name << std::endl;
	}
}

CEntityConfiguration::CEntityConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		std::cout << "Entities configuration loading failed: " << res.description() << std::endl;
		return;
	}

	auto root = doc.child("Entities");
	if (!root)
	{
		std::cout << "Invalid root element in entities configuration" << std::endl;
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
				std::cout << "Entity with name " << name << " already exists" << std::endl;
				continue;
			}

			SEntityClass entityClass;

			auto physics = iter->child("Physics");
			if (physics)
			{
				ParsePhysics(physics, name, entityClass);
			}
			
			auto render = iter->child("Render");
			if (render)
			{
				entityClass.texture = CGame::Get().GetResourceSystem()->GetTextureId(render.attribute("texture").value());
				if (entityClass.texture < 0)
				{
					std::cout << "Invalid texture " << render.attribute("texture").value() << " specified for entity " << name << std::endl;
				}
				entityClass.vScale = render.attribute("scale").as_vector();
			}

			m_entityClasses[std::move(name)] = std::move(entityClass);
		}
	}
}

const CEntityConfiguration::SEntityClass* CEntityConfiguration::GetEntityClass(const std::string& name) const
{
	auto fnd = m_entityClasses.find(name);
	return fnd != m_entityClasses.end() ? &fnd->second : nullptr;
}