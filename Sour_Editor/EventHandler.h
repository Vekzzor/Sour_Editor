#pragma once

class InputMapping;

class EventHandler
{
public:


	EventHandler();
	~EventHandler();
	static EventHandler* Instance();

	void handleEvents(sf::Window& _window);

	
private:
};
