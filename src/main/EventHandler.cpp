#include "EventHandler.h"
#include "pch.h"

EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

EventHandler* EventHandler::Instance()
{
	static EventHandler instance;
	return &instance;
}

void EventHandler::HandleEvents(sf::Window& _window)
{
	sf::Event event;
	ImGuiIO& io		 = ImGui::GetIO();
	InputMapping* IM = InputMapping::Instance();
	IM->ResetTemporaryData();
	InputMapping::KeyTransitionMap& kPT = IM->GetKeyPressTransitions();
	InputMapping::KeyTransitionMap& kRT = IM->GetKeyReleaseTransitions();
	InputMapping::KeyMap& kS			= IM->GetKeyStates();

	InputMapping::FunctionKeyMap::iterator itKeyboard;
	InputMapping::FunctionMouseMap::iterator itMouse;
	std::map<int, InputMapping::KeyTransition>& mKT = IM->GetMouseKeyTransitions();
	InputMapping::MouseKeyMap& mKS					= IM->GetMouseKeyStates();
	InputMapping::KeyModifiers modifier;
	while(_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);
		switch(event.type)
		{
		case sf::Event::Closed:
			_window.close();
			break;
		case sf::Event::KeyPressed:
			modifier = (event.key.alt << 0 | event.key.control << 1 | event.key.shift << 2 |
						event.key.system << 3);

			kPT[event.key.code] = event.key;
			kS[event.key.code]  = std::make_pair(InputMapping::KeyState::kPressed, modifier);

			itKeyboard = IM->GetFunctionPressMap().find(std::make_pair(event.key.code, modifier));
			if(itKeyboard != IM->GetFunctionPressMap().end())
				itKeyboard->second();
			break;
		case sf::Event::KeyReleased:
			modifier = (event.key.alt << 0 | event.key.control << 1 | event.key.shift << 2 |
						event.key.system << 3);

			kRT[event.key.code] = event.key;
			kS[event.key.code]  = std::make_pair(InputMapping::KeyState::kReleased,
												 InputMapping::KeyModifierFlags::kNone);
			//VOLATILE
			itKeyboard = IM->GetFunctionReleaseMap().find(
				std::make_pair(event.key.code, InputMapping::KeyModifierFlags::kNone));
			if(itKeyboard != IM->GetFunctionReleaseMap().end())
				itKeyboard->second();
			break;
		case sf::Event::MouseButtonPressed:
			if(io.WantCaptureMouse)
				break;
			modifier = (kS[sf::Keyboard::LAlt].first << 0 | kS[sf::Keyboard::LControl].first << 1 |
						kS[sf::Keyboard::LShift].first << 2 | kS[sf::Keyboard::LSystem].first << 3);

			mKT[event.mouseButton.button] = InputMapping::KeyTransition::kPress;
			mKS[event.mouseButton.button] =
				std::make_pair(InputMapping::KeyState::kPressed, modifier);

			itMouse = IM->GetFunctionMousePressMap().find(
				std::make_pair(event.mouseButton.button, modifier));
			if(itMouse != IM->GetFunctionMousePressMap().end())
				itMouse->second();
			break;
		case sf::Event::MouseButtonReleased:
			if(io.WantCaptureMouse)
				break;
			//modifier = (kS[sf::Keyboard::LAlt].first << 0 | kS[sf::Keyboard::LControl].first << 1 | kS[sf::Keyboard::LShift].first << 2 | kS[sf::Keyboard::LSystem].first << 3);

			mKT[event.mouseButton.button] = InputMapping::KeyTransition::kRelease;
			mKS[event.mouseButton.button] = std::make_pair(InputMapping::KeyState::kReleased,
														   InputMapping::KeyModifierFlags::kNone);

			//VOLATILE
			itMouse = IM->GetFunctionMouseReleaseMap().find(
				std::make_pair(event.mouseButton.button, InputMapping::KeyModifierFlags::kNone));
			if(itMouse != IM->GetFunctionMouseReleaseMap().end())
				itMouse->second();
			break;
		case sf::Event::MouseMoved:
			if(io.WantCaptureMouse)
				break;
			IM->SetMouseMoved(true);
			IM->SetMousePos(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseWheelScrolled:
			if(io.WantCaptureMouse)
				break;
			IM->SetMouseScrolling(true);
			IM->SetMouseScrollDelta(event.mouseWheelScroll.delta);
			break;
		}
	}
}
