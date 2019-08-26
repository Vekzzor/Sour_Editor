// PhysicsDestructionBox2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

b2World world(b2Vec2(0.0f, 9.82f));
sf::ContextSettings settings(0,0,16);
sf::RenderWindow window(sf::VideoMode(1000, 1000), "Voronoi Tesselation", sf::Style::Default, settings);
static const int MINSIZE = 0; //300
static const int MAXSIZE = 1000; //400
static const float CENTER = float(MINSIZE + (MAXSIZE / 2));
static const int SEEDS = 1;

class MyQueryCallback : public b2QueryCallback {
public:
	std::vector<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture) {
		foundBodies.push_back(fixture->GetBody());
		return true;//keep going to find all fixtures in the query area
	}
};

void applyBlastImpulse(b2Body* body, b2Vec2 blastCenter, b2Vec2 applyPoint, float blastPower) {
	b2Vec2 blastDir = applyPoint - blastCenter;
	float distance = blastDir.Normalize();
	//ignore bodies exactly at the blast point - blast direction is undefined
	if (distance == 0)
		return;
	float invDistance = 1 / distance;
	float impulseMag = blastPower * invDistance * invDistance;
	body->ApplyLinearImpulse(impulseMag * blastDir, applyPoint,true);
}

class PhysicsShapeDraw : public b2Draw
{
public:
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		int count = vertexCount;
		sf::ConvexShape convex;
		convex.setPointCount(count);

		// define the points
		for (int i = 0; i < count; i++)
		{
			convex.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
		}
		/*convex.setPoint(0, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[0].x*150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[0].y*150));
		convex.setPoint(1, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[1].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[1].y * 150));
		convex.setPoint(2, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[2].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[2].y * 150));
		*/
		convex.setFillColor(sf::Color(218, 165, 32));
		window.draw(convex);
	
	}
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
		
	}
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
		
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {}
	void DrawTransform(const b2Transform& xf) {}
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) 
	{
		sf::CircleShape point;
		point.setPosition(p.x - size, p.y - size);
		point.setRadius(size);
		window.draw(point);
	}
};

class Polygon : public sf::Drawable, public sf::Transformable
{
public:
	Polygon()
	{
		m_center.setRadius(20);
		m_center.setFillColor(sf::Color(0, 0, 0));
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		m_body = world.CreateBody(&bodyDef);
	}
	void addTexture(sf::Texture texture)
	{
		this->m_texture = texture;
	}
	void createPhysicsBody(std::vector<b2Vec2>&vertices)
	{
	


		b2PolygonShape polygon;
		int fixtures = vertices.size() / 8;
		bool created = false;
		for (int i = 0; i < fixtures; i++)
		{
			int count = 8 * (i + 1);
			std::vector<b2Vec2>::const_iterator first = vertices.begin() + count - 8;
			std::vector<b2Vec2>::const_iterator last = vertices.begin() + count;
			std::vector<b2Vec2> newVec(first, last);
			polygon.Set(newVec.data(), newVec.size());

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &polygon;
			m_body->CreateFixture(&fixtureDef);
			created = true;
		}
		if (vertices.size() % 8 > 2)
		{
			std::vector<b2Vec2>::const_iterator first = vertices.begin() + (fixtures * 8);
			std::vector<b2Vec2>::const_iterator last = vertices.end();
			std::vector<b2Vec2> newVec(first, last);
			polygon.Set(newVec.data(), newVec.size());

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &polygon;
			m_body->CreateFixture(&fixtureDef);
			created = true;
		}
		//m_body->SetTransform(b2Vec2(this->getPosition().x, this->getPosition().y), 0);
		this->setOrigin(0, 0);
		//this->setScale(0.1f, 0.1f);
		this->m_body->SetLinearVelocity(b2Vec2(0, 1000));
	}
	//#DEPRICATED
	void createPhysicsBody()
	{
		std::vector<b2Vec2> vertices(m_vertices.size());
		for (int i = 0; i < m_vertices.size(); i++)
			vertices[i] = b2Vec2(m_vertices[i].position.x, m_vertices[i].position.y);

		b2PolygonShape polygon;
		int fixtures = vertices.size() / 8;
		for (int i = 0; i < fixtures; i++)
		{
			int count = 8*(i+1);
			std::vector<b2Vec2>::const_iterator first = vertices.begin() + count - 8;
			std::vector<b2Vec2>::const_iterator last = vertices.begin() + count;
			std::vector<b2Vec2> newVec(first, last);
			polygon.Set(newVec.data(), newVec.size());
			b2FixtureDef fixtureDef;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &polygon;
			m_body->CreateFixture(&fixtureDef);
		}
		if (vertices.size() % 8 != 0)
		{
			std::vector<b2Vec2>::const_iterator first = vertices.begin()+ (fixtures*8);
			std::vector<b2Vec2>::const_iterator last = vertices.end();
			std::vector<b2Vec2> newVec(first, last);
			polygon.Set(newVec.data(), newVec.size());

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &polygon;
			m_body->CreateFixture(&fixtureDef);
		}

		//m_body->SetTransform(b2Vec2(this->getPosition().x, this->getPosition().y), 0);
		//this->setOrigin(0, 0);
	}
	std::vector<sf::Vertex> m_vertices;
	sf::CircleShape m_center;
	sf::PrimitiveType m_type = sf::Triangles;
	bool renderPoly = true;
	bool renderSite = false;

