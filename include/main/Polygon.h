#pragma once
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
typedef std::array<float, 2> float2;

class Polygon : public sf::Drawable, public sf::Transformable
{
public:
	Polygon();
	Polygon(b2World& world);
	void addTexture(sf::Texture texture)
	{
		this->m_texture = texture;
	}
	void createPhysicsBody(std::vector<b2Vec2>& vertices);
	//#DEPRICATED
	void createPhysicsBody();
	void addFixtures() {}
	void update();
	std::vector<sf::Vertex> m_vertices;
	sf::CircleShape m_center;
	sf::PrimitiveType m_type = sf::Triangles;

	sf::CircleShape m_lower;
	sf::CircleShape m_upper;

	float2 AABB_lowerBounds;
	float2 AABB_upperBounds;
	bool renderPoly		= true;
	bool renderSite		= false;
	bool renderVertices = false;
	bool renderEdges	= false;

	b2Body* m_body;
	std::vector<b2Fixture*> m_fixtures;

	//random fun stuff
	float2 thrust = {0.0f,0.0f};

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::Transform m_transformation;
	sf::Texture m_texture;
};