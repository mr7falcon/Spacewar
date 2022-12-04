#include <iostream>

#include <pugixml.hpp>

#include "Game.h"
#include "ControllerConfiguration.h"
#include "KeyboardController.h"
#include "GamepadController.h"
#include "NetworkProxy.h"

inline static sf::Keyboard::Key ParseKey(const std::string& key)
{
	if (key == "A")
		return sf::Keyboard::A;
	if (key == "B")
		return sf::Keyboard::B;
	if (key == "C")
		return sf::Keyboard::C;
	if (key == "D")
		return sf::Keyboard::D;
	if (key == "E")
		return sf::Keyboard::E;
	if (key == "F")
		return sf::Keyboard::F;
	if (key == "G")
		return sf::Keyboard::G;
	if (key == "H")
		return sf::Keyboard::H;
	if (key == "I")
		return sf::Keyboard::I;
	if (key == "J")
		return sf::Keyboard::J;
	if (key == "K")
		return sf::Keyboard::K;
	if (key == "L")
		return sf::Keyboard::L;
	if (key == "M")
		return sf::Keyboard::M;
	if (key == "N")
		return sf::Keyboard::N;
	if (key == "O")
		return sf::Keyboard::O;
	if (key == "P")
		return sf::Keyboard::P;
	if (key == "Q")
		return sf::Keyboard::Q;
	if (key == "R")
		return sf::Keyboard::R;
	if (key == "S")
		return sf::Keyboard::S;
	if (key == "T")
		return sf::Keyboard::T;
	if (key == "U")
		return sf::Keyboard::U;
	if (key == "V")
		return sf::Keyboard::V;
	if (key == "W")
		return sf::Keyboard::W;
	if (key == "X")
		return sf::Keyboard::X;
	if (key == "Y")
		return sf::Keyboard::Y;
	if (key == "Z")
		return sf::Keyboard::Z;
	if (key == "Num0")
		return sf::Keyboard::Num0;
	if (key == "Num1")
		return sf::Keyboard::Num1;
	if (key == "Num2")
		return sf::Keyboard::Num2;
	if (key == "Num3")
		return sf::Keyboard::Num3;
	if (key == "Num4")
		return sf::Keyboard::Num4;
	if (key == "Num5")
		return sf::Keyboard::Num5;
	if (key == "Num6")
		return sf::Keyboard::Num6;
	if (key == "Num7")
		return sf::Keyboard::Num7;
	if (key == "Num8")
		return sf::Keyboard::Num8;
	if (key == "Num9")
		return sf::Keyboard::Num9;
	if (key == "Escape")
		return sf::Keyboard::Escape;
	if (key == "LControl")
		return sf::Keyboard::LControl;
	if (key == "LShift")
		return sf::Keyboard::LShift;
	if (key == "LAlt")
		return sf::Keyboard::LAlt;
	if (key == "LSystem")
		return sf::Keyboard::LSystem;
	if (key == "RControl")
		return sf::Keyboard::RControl;
	if (key == "RShift")
		return sf::Keyboard::RShift;
	if (key == "RAlt")
		return sf::Keyboard::RAlt;
	if (key == "RSystem")
		return sf::Keyboard::RSystem;
	if (key == "Menu")
		return sf::Keyboard::Menu;
	if (key == "LBracket")
		return sf::Keyboard::LBracket;
	if (key == "RBracket")
		return sf::Keyboard::RBracket;
	if (key == "Semicolon")
		return sf::Keyboard::Semicolon;
	if (key == "Comma")
		return sf::Keyboard::Comma;
	if (key == "Period")
		return sf::Keyboard::Period;
	if (key == "Quote")
		return sf::Keyboard::Quote;
	if (key == "Slash")
		return sf::Keyboard::Slash;
	if (key == "Backslash")
		return sf::Keyboard::Backslash;
	if (key == "Tilde")
		return sf::Keyboard::Tilde;
	if (key == "Equal")
		return sf::Keyboard::Equal;
	if (key == "Hyphen")
		return sf::Keyboard::Hyphen;
	if (key == "Space")
		return sf::Keyboard::Space;
	if (key == "Enter")
		return sf::Keyboard::Enter;
	if (key == "Backspace")
		return sf::Keyboard::Backspace;
	if (key == "Tab")
		return sf::Keyboard::Tab;
	if (key == "PageUp")
		return sf::Keyboard::PageUp;
	if (key == "PageDown")
		return sf::Keyboard::PageDown;
	if (key == "End")
		return sf::Keyboard::End;
	if (key == "Home")
		return sf::Keyboard::Home;
	if (key == "Insert")
		return sf::Keyboard::Insert;
	if (key == "Delete")
		return sf::Keyboard::Delete;
	if (key == "Add")
		return sf::Keyboard::Add;
	if (key == "Substract")
		return sf::Keyboard::Subtract;
	if (key == "Multiply")
		return sf::Keyboard::Multiply;
	if (key == "Divide")
		return sf::Keyboard::Divide;
	if (key == "Left")
		return sf::Keyboard::Left;
	if (key == "Right")
		return sf::Keyboard::Right;
	if (key == "Up")
		return sf::Keyboard::Up;
	if (key == "Down")
		return sf::Keyboard::Down;
	if (key == "Numpad0")
		return sf::Keyboard::Numpad0;
	if (key == "Numpad1")
		return sf::Keyboard::Numpad1;
	if (key == "Numpad2")
		return sf::Keyboard::Numpad2;
	if (key == "Numpad3")
		return sf::Keyboard::Numpad3;
	if (key == "Numpad4")
		return sf::Keyboard::Numpad4;
	if (key == "Numpad5")
		return sf::Keyboard::Numpad5;
	if (key == "Numpad6")
		return sf::Keyboard::Numpad6;
	if (key == "Numpad7")
		return sf::Keyboard::Numpad7;
	if (key == "Numpad8")
		return sf::Keyboard::Numpad8;
	if (key == "Numpad9")
		return sf::Keyboard::Numpad9;
	if (key == "F1")
		return sf::Keyboard::F1;
	if (key == "F2")
		return sf::Keyboard::F2;
	if (key == "F3")
		return sf::Keyboard::F3;
	if (key == "F4")
		return sf::Keyboard::F4;
	if (key == "F5")
		return sf::Keyboard::F5;
	if (key == "F6")
		return sf::Keyboard::F6;
	if (key == "F7")
		return sf::Keyboard::F7;
	if (key == "F8")
		return sf::Keyboard::F8;
	if (key == "F9")
		return sf::Keyboard::F9;
	if (key == "F10")
		return sf::Keyboard::F10;
	if (key == "F11")
		return sf::Keyboard::F11;
	if (key == "F12")
		return sf::Keyboard::F12;
	if (key == "F13")
		return sf::Keyboard::F13;
	if (key == "F14")
		return sf::Keyboard::F14;
	if (key == "F15")
		return sf::Keyboard::F15;
	if (key == "Pause")
		return sf::Keyboard::Pause;
	return sf::Keyboard::Unknown;
}

