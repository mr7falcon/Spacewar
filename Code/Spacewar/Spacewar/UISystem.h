#pragma once

#include <functional>
#include <filesystem>
#include <string>
#include <map>
#include <memory>
#include <iostream>

#include "EntitySystem.h"
#include "IController.h"

class ILayout;

class CUISystem
{
public:

	CUISystem(const std::filesystem::path& root);
	~CUISystem();
	CUISystem(const CUISystem&) = delete;

	void Update();

	void LoadGlobalLayout(const std::string& path);
	void ReloadPlayerLayout(const std::string& id, SmartId playerId);
	void ResetLayout();

	template <typename R, typename... V>
	R InvokeFunction(const std::string& name, const std::tuple<V...>& args)
	{
		auto fnd = m_functions.find(name);
		if (fnd != m_functions.end())
		{
			auto f = (R(*)(V...))fnd->second;
			return std::apply(f, args);
		}
		return R();
	}

private:

	void LoadGlobalLayoutInternal();

private:

	const std::filesystem::path m_root;

	std::map<std::string, void*> m_functions;

	std::unique_ptr<ILayout> m_pLayout;
	std::string m_newLayout;

	std::shared_ptr<IController> m_pController;
};