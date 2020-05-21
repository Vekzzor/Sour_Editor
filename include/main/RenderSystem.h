#pragma once
#include "ECS/ECS.h"
class RenderSystem : public System
{
public:
	RenderSystem();
	~RenderSystem();
	void DrawMeshes(sf::RenderWindow &_window);
	void DrawMesh(sf::RenderWindow &_window, Entity _entity);
	void DrawWireframeMeshes(sf::RenderWindow &_window);
	void DrawWireframeMesh(sf::RenderWindow &_window, Entity _entity);
};

