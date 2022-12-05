#pragma once

#include "StdAfx.h"
#include "Game.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/RenderProxy.h"
#include "ConfigurationSystem/ConfigurationSystem.h"
#include "Controllers/Controller.h"
#include "ResourceSystem.h"
#include "UISystem.h"

#include <optional>

/**
 * @interface ILayout
 * Interface providing the base Layout functional.
 * Layout is a set of tools to display, update and manage text and icons
 * which currently used in the game User Interface. Layout can store any
 * sublayouts which are full-fledged subsystems with their own entities
 * and which can be managed by different controllers.
 */
class ILayout : public IControllerEventListener
{
public:

	virtual ~ILayout() = default;

	/**
	* @function Unload
	* Remove all the layout entities and updaters. Unlink the controller.
	*/
	virtual void Unload() = 0;

	/**
	* @function SetController
	* Link the layout to the controller which it will be managed by.
	*
	* @param pController - pointer to the controller.
	*/
	virtual void SetController(const std::weak_ptr<CController>& pController) = 0;

	/**
	* @function IsLoaded
	* Check if the layout is loaded or not.
	*
	* @return True if the layout is loaded, false otherwise
	*/
	virtual bool IsLoaded() const = 0;

	/**
	* @function IsSublayoutLoaded
	* Check if the sublayout is loaded or not.
	*
	* @param id - identifier of the sublayout.
	* @return True if the sublayout with this id exists and is loaded, false otherwise.
	*/
	virtual bool IsSubLayoutLoaded(const std::string& id) const = 0;

	/**
	* @function Update
	* Update the layout items values, the controller and the sublayouts.
	*/
	virtual void Update() = 0;

	/**
	* @function OnControllerEvent
	* Inherited method wich is called when the controller has some new events to handle.
	* Trigger the current active item bindings corresponding to the event.
	* For more information see CController.
	*
	* @param evt - event code.
	*/
	virtual void OnControllerEvent(EControllerEvent evt) override = 0;
	
	/**
	* @function OnTextEntered
	* Inherited method wich is called when the controller has some new text to handle.
	* Change the entered layout text. For more information see CController.
	*
	* @param chr - entered character code.
	*/
	virtual void OnTextEntered(char chr) override = 0;

	/**
	* @function ActivateItem
	* Make an item with the specified id active. Active items have a specific style
	* and only the active item bindings is triggered by events.
	*
	* @param id - identifier of the item to activate.
	*/
	virtual void ActivateItem(const std::string& id) = 0;

	/**
	* @function SetWriteText
	* Make the layout remember the entered text if the bWrite parameter is true.
	* Disable the entered text remembering otherwise.
	*
	* @param bWrite - enable or disable the entered text remembering.
	*/
	virtual void SetWriteText(bool bWrite) = 0;

	/**
	* @function GetText
	* Get the current remembered text by the layout.
	*
	* @return The current remembered text.
	*/
	virtual const std::string& GetText() const = 0;
};

/**
 * @class CLayout
 * Class to display some text and icons. All the items are loaded from the .xml file
 * and created in render entities (see CRenderEntity). Besides, items can have bindings
 * and updaters. Bindings are themethods which are triggered on some controller inputs.
 * Updaters are the methods called each frame to update the corresponding item values.
 * After all, layouts have the sublayouts, which have the same functional features.
 * Each layout positioning is defined by it's origin offset with respect to the base layout,
 * which has a zero origin.
 * 
 * @template params V - types of the arguments which will be passed in some bindings or
 * updaters. The arguments is set in the Load method.
 */
template <typename... V>
class CLayout : public ILayout
{
public:

	CLayout(const std::string& path, const sf::Vector2f& vOrg)
		: m_path(path), m_vOrg(vOrg) {}

	~CLayout()
	{
		Unload();
	}

	struct SActiveStyle
	{
		sf::Color color = sf::Color::White;
		uint32_t style = sf::Text::Regular;
		unsigned int size = 0;
	};

public:

