#pragma once

#include "Controllers/Controller.h"
#include "EntitySystem.h"

#include <functional>
#include <filesystem>
#include <string>
#include <map>
#include <memory>

class ILayout;

/**
 * @class CUISystem
 * Class to manage layouts. Also contains functions, available to invoke from the layouts.
 * For more information see CLayout.
 */
class CUISystem
{
public:

	CUISystem(const std::filesystem::path& root);
	~CUISystem();
	CUISystem(const CUISystem&) = delete;

	void Update();

	// Function to load the root hierarchical layout
	void LoadGlobalLayout(const std::string& path);

	// Function to load the player sublayouts of the current layout.
	// Player sublayouts are defined by the player entity SmartId.
	void ReloadPlayerLayout(const std::string& id, SmartId playerId);
	
	/**
	 * @function InvokeFunction
	 * Invoke function from the registered ones by it's name.
	 * 
	 * @template param R - type of the function return value
	 * @template param V - types of the function arguments
	 * @param name - function name to call
	 * @param args - tuple with the function arguments to apply
	 */
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

	std::shared_ptr<CController> m_pController;
};