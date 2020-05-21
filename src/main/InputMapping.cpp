#include "InputMapping.h"
#include "pch.h"

InputMapping::InputMapping()
{
	for(auto& key : key_map_)
	{
		key.first  = KeyState::kReleased;
		key.second = KeyModifierFlags::kNone;
	}
	for(auto& button : mouse_key_map_)
	{
		button.first  = KeyState::kReleased;
		button.second = KeyModifierFlags::kNone;
	}
}

InputMapping::~InputMapping() {}

InputMapping* InputMapping::Instance()
{
	static InputMapping instance;
	return &instance;
}

std::pair<InputMapping::KeyState, InputMapping::KeyModifiers> InputMapping::GetKeyState(int _key)
{
	return key_map_[_key];
}

sf::Event::KeyEvent InputMapping::GetKeyPressTransition(int _key, KeyModifiers _modifiers)
{
	KeyTransitionMap::iterator it = key_press_transition_.find(_key);

	return (it != key_press_transition_.end())
			   ? it->second
			   : sf::Event::KeyEvent{sf::Keyboard::Key::Unknown, 0, 0, 0};
}

sf::Event::KeyEvent InputMapping::GetKeyReleaseTransition(int _key, KeyModifiers _modifiers)
{
	KeyTransitionMap::iterator it = key_release_transition_.find(_key);

	return (it != key_release_transition_.end())
			   ? it->second
			   : sf::Event::KeyEvent{sf::Keyboard::Key::Unknown, 0, 0, 0};
}

std::pair<InputMapping::KeyState, InputMapping::KeyModifiers>
InputMapping::GetMouseKeyState(int _key)
{
	return mouse_key_map_[_key];
}

InputMapping::KeyTransition InputMapping::GetMouseKeyTransition(int _key)
{
	std::map<int, KeyTransition>::iterator it = mouse_key_transition_.find(_key);
	return (it != mouse_key_transition_.end()) ? it->second : KeyTransition::kUnknown;
}

void InputMapping::AddFunctionToKey(std::function<void()> _f,
									sf::Keyboard::Key _key,
									KeyState _triggerState,
									KeyModifiers _modifiers)
{
	if(_triggerState == InputMapping::kPressed)
		function_on_press_map_[std::make_pair(_key, _modifiers)] = _f;
	else
		function_on_release_map_[std::make_pair(_key, _modifiers)] = _f;
}

void InputMapping::AddFunctionToMouseKey(std::function<void()> _f,
										 sf::Mouse::Button _key,
										 KeyState _triggerState,
										 KeyModifiers _modifiers)
{
	if(_triggerState == InputMapping::kPressed)
		function_on_mouse_press_map_[std::make_pair(_key, _modifiers)] = _f;
	else
		function_on_mouse_release_map_[std::make_pair(_key, _modifiers)] = _f;
}

void InputMapping::ResetTemporaryData()
{
	key_press_transition_.clear();
	key_release_transition_.clear();

	mouse_key_transition_.clear();
	mouse_pos_delta_	= std::make_pair(0, 0);
	mouse_moved_	   = false;
	mouse_scroll_delta_ = 0;
	mouse_scrolling_   = false;
}