	b2Body* m_body;
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the entity's transform -- combine it with the one that was passed by the caller
		states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

		// apply the texture
		//states.texture = &m_texture;
		// you may also override states.shader or states.blendMode if you want
		// draw the vertex array
		if (renderPoly)
			target.draw(m_vertices.data(), m_vertices.size(), m_type, states);
		if (renderSite)
			target.draw(m_center, states);
		//target.draw(&m_site, 1, sf::Points, states);
	}
	sf::Transform m_transformation;
	sf::Texture m_texture;
};

struct VoronoiEdge
{
	VoronoiEdge(sf::Vector2f _v1, sf::Vector2f _v2, sf::Vector2f* _site) : v1{ _v1 }, v2{ _v2 }, site{ _site }{};
	sf::Vector2f v1;
	sf::Vector2f v2;
	sf::Vector2f* site;
};

struct VoronoiCell
{
	sf::Vector2f* site;
	std::vector<VoronoiEdge*> edges;

	VoronoiCell(sf::Vector2f* sitePos) : site{ sitePos } {};
};

void addVoronoiEdge(HALF_EDGE::HE_Edge* e, sf::Vector2f center, std::vector<VoronoiEdge*>& voronoiEdges)
{
	//Ignore if this edge has no neighboring triangle
	if (e->twin == nullptr)
	{
		return;
	}
	//Get the circumcenter of the neighbor
	HALF_EDGE::HE_Edge* eNeighbor = e->twin;
	sf::Vector2f voronoiVertexNeighbor = eNeighbor->face->circumCenter;


	//Create a new voronoi edge between the voronoi vertices
	voronoiEdges.push_back(DBG_NEW
		VoronoiEdge(center, voronoiVertexNeighbor, e->prev->vert->point));
}

void CreateSeedPoints(std::vector<HALF_EDGE::HE_Vertex*>& BWpoints)
{
	for (size_t i = 0; i < SEEDS; i++)
	{
		float x = float(rand() % MAXSIZE + MINSIZE);
		float y = float(rand() % MAXSIZE + MINSIZE);
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(x, y, i));
	}

	//BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(CENTER, CENTER, 0));
	{
		float minMax = MAXSIZE * 2;
		sf::Vector2f starTop = { CENTER, CENTER - minMax };
		sf::Vector2f starLeft = { CENTER - minMax, CENTER };
		sf::Vector2f starRight = { CENTER + minMax, CENTER };
		sf::Vector2f starBottom = { CENTER, CENTER + minMax };

		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(MINSIZE, MINSIZE, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(MAXSIZE + MINSIZE, MINSIZE, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(MAXSIZE + MINSIZE, MAXSIZE + MINSIZE, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(MINSIZE, MAXSIZE + MINSIZE, BWpoints.size()));

		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, BWpoints.size()));
	}
}

