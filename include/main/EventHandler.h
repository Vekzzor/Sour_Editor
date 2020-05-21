#pragma once

class InputMapping;

class EventHandler
{
public:
	EventHandler();
	~EventHandler();
	static EventHandler* Instance();

	void HandleEvents(sf::Window& _window);

private:
};
