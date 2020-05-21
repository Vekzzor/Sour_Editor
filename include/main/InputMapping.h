#pragma once
#include <functional>
class InputMapping
{
public:
	enum KeyTransition
	{
		kUnknown,
		kPress,
		kRelease
	};
	enum KeyState
	{
		kReleased,
		kPressed
	};
	enum KeyModifierFlags // : unsigned char
	{
		kNone	= 0,
		kAlt		= 1 << 0,
		kControl = 1 << 1,
		kShift   = 1 << 2,
		kSystem  = 1 << 3,
	};
	typedef unsigned int KeyModifiers;
	typedef std::pair<KeyState, KeyModifiers> KeyStateModifierPair;
	typedef std::map<int, sf::Event::KeyEvent> KeyTransitionMap;
	typedef std::array<KeyStateModifierPair, sf::Keyboard::KeyCount> KeyMap;
	typedef std::array<KeyStateModifierPair, sf::Mouse::ButtonCount> MouseKeyMap;
	typedef std::map<std::pair<sf::Keyboard::Key, KeyModifiers>, std::function<void()>>
		FunctionKeyMap;
	typedef std::map<std::pair<sf::Mouse::Button, KeyModifiers>, std::function<void()>>
		FunctionMouseMap;

	InputMapping();
	~InputMapping();
	static InputMapping* Instance();
	//KEYBOARD
	KeyStateModifierPair GetKeyState(int _key);
	sf::Event::KeyEvent GetKeyPressTransition(int _key, KeyModifiers _modifiers = KeyModifierFlags::kNone);
	sf::Event::KeyEvent GetKeyReleaseTransition(int _key,
												KeyModifiers _modifiers = KeyModifierFlags::kNone);
	const bool EmptyKeyPressTransistion() const
	{
		return key_press_transition_.empty();
	};
	const bool EmptyKeyReleaseTransistion() const
	{
		return key_release_transition_.empty();
	};
	//MOUSE
	KeyStateModifierPair GetMouseKeyState(int _key);
	KeyTransition GetMouseKeyTransition(int _key);

	const bool EmptyMouseKeyTransistion() const
	{
		return mouse_key_transition_.empty();
	};
	const std::pair<int, int> GetMousePos() const
	{
		return mouse_pos_;
	};
	const std::pair<int, int> GetMousePosDelta() const
	{
		return mouse_pos_delta_;
	};
	const float GetMouseScrollDelta() const
	{
		return mouse_scroll_delta_;
	};
	const bool IsMouseMoving() const
	{
		return mouse_moved_;
	};
	const bool IsMouseScrolling() const
	{
		return mouse_scrolling_;
	};

	void AddFunctionToKey(std::function<void()> _f,
						  sf::Keyboard::Key _key,
						  KeyState _triggerState  = KeyState::kPressed,
						  KeyModifiers _modifiers = KeyModifierFlags::kNone);
	void AddFunctionToMouseKey(std::function<void()> _f,
							   sf::Mouse::Button _key,
							   KeyState _triggerState  = KeyState::kPressed,
							   KeyModifiers _modifiers = KeyModifierFlags::kNone);

protected:
	friend class EventHandler;
	//KEYBOARD
	KeyTransitionMap& GetKeyPressTransitions()
	{
		return key_press_transition_;
	};
	KeyTransitionMap& GetKeyReleaseTransitions()
	{
		return key_release_transition_;
	};
	KeyMap& GetKeyStates()
	{
		return key_map_;
	};

	//MOUSE
	std::map<int, KeyTransition>& GetMouseKeyTransitions()
	{
		return mouse_key_transition_;
	};
	MouseKeyMap& GetMouseKeyStates()
	{
		return mouse_key_map_;
	};
	void SetMousePos(int x, int y)
	{
		mouse_pos_delta_ = std::make_pair(x - mouse_pos_.first, y - mouse_pos_.second);
		mouse_pos_		= std::make_pair(x, y);
	};
	void SetMouseScrollDelta(float _delta)
	{
		mouse_scroll_delta_ = _delta;
	};
	void SetMouseScrolling(bool _moved)
	{
		mouse_scrolling_ = _moved;
	};
	void SetMouseMoved(bool _moved)
	{
		mouse_moved_ = _moved;
	};

	//UTILITY
	void ResetTemporaryData();
	FunctionKeyMap& GetFunctionPressMap()
	{
		return function_on_press_map_;
	};
	FunctionKeyMap& GetFunctionReleaseMap()
	{
		return function_on_release_map_;
	};

	FunctionMouseMap& GetFunctionMousePressMap()
	{
		return function_on_mouse_press_map_;
	};
	FunctionMouseMap& GetFunctionMouseReleaseMap()
	{
		return function_on_mouse_release_map_;
	};

private:
	KeyTransitionMap key_press_transition_;
	KeyTransitionMap key_release_transition_;

	KeyMap key_map_;

	std::map<int, KeyTransition> mouse_key_transition_;
	MouseKeyMap mouse_key_map_;

	std::pair<int, int> mouse_pos_{0, 0};
	std::pair<int, int> mouse_pos_delta_{0, 0};
	bool mouse_moved_		 = false;
	float mouse_scroll_delta_ = 0;
	bool mouse_scrolling_	= false;

	FunctionKeyMap function_on_press_map_;
	FunctionKeyMap function_on_release_map_;

	FunctionMouseMap function_on_mouse_press_map_;
	FunctionMouseMap function_on_mouse_release_map_;

	std::map<std::pair<int, KeyState>, std::function<void()>> function_map_;

	//AddFunctionToAction   ---an Action can hold multiple functions, use linked list to iterate through?
	//AddActionToKey -- Could be stored in an ini config file
	//Additional note: how do we solve actions for multiple context? each key stores one action for several contexts? <- send events
};