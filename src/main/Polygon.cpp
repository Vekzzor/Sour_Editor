#include "pch.h"

Polygon::Polygon()
{
	m_center.setRadius(20);
	m_center.setFillColor(sf::Color(0, 0, 0));
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
}

Polygon::Polygon(b2World& world)
{
	m_center.setRadius(20);
	m_center.setFillColor(sf::Color(0, 0, 0));
	m_lower.setRadius(20);
	m_lower.setFillColor(sf::Color(255, 0, 0));
	m_upper.setRadius(20);
	m_upper.setFillColor(sf::Color(0, 255, 0));
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	m_body		 = world.CreateBody(&bodyDef);
}

void Polygon::createPhysicsBody(std::vector<b2Vec2>& vertices)
{
	b2PolygonShape polygon;
	int vertCount = vertices.size();
	std::vector<int> test(std::ceil(vertCount / 8.0f));
	for(int i = 0; i < test.size(); i++)
	{
		test[i] = std::min(8, vertCount);
		vertCount -= 8;
	}
	if(test.back() < 3)
	{
		test[test.size() - 2] -= 2;
		test.back() += 2;
	}
	vertCount	= 0;
	bool created = false;
	for(int i = 0; i < test.size(); i++)
	{
		vertCount += test[i];
		std::vector<b2Vec2>::const_iterator first =
			vertices.begin() + std::fmin(vertCount - test[i], test.size() - 8);
		std::vector<b2Vec2>::const_iterator last = vertices.begin() + vertCount;
		std::vector<b2Vec2> newVec(first, last);
		polygon.Set(newVec.data(), newVec.size());
		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polygon;
		m_fixtures.push_back(m_body->CreateFixture(&fixtureDef));
		created = true;
	}
	/*if (vertices.size() % 8 > 3)
	{
		std::vector<b2Vec2>::const_iterator first = vertices.begin() + (fixtures * 8);
		std::vector<b2Vec2>::const_iterator last = vertices.end();
		std::vector<b2Vec2> newVec(first, last);
		polygon.Set(newVec.data(), newVec.size());

		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape = &polygon;
		m_fixtures.push_back(m_body->CreateFixture(&fixtureDef));
		created = true;
	}*/
	//m_body->SetTransform(b2Vec2(this->getPosition().x, this->getPosition().y), 0);
	this->setOrigin(0, 0);
	//this->setScale(0.1f, 0.1f);
	//this->m_body->SetLinearVelocity(b2Vec2(0, 1000));
}

//#DEPRICATED

void Polygon::createPhysicsBody()
{
	std::vector<b2Vec2> vertices(m_vertices.size());
	for(int i = 0; i < m_vertices.size(); i++)
		vertices[i] = b2Vec2(m_vertices[i].position.x / 10000, m_vertices[i].position.y / 10000);

	b2PolygonShape polygon;
	int fixtures = vertices.size() / 8;
	for(int i = 0; i < fixtures; i++)
	{
		int count								  = 8 * (i + 1);
		std::vector<b2Vec2>::const_iterator first = vertices.begin() + count - 8;
		std::vector<b2Vec2>::const_iterator last  = vertices.begin() + count;
		std::vector<b2Vec2> newVec(first, last);
		polygon.Set(newVec.data(), newVec.size());
		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polygon;
		m_fixtures.push_back(m_body->CreateFixture(&fixtureDef));
	}
	if(vertices.size() % 8 != 0)
	{
		std::vector<b2Vec2>::const_iterator first = vertices.begin() + (fixtures * 8);
		std::vector<b2Vec2>::const_iterator last  = vertices.end();
		std::vector<b2Vec2> newVec(first, last);
		polygon.Set(newVec.data(), newVec.size());

		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polygon;
		m_fixtures.push_back(m_body->CreateFixture(&fixtureDef));
	}

	//m_body->SetTransform(b2Vec2(this->getPosition().x, this->getPosition().y), 0);
	this->setOrigin(0, 0);
	//this->m_body->SetLinearVelocity(b2Vec2(0, 1000));
}

void Polygon::update()
{
	auto lastPos = this->getPosition();
	auto newPos =
		sf::Vector2f(this->m_body->GetPosition().x * 10000, this->m_body->GetPosition().y * 10000);

	auto delta = newPos - lastPos;
	AABB_lowerBounds[0] += delta.x, AABB_lowerBounds[1] += delta.y;
	AABB_upperBounds[0] += delta.x, AABB_upperBounds[1] += delta.y;
	this->setPosition(newPos);
	this->setRotation(this->m_body->GetAngle() * (180 / b2_pi));

	sf::Vector2f minBound = {FLT_MAX, FLT_MAX};
	sf::Vector2f maxBound = {-FLT_MAX, -FLT_MAX};
	for(auto& vertex : m_vertices)
	{
		sf::Vector2f pos = this->getTransform() * vertex.position;
		minBound.x		 = std::min(pos.x, minBound.x);
		minBound.y		 = std::min(pos.y, minBound.y);
		maxBound.x		 = std::max(pos.x, maxBound.x);
		maxBound.y		 = std::max(pos.y, maxBound.y);
	}

	m_lower.setPosition(minBound);
	m_upper.setPosition(maxBound);
	AABB_lowerBounds[0] = minBound.x, AABB_lowerBounds[1] = minBound.y;
	AABB_upperBounds[0] = maxBound.x, AABB_upperBounds[1] = maxBound.y;
}

void Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	//states.texture = &m_texture;
	// you may also override states.shader or states.blendMode if you want
	// draw the vertex array
	if(renderPoly)
		target.draw(m_vertices.data(), m_vertices.size(), m_type, states);
	if(renderSite)
		target.draw(m_center, states);
	//target.draw(&m_site, 1, sf::Points, states);
	target.draw(m_lower);
	target.draw(m_upper);
}