	/**
	* @function Load
	* Create items, bindings, updaters and sublayouts from the xml description.
	* Besides, this function creates the tuple of arguments which can be applied
	* in some functions invoked from this layout.
	*
	* @param base - base layouts folder, which the current layout will be loaded from.
	* @params args - arguments to pass into some updaters and bindings.
	*/
	void Load(const std::filesystem::path& base, const V&... args)
	{
		m_pArguments = std::make_unique<std::tuple<V...>>(args...);

		if (IsLoaded())
		{
			return;
		}

		pugi::xml_document doc;

		auto res = doc.load_file((base / m_path).c_str());
		if (!res)
		{
			Log("Failed to load layout: ", res.description());
			return;
		}

		auto root = doc.child("Layout");
		if (!root)
		{
			Log("Invalid root element in layout");
			return;
		}

		m_activeStyle.color = CConfigurationSystem::ParseColor(root.attribute("activeColor").value());
		m_activeStyle.style = ParseStyle(root.attribute("activeStyle").value());
		m_activeStyle.size = root.attribute("activeCharacterSize").as_uint();

		m_bWriteText = root.attribute("enableText").as_bool();

		for (auto iter = root.begin(); iter != root.end(); ++iter)
		{
			std::string tag = iter->name();
			if (tag == "Text")
			{
				LoadTextItem(*iter);
			}
			else if (tag == "Icon")
			{
				LoadIconItem(*iter);
			}
			else if (tag == "PlayerLayout")
			{
				LoadSubLayout<SmartId>(*iter);
			}
		}

		std::string activeItem = root.attribute("activeItem").value();
		if (!activeItem.empty())
		{
			ActivateItem(activeItem);
		}
	}

	virtual void Unload() override
	{
		for (const auto& [id, sid] : m_items)
		{
			CGame::Get().GetRenderSystem()->RemoveEntity(sid);
		}
		
		m_items.clear();
		m_actionBindings.clear();
		m_updateBindings.clear();
		m_subLayouts.clear();

		if (auto pController = m_pController.lock())
		{
			pController->UnregisterEventListener(this);
		}
	}

	virtual bool IsLoaded() const override
	{
		return !m_items.empty();
	}

	virtual void SetController(const std::weak_ptr<CController>& pController) override
	{
		if (auto pOldController = m_pController.lock())
		{
			pOldController->UnregisterEventListener(this);
		}
		m_pController = pController;
		if (auto pNewController = m_pController.lock())
		{
			pNewController->RegisterEventListener(this);
		}
	}

	/**
	* @function ActivateSubLayout
	* Load the layout with the specified id and provide it arguments to apply to some functions.
	*
	* @param id - identifier of the corresponding sublayout.
	* @params root - base folder for the corresponding sublayout to load.
	* @params args - arguments to pass into some sublayout's updaters and bindings.
	*/
	template <typename... T>
	ILayout* ActivateSubLayout(const std::string& id, const std::filesystem::path& root, const T&... args)
	{
		auto fnd = m_subLayouts.find(id);
		if (fnd != m_subLayouts.end())
		{
			if (auto pLayout = static_cast<CLayout<T...>*>(fnd->second.get()))
			{
				pLayout->Load(root, args...);
				return pLayout;
			}
		}
		return nullptr;
	}

	virtual bool IsSubLayoutLoaded(const std::string& id) const override
	{
		auto fnd = m_subLayouts.find(id);
		return (fnd != m_subLayouts.end() && fnd->second) ? fnd->second->IsLoaded() : false;
	}

	virtual void Update() override
	{
		if (auto pController = m_pController.lock())
		{
			pController->Update();
		}

		if (m_pArguments)
		{
			for (const auto& [sid, bind] : m_updateBindings)
			{
				std::string value = InvokeFunction<std::string>(bind);
				CGame::Get().GetRenderProxy()->OnCommand<RenderCommand::SetTextCommand>(sid, std::move(value));
			}
		}

		for (auto& [id, pLayout] : m_subLayouts)
		{
			if (pLayout)
			{
				pLayout->Update();
			}
		}
	}

	virtual void OnControllerEvent(EControllerEvent evt) override
	{
		auto fnd = m_actionBindings.find(m_activeItem);
		if (fnd != m_actionBindings.end())
		{
			for (const SActionBinding& bind : fnd->second)
			{
				if (bind.evt == evt)
				{
					InvokeFunction<void>(bind.function, bind.argument);
				}
			}
		}
	}

	virtual void OnTextEntered(char chr) override
	{
		if (m_bWriteText)
		{
			if (chr == '\b' && !m_text.empty())
			{
				m_text.pop_back();
			}
			else
			{
				m_text += chr;
			}
		}
	}

	inline SActiveStyle GetActiveStyle(SmartId sid)
	{
		if (CRenderEntity* pEntity = CGame::Get().GetRenderSystem()->GetEntity(sid))
		{
			SActiveStyle style;
			style.color = pEntity->GetColor();
			style.style = pEntity->GetStyle();
			style.size = pEntity->GetCharacterSize();
			return style;
		}
		return m_activeStyle;
	}