void CreateEdgeList(std::vector<Triangle>& triangles, std::vector<HALF_EDGE::HE_Vertex*>& BWpoints,std::map< std::pair<unsigned int, unsigned int>, HALF_EDGE::HE_Edge* >& edgeList, std::vector<HALF_EDGE::HE_Face*>& faceList)
{
	std::vector<std::pair<unsigned int, unsigned int>> EdgeIndex;
	for (const Triangle &triangle : triangles)
	{
		EdgeIndex.push_back(std::make_pair(triangle.v1->arrayIndex, triangle.v2->arrayIndex));
		EdgeIndex.push_back(std::make_pair(triangle.v2->arrayIndex, triangle.v3->arrayIndex));
		EdgeIndex.push_back(std::make_pair(triangle.v3->arrayIndex, triangle.v1->arrayIndex));
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		const int offset = i * 3;
		HALF_EDGE::HE_Face* tempF = DBG_NEW HALF_EDGE::HE_Face();
		tempF->circumCenter = sf::Vector2f(triangles[i].circle.x, triangles[i].circle.y);
		tempF->radius = triangles[i].circle.radius;
		for (size_t k = 0; k < 3; k++)
		{
			const int location = offset + k;

			std::pair<unsigned int, unsigned int> memEdges(EdgeIndex[location]);
			edgeList[memEdges] = DBG_NEW HALF_EDGE::HE_Edge();
			edgeList[memEdges]->face = tempF;

			edgeList[memEdges]->vert = BWpoints[memEdges.second];
			edgeList[memEdges]->vert->edge = edgeList[EdgeIndex[offset]];
		}

		tempF->edge = edgeList[EdgeIndex[offset]];

		for (size_t k = 0; k < 3; k++)
		{
			const int location = offset + k;
			edgeList[EdgeIndex[location]]->setNext(
				edgeList[EdgeIndex[offset + (k + 1) % 3]]);

			std::pair<unsigned int, unsigned int> otherPair = std::make_pair(EdgeIndex[location].second, EdgeIndex[location].first);
			if (edgeList.find(otherPair) != edgeList.end())
			{
				edgeList[EdgeIndex[location]]->setTwin(edgeList[otherPair]);
			}

		}
		faceList.push_back(tempF);
	}
}

bool LiangBarsky(double edgeLeft, double edgeRight, double edgeBottom, double edgeTop,   // Define the x/y clipping values for the border.
	sf::Vector2f src0, sf::Vector2f src1,                 // Define the start and end points of the line.
	sf::Vector2f &clip0, sf::Vector2f &clip1)         // The output values, so declare these outside.
{

	double t0 = 0.0;    double t1 = 1.0;
	double xdelta = src1.x - src0.x;
	double ydelta = src1.y - src0.y;
	double p, q, r;

	for (int edge = 0; edge < 4; edge++) {   // Traverse through left, right, bottom, top edges.
		if (edge == 0) { p = -xdelta;    q = -(edgeLeft - src0.x); }
		if (edge == 1) { p = xdelta;     q = (edgeRight - src0.x); }
		if (edge == 2) { p = ydelta;    q = (edgeBottom - src0.y); }
		if (edge == 3) { p = -ydelta;     q = -(edgeTop - src0.y); }
		r = q / p;
		if (p == 0 && q < 0) return false;   // Don't draw line at all. (parallel line outside)

		if (p < 0) {
			if (r > t1) return false;         // Don't draw line at all.
			else if (r > t0) t0 = r;            // Line is clipped!
		}
		else if (p > 0) {
			if (r < t0) return false;      // Don't draw line at all.
			else if (r < t1) t1 = r;         // Line is clipped!
		}
	}

	clip0 = { src0.x + float(t0 * xdelta) , src0.y + float(t0 * ydelta) };
	clip1 = { src0.x + float(t1 * xdelta) , src0.y + float(t1 * ydelta) };

	return true;        // (clipped) line is drawn
}


