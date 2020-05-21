#include "RenderSystem.h"
#include "pch.h"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::DrawMeshes(sf::RenderWindow& _window)
{
	for(auto const& entity : entities_)
	{
		DrawMesh(_window, entity);
	}
}

void RenderSystem::DrawMesh(sf::RenderWindow& _window, Entity _entity)
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	auto& transform			 = coordinator.GetComponent<sf::Transformable>(_entity);
	auto& mesh				 = coordinator.GetComponent<Mesh2D>(_entity);

	sf::RenderStates states;
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= transform.getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	states.texture = mesh.texture;
	// draw the vertex array
	_window.draw(mesh.vertices.data(), mesh.vertices.size(), mesh.primitive_type, states);
}

void RenderSystem::DrawWireframeMeshes(sf::RenderWindow& _window)
{
	for(auto const& entity : entities_)
	{
		DrawWireframeMesh(_window, entity);
	}
}

void RenderSystem::DrawWireframeMesh(sf::RenderWindow& _window, Entity _entity)
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	auto& transform			 = coordinator.GetComponent<sf::Transformable>(_entity);
	auto& mesh				 = coordinator.GetComponent<Mesh2D>(_entity);

	sf::RenderStates states;
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= transform.getTransform(); // getTransform() is defined by sf::Transformable
	states.blendMode = sf::BlendMode(
		sf::BlendMode(sf::BlendMode::SrcColor, sf::BlendMode::One, sf::BlendMode::Add));
	// apply the texture
	states.texture = mesh.texture;
	_window.draw(
		mesh.vertices.data(), mesh.vertices.size(), sf::PrimitiveType::LineStrip, states);
}