	inline void SetActiveStyle(SmartId sid, const SActiveStyle& style)
	{
		CRenderProxy* pRenderProxy = CGame::Get().GetRenderProxy();
		pRenderProxy->OnCommand<RenderCommand::SetColorCommand>(sid, style.color);
		pRenderProxy->OnCommand<RenderCommand::SetStyleCommand>(sid, style.style);
		pRenderProxy->OnCommand<RenderCommand::SetCharacterSizeCommand>(sid, style.size);
	}

	virtual void ActivateItem(const std::string& id) override
	{
		auto fnd = m_items.find(id);
		if (fnd != m_items.end())
		{
			SActiveStyle tmpStyle = m_activeStyle;
			if (m_activeItem != InvalidLink)
			{
				tmpStyle = GetActiveStyle(m_activeItem);
				SetActiveStyle(m_activeItem, m_activeStyle);
			}
			m_activeItem = fnd->second;
			m_activeStyle = GetActiveStyle(m_activeItem);
			SetActiveStyle(m_activeItem, tmpStyle);
		}
	}

	virtual void SetWriteText(bool bWrite) override
	{
		if (m_bWriteText != bWrite)
		{
			m_text.clear();
		}
		m_bWriteText = bWrite;
	}

	virtual const std::string& GetText() const override
	{
		return m_text;
	}

private:

	static uint32_t ParseStyle(const std::string& str)
	{
		static constexpr char delimeter = ',';

		uint32_t style = sf::Text::Regular;

		auto parse = [](const std::string& token) -> uint32_t
		{
			if (token == "Bold")
				return sf::Text::Bold;
			if (token == "Italic")
				return sf::Text::Italic;
			if (token == "Underlined")
				return sf::Text::Underlined;
			if (token == "StrikeThrough")
				return sf::Text::StrikeThrough;
			return 0;
		};
		
		size_t offset = 0;
		size_t delimPos = str.find(delimeter);
		while (delimPos != std::string::npos)
		{
			std::string token = str.substr(offset, delimPos - offset);
			style |= parse(token);
			offset = delimPos + 1;
			delimPos = str.find(delimeter, offset);
		}
		style |= parse(str.substr(offset));

		return style;
	}

	void LoadTextItem(const pugi::xml_node& node)
	{
		std::string id = node.attribute("id").value();
		if (!id.empty())
		{
			CRenderSystem* pRenderSystem = CGame::Get().GetRenderSystem();
			SmartId sid = pRenderSystem->CreateEntity(CRenderEntity::Text);
			if (CRenderEntity* pEntity = pRenderSystem->GetEntity(sid))
			{
				std::string value = node.attribute("value").value();
				if (IsFunction(value))
				{
					m_updateBindings[sid] = std::move(value);
				}
				else
				{
					pEntity->SetText(value);
				}

				const CResourceSystem* pResourceSystem = CGame::Get().GetResourceSystem();
				int fontId = pResourceSystem->GetFontId(node.attribute("font").value());
				if (fontId >= 0)
				{
					if (const sf::Font* pFont = pResourceSystem->GetFont(fontId))
					{
						pEntity->SetFont(*pFont);
					}
				}

				sf::Color color = CConfigurationSystem::ParseColor(node.attribute("color").value());
				pEntity->SetColor(color);

				unsigned int size = node.attribute("size").as_uint();
				pEntity->SetCharacterSize(size);

				uint32_t style = ParseStyle(node.attribute("style").value());
				pEntity->SetStyle(style);

				sf::Transform transform;
				transform.translate(m_vOrg + node.attribute("position").as_vector());
				transform.rotate(node.attribute("rotation").as_float());
				pEntity->SetTransform(transform);

				for (auto iter = node.begin(); iter != node.end(); ++iter)
				{
					LoadBinding(sid, *iter);
				}

				m_items.emplace(std::move(id), sid);
			}
		}
		else
		{
			Log("Failed to get item id in ", m_path);
		}
	}