int main()
{
	PhysicsShapeDraw debugDraw;
	world.SetDebugDraw(&debugDraw);
	debugDraw.SetFlags(b2Draw::e_shapeBit);

	std::vector<HALF_EDGE::HE_Vertex*> BWpoints;
	CreateSeedPoints(BWpoints);
	std::cout << "BWPoints Size: " << BWpoints.size() << std::endl;

	std::vector<Triangle> triangles;
	std::vector<HALF_EDGE::HE_Face*>						   			   faceList;
	Delunay::Triangulate(BWpoints, faceList);

	std::map< std::pair<unsigned int, unsigned int>, HALF_EDGE::HE_Edge* > edgeList;
	//CreateEdgeList(triangles, BWpoints, edgeList, faceList);
	

	std::vector<VoronoiEdge*> allVoronoiEdges;
	for (size_t i = 0; i < faceList.size(); i++)
	{
		HALF_EDGE::HE_Edge* e1 = faceList[i]->edge;
		HALF_EDGE::HE_Edge* e2 = e1->next;
		HALF_EDGE::HE_Edge* e3 = e2->next;

		sf::Vector2f* v1 = e1->vert->point;
		sf::Vector2f* v2 = e2->vert->point;
		sf::Vector2f* v3 = e3->vert->point;

		sf::Vector2f voronoiVertex = faceList[i]->circumCenter;

		addVoronoiEdge(e1, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e2, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e3, voronoiVertex, allVoronoiEdges);
	}

	std::vector<VoronoiCell> voronoiCells;
	voronoiCells.reserve(BWpoints.size());
	for (int i = 0; i < allVoronoiEdges.size(); i++)
	{
		VoronoiEdge* e = allVoronoiEdges[i];

		int index = -1;
		//Find the position in the list of all cells that includes this site
		for (int k = 0; k < voronoiCells.size(); k++)
		{
			if (e->site == voronoiCells[k].site)
			{
				index = k;
				break;
			}
		}

		int cellPos = index;
		//No cell was found so we need to create a new cell
		if (cellPos == -1)
		{
			VoronoiCell newCell(e->site);

			voronoiCells.push_back(newCell);

			voronoiCells.back().edges.push_back(e);
		}
		else
		{
			voronoiCells[cellPos].edges.push_back(e);
		}
	}


	std::vector<Polygon> polygons;
	std::vector<std::vector<b2Vec2>> physicsPolygons;
	physicsPolygons.reserve(BWpoints.size());
	polygons.resize(BWpoints.size());
	std::vector<sf::CircleShape> seedPoints;
	int counter = 0;
	
	for (auto &cell : voronoiCells)
	{
		sf::Color color(rand() % 200, rand() % 200, rand() % 200);
		std::map<std::pair<int, int>, sf::Vector2f> box2DVertices;
		std::vector<b2Vec2> physicsVertices;
		Polygon polygon;
		sf::Vector2f polyCenter;
		int vertCount = 0;
		for (int i = 0; i < cell.edges.size(); i++)
		{
			sf::Vector2f p3(cell.edges[i]->v1);
			sf::Vector2f p2(cell.edges[i]->v2);
			sf::Vector2f p1(*cell.site);
			sf::Vertex test[3];
			test[0] = p1;
			test[0].color = color;
			test[1] = p2;
			test[1].color = color;
			test[2] = p3;
			test[2].color = color;

			sf::Vector2f v1Result;
			sf::Vector2f v2Result;
			bool draw = true;
			float halfwidth = float(MAXSIZE / 2) + 1.0f;

			if (LiangBarsky(CENTER - halfwidth, CENTER + halfwidth,
				CENTER + halfwidth, CENTER - halfwidth,
				test[1].position, test[2].position, v1Result, v2Result))
			{
				sf::Vector2f v1ResultN;
				sf::Vector2f v2ResultN;
				if (v1Result != test[1].position)
				{
					if (LiangBarsky(CENTER - halfwidth, CENTER + halfwidth,
						CENTER + halfwidth, CENTER - halfwidth,
						test[0].position, test[1].position, v1ResultN, v2ResultN))
					{
						sf::Vertex nVertex;
						nVertex = p1;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v1Result;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2ResultN;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						polyCenter += v1Result;
						polyCenter += v2ResultN;
						vertCount += 2;

						physicsVertices.push_back(b2Vec2(std::round(v1Result.x), std::round(v1Result.y)));
						physicsVertices.push_back(b2Vec2(std::round(v2ResultN.x), std::round(v2ResultN.y)));
					}


				}
				if (v2Result != test[2].position)
				{
					if (LiangBarsky(CENTER - halfwidth, CENTER + halfwidth,
						CENTER + halfwidth, CENTER - halfwidth,
						test[0].position, test[2].position, v1ResultN, v2ResultN))
					{
						sf::Vertex nVertex;
						nVertex = p1;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2Result;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2ResultN;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						polyCenter += v2Result;
						polyCenter += v2ResultN;
						vertCount += 2;

						physicsVertices.push_back(b2Vec2(std::round(v2Result.x), std::round(v2Result.y)));
						physicsVertices.push_back(b2Vec2(std::round(v2ResultN.x), std::round(v2ResultN.y)));
					}


				}

				test[1].position = v1Result;
				test[2].position = v2Result;
			}
			else
			{
				bool passed = false;
				if (LiangBarsky(CENTER - halfwidth, CENTER + halfwidth,
					CENTER + halfwidth, CENTER - halfwidth,
					test[0].position, test[2].position, v1Result, v2Result))
				{
					test[0].position = v1Result;
					test[2].position = v2Result;
					passed = true;
				}
				if (LiangBarsky(CENTER - halfwidth, CENTER + halfwidth,
					CENTER + halfwidth, CENTER - halfwidth,
					test[0].position, test[1].position, v1Result, v2Result))
				{
					test[0].position = v1Result;
					test[1].position = v2Result;
					passed = true;
				}
				if (!passed)
					draw = false;
			}

			if (draw)
			{
				polyCenter += test[1].position;
				polyCenter += test[2].position;
				vertCount += 2;
				polygon.m_vertices.push_back(test[0]);
				polygon.m_vertices.push_back(test[1]);
				polygon.m_vertices.push_back(test[2]);
				physicsVertices.push_back(b2Vec2(std::round(test[1].position.x), std::round(test[1].position.y)));
				physicsVertices.push_back(b2Vec2(std::round(test[2].position.x), std::round(test[2].position.y)));
			}
		}

		if (polygon.m_vertices.size() != 0)
		{
			polyCenter.x /= vertCount;
			polyCenter.y /= vertCount;
			polygon.m_center.setPosition(sf::Vector2f(cell.site->x, cell.site->y) - sf::Vector2f(polygon.m_center.getRadius(), polygon.m_center.getRadius()));
			polygon.setOrigin(polyCenter);
			polygon.setPosition(polyCenter);

			physicsPolygons.push_back(physicsVertices);
			polygons[counter] = polygon;
			counter++;
		}
	}

	polygons.resize(counter);
	polygons.shrink_to_fit();
	physicsPolygons.shrink_to_fit();
	for (auto &physPol : physicsPolygons)
	{
		std::vector<bool> remove(physPol.size(), false);
		{
			for (size_t i = 0; i < physPol.size(); i++)
			{
				for (size_t k = i; k < physPol.size(); k++)
				{
					if (i == k) {
						continue;
					}
					if (physPol[i] == physPol[k])
					{
						remove[k] = true;
					}
				}
			}
			auto is_duplicate = [&](auto const& e) { return remove[&e - &physPol[0]]; };
			erase_where(physPol, is_duplicate);
			remove.clear();
		}
	}
	counter = 0;
	/*for (auto& polygon : polygons)
	{
		polygon.createPhysicsBody(physicsPolygons[counter]);
		counter++;
	}*/

	double dt = 1 / 60;
	double timer = 0;
	
	sf::Vector2f oldPos;
	bool moving = false;
	float zoom = 5;

	sf::View view = window.getDefaultView();
	view.setCenter(sf::Vector2f(CENTER, CENTER));
	view.zoom(zoom);
	window.setView(view);

	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	float32 timeStep = 1.0f / 60.0f;

	b2BodyDef myBodyDef;
	myBodyDef.type = b2_staticBody; //this will be a dynamic body
	myBodyDef.position.Set(0, MAXSIZE+(CENTER/2)); //set the starting position
	myBodyDef.angle = 0;
	b2Body* dynamicBody = world.CreateBody(&myBodyDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(50000, 100);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	dynamicBody->CreateFixture(&boxFixtureDef);

	sf::ConvexShape plane;
	plane.setPointCount(boxShape.m_count);
	for (int i = 0; i < plane.getPointCount(); i++)
		plane.setPoint(i, sf::Vector2f(boxShape.m_vertices[i].x, boxShape.m_vertices[i].y));
	plane.setOrigin(0,0);
	plane.setPosition(dynamicBody->GetPosition().x, dynamicBody->GetPosition().y);
	plane.setFillColor(sf::Color(0,200,200));
	while (window.isOpen())
	{
		auto start = std::chrono::system_clock::now();
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				if (event.key.code == sf::Keyboard::Num1)
				{ }
				if (event.key.code == sf::Keyboard::Num2)
				{ }
				break;
			case sf::Event::MouseButtonPressed:
				// Mouse button is pressed, get the position and set moving as active
				if (event.mouseButton.button == 0) {
					moving = true;
					oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				}
				break;
			case  sf::Event::MouseButtonReleased:
				// Mouse button is released, no longer move
				if (event.mouseButton.button == 0) {
					moving = false;
				}
				break;
			case sf::Event::MouseMoved:
			{
				// Ignore mouse movement unless a button is pressed (see above)
				if (!moving)
					break;
				// Determine the new position in world coordinates
				const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
				// Determine how the cursor has moved
				// Swap these to invert the movement direction
				const sf::Vector2f deltaPos = oldPos - newPos;

				// Move our view accordingly and update the window
				view.setCenter(view.getCenter() + deltaPos);
				window.setView(view);

				// Save the new position as the old one
				// We're recalculating this, since we've changed the view
				oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
				break;
			}
			case sf::Event::MouseWheelScrolled:
				// Ignore the mouse wheel unless we're not moving
				if (moving)
					break;

				// Determine the scroll direction and adjust the zoom level
				// Again, you can swap these to invert the direction
				if (event.mouseWheelScroll.delta <= -1)
					zoom = std::min(70.f, zoom + 1.0f);
				else if (event.mouseWheelScroll.delta >= 1)
					zoom = std::max(.5f, zoom - 1.0f);

				// Update our view
				view.setSize(window.getDefaultView().getSize()); // Reset the size
				view.zoom(zoom); // Apply the zoom level (this transforms the view)
				window.setView(view);
				break;
			}
		}

		
		if (timer >= 1.0f/60.0f)
		{
			world.Step(timeStep, velocityIterations, positionIterations);
			timer = 0;
		}
		window.clear({ 0,0,0 });

		/*for (int i = 0; i < polygons.size(); i++)
		{
		
			polygons[i].setPosition(sf::Vector2f(polygons[i].m_body->GetPosition().x, polygons[i].m_body->GetPosition().y));
			polygons[i].setRotation(polygons[i].m_body->GetAngle()*(180 / b2_pi));
			
		}*/
		for (int i = 0; i < polygons.size(); i++)
		{
			window.draw(polygons[i]);
		}
		window.draw(plane);
		//world.DrawDebugData();
		
		//window.draw(sf::CircleShape(500));

		/*for (int i = 0; i < polygons.size(); i++)
		{
			b2Vec2 localCenter = polygons[i].m_body->GetPosition();
			sf::CircleShape cent;
			cent.setRadius(200);
			cent.setPosition(sf::Vector2f(localCenter.x, localCenter.y));
			window.draw(cent);
		}*/

		window.display();

		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
		dt = elapsed_seconds.count();
		timer += dt;
	}

	for (auto &face : faceList)
	{
		delete face;
	}
	faceList.clear();
	for (auto &edge : edgeList)
	{
		delete edge.second;
	}
	edgeList.clear();
	for (auto &edge : allVoronoiEdges)
	{
		delete edge;
	}
	allVoronoiEdges.clear();

	for (auto &point : BWpoints)
	{
		delete point;
	}
	BWpoints.clear();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
