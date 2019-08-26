#include "pch.h"
#include "EventHandler.h"


EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

EventHandler* EventHandler::Instance()
{
	static EventHandler instance;
	return &instance;
}

void EventHandler::handleEvents(sf::Window & _window)
{
	sf::Event event;
	ImGuiIO& io = ImGui::GetIO();
	InputMapping* IM = InputMapping::Instance();
	IM->resetTemporaryData();
	InputMapping::KeyTransitionMap& kPT = IM->getKeyPressTransitions();
	InputMapping::KeyTransitionMap& kRT = IM->getKeyReleaseTransitions();
	InputMapping::KeyMap& kS = IM->getKeyStates();

	InputMapping::FunctionKeyMap::iterator itKeyboard;
	InputMapping::FunctionMouseMap::iterator itMouse;
	std::map<int, InputMapping::KeyTransition>& mKT = IM->getMouseKeyTransitions();
	InputMapping::MouseKeyMap& mKS = IM->getMouseKeyStates();
	InputMapping::KeyModifiers modifier;
	while (_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);
		switch (event.type)
		{
		case sf::Event::Closed:
			_window.close();
			break;
		case sf::Event::KeyPressed:
			modifier = (event.key.alt << 0 | event.key.control << 1 | event.key.shift << 2 | event.key.system << 3);

			kPT[event.key.code] = event.key;
			kS[event.key.code] = std::make_pair(InputMapping::KeyState::Pressed, modifier);

			itKeyboard = IM->getFunctionPressMap().find(std::make_pair(event.key.code, modifier));
			if (itKeyboard != IM->getFunctionPressMap().end())
				itKeyboard->second();
			break;
		case sf::Event::KeyReleased:
			modifier = (event.key.alt << 0 | event.key.control << 1 | event.key.shift << 2 | event.key.system << 3);

			kRT[event.key.code] = event.key;
			kS[event.key.code] = std::make_pair(InputMapping::KeyState::Released, InputMapping::KeyModifierFlags::None);
			//VOLATILE
			itKeyboard = IM->getFunctionReleaseMap().find(std::make_pair(event.key.code, InputMapping::KeyModifierFlags::None));
			if (itKeyboard != IM->getFunctionReleaseMap().end())
				itKeyboard->second();
			break;
		case sf::Event::MouseButtonPressed:
			if (io.WantCaptureMouse)
				break;
			modifier = (kS[sf::Keyboard::LAlt].first << 0 | kS[sf::Keyboard::LControl].first << 1 | kS[sf::Keyboard::LShift].first << 2 | kS[sf::Keyboard::LSystem].first << 3);

			mKT[event.mouseButton.button] = InputMapping::KeyTransition::Press;
			mKS[event.mouseButton.button] = std::make_pair(InputMapping::KeyState::Pressed, modifier);

			itMouse = IM->getFunctionMousePressMap().find(std::make_pair(event.mouseButton.button, modifier));
			if (itMouse != IM->getFunctionMousePressMap().end())
				itMouse->second();
			break;
		case  sf::Event::MouseButtonReleased:
			if (io.WantCaptureMouse)
				break;
			//modifier = (kS[sf::Keyboard::LAlt].first << 0 | kS[sf::Keyboard::LControl].first << 1 | kS[sf::Keyboard::LShift].first << 2 | kS[sf::Keyboard::LSystem].first << 3);

			mKT[event.mouseButton.button] = InputMapping::KeyTransition::Release;
			mKS[event.mouseButton.button] = std::make_pair(InputMapping::KeyState::Released, InputMapping::KeyModifierFlags::None);

			//VOLATILE
			itMouse = IM->getFunctionMouseReleaseMap().find(std::make_pair(event.mouseButton.button, InputMapping::KeyModifierFlags::None));
			if (itMouse != IM->getFunctionMouseReleaseMap().end())
				itMouse->second();
			break;
		case sf::Event::MouseMoved:
			if (io.WantCaptureMouse)
				break;
			IM->setMouseMoved(true);
			IM->setMousePos(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseWheelScrolled:
			if (io.WantCaptureMouse)
				break;
			IM->setMouseScrolling(true);
			IM->setMouseScrollDelta(event.mouseWheelScroll.delta);
			break;
		}
	}
}
