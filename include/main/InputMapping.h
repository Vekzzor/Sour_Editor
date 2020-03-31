#pragma once
#include <functional>
class InputMapping
{
public:
	enum KeyTransition
	{
		Unknown,
		Press,
		Release
	};
	enum KeyState
	{
		Released,
		Pressed
	};
	enum KeyModifierFlags // : unsigned char
	{
		None	= 0,
		Alt		= 1 << 0,
		Control = 1 << 1,
		Shift   = 1 << 2,
		System  = 1 << 3,
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
	KeyStateModifierPair getKeyState(int _key);
	sf::Event::KeyEvent getKeyPressTransition(int _key, KeyModifiers Mod = KeyModifierFlags::None);
	sf::Event::KeyEvent getKeyReleaseTransition(int _key,
												KeyModifiers Mod = KeyModifierFlags::None);
	const bool EmptyKeyPressTransistion() const
	{
		return m_keyPressTransition.empty();
	};
	const bool EmptyKeyReleaseTransistion() const
	{
		return m_keyReleaseTransition.empty();
	};
	//MOUSE
	KeyStateModifierPair getMouseKeyState(int _key);
	KeyTransition getMouseKeyTransition(int _key);

	const bool EmptyMouseKeyTransistion() const
	{
		return m_mouseKeyTransition.empty();
	};
	const std::pair<int, int> getMousePos() const
	{
		return m_mousePos;
	};
	const float getMouseScrollDelta() const
	{
		return m_mouseScrollDelta;
	};
	const bool isMouseMoving() const
	{
		return m_mouseMoved;
	};
	const bool isMouseScrolling() const
	{
		return m_mouseScrolling;
	};

	void addFunctionToKey(std::function<void()> _f,
						  sf::Keyboard::Key _key,
						  KeyState _triggerState  = KeyState::Pressed,
						  KeyModifiers _modifiers = KeyModifierFlags::None);
	void addFunctionToMouseKey(std::function<void()> _f,
							   sf::Mouse::Button _key,
							   KeyState _triggerState  = KeyState::Pressed,
							   KeyModifiers _modifiers = KeyModifierFlags::None);

protected:
	friend class EventHandler;
	//KEYBOARD
	KeyTransitionMap& getKeyPressTransitions()
	{
		return m_keyPressTransition;
	};
	KeyTransitionMap& getKeyReleaseTransitions()
	{
		return m_keyReleaseTransition;
	};
	KeyMap& getKeyStates()
	{
		return m_keyMap;
	};

	//MOUSE
	std::map<int, KeyTransition>& getMouseKeyTransitions()
	{
		return m_mouseKeyTransition;
	};
	MouseKeyMap& getMouseKeyStates()
	{
		return m_mouseKeyMap;
	};
	void setMousePos(int x, int y)
	{
		m_mousePos		= std::make_pair(x, y);
		m_mousePosDelta = std::make_pair(x - m_mousePos.first, y - m_mousePos.second);
	};
	void setMouseScrollDelta(float _delta)
	{
		m_mouseScrollDelta = _delta;
	};
	void setMouseScrolling(bool _moved)
	{
		m_mouseScrolling = _moved;
	};
	void setMouseMoved(bool _moved)
	{
		m_mouseMoved = _moved;
	};

	//UTILITY
	void resetTemporaryData();
	FunctionKeyMap& getFunctionPressMap()
	{
		return m_functionOnPressMap;
	};
	FunctionKeyMap& getFunctionReleaseMap()
	{
		return m_functionOnReleaseMap;
	};

	FunctionMouseMap& getFunctionMousePressMap()
	{
		return m_functionOnMousePressMap;
	};
	FunctionMouseMap& getFunctionMouseReleaseMap()
	{
		return m_functionOnMouseReleaseMap;
	};

private:
	KeyTransitionMap m_keyPressTransition;
	KeyTransitionMap m_keyReleaseTransition;

	KeyMap m_keyMap;

	std::map<int, KeyTransition> m_mouseKeyTransition;
	MouseKeyMap m_mouseKeyMap;

	std::pair<int, int> m_mousePos{0, 0};
	std::pair<int, int> m_mousePosDelta{0, 0};
	bool m_mouseMoved		 = false;
	float m_mouseScrollDelta = 0;
	bool m_mouseScrolling	= false;

	FunctionKeyMap m_functionOnPressMap;
	FunctionKeyMap m_functionOnReleaseMap;

	FunctionMouseMap m_functionOnMousePressMap;
	FunctionMouseMap m_functionOnMouseReleaseMap;

	std::map<std::pair<int, KeyState>, std::function<void()>> m_functionMap;

	//AddFunctionToAction   ---an Action can hold multiple functions, use linked list to iterate through?
	//AddActionToKey -- Could be stored in an ini config file
	//Additional note: how do we solve actions for multiple context? each key stores one action for several contexts?
};