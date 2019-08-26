#include "pch.h"
#include "InputMapping.h"


InputMapping::InputMapping()
{
	for (auto& key : m_keyMap)
	{
		key.first = KeyState::Released;
		key.second = KeyModifierFlags::None;
	}
	for (auto& button : m_mouseKeyMap)
	{
		button.first = KeyState::Released;
		button.second = KeyModifierFlags::None;
	}
}

InputMapping::~InputMapping()
{
}

InputMapping* InputMapping::Instance()
{
	static InputMapping instance;
	return &instance;
}

std::pair<InputMapping::KeyState, InputMapping::KeyModifiers> InputMapping::getKeyState(int _key)
{
	return m_keyMap[_key];
}

sf::Event::KeyEvent InputMapping::getKeyPressTransition(int _key, KeyModifiers Mod)
{
	KeyTransitionMap::iterator it = m_keyPressTransition.find(_key);

	return (it != m_keyPressTransition.end()) ? it->second : sf::Event::KeyEvent{sf::Keyboard::Key::Unknown,0,0,0};
}

sf::Event::KeyEvent InputMapping::getKeyReleaseTransition(int _key, KeyModifiers Mod)
{
	KeyTransitionMap::iterator it = m_keyReleaseTransition.find(_key);

	return (it != m_keyReleaseTransition.end()) ? it->second : sf::Event::KeyEvent{ sf::Keyboard::Key::Unknown,0,0,0 };
}

std::pair<InputMapping::KeyState, InputMapping::KeyModifiers> InputMapping::getMouseKeyState(int _key)
{
	return m_mouseKeyMap[_key];
}

InputMapping::KeyTransition InputMapping::getMouseKeyTransition(int _key)
{
	std::map<int, KeyTransition>::iterator it = m_mouseKeyTransition.find(_key);
	return (it != m_mouseKeyTransition.end()) ? it->second : KeyTransition::Unknown;
}

void InputMapping::addFunctionToKey(std::function<void()> _f, sf::Keyboard::Key _key, KeyState _triggerState, KeyModifiers _modifiers)
{
	if (_triggerState == InputMapping::Pressed)
		m_functionOnPressMap[std::make_pair(_key, _modifiers)] = _f;
	else
		m_functionOnReleaseMap[std::make_pair(_key, _modifiers)] = _f;
}

void InputMapping::addFunctionToMouseKey(std::function<void()> _f, sf::Mouse::Button _key, KeyState _triggerState, KeyModifiers _modifiers)
{
	if(_triggerState == InputMapping::Pressed)
		m_functionOnMousePressMap[std::make_pair(_key, _modifiers)] = _f;
	else
		m_functionOnMouseReleaseMap[std::make_pair(_key, _modifiers)] = _f;
}

void InputMapping::resetTemporaryData()
{
	m_keyPressTransition.clear();
	m_keyReleaseTransition.clear();

	m_mouseKeyTransition.clear();
	m_mousePosDelta = std::make_pair(0, 0);
	m_mouseMoved = false;
	m_mouseScrollDelta = 0;
	m_mouseScrolling = false;
}