CControllerConfiguration::CControllerConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		Log("Failed to load controller configuration: ", res.description());
		return;
	}

	auto root = doc.child("Schemas");
	if (!root)
	{
		Log("Invalid root element in controller configuration");
		return;
	}

	auto getKey = [](const pugi::xml_node& schema, const char* control) -> sf::Keyboard::Key
	{
		auto child = schema.child(control);
		return ParseKey(child.attribute("key").value());
	};

	for (auto iter = root.begin(); iter != root.end(); ++iter)
	{
		std::string name = iter->attribute("name").value();
		if (!name.empty())
		{
			auto fnd = m_configurations.find(name);
			if (fnd != m_configurations.end())
			{
				Log("Controller schema with name ", name, " already exists");
				continue;
			}

			SConfiguration config;
			
			config.moveForward = getKey(*iter, "MoveForward");
			config.moveBack = getKey(*iter, "MoveBack");
			config.rotatePositive = getKey(*iter, "RotatePositive");
			config.rotateNegative = getKey(*iter, "RotateNegative");
			config.shoot = getKey(*iter, "Shoot");
			config.escape = getKey(*iter, "Escape");

			m_configurations.emplace(std::move(name), std::move(config));
		}
	}
}

const CControllerConfiguration::SConfiguration* CControllerConfiguration::GetConfiguration(const std::string& name) const
{
	auto fnd = m_configurations.find(name);
	return fnd != m_configurations.end() ? &fnd->second : nullptr;
}

std::shared_ptr<IController> CControllerConfiguration::CreateController(const std::string& config) const
{
	const auto* pKeyboardSchema = GetConfiguration(config);
	if (pKeyboardSchema)
	{
		auto pController = std::make_shared<CKeyboardController>(pKeyboardSchema);
		CGame::Get().RegisterWindowEventListener(pController);
		return pController;
	}
	else if (config == "Network")
	{
		return CGame::Get().GetNetworkProxy()->CreateNetworkController();
	}
	else if (config == "Gamepad1")
	{
		return std::make_shared<CGamepadController>(0);
	}
	else if (config == "Gamepad2")
	{
		return std::make_shared<CGamepadController>(1);
	}
	return nullptr;
}

std::shared_ptr<IController> CControllerConfiguration::CreateDefaultController() const
{
	if (!m_configurations.empty())
	{
		auto pController = std::make_shared<CKeyboardController>(&m_configurations.begin()->second);
		CGame::Get().RegisterWindowEventListener(pController);
		return pController;
	}
	return nullptr;
}