	void LoadIconItem(const pugi::xml_node& node)
	{
		std::string id = node.attribute("id").value();
		if (!id.empty())
		{
			int textureId = CGame::Get().GetResourceSystem()->GetTextureId(node.attribute("value").value());
			if (textureId != -1)
			{
				SmartId sid = CGame::Get().GetRenderSystem()->CreateEntity(CRenderEntity::Sprite);

				CRenderProxy* pRenderProxy = CGame::Get().GetRenderProxy();
				pRenderProxy->OnCommand<RenderCommand::SetTextureCommand>(sid, textureId);
				
				sf::Vector2f vSize = node.attribute("size").as_vector();
				pRenderProxy->OnCommand<RenderCommand::SetSizeCommand>(sid, std::move(vSize));

				sf::Transform transform;
				transform.translate(m_vOrg + node.attribute("position").as_vector());
				transform.rotate(node.attribute("rotation").as_float());
				pRenderProxy->OnCommand<RenderCommand::SetTransformCommand>(sid, std::move(transform));

				m_items.emplace(std::move(id), sid);
			}
		}
		else
		{
			Log("Failed to get item id in ", m_path);
		}
	}

	inline static EControllerEvent ParseControllerEvent(const std::string& key)
	{
		if (key == "Enter")
			return EControllerEvent_Layout_Enter;
		if (key == "Up")
			return EControllerEvent_Layout_Up;
		if (key == "Down")
			return EControllerEvent_Layout_Down;
		if (key == "Left")
			return EControllerEvent_Layout_Left;
		if (key == "Right")
			return EControllerEvent_Layout_Right;
		if (key == "Esc")
			return EControllerEvent_Layout_Escape;
		return EControllerEvent_Invalid;
	}

	void LoadBinding(SmartId sid, const pugi::xml_node& node)
	{
		EControllerEvent evt = ParseControllerEvent(node.attribute("key").value());
		if (evt != EControllerEvent_Invalid)
		{
			SActionBinding bind;
			bind.evt = evt;
			bind.function = node.attribute("function").value();
			std::string argVal = node.attribute("argument").value();
			if (!argVal.empty())
			{
				bind.argument = std::move(argVal);
			}
			m_actionBindings[sid].push_back(std::move(bind));
		}
	}

	template <typename... T>
	void LoadSubLayout(const pugi::xml_node& node)
	{
		std::string id = node.attribute("id").value();
		if (!id.empty())
		{
			std::string path = node.attribute("path").value();
			sf::Vector2f vOrg = node.attribute("position").as_vector();
			m_subLayouts.emplace(std::move(id), std::make_unique<CLayout<T...>>(std::move(path), std::move(vOrg)));
		}
		else
		{
			Log("Failed to get subLayout id in ", m_path);
		}
	}

	/**
	* @function InvokeFunction
	* Invoke a binding or an updater. Function invokation arguments are depended on it's type:
	* If the function starts with '#' then the specified layout arguments will be applied to it;
	* If the function starts with '$' then the sprcified binding argument will be passed into it;
	* If the function starts with '!' then there are no special arguments will be passed into it.
	* This function can handle any return value type, but in general there are only two cases:
	* string for updaters and void for bindings.
	*
	* @template param R - return type of the invokable function.
	* @param func - name of the invokable function.
	* @param argument - optional binding argument to pass into the function.
	*/
	template <typename R>
	inline R InvokeFunction(const std::string& func, const std::optional<std::string>& argument = std::nullopt)
	{
		if (func[0] == '#' && m_pArguments)
		{
			return CGame::Get().GetUISystem()->InvokeFunction<R, V...>(func.substr(1), *m_pArguments);
		}
		else if (func[0] == '$' && argument)
		{
			return CGame::Get().GetUISystem()->InvokeFunction<R, ILayout*, std::string>(func.substr(1), std::make_tuple(this, *argument));
		}
		else if (func[0] == '!')
		{
			return CGame::Get().GetUISystem()->InvokeFunction<R, ILayout*>(func.substr(1), this);
		}
		return R();
	}

	inline bool IsFunction(const std::string& val)
	{
		return !val.empty() && !isalpha(val[0]);
	}

private:

	std::string m_path;
	sf::Vector2f m_vOrg;
	std::unique_ptr<std::tuple<V...>> m_pArguments;
	
	std::weak_ptr<CController> m_pController;

	std::map<std::string, SmartId> m_items;
	SmartId m_activeItem = InvalidLink;

	SActiveStyle m_activeStyle;

	struct SActionBinding
	{
		EControllerEvent evt = EControllerEvent_Invalid;
		std::string function;
		std::optional<std::string> argument;
	};
	std::map<SmartId, std::vector<SActionBinding>> m_actionBindings;
	std::map<SmartId, std::string> m_updateBindings;

	std::map<std::string, std::unique_ptr<ILayout>> m_subLayouts;

	bool m_bWriteText = false;
	std::string m_text;
};